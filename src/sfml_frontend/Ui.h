#pragma once

#include <SFML/Graphics.hpp>

#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

namespace ui {

struct Theme {
    sf::Color background{16, 21, 27};
    sf::Color water{25, 62, 82};
    sf::Color sand{190, 170, 120};
    sf::Color grass{66, 112, 62};
    sf::Color grassDark{48, 88, 48};
    sf::Color panel{23, 29, 36, 238};
    sf::Color panelLight{32, 40, 50, 245};
    sf::Color border{68, 83, 99};
    sf::Color text{232, 238, 244};
    sf::Color textDim{146, 160, 176};
    sf::Color accent{242, 184, 84};
    sf::Color hp{214, 72, 72};
    sf::Color food{220, 150, 62};
    sf::Color waterStat{66, 148, 218};
    sf::Color energy{108, 196, 112};
    sf::Color danger{232, 78, 78};
    sf::Color success{92, 198, 122};
    sf::Color warning{234, 190, 82};
    sf::Color shadow{0, 0, 0, 110};
};

const Theme& theme();

enum class Align {
    Left,
    Center,
    Right
};

sf::Text makeText(const sf::Font& font, const std::string& value,
                  unsigned size, sf::Color color, bool bold = false);

void drawText(sf::RenderTarget& target, const sf::Font& font,
              const std::string& value, unsigned size, sf::Color color,
              sf::Vector2f position, Align align = Align::Left,
              bool bold = false, float outline = 0.0f,
              sf::Color outlineColor = sf::Color::Black);

void drawPanel(sf::RenderTarget& target, const sf::FloatRect& rect,
               sf::Color fill = theme().panel,
               sf::Color border = theme().border, float borderWidth = 1.0f);

void drawBar(sf::RenderTarget& target, sf::Vector2f position,
             sf::Vector2f size, float value, float maximum, sf::Color color,
             bool showBackground = true);

void drawKeyCap(sf::RenderTarget& target, const sf::Font& font,
                const std::string& label, sf::Vector2f position,
                unsigned size = 15);

bool contains(const sf::FloatRect& rect, sf::Vector2f point);

std::vector<std::string> wrapText(const sf::Font& font,
                                  const std::string& value, unsigned size,
                                  float maxWidth);

std::string format(
    const std::string& pattern,
    std::initializer_list<std::pair<std::string, std::string>> values);

std::string formatInt(const std::string& pattern, const std::string& key,
                      int value);

std::string formatFloat(const std::string& pattern, const std::string& key,
                        float value, int decimals = 0);

// Smoothly interpolate a colour; used for damage flashes and low-stat bars.
sf::Color mix(sf::Color a, sf::Color b, float t);

} // namespace ui
