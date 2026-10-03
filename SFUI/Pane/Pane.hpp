#pragma once

#include "../UIElement.hpp"

class Pane : public UIElement
{
    public:

    void handleEvent(const sf::Event &event, const sf::RenderWindow &window) override;
    void handleVisual() override;
    void draw(sf::RenderWindow &window) override;

    void addElement(UIElement *element);
    void removeElement(UIElement *element);
    void clearElements();
    void flexDirection(bool horizontal);
    void setPadding(float padding);
    void setSpacing(float spacing);
    void setAlignment(sf::Vector2f alignment);
    
};