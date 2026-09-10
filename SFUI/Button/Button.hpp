#pragma once

/* 
#include <optional> */

#include "../UIElement.hpp"

class Button : public UIElement
{
public:
    void handleEvent(const sf::Event &event, const sf::RenderWindow &window) override;
    void handleVisual() override;
    void draw(sf::RenderWindow &window) override;

private:
    void adjustTextPosition();
    void adjustTextSize();

    sf::RoundedRectangleShape m_shape;
    std::optional<sf::Text> m_text;
    bool m_hovered = false;
    bool m_pressed = false;
};
