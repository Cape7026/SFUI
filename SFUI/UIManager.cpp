#include "UIManager.hpp"

void UIManager::add(std::shared_ptr<UIElement> elem)
{
    m_elements.push_back(std::move(elem));
}

void UIManager::handleEvent(const sf::Event &event, const sf::RenderWindow &window)
{
    for (auto &e : m_elements)
        e->handleEvent(event, window);
}

void UIManager::handelVisual()
{
    for (auto &e : m_elements)
        e->handleVisual();
}

void UIManager::draw(sf::RenderWindow &window)
{
    for (auto &e : m_elements)
        e->draw(window);
}