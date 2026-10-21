#include <SFML/Graphics.hpp>
#include <optional>
#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>

constexpr float PI = 3.14159265358979323846f;
constexpr float G = 9.81f;

float toRadians(float degrees)
{
    return degrees * PI / 180.f;
}

float toDegrees(float radians)
{
    return radians * 180.f / PI;
}

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({1000, 700}),
        "Mathematical Pendulum - Lab 5"
    );

    window.setFramerateLimit(60);

    // Parameters
    float length = 1.f;             // metres
    float initialAngle = 30.f;       // degrees
    float angle = toRadians(initialAngle);
    float angularVelocity = 0.f;
    float elapsedTime = 0.f;
    float simulationTime = 0.f;
    float scale = 180.f;             // pixels per metre

    bool running = false;

    sf::Clock frameClock;

    sf::CircleShape bob(22.f);
    bob.setOrigin({22.f, 22.f});
    bob.setFillColor(sf::Color(70, 170, 255));

    sf::CircleShape pivot(7.f);
    pivot.setOrigin({7.f, 7.f});
    pivot.setFillColor(sf::Color::White);

    sf::RectangleShape rod({2.f, 1.f});
    rod.setFillColor(sf::Color(210, 210, 210));

    sf::CircleShape trajectory(2.f);
    trajectory.setFillColor(sf::Color(100, 100, 100));

    sf::Font font;
    bool fontLoaded = font.openFromFile("C:/Windows/Fonts/arial.ttf");

    std::optional<sf::Text> info;

    if (fontLoaded)
    {
        info.emplace(font, "");
        info->setCharacterSize(19);
        info->setFillColor(sf::Color::White);
        info->setPosition({30.f, 25.f});
    }

    const sf::Vector2f pivotPosition(500.f, 150.f);

    while (window.isOpen())
    {
        float dt = frameClock.restart().asSeconds();

        // Avoid a large time step after moving or resizing the window.
        if (dt > 0.05f)
            dt = 0.05f;

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyEvent->code == sf::Keyboard::Key::Escape)
                    window.close();

                if (keyEvent->code == sf::Keyboard::Key::Space)
                    running = !running;

                if (keyEvent->code == sf::Keyboard::Key::R)
                {
                    angle = toRadians(initialAngle);
                    angularVelocity = 0.f;
                    elapsedTime = 0.f;
                    simulationTime = 0.f;
                    running = false;
                }

                if (keyEvent->code == sf::Keyboard::Key::Down)
                {
                    length += 0.1f;
                    if (length > 2.5f)
                        length = 2.5f;
                }

                if (keyEvent->code == sf::Keyboard::Key::Up)
                {
                    length -= 0.1f;
                    if (length < 0.5f)
                        length = 0.5f;
                }

                if (keyEvent->code == sf::Keyboard::Key::Right)
                {
                    initialAngle += 5.f;
                    if (initialAngle > 80.f)
                        initialAngle = 80.f;

                    if (!running)
                    {
                        angle = toRadians(initialAngle);
                        angularVelocity = 0.f;
                    }
                }

                if (keyEvent->code == sf::Keyboard::Key::Left)
                {
                    initialAngle -= 5.f;
                    if (initialAngle < 5.f)
                        initialAngle = 5.f;

                    if (!running)
                    {
                        angle = toRadians(initialAngle);
                        angularVelocity = 0.f;
                    }
                }
            }
        }

        // Equation of motion:
        // theta'' = -(g / L) * sin(theta)
        // Numerical integration: semi-implicit Euler method.
        if (running)
        {
            float angularAcceleration =
                -(G / length) * std::sin(angle);

            angularVelocity += angularAcceleration * dt;
            angle += angularVelocity * dt;

            elapsedTime += dt;
            simulationTime += dt;
        }

        float bobX = pivotPosition.x
                   + length * scale * std::sin(angle);

        float bobY = pivotPosition.y
                   + length * scale * std::cos(angle);

        sf::Vector2f bobPosition(bobX, bobY);

        // Draw rod between pivot and bob.
        float dx = bobX - pivotPosition.x;
        float dy = bobY - pivotPosition.y;
        float rodLength = std::sqrt(dx * dx + dy * dy);
        float rodAngle = std::atan2(dy, dx) * 180.f / PI;

        rod.setSize({rodLength, 3.f});
        rod.setOrigin({0.f, 1.5f});
        rod.setPosition(pivotPosition);
        rod.setRotation(sf::degrees(rodAngle));

        // Theoretical small-angle period.
        float theoreticalPeriod = 2.f * PI * std::sqrt(length / G);

        std::ostringstream text;
        text << std::fixed << std::setprecision(2);
        text << "MATHEMATICAL PENDULUM\n\n";
        text << "Length: " << length << " m  (Up/Down)\n";
        text << "Initial angle: " << initialAngle
             << " deg  (Left/Right)\n";
        text << "Current angle: " << toDegrees(angle) << " deg\n";
        text << "Angular velocity: " << angularVelocity << " rad/s\n";
        text << "Time: " << simulationTime << " s\n";
        text << "Theoretical period: " << theoreticalPeriod << " s\n\n";
        text << "SPACE - Start/Pause\n";
        text << "R - Reset\n";
        text << "ESC - Exit\n";
        text << (running ? "\nStatus: RUNNING" : "\nStatus: PAUSED");

        if (info)
            info->setString(text.str());

        window.clear(sf::Color(24, 28, 38));

        // Reference line below the pivot.
        sf::RectangleShape reference({260.f, 1.f});
        reference.setFillColor(sf::Color(65, 70, 82));
        reference.setPosition({370.f, pivotPosition.y});
        window.draw(reference);

        window.draw(rod);
        window.draw(pivot);

        bob.setPosition(bobPosition);
        window.draw(bob);

        if (info)
        {
            window.draw(*info);
        }
        else
        {
            // The simulation still runs if the font is unavailable.
            // Put arial.ttf in the project folder to enable text.
        }

        window.display();
    }

    return 0;
}