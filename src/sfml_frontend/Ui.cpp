#include "Ui.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace ui {

const Theme& theme() {
    static const Theme value;
    return value;
}

sf::Text makeText(const sf::Font& font, const std::string& value,
                  unsigned size, sf::Color color, bool bold) {
    // Localization strings are UTF-8.  sf::String's std::string constructor
    // uses the ANSI locale, which would mangle Cyrillic text, so decode
    // explicitly.
    sf::Text text(font, sf::String::fromUtf8(value.begin(), value.end()),
                  size);
    text.setFillColor(color);
    if (bold) {
        text.setStyle(sf::Text::Bold);
    }
    return text;
}

void drawText(sf::RenderTarget& target, const sf::Font& font,
              const std::string& value, unsigned size, sf::Color color,
              sf::Vector2f position, Align align, bool bold, float outline,
              sf::Color outlineColor) {
    sf::Text text = makeText(font, value, size, color, bold);
    const sf::FloatRect bounds = text.getLocalBounds();

    float offsetX = 0.0f;
    if (align == Align::Center) {
        offsetX = -bounds.size.x * 0.5f;
    } else if (align == Align::Right) {
        offsetX = -bounds.size.x;
    }
    text.setPosition({position.x + offsetX - bounds.position.x,
                      position.y - bounds.position.y});

    if (outline > 0.0f) {
        text.setOutlineThickness(outline);
        text.setOutlineColor(outlineColor);
    }
    target.draw(text);
}

void drawPanel(sf::RenderTarget& target, const sf::FloatRect& rect,
               sf::Color fill, sf::Color border, float borderWidth) {
    sf::RectangleShape shadow(rect.size);
    shadow.setPosition({rect.position.x + 3.0f, rect.position.y + 4.0f});
    shadow.setFillColor(theme().shadow);
    target.draw(shadow);

    sf::RectangleShape shape(rect.size);
    shape.setPosition(rect.position);
    shape.setFillColor(fill);
    shape.setOutlineThickness(borderWidth);
    shape.setOutlineColor(border);
    target.draw(shape);
}

void drawBar(sf::RenderTarget& target, sf::Vector2f position,
             sf::Vector2f size, float value, float maximum, sf::Color color,
             bool showBackground) {
    if (showBackground) {
        sf::RectangleShape background(size);
        background.setPosition(position);
        background.setFillColor(sf::Color(10, 13, 17, 210));
        background.setOutlineThickness(1.0f);
        background.setOutlineColor(sf::Color(0, 0, 0, 120));
        target.draw(background);
    }

    const float ratio =
        maximum > 0.0001f ? std::clamp(value / maximum, 0.0f, 1.0f) : 0.0f;
    if (ratio > 0.0f) {
        sf::RectangleShape fill({size.x * ratio, size.y});
        fill.setPosition(position);
        fill.setFillColor(color);
        target.draw(fill);

        sf::RectangleShape shine({size.x * ratio, std::max(1.0f, size.y * 0.28f)});
        shine.setPosition(position);
        shine.setFillColor(sf::Color(255, 255, 255, 34));
        target.draw(shine);
    }
}

void drawKeyCap(sf::RenderTarget& target, const sf::Font& font,
                const std::string& label, sf::Vector2f position,
                unsigned size) {
    sf::Text text = makeText(font, label, size, theme().text, true);
    const sf::FloatRect bounds = text.getLocalBounds();
    const sf::Vector2f padding{8.0f, 3.0f};
    const sf::Vector2f boxSize{bounds.size.x + padding.x * 2.0f,
                               bounds.size.y + padding.y * 2.0f};

    sf::RectangleShape box(boxSize);
    box.setPosition(position);
    box.setFillColor(sf::Color(48, 58, 70, 245));
    box.setOutlineThickness(1.0f);
    box.setOutlineColor(theme().border);
    target.draw(box);

    text.setPosition({position.x + padding.x - bounds.position.x,
                      position.y + padding.y - bounds.position.y});
    target.draw(text);
}

bool contains(const sf::FloatRect& rect, sf::Vector2f point) {
    return rect.contains(point);
}

std::vector<std::string> wrapText(const sf::Font& font,
                                  const std::string& value, unsigned size,
                                  float maxWidth) {
    std::vector<std::string> lines;
    if (value.empty()) {
        return lines;
    }

    std::string current;
    std::string word;
    const auto flushWord = [&] {
        if (word.empty()) {
            return;
        }
        const std::string candidate =
            current.empty() ? word : current + " " + word;
        sf::Text measure = makeText(font, candidate, size, sf::Color::White);
        if (measure.getLocalBounds().size.x <= maxWidth || current.empty()) {
            current = candidate;
        } else {
            lines.push_back(current);
            current = word;
        }
        word.clear();
    };

    for (char character : value) {
        if (character == '\n') {
            flushWord();
            if (!current.empty()) {
                lines.push_back(current);
                current.clear();
            }
            continue;
        }
        if (character == ' ') {
            flushWord();
            continue;
        }
        word.push_back(character);
    }
    flushWord();
    if (!current.empty()) {
        lines.push_back(current);
    }
    return lines;
}

std::string format(
    const std::string& pattern,
    std::initializer_list<std::pair<std::string, std::string>> values) {
    std::string result = pattern;
    for (const auto& value : values) {
        const std::string token = "{" + value.first + "}";
        std::size_t position = 0;
        while ((position = result.find(token, position)) != std::string::npos) {
            result.replace(position, token.size(), value.second);
            position += value.second.size();
        }
    }
    return result;
}

std::string formatInt(const std::string& pattern, const std::string& key,
                      int value) {
    return format(pattern, {{key, std::to_string(value)}});
}

std::string formatFloat(const std::string& pattern, const std::string& key,
                        float value, int decimals) {
    char buffer[64] = {};
    std::snprintf(buffer, sizeof(buffer), "%.*f", decimals,
                  static_cast<double>(value));
    return format(pattern, {{key, buffer}});
}

sf::Color mix(sf::Color a, sf::Color b, float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    const auto blend = [t](std::uint8_t x, std::uint8_t y) {
        return static_cast<std::uint8_t>(
            std::round(static_cast<float>(x) +
                       (static_cast<float>(y) - static_cast<float>(x)) * t));
    };
    return sf::Color(blend(a.r, b.r), blend(a.g, b.g), blend(a.b, b.b),
                     blend(a.a, b.a));
}

} // namespace ui
