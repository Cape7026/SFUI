#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <optional>
#include <iostream>
#include "RoundedRectangleShape.hpp"

void letterboxView(sf::View &view, unsigned int windowWidth, unsigned int windowHeight)
{
    float windowRatio = static_cast<float>(windowWidth) / windowHeight;
    float viewRatio = view.getSize().x / view.getSize().y;

    float sizeX = 1.f, sizeY = 1.f;
    float posX = 0.f, posY = 0.f;

    if (windowRatio > viewRatio)
    {
        sizeX = viewRatio / windowRatio;
        posX = (1.f - sizeX) / 2.f;
    }
    else
    {
        sizeY = windowRatio / viewRatio;
        posY = (1.f - sizeY) / 2.f;
    }

    view.setViewport(sf::FloatRect({posX, posY}, {sizeX, sizeY}));
}

int main()
{
    sf::ContextSettings settings{0, 0, 16};
    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFUI", sf::State::Windowed, settings);
    window.setFramerateLimit(100);

    sf::View camera;
    camera.setSize({static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)});
    camera.setCenter({static_cast<float>(window.getSize().x) / 2.f, static_cast<float>(window.getSize().y) / 2.f});

    sf::Font font;
    if (!font.openFromFile("Aptos.ttf"))
    {
        MessageBoxA(
            nullptr,
            "Failed to load arial.ttf.",
            "Font Loading Error",
            MB_OK | MB_ICONERROR);

        return -1;
    }
    font.setSmooth(false);

    sf::RoundedRectangleShape other;
    other.setSize({100.f, 100.f});
    other.setPosition({window.getSize().x / 2.f, window.getSize().y / 2.f});
    other.setFillColor(sf::Color(200, 200, 200));
    other.setCornerPointCount(256);

    sf::RoundedRectangleShape parent;
    parent.setSize({600.f, 500.f});
    parent.setPosition({window.getSize().x / 2.f, window.getSize().y / 2.f});
    parent.setFillColor(sf::Color(60, 60, 60));
    parent.setOrigin({parent.getSize().x / 2.f, parent.getSize().y / 2.f});
    parent.setCornerPointCount(256);

    sf::CircleShape child;
    child.setRadius(150.f);
    child.setPointCount(256);
    child.setFillColor(sf::Color::Red);
    child.setOrigin({child.getRadius(), child.getRadius()});
    child.setPosition({0.f, 0.f});

    sf::View parentView;
    parentView.setSize(parent.getSize());
    parentView.setCenter(parent.getPosition());

    sf::RectangleShape parentViewOutline;
    parentViewOutline.setSize(parentView.getSize());
    parentViewOutline.setOrigin({parentView.getSize().x / 2.f, parentView.getSize().y / 2.f});
    parentViewOutline.setFillColor(sf::Color::Transparent);
    parentViewOutline.setOutlineColor(sf::Color::Green);
    parentViewOutline.setOutlineThickness(3.f);

    sf::Clock clock;

    const float baseSpeed = 550.f;
    const float rotationSpeed = 0.f;
    float dt = 0.f;
    bool isFocused = true;

    while (window.isOpen())
    {
        dt = clock.restart().asSeconds();

        while (std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (event->is<sf::Event::FocusLost>())
            {
                isFocused = false;
            }
            else if (event->is<sf::Event::FocusGained>())
            {
                isFocused = true;
            }
            else if (event->is<sf::Event::Resized>())
            {
                camera.setSize({static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)});
                camera.setCenter({window.getSize().x / 2.f, window.getSize().y / 2.f});
                letterboxView(camera, window.getSize().x, window.getSize().y);
                window.setView(camera);
            }
        }

        sf::Vector2f movement(0.f, 0.f);

        if (isFocused)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S))
                movement.y += 1.f;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W))
                movement.y -= 1.f;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A))
                movement.x -= 1.f;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D))
                movement.x += 1.f;

            float length = sqrt(
                movement.x * movement.x +
                movement.y * movement.y);

            if (length != 0.f)
            {
                movement.x /= length;
                movement.y /= length;
            }
        }
        camera.move(movement * baseSpeed * dt);
        camera.rotate(sf::degrees(rotationSpeed * dt));
        window.clear(sf::Color::Black);
        window.setView(camera);
        window.draw(parent);
        sf::FloatRect bounds = parent.getGlobalBounds();
        sf::Vector2i topLeft = window.mapCoordsToPixel(bounds.position, camera);
        sf::Vector2i bottomRight = window.mapCoordsToPixel(bounds.position + bounds.size, camera);
        parentView.setSize(parent.getSize());
        parentView.setCenter(parent.getPosition());
        float viewportX = static_cast<float>(topLeft.x) / static_cast<float>(window.getSize().x);
        float viewportY = static_cast<float>(topLeft.y) / static_cast<float>(window.getSize().y);
        float viewportWidth = static_cast<float>(bottomRight.x - topLeft.x) / static_cast<float>(window.getSize().x);
        float viewportHeight = static_cast<float>(bottomRight.y - topLeft.y) / static_cast<float>(window.getSize().y);
        parentView.setViewport(sf::FloatRect({viewportX, viewportY}, {viewportWidth, viewportHeight}));
        parentViewOutline.setPosition(parentView.getCenter());
        window.setView(parentView);
        window.draw(child);
        window.setView(camera);
        window.draw(parentViewOutline);
        window.draw(other);
        window.display();
    }
}