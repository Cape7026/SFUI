#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "UIElement.hpp"

class UIManager
{
public:
    void add(std::shared_ptr<UIElement> elem);

    void handleEvent(const sf::Event &event, const sf::RenderWindow &window);
    void handelVisual();
    void draw(sf::RenderWindow &window);

private:
    std::vector<std::shared_ptr<UIElement>> m_elements;
};