#include "Theme.hpp"


Theme::Theme()
{
    if (!font.openFromFile("Aptos.ttf"))
    {
        MessageBoxA(
            nullptr,
            "Failed to load Aptos.ttf.",
            "Font Loading Error",
            MB_OK | MB_ICONERROR);
    }
};

Theme darkTheme = {
    sf::Color(32, 32, 32),
    sf::Color(75, 181, 236),

    sf::Color(45, 45, 45),
    sf::Color(50, 50, 50),
    sf::Color(39, 39, 39),
    sf::Color(75, 181, 236),
    sf::Color(53, 53, 53),
    sf::Color(48, 48, 48),
    sf::Color(26, 26, 26),

    sf::Color(255, 255, 255),
    sf::Color(100, 100, 100),

    sf::Color(154, 154, 154),
    sf::Color(75, 181, 236),
    sf::Color(75, 181, 236),
    sf::Color(69, 69, 69),
    sf::Color(36, 36, 36),
    sf::Color(26, 26, 26),

    1.f,
    16u
};