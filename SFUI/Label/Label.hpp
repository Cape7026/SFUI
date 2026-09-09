#pragma once

#include "../UIElement.hpp"

class Label : public UIElement
{
public:
    Label()
    {
        std::cout << "hello world";
    }
    void handleEvent(const sf::Event &event, const sf::RenderWindow &window) override;
    void handleVisual() override;
    void draw(sf::RenderWindow &window) override;

private:
    std::optional<sf::Text> m_text;
};

