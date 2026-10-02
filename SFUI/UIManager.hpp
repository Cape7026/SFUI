#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "UIElement.hpp"

#include "UIThemeManager.hpp"

class UIManager : public UIThemeManager
{

public:
    UIManager(sf::RenderWindow *window) : m_window(window)
    {
    }

    sf::RenderWindow *getWindow() const;

    void add(std::shared_ptr<UIElement> element);
    void clearWindow(sf::RenderWindow &window);

    void handleEvent(const sf::Event &event, const sf::RenderWindow &window);
    void handleVisual();
    void drawElements(sf::RenderWindow &window);

private:
    std::vector<std::shared_ptr<UIElement>> m_elements;

protected:
    sf::RenderWindow *m_window;
};



