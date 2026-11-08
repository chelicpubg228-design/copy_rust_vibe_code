#include "WorldRenderer.h"

#include "Ui.h"

#include <algorithm>
#include <cmath>
#include <random>

namespace ui {

namespace {

constexpr float kPi = 3.14159265358979323846f;

sf::Vector2f toSf(cr::Vec2 value) {
    return {value.x, value.y};
}

sf::Vector2f perpendicular(sf::Vector2f value) {
    return {-value.y, value.x};
}

sf::Color withAlpha(sf::Color color, int alpha) {
    return sf::Color(color.r, color.g, color.b,
                     static_cast<std::uint8_t>(std::clamp(alpha, 0, 255)));
}

void drawCircle(sf::RenderTarget& target, sf::Vector2f position, float radius,
                sf::Color color, sf::Vector2f scale = {1.0f, 1.0f}) {
    sf::CircleShape shape(radius);
    shape.setOrigin({radius, radius});
    shape.setPosition(position);
    shape.setScale(scale);
    shape.setFillColor(color);
    target.draw(shape);
}

void drawShadow(sf::RenderTarget& target, sf::Vector2f position, float radius,
                float strength = 70.0f) {
    drawCircle(target, position, radius,
               sf::Color(0, 0, 0, static_cast<std::uint8_t>(strength)),
               {1.45f, 0.42f});
}

void drawHealthBar(sf::RenderTarget& target, sf::Vector2f position, float width,
                   float ratio, sf::Color color) {
    const float height = 5.0f;
    sf::RectangleShape background({width, height});
    background.setPosition({position.x - width * 0.5f, position.y});
    background.setFillColor(sf::Color(10, 12, 16, 210));
    background.setOutlineThickness(1.0f);
    background.setOutlineColor(sf::Color(0, 0, 0, 150));
    target.draw(background);

    sf::RectangleShape fill({width * std::clamp(ratio, 0.0f, 1.0f), height});
    fill.setPosition({position.x - width * 0.5f, position.y});
    fill.setFillColor(color);
    target.draw(fill);
}

void drawRing(sf::RenderTarget& target, sf::Vector2f position, float radius,
              sf::Color color, float thickness = 2.0f) {
    sf::CircleShape ring(radius);
    ring.setOrigin({radius, radius});
    ring.setPosition(position);
    ring.setFillColor(sf::Color::Transparent);
    ring.setOutlineThickness(thickness);
    ring.setOutlineColor(color);
    target.draw(ring);
}

sf::Color treeFoliage(cr::NodeType type) {
    switch (type) {
    case cr::NodeType::TreeBig:
        return {38, 92, 52};
    case cr::NodeType::TreeNormal:
        return {48, 108, 58};
    default:
        return {62, 124, 66};
    }
}

sf::Color animalBody(cr::AnimalType type) {
    switch (type) {
    case cr::AnimalType::Mouse:
        return {142, 138, 132};
    case cr::AnimalType::Deer:
        return {150, 104, 62};
    case cr::AnimalType::Wolf:
        return {104, 112, 122};
    case cr::AnimalType::Bear:
        return {86, 60, 42};
    }
    return {140, 140, 140};
}

float nightAmount(float timeOfDay) {
    const float t = timeOfDay - std::floor(timeOfDay);
    if (t < 0.20f) {
        return 0.55f * (1.0f - t / 0.20f);
    }
    if (t < 0.30f) {
        return 0.0f;
    }
    if (t < 0.70f) {
        return 0.0f;
    }
    if (t < 0.80f) {
        return 0.55f * ((t - 0.70f) / 0.10f);
    }
    return 0.55f * (1.0f - (t - 0.80f) / 0.20f);
}

} // namespace

void WorldRenderer::rebuild(const cr::Game& game) {
    seed_ = game.world.seed;
    grass_.clear();
    flowers_.clear();
    patches_.clear();

    std::mt19937 rng(seed_ ^ 0x51ed270bu);
    std::uniform_real_distribution<float> angle(0.0f, 2.0f * kPi);
    std::uniform_real_distribution<float> unit(0.0f, 1.0f);
    const float limit = game.world.radius - 150.0f;

    grass_.reserve(1400);
    for (int i = 0; i < 1400; ++i) {
        const float a = angle(rng);
        const float r = std::sqrt(unit(rng)) * limit;
        grass_.push_back({std::cos(a) * r, std::sin(a) * r});
    }

    flowers_.reserve(180);
    for (int i = 0; i < 180; ++i) {
        const float a = angle(rng);
        const float r = std::sqrt(unit(rng)) * limit;
        flowers_.push_back({std::cos(a) * r, std::sin(a) * r});
    }

    patches_.reserve(30);
    for (int i = 0; i < 30; ++i) {
        const float a = angle(rng);
        const float r = 220.0f + std::sqrt(unit(rng)) * (limit - 220.0f);
        patches_.push_back({std::cos(a) * r, std::sin(a) * r});
    }
}

void WorldRenderer::drawBackground(sf::RenderTarget& target,
                                   const cr::Game& game, float) {
    const float radius = game.world.radius;

    drawCircle(target, {0.0f, 0.0f}, radius * 1.8f, theme().water);
    drawCircle(target, {0.0f, 0.0f}, radius + 26.0f, {36, 92, 108});
    drawCircle(target, {0.0f, 0.0f}, radius, theme().sand);
    drawCircle(target, {0.0f, 0.0f}, radius - 26.0f, theme().grass);
    drawCircle(target, {0.0f, 0.0f}, radius - 110.0f, {61, 106, 59});

    // Soft darker patches break up the large green surface.
    for (std::size_t i = 0; i < patches_.size(); ++i) {
        const float size = 90.0f + static_cast<float>(i % 7) * 26.0f;
        drawCircle(target, patches_[i], size, sf::Color(40, 78, 44, 30),
                   {1.0f, 0.62f});
    }

    // Grass tufts: one draw call for all of them.
    sf::VertexArray tufts(sf::PrimitiveType::Lines);
    tufts.resize(grass_.size() * 2);
    for (std::size_t i = 0; i < grass_.size(); ++i) {
        const sf::Vector2f base = grass_[i];
        const float height = 3.0f + static_cast<float>(i % 5);
        tufts[i * 2] = sf::Vertex{base, sf::Color(44, 82, 48, 150)};
        tufts[i * 2 + 1] = sf::Vertex{
            {base.x + ((i % 3) - 1) * 2.0f, base.y - height},
            sf::Color(84, 138, 76, 170)};
    }
    target.draw(tufts);

    sf::VertexArray flowerPoints(sf::PrimitiveType::Points);
    flowerPoints.resize(flowers_.size());
    for (std::size_t i = 0; i < flowers_.size(); ++i) {
        const sf::Color color = (i % 3 == 0) ? sf::Color(238, 212, 118)
                                             : sf::Color(226, 170, 190);
        flowerPoints[i] = sf::Vertex{flowers_[i], color};
    }
    target.draw(flowerPoints);
}

void WorldRenderer::drawBase(sf::RenderTarget& target, const cr::Game& game,
                             float time) {
    (void)game;

    drawCircle(target, {0.0f, 0.0f}, cr::kBaseSafeRadius,
               sf::Color(242, 184, 84, 9));
    drawRing(target, {0.0f, 0.0f}, cr::kBaseSafeRadius,
             sf::Color(242, 184, 84, 70), 2.0f);
    drawRing(target, {0.0f, 0.0f}, cr::kBaseRadius,
             sf::Color(242, 184, 84, 35), 1.0f);

    drawCircle(target, {0.0f, 0.0f}, 128.0f, {112, 96, 72});
    drawCircle(target, {0.0f, 0.0f}, 112.0f, {94, 84, 62});
    drawRing(target, {0.0f, 0.0f}, 128.0f, {150, 130, 96, 120}, 2.0f);

    // Tent (top-left of the camp).
    sf::ConvexShape tent;
    tent.setPointCount(3);
    tent.setPoint(0, {-122.0f, 28.0f});
    tent.setPoint(1, {-62.0f, -48.0f});
    tent.setPoint(2, {-16.0f, 28.0f});
    tent.setFillColor({92, 76, 54});
    tent.setOutlineThickness(2.0f);
    tent.setOutlineColor({54, 44, 32});
    target.draw(tent);
    sf::ConvexShape tentDoor;
    tentDoor.setPointCount(3);
    tentDoor.setPoint(0, {-92.0f, 26.0f});
    tentDoor.setPoint(1, {-62.0f, -14.0f});
    tentDoor.setPoint(2, {-36.0f, 26.0f});
    tentDoor.setFillColor({38, 32, 25});
    target.draw(tentDoor);

    // Storage crate (right side).
    sf::RectangleShape crate({48.0f, 42.0f});
    crate.setPosition({48.0f, 18.0f});
    crate.setFillColor({124, 86, 48});
    crate.setOutlineThickness(2.0f);
    crate.setOutlineColor({70, 46, 24});
    target.draw(crate);
    sf::RectangleShape crateBand({48.0f, 6.0f});
    crateBand.setPosition({48.0f, 34.0f});
    crateBand.setFillColor({78, 52, 28});
    target.draw(crateBand);

    // Campfire logs.
    sf::RectangleShape logA({44.0f, 9.0f});
    logA.setOrigin({22.0f, 4.5f});
    logA.setPosition({0.0f, 6.0f});
    logA.setRotation(sf::degrees(24.0f));
    logA.setFillColor({92, 62, 36});
    target.draw(logA);
    sf::RectangleShape logB({44.0f, 9.0f});
    logB.setOrigin({22.0f, 4.5f});
    logB.setPosition({0.0f, 6.0f});
    logB.setRotation(sf::degrees(-24.0f));
    logB.setFillColor({76, 50, 30});
    target.draw(logB);

    // Animated flame.
    const float flicker = 0.75f + 0.25f * std::sin(time * 9.0f);
    sf::ConvexShape flame;
    flame.setPointCount(3);
    flame.setPoint(0, {-13.0f, 4.0f});
    flame.setPoint(1, {0.0f, -30.0f * flicker});
    flame.setPoint(2, {13.0f, 4.0f});
    flame.setFillColor({242, 128, 44, 225});
    target.draw(flame);
    sf::ConvexShape inner;
    inner.setPointCount(3);
    inner.setPoint(0, {-6.0f, 3.0f});
    inner.setPoint(1, {0.0f, -17.0f * flicker});
    inner.setPoint(2, {6.0f, 3.0f});
    inner.setFillColor({255, 216, 110, 235});
    target.draw(inner);
}

void WorldRenderer::drawNode(sf::RenderTarget& target, const cr::Game&,
                             const cr::Node& node, float time, bool hovered) {
    const cr::NodeRule& rule = cr::nodeRule(node.type);
    const float r = rule.visualRadius;
    const float sway =
        std::sin(time * 2.0f + static_cast<float>(node.id) * 0.7f) *
        (node.shake > 0.0f ? 6.0f : 1.4f);
    const sf::Vector2f p = toSf(node.pos) + sf::Vector2f(sway, 0.0f);

    if (hovered) {
        drawRing(target, p, r + 10.0f, withAlpha(theme().accent, 210), 2.0f);
    }

    const bool tree = node.type == cr::NodeType::TreeBig ||
                      node.type == cr::NodeType::TreeNormal ||
                      node.type == cr::NodeType::TreeSmall;

    if (!node.alive) {
        // Show a stump / rubble so the player understands the node respawns.
        if (tree) {
            drawShadow(target, p + sf::Vector2f(0.0f, r * 0.5f), r * 0.7f, 45.0f);
            sf::RectangleShape stump({r * 0.42f, r * 0.42f});
            stump.setOrigin({r * 0.21f, r * 0.42f});
            stump.setPosition(p + sf::Vector2f(0.0f, r * 0.35f));
            stump.setFillColor({88, 60, 38, 190});
            target.draw(stump);
            drawCircle(target, p + sf::Vector2f(0.0f, r * 0.02f), r * 0.21f,
                       {130, 94, 58, 180}, {1.0f, 0.55f});
        } else {
            drawCircle(target, p, r * 0.45f, sf::Color(78, 74, 66, 130),
                       {1.0f, 0.55f});
        }
        return;
    }

    if (tree) {
        drawShadow(target, p + sf::Vector2f(0.0f, r * 0.75f), r * 0.95f);
        sf::RectangleShape trunk({r * 0.30f, r * 1.45f});
        trunk.setOrigin({r * 0.15f, r * 1.45f});
        trunk.setPosition(p + sf::Vector2f(0.0f, r * 0.62f));
        trunk.setFillColor({96, 66, 42});
        target.draw(trunk);

        const sf::Color foliage = treeFoliage(node.type);
        const sf::Color light = mix(foliage, {210, 235, 170}, 0.18f);
        const sf::Color dark = mix(foliage, {15, 35, 20}, 0.24f);

        drawCircle(target, p + sf::Vector2f(-r * 0.42f, -r * 0.72f),
                   r * 0.72f, dark);
        drawCircle(target, p + sf::Vector2f(r * 0.44f, -r * 0.66f),
                   r * 0.68f, dark);
        drawCircle(target, p + sf::Vector2f(0.0f, -r * 0.86f), r * 0.86f,
                   foliage);
        drawCircle(target, p + sf::Vector2f(-r * 0.15f, -r * 1.15f),
                   r * 0.52f, light);
    } else if (node.type == cr::NodeType::Stone ||
               node.type == cr::NodeType::Iron ||
               node.type == cr::NodeType::Sulfur) {
        drawShadow(target, p + sf::Vector2f(0.0f, r * 0.55f), r * 0.9f);
        sf::ConvexShape rock;
        rock.setPointCount(8);
        for (std::size_t i = 0; i < 8; ++i) {
            const float angle =
                static_cast<float>(i) / 8.0f * 2.0f * kPi;
            const float radius =
                r * (0.78f + 0.18f *
                                std::sin(static_cast<float>(i) * 2.1f +
                                         static_cast<float>(node.id)));
            rock.setPoint(i, {std::cos(angle) * radius,
                              std::sin(angle) * radius * 0.82f});
        }
        rock.setPosition(p);
        rock.setFillColor({126, 128, 132});
        rock.setOutlineThickness(2.0f);
        rock.setOutlineColor({78, 80, 86});
        target.draw(rock);

        sf::ConvexShape highlight;
        highlight.setPointCount(4);
        highlight.setPoint(0, {-r * 0.52f, -r * 0.30f});
        highlight.setPoint(1, {-r * 0.10f, -r * 0.58f});
        highlight.setPoint(2, {r * 0.28f, -r * 0.20f});
        highlight.setPoint(3, {-r * 0.12f, r * 0.04f});
        highlight.setPosition(p);
        highlight.setFillColor({176, 180, 184, 150});
        target.draw(highlight);

        const sf::Color speck = node.type == cr::NodeType::Iron
                                    ? sf::Color(190, 104, 52)
                                    : (node.type == cr::NodeType::Sulfur
                                           ? sf::Color(226, 206, 78)
                                           : sf::Color(106, 108, 112));
        const int speckCount = node.type == cr::NodeType::Stone ? 3 : 6;
        for (int i = 0; i < speckCount; ++i) {
            const float angle = static_cast<float>(i) * 2.4f +
                                static_cast<float>(node.id);
            const float radius = r * (0.18f + 0.1f * (i % 3));
            drawCircle(target,
                       p + sf::Vector2f(std::cos(angle) * radius,
                                        std::sin(angle) * radius * 0.7f),
                       r * 0.11f, speck);
        }
    } else if (node.type == cr::NodeType::Cloth) {
        drawShadow(target, p + sf::Vector2f(0.0f, r * 0.55f), r * 0.7f, 55.0f);
        for (int i = 0; i < 4; ++i) {
            const float offset = (static_cast<float>(i) - 1.5f) * r * 0.34f;
            sf::RectangleShape stem({2.5f, r * 1.15f});
            stem.setOrigin({1.25f, r * 1.15f});
            stem.setPosition(p + sf::Vector2f(offset, r * 0.45f));
            stem.setRotation(sf::degrees((static_cast<float>(i) - 1.5f) * 7.0f));
            stem.setFillColor({66, 116, 58});
            target.draw(stem);
            drawCircle(target,
                       p + sf::Vector2f(offset * 1.25f, -r * 0.68f),
                       r * 0.30f, {226, 226, 214});
        }
    } else if (node.type == cr::NodeType::Berry) {
        drawShadow(target, p + sf::Vector2f(0.0f, r * 0.55f), r * 0.8f, 55.0f);
        drawCircle(target, p + sf::Vector2f(-r * 0.32f, -r * 0.15f),
                   r * 0.62f, {52, 104, 54});
        drawCircle(target, p + sf::Vector2f(r * 0.34f, -r * 0.12f),
                   r * 0.58f, {58, 116, 60});
        drawCircle(target, p + sf::Vector2f(0.0f, -r * 0.48f), r * 0.58f,
                   {66, 128, 66});
        for (int i = 0; i < 6; ++i) {
            const float angle = static_cast<float>(i) * 1.9f + 0.4f;
            drawCircle(target,
                       p + sf::Vector2f(std::cos(angle) * r * 0.52f,
                                        std::sin(angle) * r * 0.40f -
                                            r * 0.12f),
                       r * 0.16f, {206, 58, 68});
        }
    }

    if (node.hp < node.maxHp - 0.01f) {
        drawHealthBar(target, p + sf::Vector2f(0.0f, -r * 1.85f), r * 2.1f,
                      node.hp / std::max(1.0f, node.maxHp), {220, 190, 90});
    }
}

void WorldRenderer::drawAnimal(sf::RenderTarget& target, const cr::Game&,
                               const cr::Animal& animal, float time,
                               bool hovered) {
    if (!animal.alive) {
        return;
    }

    const cr::AnimalRule& rule = cr::animalRule(animal.type);
    const float r = rule.visualRadius;
    const sf::Vector2f p = toSf(animal.pos);
    sf::Vector2f facing = toSf(animal.wanderDir);
    if (facing.x * facing.x + facing.y * facing.y < 0.01f) {
        facing = {0.0f, 1.0f};
    }
    const float length =
        std::sqrt(facing.x * facing.x + facing.y * facing.y);
    facing = facing / length;
    const sf::Vector2f side = perpendicular(facing);

    const float flash = std::clamp(animal.hurtFlash / 0.30f, 0.0f, 1.0f);
    sf::Color body = mix(animalBody(animal.type), {255, 235, 225}, flash * 0.8f);
    const sf::Color dark = mix(body, {20, 20, 20}, 0.35f);

    if (hovered) {
        drawRing(target, p, r * 1.9f, withAlpha(theme().accent, 210), 2.0f);
    }

    drawShadow(target, p + sf::Vector2f(0.0f, r * 0.9f), r * 1.25f);

    // Legs.
    for (int i = 0; i < 4; ++i) {
        const float sideSign = (i % 2 == 0) ? -1.0f : 1.0f;
        const float frontSign = (i < 2) ? 1.0f : -1.0f;
        sf::RectangleShape leg({3.0f, r * 0.85f});
        leg.setOrigin({1.5f, 0.0f});
        leg.setPosition(p + side * (sideSign * r * 0.55f) +
                        facing * (frontSign * r * 0.55f) +
                        sf::Vector2f(0.0f, r * 0.35f));
        leg.setFillColor(dark);
        target.draw(leg);
    }

    // Tail (wolf / deer / mouse).
    if (animal.type != cr::AnimalType::Bear) {
        sf::VertexArray tail(sf::PrimitiveType::Lines, 2);
        tail[0] = sf::Vertex{p - facing * (r * 1.1f), dark};
        tail[1] = sf::Vertex{p - facing * (r * 1.9f) +
                                 side * std::sin(time * 3.0f) * r * 0.3f,
                             withAlpha(dark, 200)};
        target.draw(tail);
    }

    drawCircle(target, p, r * 1.15f, body, {1.3f, 0.82f});

    const sf::Vector2f head = p + facing * (r * 1.05f);
    drawCircle(target, head, r * 0.62f, body);
    drawCircle(target, head + facing * (r * 0.22f), r * 0.17f,
               {22, 20, 18});

    if (animal.type == cr::AnimalType::Wolf ||
        animal.type == cr::AnimalType::Bear ||
        animal.type == cr::AnimalType::Mouse) {
        drawCircle(target, head + side * (r * 0.42f) - facing * (r * 0.15f),
                   r * 0.24f, dark);
        drawCircle(target, head - side * (r * 0.42f) - facing * (r * 0.15f),
                   r * 0.24f, dark);
    }

    if (animal.type == cr::AnimalType::Deer) {
        sf::VertexArray antlers(sf::PrimitiveType::Lines, 4);
        const sf::Vector2f left = head + side * (r * 0.26f);
        const sf::Vector2f right = head - side * (r * 0.26f);
        antlers[0] = sf::Vertex{left, {96, 68, 38}};
        antlers[1] = sf::Vertex{left + sf::Vector2f(0.0f, -r * 1.15f) +
                                    side * (r * 0.25f),
                                {96, 68, 38}};
        antlers[2] = sf::Vertex{right, {96, 68, 38}};
        antlers[3] = sf::Vertex{right + sf::Vector2f(0.0f, -r * 1.15f) -
                                    side * (r * 0.25f),
                                {96, 68, 38}};
        target.draw(antlers);
    }

    if (animal.aggro) {
        sf::ConvexShape marker;
        marker.setPointCount(3);
        marker.setPoint(0, {-5.0f, 0.0f});
        marker.setPoint(1, {5.0f, 0.0f});
        marker.setPoint(2, {0.0f, 9.0f});
        marker.setPosition(p + sf::Vector2f(0.0f, -r * 2.1f));
        marker.setFillColor(theme().danger);
        target.draw(marker);
    }

    if (animal.hp < animal.maxHp - 0.01f || animal.aggro) {
        drawHealthBar(target, p + sf::Vector2f(0.0f, -r * 1.75f), r * 2.5f,
                      animal.hp / std::max(1.0f, animal.maxHp),
                      animal.aggro ? theme().danger : sf::Color(220, 190, 90));
    }
}

void WorldRenderer::drawPlayer(sf::RenderTarget& target, const cr::Game& game,
                               float time) {
    const cr::PlayerState& player = game.player;
    const sf::Vector2f p = toSf(player.pos);
    sf::Vector2f facing = toSf(player.facing);
    if (facing.x * facing.x + facing.y * facing.y < 0.01f) {
        facing = {0.0f, 1.0f};
    }
    const float length =
        std::sqrt(facing.x * facing.x + facing.y * facing.y);
    facing = facing / length;
    const sf::Vector2f side = perpendicular(facing);

    const float hurt =
        std::clamp(player.hurtFlash / 0.40f, 0.0f, 1.0f) * 0.85f;
    const sf::Color jacket = mix({46, 108, 96}, {244, 92, 76}, hurt);
    const sf::Color skin = mix({224, 184, 144}, {255, 160, 150}, hurt);

    drawShadow(target, p + sf::Vector2f(0.0f, 13.0f), 18.0f, 80.0f);

    if (player.moving) {
        const float step = std::sin(player.stepPhase) * 5.0f;
        drawCircle(target, p + side * step + sf::Vector2f(0.0f, 8.0f), 5.0f,
                   {40, 48, 54});
        drawCircle(target, p - side * step + sf::Vector2f(0.0f, 8.0f), 5.0f,
                   {40, 48, 54});
    }

    drawCircle(target, p, 14.0f, jacket);
    drawCircle(target, p - facing * 5.0f, 10.0f, mix(jacket, {0, 0, 0}, 0.25f));

    // Backpack.
    sf::CircleShape backpack(7.0f);
    backpack.setOrigin({7.0f, 7.0f});
    backpack.setPosition(p - facing * 11.0f);
    backpack.setFillColor({84, 68, 46});
    target.draw(backpack);

    const sf::Vector2f head = p + facing * 5.0f;
    drawCircle(target, head, 8.0f, skin);
    drawCircle(target, head - facing * 2.0f, 6.0f, {92, 66, 42});

    // Tool / weapon.
    const bool hasTool = game.inventory.selectedTool != cr::ToolType::Count &&
                         game.hasTool(game.inventory.selectedTool);
    if (hasTool) {
        const cr::ToolType tool = game.inventory.selectedTool;
        sf::Color handle{126, 86, 48};
        sf::Color headColor{150, 152, 156};
        if (tool == cr::ToolType::IronAxe ||
            tool == cr::ToolType::IronPick) {
            headColor = {196, 200, 206};
        } else if (tool == cr::ToolType::Torch) {
            headColor = {232, 142, 58};
        }

        const float baseAngle =
            std::atan2(facing.y, facing.x) * 180.0f / kPi + 90.0f;
        float swing = 0.0f;
        if (player.attackAnim > 0.0f) {
            const float phase = 1.0f - player.attackAnim / 0.28f;
            swing = std::sin(std::clamp(phase, 0.0f, 1.0f) * kPi) * 78.0f;
        }

        sf::RectangleShape handleShape({4.0f, 26.0f});
        handleShape.setOrigin({2.0f, 25.0f});
        handleShape.setPosition(p + facing * 7.0f);
        handleShape.setRotation(sf::degrees(baseAngle + swing));
        handleShape.setFillColor(handle);
        target.draw(handleShape);

        sf::RectangleShape headShape({10.0f, 8.0f});
        headShape.setOrigin({5.0f, 27.0f});
        headShape.setPosition(p + facing * 7.0f);
        headShape.setRotation(sf::degrees(baseAngle + swing));
        headShape.setFillColor(headColor);
        target.draw(headShape);

        if (tool == cr::ToolType::Torch) {
            const float radians = (baseAngle + swing - 90.0f) * kPi / 180.0f;
            const sf::Vector2f tip =
                p + facing * 7.0f +
                sf::Vector2f(std::cos(radians), std::sin(radians)) * 30.0f;
            drawCircle(target, tip, 5.0f + std::sin(time * 12.0f) * 1.2f,
                       {255, 166, 66, 220});
        }
    } else {
        drawCircle(target, p + facing * 15.0f + side * 7.0f, 4.5f, skin);
        drawCircle(target, p + facing * 15.0f - side * 7.0f, 4.5f, skin);
    }

    // Facing indicator.
    sf::ConvexShape nose;
    nose.setPointCount(3);
    nose.setPoint(0, {-4.0f, 0.0f});
    nose.setPoint(1, {4.0f, 0.0f});
    nose.setPoint(2, {0.0f, 7.0f});
    nose.setPosition(p + facing * 17.0f);
    nose.setRotation(sf::degrees(std::atan2(facing.y, facing.x) * 180.0f /
                                    kPi -
                                90.0f));
    nose.setFillColor({238, 214, 160});
    target.draw(nose);
}

void WorldRenderer::drawNight(sf::RenderTarget& target, const cr::Game& game,
                              float) {
    const float night = nightAmount(game.timeOfDay);
    if (night <= 0.01f) {
        return;
    }

    const sf::View view = target.getView();
    const sf::Vector2f center = view.getCenter();
    const sf::Vector2f size = view.getSize();

    sf::RectangleShape overlay(size);
    overlay.setPosition(center - size * 0.5f);
    overlay.setFillColor(
        sf::Color(10, 18, 42, static_cast<std::uint8_t>(night * 190.0f)));
    target.draw(overlay);

    sf::RenderStates additive;
    additive.blendMode = sf::BlendAdd;

    const auto glow = [&](sf::Vector2f position, float radius,
                          sf::Color color) {
        for (int i = 0; i < 3; ++i) {
            const float scale = 1.0f - static_cast<float>(i) * 0.26f;
            sf::CircleShape circle(radius * scale);
            circle.setOrigin({radius * scale, radius * scale});
            circle.setPosition(position);
            circle.setFillColor(withAlpha(
                color, static_cast<int>(28.0f * night *
                                        (1.0f - static_cast<float>(i) * 0.22f))));
            target.draw(circle, additive);
        }
    };

    glow({0.0f, 0.0f}, 280.0f, {255, 154, 64});
    if (game.hasTool(cr::ToolType::Torch)) {
        glow(toSf(game.player.pos), 190.0f, {255, 184, 96});
    }
}

void WorldRenderer::drawFocus(sf::RenderTarget& target, const cr::Game& game,
                              float time, const RenderFocus& focus) {
    const float pulse = 24.0f + std::sin(time * 6.0f) * 4.0f;

    if (focus.nodeId >= 0) {
        if (const cr::Node* node = game.nodeById(focus.nodeId)) {
            if (node->alive) {
                drawRing(target, toSf(node->pos), pulse,
                         withAlpha(theme().accent, 230), 2.0f);
            }
        }
    }
    if (focus.animalId >= 0) {
        if (const cr::Animal* animal = game.animalById(focus.animalId)) {
            if (animal->alive) {
                drawRing(target, toSf(animal->pos), pulse,
                         withAlpha(theme().danger, 230), 2.0f);
            }
        }
    }
    if (focus.hasMoveTarget) {
        const sf::Vector2f p = toSf(focus.moveTarget);
        const float size = 9.0f;
        sf::VertexArray cross(sf::PrimitiveType::Lines, 4);
        cross[0] = sf::Vertex{p + sf::Vector2f(-size, 0.0f),
                              withAlpha(theme().accent, 210)};
        cross[1] = sf::Vertex{p + sf::Vector2f(size, 0.0f),
                              withAlpha(theme().accent, 210)};
        cross[2] = sf::Vertex{p + sf::Vector2f(0.0f, -size),
                              withAlpha(theme().accent, 210)};
        cross[3] = sf::Vertex{p + sf::Vector2f(0.0f, size),
                              withAlpha(theme().accent, 210)};
        target.draw(cross);
    }
}

void WorldRenderer::draw(sf::RenderTarget& target, const cr::Game& game,
                         float time, cr::Vec2 mouseWorld,
                         const RenderFocus& focus) {
    (void)mouseWorld;
    if (seed_ != game.world.seed) {
        rebuild(game);
    }

    drawBackground(target, game, time);
    drawBase(target, game, time);

    std::vector<const cr::Node*> nodes;
    nodes.reserve(game.world.nodes.size());
    for (const cr::Node& node : game.world.nodes) {
        nodes.push_back(&node);
    }
    std::sort(nodes.begin(), nodes.end(),
              [](const cr::Node* a, const cr::Node* b) {
                  return a->pos.y < b->pos.y;
              });
    for (const cr::Node* node : nodes) {
        drawNode(target, game, *node, time, node->id == focus.nodeId);
    }

    std::vector<const cr::Animal*> animals;
    animals.reserve(game.world.animals.size());
    for (const cr::Animal& animal : game.world.animals) {
        if (animal.alive) {
            animals.push_back(&animal);
        }
    }
    std::sort(animals.begin(), animals.end(),
              [](const cr::Animal* a, const cr::Animal* b) {
                  return a->pos.y < b->pos.y;
              });
    for (const cr::Animal* animal : animals) {
        drawAnimal(target, game, *animal, time, animal->id == focus.animalId);
    }

    if (!game.dead) {
        drawPlayer(target, game, time);
    }

    drawNight(target, game, time);
    drawFocus(target, game, time, focus);
}

} // namespace ui
