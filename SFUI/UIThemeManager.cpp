#include "UIThemeManager.hpp"

void UIThemeManager::setTheme(const Theme &theme)
{
    m_theme = theme;
}
Theme UIThemeManager::getTheme()
{
    return m_theme;
}
