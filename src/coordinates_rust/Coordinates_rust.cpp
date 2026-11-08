#include "../core/Game.h"

#include <algorithm>
#include <cmath>
#include <random>

namespace cr {

namespace {

constexpr float kPi = 3.14159265358979323846f;

Vec2 randomRingPoint(std::mt19937& rng, float minRadius, float maxRadius) {
    std::uniform_real_distribution<float> angleDistribution(0.0f, 2.0f * kPi);
    std::uniform_real_distribution<float> unit(0.0f, 1.0f);

    const float angle = angleDistribution(rng);
    const float radius = minRadius + (maxRadius - minRadius) * std::sqrt(unit(rng));
    return {std::cos(angle) * radius, std::sin(angle) * radius};
}

} // namespace

int Game::addNode(NodeType type, Vec2 position) {
    const NodeRule& rule = nodeRule(type);
    Node node;
    node.id = static_cast<int>(world.nodes.size());
    node.type = type;
    node.pos = position;
    node.hp = rule.maxHp;
    node.maxHp = rule.maxHp;
    node.alive = true;
    node.respawnTimer = 0.0f;
    node.shake = 0.0f;
    world.nodes.push_back(node);
    return node.id;
}

int Game::addAnimal(AnimalType type, Vec2 position) {
    const AnimalRule& rule = animalRule(type);
    Animal animal;
    animal.id = static_cast<int>(world.animals.size());
    animal.type = type;
    animal.pos = position;
    animal.hp = rule.maxHp;
    animal.maxHp = rule.maxHp;
    animal.alive = true;
    animal.aggro = false;
    animal.wanderTimer = randomRange(0.5f, 3.0f);
    animal.attackTimer = 0.0f;
    animal.hurtFlash = 0.0f;
    animal.wanderDir = Vec2{randomRange(-1.0f, 1.0f),
                            randomRange(-1.0f, 1.0f)}
                           .normalized();
    world.animals.push_back(animal);
    return animal.id;
}

int Game::placeNode(NodeType type, float minRadius, float maxRadius,
                    std::mt19937& rng, float minSeparation) {
    const float worldLimit = world.radius - 70.0f;

    for (int attempt = 0; attempt < 550; ++attempt) {
        Vec2 position = randomRingPoint(rng, minRadius, maxRadius);
        if (position.length() > worldLimit) {
            continue;
        }
        if (inBaseSafeZone(position)) {
            continue;
        }

        // After many attempts relax the separation requirement a little so a
        // world always gets its full amount of resources.
        const float separation = attempt > 360 ? minSeparation * 0.18f
                                                : minSeparation;
        bool tooClose = false;
        for (const Node& node : world.nodes) {
            if (distance(node.pos, position) < separation) {
                tooClose = true;
                break;
            }
        }
        if (!tooClose) {
            return addNode(type, position);
        }
    }

    // Last resort: a position that is at least not inside the base.
    for (int attempt = 0; attempt < 200; ++attempt) {
        Vec2 position = randomRingPoint(rng, minRadius, maxRadius);
        if (!inBaseSafeZone(position) && position.length() < worldLimit) {
            return addNode(type, position);
        }
    }
    return -1;
}

void Game::placeAnimal(AnimalType type, float minRadius, float maxRadius,
                       std::mt19937& rng) {
    const float worldLimit = world.radius - 90.0f;

    for (int attempt = 0; attempt < 400; ++attempt) {
        Vec2 position = randomRingPoint(rng, minRadius, maxRadius);
        if (position.length() > worldLimit || inBaseSafeZone(position)) {
            continue;
        }

        bool tooClose = false;
        for (const Animal& animal : world.animals) {
            if (distance(animal.pos, position) < 95.0f) {
                tooClose = true;
                break;
            }
        }
        if (!tooClose) {
            addAnimal(type, position);
            return;
        }
    }

    for (int attempt = 0; attempt < 120; ++attempt) {
        Vec2 position = randomRingPoint(rng, minRadius, maxRadius);
        if (!inBaseSafeZone(position) && position.length() < worldLimit) {
            addAnimal(type, position);
            return;
        }
    }
}

void Game::generateWorld(std::uint32_t seed) {
    world = {};
    world.seed = seed;
    world.radius = kWorldRadius;
    world.nodes.reserve(320);
    world.animals.reserve(64);

    std::mt19937 rng(seed);

    // ---------------------------------------------------------------------
    // A friendly starting area: a few trees, stones, berries and cloth near
    // the camp so the first minutes are not a blind walk through the world.
    // ---------------------------------------------------------------------
    placeNode(NodeType::TreeSmall, 185.0f, 290.0f, rng, 62.0f);
    placeNode(NodeType::TreeSmall, 250.0f, 370.0f, rng, 62.0f);
    placeNode(NodeType::TreeNormal, 280.0f, 420.0f, rng, 68.0f);
    placeNode(NodeType::TreeNormal, 330.0f, 470.0f, rng, 68.0f);
    placeNode(NodeType::TreeBig, 390.0f, 520.0f, rng, 76.0f);
    placeNode(NodeType::Stone, 260.0f, 380.0f, rng, 58.0f);
    placeNode(NodeType::Stone, 330.0f, 500.0f, rng, 58.0f);
    placeNode(NodeType::Berry, 200.0f, 330.0f, rng, 55.0f);
    placeNode(NodeType::Berry, 280.0f, 420.0f, rng, 55.0f);
    placeNode(NodeType::Cloth, 240.0f, 380.0f, rng, 55.0f);

    // A small early source of iron and sulfur, still within a short walk.
    placeNode(NodeType::Iron, 430.0f, 560.0f, rng, 72.0f);
    placeNode(NodeType::Sulfur, 470.0f, 620.0f, rng, 72.0f);

    // ---------------------------------------------------------------------
    // The rest of the island.
    // ---------------------------------------------------------------------
    struct NodeSpawn {
        NodeType type;
        int count;
        float minRadius;
        float maxRadius;
        float separation;
    };

    const NodeSpawn nodeSpawns[] = {
        {NodeType::TreeBig, 17, 450.0f, world.radius - 120.0f, 100.0f},
        {NodeType::TreeNormal, 33, 300.0f, world.radius - 120.0f, 80.0f},
        {NodeType::TreeSmall, 47, 220.0f, world.radius - 120.0f, 66.0f},
        {NodeType::Stone, 46, 300.0f, world.radius - 120.0f, 70.0f},
        {NodeType::Iron, 28, 500.0f, world.radius - 140.0f, 90.0f},
        {NodeType::Sulfur, 16, 600.0f, world.radius - 140.0f, 96.0f},
        {NodeType::Cloth, 24, 250.0f, world.radius - 120.0f, 64.0f},
        {NodeType::Berry, 31, 220.0f, world.radius - 120.0f, 60.0f},
    };

    for (const NodeSpawn& spawn : nodeSpawns) {
        for (int i = 0; i < spawn.count; ++i) {
            placeNode(spawn.type, spawn.minRadius, spawn.maxRadius, rng,
                      spawn.separation);
        }
    }

    // Animals: harmless ones close to the camp, predators farther away.
    for (int i = 0; i < 12; ++i) {
        placeAnimal(AnimalType::Mouse, 280.0f, world.radius - 120.0f, rng);
    }
    for (int i = 0; i < 16; ++i) {
        placeAnimal(AnimalType::Deer, 350.0f, world.radius - 120.0f, rng);
    }
    for (int i = 0; i < 9; ++i) {
        placeAnimal(AnimalType::Wolf, 520.0f, world.radius - 150.0f, rng);
    }
    for (int i = 0; i < 5; ++i) {
        placeAnimal(AnimalType::Bear, 700.0f, world.radius - 180.0f, rng);
    }
}

void Game::respawnNode(Node& node) {
    const float worldLimit = world.radius - 90.0f;
    Vec2 position = node.pos;

    for (int attempt = 0; attempt < 180; ++attempt) {
        position = randomRingPoint(rng_, 260.0f, worldLimit);
        if (inBaseSafeZone(position) ||
            distance(position, player.pos) < 180.0f) {
            continue;
        }

        bool tooClose = false;
        for (const Node& other : world.nodes) {
            if (&other == &node) {
                continue;
            }
            if (other.alive && distance(other.pos, position) < 45.0f) {
                tooClose = true;
                break;
            }
        }
        if (!tooClose) {
            break;
        }
    }

    const NodeRule& rule = nodeRule(node.type);
    node.pos = position;
    node.hp = rule.maxHp;
    node.maxHp = rule.maxHp;
    node.alive = true;
    node.respawnTimer = 0.0f;
    node.shake = 0.0f;
}

void Game::respawnAnimal(Animal& animal) {
    const float worldLimit = world.radius - 100.0f;
    Vec2 position = animal.pos;

    for (int attempt = 0; attempt < 180; ++attempt) {
        position = randomRingPoint(rng_, 480.0f, worldLimit);
        if (inBaseSafeZone(position) ||
            distance(position, player.pos) < 420.0f) {
            continue;
        }
        bool tooClose = false;
        for (const Animal& other : world.animals) {
            if (&other == &animal) {
                continue;
            }
            if (other.alive && distance(other.pos, position) < 90.0f) {
                tooClose = true;
                break;
            }
        }
        if (!tooClose) {
            break;
        }
    }

    const AnimalRule& rule = animalRule(animal.type);
    animal.pos = position;
    animal.hp = rule.maxHp;
    animal.maxHp = rule.maxHp;
    animal.alive = true;
    animal.aggro = false;
    animal.respawnTimer = 0.0f;
    animal.attackTimer = 0.0f;
    animal.hurtFlash = 0.0f;
    animal.wanderTimer = randomRange(0.5f, 3.0f);
    animal.wanderDir = Vec2{randomRange(-1.0f, 1.0f),
                            randomRange(-1.0f, 1.0f)}
                           .normalized();
}

} // namespace cr
