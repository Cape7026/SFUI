#include <SFML/Graphics.hpp>
#include <thorvg-1/thorvg.h>
#include <iostream>
#include <filesystem>
#include <thread>
#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>

struct FileItem
{
    std::string name;
    std::filesystem::path path;
    bool isFolder;
    tvg::Picture *icon;
    float x;
    float y;
};
void letterboxView(sf::View &view, unsigned int windowgeneralWindowWidth, unsigned int windowHeight)
{
    float windowRatio = static_cast<float>(windowgeneralWindowWidth) / windowHeight;
    float viewRatio = view.getSize().x / view.getSize().y;
    float sizeX = 1.0f, sizeY = 1.0f, posX = 0.0f, posY = 0.0f;

    if (windowRatio > viewRatio)
    {
        sizeX = viewRatio / windowRatio;
        posX = (1.0f - sizeX) / 2.0f;
    }
    else
    {
        sizeY = windowRatio / viewRatio;
        posY = (1.0f - sizeY) / 2.0f;
    }

    view.setViewport(sf::FloatRect(sf::Vector2f(posX, posY), sf::Vector2f(sizeX, sizeY)));
}

int main()
{
    const unsigned int RECT_W = 800;
    const unsigned int RECT_H = 600;
    tvg::Initializer::init(std::thread::hardware_concurrency());
    static uint32_t buffer[RECT_W * RECT_H] = {};
    auto canvas = tvg::SwCanvas::gen();
    canvas->target(buffer, RECT_W, RECT_W, RECT_H, tvg::ColorSpace::ABGR8888);
    sf::ContextSettings settings{0, 0, 8};
    sf::RenderWindow window(sf::VideoMode({RECT_W, RECT_H}), "File Manager", sf::State::Windowed, settings);
    sf::View view(sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y))));
    letterboxView(view, window.getSize().x, window.getSize().y);
    window.setView(view);

    std::filesystem::path folder = "C:/Users/Cape7026/";

    if (!std::filesystem::exists(folder))
    {
        std::cout << "Path Doesn't Exist!";
        tvg::Initializer::term();
        return 0;
    }

    std::vector<FileItem> items;

    const float iconSize = 32.f;
    const float rowHeight = 45.f;
    const float startX = 20.f;
    const float startY = 20.f;

    for (const auto &entry : std::filesystem::directory_iterator(folder))
    {
        if (!entry.is_directory())
            continue;

        FileItem item;

        item.name = entry.path().filename().string();
        item.path = entry.path();
        item.isFolder = true;
        item.x = startX;
        item.y = startY + items.size() * rowHeight;

        item.icon = tvg::Picture::gen();
        item.icon->load("folder.svg");
        item.icon->size(iconSize, iconSize);
        item.icon->translate(item.x, item.y);

        canvas->add(item.icon);

        items.emplace_back(item);
    }

    for (const auto &entry : std::filesystem::directory_iterator(folder))
    {
        if (!entry.is_regular_file())
            continue;

        FileItem item;

        item.name = entry.path().filename().string();
        item.path = entry.path();
        item.isFolder = false;
        item.x = startX;
        item.y = startY + items.size() * rowHeight;

        item.icon = tvg::Picture::gen();
        item.icon->load("File.svg");
        item.icon->size(iconSize, iconSize);
        item.icon->translate(item.x, item.y);

        canvas->add(item.icon);

        items.emplace_back(item);
    }

    sf::Font font;

    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        tvg::Initializer::term();
        return 1;
    }

    std::vector<sf::Text> texts;

    for (const auto &item : items)
    {
        sf::Text text(font);

        text.setString(item.name);
        text.setCharacterSize(16);
        text.setFillColor(sf::Color::White);
        text.setPosition({item.x + iconSize + 10.f,
                          item.y + 5.f});

        texts.emplace_back(std::move(text));
    }

    sf::Texture texture;

    if (!texture.resize({RECT_W, RECT_H}))
    {
        tvg::Initializer::term();
        return 1;
    }

    sf::Sprite sprite(texture);

    float scrollOffset = 0.f;

    const float contentHeight =
        startY + static_cast<float>(items.size()) * rowHeight;

    const float maxScroll =
        std::max(0.f, contentHeight - static_cast<float>(RECT_H));

    auto renderContent = [&]()
    {
        for (std::size_t i = 0; i < items.size(); i++)
        {
            auto &item = items[i];

            item.y =
                startY +
                static_cast<float>(i) * rowHeight -
                scrollOffset;

            item.icon->translate(item.x, item.y);

            texts[i].setPosition({item.x + iconSize + 10.f,
                                  item.y + 5.f});
        }

        canvas->update();
        canvas->draw(true);
        canvas->sync();

        texture.update(
            reinterpret_cast<const std::uint8_t *>(buffer));
    };

    renderContent();

    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (event->is<sf::Event::Resized>())
            {
                view.setSize(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
                // view.setCenter(sf::Vector2f(window.getSize().x / 2.f, window.getSize().y / 2.f));
                letterboxView(view, window.getSize().x, window.getSize().y);
                window.setView(view);
                sprite.setPosition({0,0});
            }

            if (const auto *wheel =
                    event->getIf<sf::Event::MouseWheelScrolled>())
            {
                if (wheel->wheel == sf::Mouse::Wheel::Vertical)
                {
                    scrollOffset -= wheel->delta * rowHeight;

                    scrollOffset = std::clamp(
                        scrollOffset,
                        0.f,
                        maxScroll);

                    renderContent();
                }
            }
        }

        window.clear(sf::Color(20, 20, 25));

        window.draw(sprite);

        for (const auto &text : texts)
        {
            float textY = text.getPosition().y;

            if (textY + 20.f >= 0.f &&
                textY <= static_cast<float>(RECT_H))
            {
                window.draw(text);
            }
        }

        window.display();
    }

    tvg::Initializer::term();

    return 0;
}