#include "UIManager.hpp"

sf::RenderWindow *UIManager::getWindow() const
{
    return m_window;
}

void UIManager::add(std::shared_ptr<UIElement> element)
{
    m_elements.push_back(std::move(element));
}

void UIManager::handleEvent(const sf::Event &event, const sf::RenderWindow &window)
{
    for (auto &e : m_elements)
        e->handleEvent(event, window);
}

void UIManager::handleVisual()
{
    for (auto &e : m_elements)
        e->handleVisual();
}

void UIManager::drawElements(sf::RenderWindow &window)
{
    for (auto &e : m_elements)
        e->draw(window);
}



void UIManager::clearWindow(sf::RenderWindow &window)
{
    window.clear(m_theme.windowBackground);
}




