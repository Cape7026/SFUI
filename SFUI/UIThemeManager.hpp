#pragma once

#include "Theme.hpp"

class UIThemeManager
{
public:
    void setTheme(const Theme &theme);
    Theme getTheme();

    protected:
    Theme m_theme = lightTheme;
};