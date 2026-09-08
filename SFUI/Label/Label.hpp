#pragma once

#include "../UIElement.hpp"

class Label : public UIElement
{
public:
    Label(const sf::Font& font, const std::string& text) : m_text(font, text)
    {
    }
    void handleEvent(const sf::Event &event, const sf::RenderWindow &window) override;
    void handleVisual() override;
    void draw(sf::RenderWindow &window) override;

private:
    sf::Text m_text;
};
