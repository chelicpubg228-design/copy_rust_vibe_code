#pragma once

#include <SFML/Graphics.hpp>

#include "core/Game.h"

#include <cstdint>
#include <vector>

namespace ui {

struct RenderFocus {
    int nodeId = -1;
    int animalId = -1;
    bool hasMoveTarget = false;
    cr::Vec2 moveTarget{};
};

class WorldRenderer {
public:
    void draw(sf::RenderTarget& target, const cr::Game& game, float time,
              cr::Vec2 mouseWorld, const RenderFocus& focus);

private:
    void rebuild(const cr::Game& game);
    void drawBackground(sf::RenderTarget& target, const cr::Game& game,
                        float time);
    void drawBase(sf::RenderTarget& target, const cr::Game& game, float time);
    void drawNode(sf::RenderTarget& target, const cr::Game& game,
                  const cr::Node& node, float time, bool hovered);
    void drawAnimal(sf::RenderTarget& target, const cr::Game& game,
                    const cr::Animal& animal, float time, bool hovered);
    void drawPlayer(sf::RenderTarget& target, const cr::Game& game, float time);
    void drawNight(sf::RenderTarget& target, const cr::Game& game, float time);
    void drawFocus(sf::RenderTarget& target, const cr::Game& game, float time,
                   const RenderFocus& focus);

    std::uint32_t seed_ = 0;
    std::vector<sf::Vector2f> grass_;
    std::vector<sf::Vector2f> flowers_;
    std::vector<sf::Vector2f> patches_;
};

} // namespace ui
