#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <optional>
#include <iostream>
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include "RoundedRectangleShape.hpp"
#include "WindowMods.h"

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

sf::FloatRect getViewportForBounds(const sf::FloatRect &bounds, const sf::View &view)
{
    const sf::Vector2f viewSize = view.getSize();
    const sf::Vector2f viewCenter = view.getCenter();
    const sf::FloatRect viewViewport = view.getViewport();
    const float angle = -view.getRotation().asRadians();
    const float cosine = std::cos(angle);
    const float sine = std::sin(angle);

    const std::array<sf::Vector2f, 4> corners = {
        bounds.position,
        sf::Vector2f(bounds.position.x + bounds.size.x, bounds.position.y),
        sf::Vector2f(bounds.position.x, bounds.position.y + bounds.size.y),
        bounds.position + bounds.size};

    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();

    for (const sf::Vector2f corner : corners)
    {
        const sf::Vector2f offset = corner - viewCenter;
        const float viewX = cosine * offset.x - sine * offset.y;
        const float viewY = sine * offset.x + cosine * offset.y;
        const float normalizedX = 0.5f + viewX / viewSize.x;
        const float normalizedY = 0.5f + viewY / viewSize.y;
        const float windowX = viewViewport.position.x + normalizedX * viewViewport.size.x;
        const float windowY = viewViewport.position.y + normalizedY * viewViewport.size.y;

        minX = std::min(minX, windowX);
        minY = std::min(minY, windowY);
        maxX = std::max(maxX, windowX);
        maxY = std::max(maxY, windowY);
    }

    return sf::FloatRect({minX, minY}, {maxX - minX, maxY - minY});
}

int main()
{
    SFUI::WindowMod::ensureSingleInstance();
    sf::ContextSettings settings{0, 0, 16};
    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFUI", sf::State::Windowed, settings);
    SFUI::WindowMod::resizeRenderFix(window);

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

    sf::RoundedRectangleShape parent;
    parent.setSize({700.f, 500.f});
    parent.setPosition({window.getSize().x / 2.f, window.getSize().y / 2.f});
    parent.setFillColor(sf::Color(60, 60, 60));
    parent.setOrigin({parent.getSize().x / 2.f, parent.getSize().y / 2.f});
    parent.setCornerPointCount(256);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    sf::View parentView;
    parentView.setSize(parent.getSize());
    parentView.setCenter(parent.getPosition());

    sf::RectangleShape parentViewOutline;
    parentViewOutline.setSize(parentView.getSize());
    parentViewOutline.setOrigin({parentView.getSize().x / 2.f, parentView.getSize().y / 2.f});
    parentViewOutline.setFillColor(sf::Color::Transparent);
    parentViewOutline.setOutlineColor(sf::Color::Green);
    parentViewOutline.setOutlineThickness(1.f);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    sf::CircleShape child;
    child.setRadius(50.f);
    child.setPointCount(256);
    child.setFillColor(sf::Color::Red);
    child.setOrigin({child.getRadius(), child.getRadius()});
    child.setPosition({0.f, 0.f});

    sf::RoundedRectangleShape other;
    other.setSize({100.f, 100.f});
    other.setPosition({window.getSize().x / 2.f, window.getSize().y / 2.f});
    other.setFillColor(sf::Color(200, 200, 200));
    other.setCornerPointCount(256);

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
            if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->code == sf::Keyboard::Key::F11)
                    SFUI::WindowMod::toggleFullscreen(window);
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
        window.clear(sf::Color::White);
        window.setView(camera);
        window.draw(parent);
        sf::FloatRect bounds = parent.getGlobalBounds();
        parentView.setSize(parent.getSize());
        parentView.setCenter(parent.getSize() / 2.f);
        parentView.setViewport(getViewportForBounds(bounds, camera));
        parentViewOutline.setPosition(parent.getPosition());

        window.setView(parentView);

        window.draw(other);

        window.setView(camera);
        window.draw(parentViewOutline);

        window.draw(child);

        window.display();
    }
}
