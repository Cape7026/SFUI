/* #include <SFML/Graphics.hpp>
#include <Windows.h>
#include <optional>
#include <iostream>
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include "RoundedRectangleShape.hpp"
#include "windowsResizeFix.h"

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
    sf::ContextSettings settings{0, 0, 16};
    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFUI", sf::State::Windowed, settings);
    sfml::Win32ResizeFix resizeFix(window);
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
    parent.setSize({10.f, 150.f});
    parent.setPosition({window.getSize().x / 2.f, window.getSize().y / 2.f});
    parent.setFillColor(sf::Color(60, 60, 60));
    parent.setOrigin({parent.getSize().x / 2.f, parent.getSize().y / 2.f});
    parent.setCornerPointCount(256);

    sf::CircleShape child;
    child.setRadius(50.f);
    child.setPointCount(256);
    child.setFillColor(sf::Color::Red);
    child.setOrigin({child.getRadius(), child.getRadius()});
    child.setPosition({50.f, 0.f});

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
        window.clear(sf::Color::White);
        window.setView(camera);
        window.draw(parent);
        sf::FloatRect bounds = parent.getGlobalBounds();
        parentView.setSize(parent.getSize());
        parentView.setCenter(parent.getSize() / 2.f);
        parentView.setViewport(getViewportForBounds(bounds, camera));
        parentViewOutline.setPosition(parent.getPosition());
        window.setView(parentView);
        window.draw(child);
        window.setView(camera);
        window.draw(parentViewOutline);
        window.draw(other);
        window.display();
    }
}


 */

#include <SFML/Graphics.hpp>
#include "WindowMods.h"

#include <algorithm>
#include <string>

int main()
{

    sf::RenderWindow window;
    SFUI::WindowMod::createSingle(window, sf::VideoMode({640, 480}), "Resize me");
    SFUI::WindowMod::resizeRenderFix(window);

    window.setVerticalSyncEnabled(true);

    sf::RectangleShape square({100.f, 100.f});
    square.setOrigin({50.f, 50.f});
    square.setFillColor(sf::Color(200, 120, 40));

    sf::RectangleShape outline;
    outline.setFillColor(sf::Color::Transparent);
    outline.setOutlineColor(sf::Color::Green);
    outline.setOutlineThickness(-4.f);

    const sf::Clock time;
    sf::Clock frameClock;
    sf::Clock titleClock;
    sf::Time worstGap;
    int frames = 0;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto *resized = event->getIf<sf::Event::Resized>())
                window.setView(sf::View(sf::FloatRect({0.f, 0.f}, sf::Vector2f(resized->size))));
            if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->code == sf::Keyboard::Key::F11)
                    SFUI::WindowMod::toggleFullscreen(window);
            }
        }

        worstGap = std::max(worstGap, frameClock.restart());
        ++frames;
        if (titleClock.getElapsedTime() >= sf::seconds(1))
        {
            window.setTitle("Resize me - " + std::to_string(frames) + " FPS, worst frame gap " + std::to_string(worstGap.asMilliseconds()) + " ms");
            frames = 0;
            worstGap = sf::Time::Zero;
            titleClock.restart();
        }

        const sf::Vector2f size(window.getSize());
        square.setPosition(size / 2.f);
        square.setRotation(sf::degrees(time.getElapsedTime().asSeconds() * 90.f));
        outline.setSize(size);

        window.clear(sf::Color(30, 30, 60));
        window.draw(outline);
        window.draw(square);
        window.display();
    }
}