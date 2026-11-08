#include "../core/Game.h"

#include <limits>

namespace cr {

Vec2 Game::basePosition() {
    return {0.0f, 0.0f};
}

float Game::distanceToPlayer(Vec2 position) const {
    return distance(player.pos, position);
}

float Game::distanceToBase() const {
    return player.pos.length();
}

bool Game::nearBase() const {
    return distanceToBase() <= kBaseRadius;
}

bool Game::inBaseSafeZone(Vec2 position) const {
    return position.length() <= kBaseSafeRadius;
}

Node* Game::nodeById(int id) {
    if (id < 0 || id >= static_cast<int>(world.nodes.size())) {
        return nullptr;
    }
    return &world.nodes[static_cast<std::size_t>(id)];
}

Animal* Game::animalById(int id) {
    if (id < 0 || id >= static_cast<int>(world.animals.size())) {
        return nullptr;
    }
    return &world.animals[static_cast<std::size_t>(id)];
}

const Node* Game::nodeById(int id) const {
    if (id < 0 || id >= static_cast<int>(world.nodes.size())) {
        return nullptr;
    }
    return &world.nodes[static_cast<std::size_t>(id)];
}

const Animal* Game::animalById(int id) const {
    if (id < 0 || id >= static_cast<int>(world.animals.size())) {
        return nullptr;
    }
    return &world.animals[static_cast<std::size_t>(id)];
}

Node* Game::nearestNode(Vec2 position, float maxDistance) const {
    Node* best = nullptr;
    float bestDistance = std::numeric_limits<float>::max();

    for (const Node& node : world.nodes) {
        if (!node.alive) {
            continue;
        }
        const float nodeDistance = distance(position, node.pos);
        const float reach = maxDistance +
                            nodeRule(node.type).visualRadius * 0.35f;
        if (nodeDistance <= reach && nodeDistance < bestDistance) {
            bestDistance = nodeDistance;
            best = const_cast<Node*>(&node);
        }
    }
    return best;
}

Animal* Game::nearestAnimal(Vec2 position, float maxDistance) const {
    Animal* best = nullptr;
    float bestDistance = std::numeric_limits<float>::max();

    for (const Animal& animal : world.animals) {
        if (!animal.alive) {
            continue;
        }
        const float animalDistance = distance(position, animal.pos);
        const float reach = maxDistance +
                            animalRule(animal.type).visualRadius * 0.25f;
        if (animalDistance <= reach && animalDistance < bestDistance) {
            bestDistance = animalDistance;
            best = const_cast<Animal*>(&animal);
        }
    }
    return best;
}

Node* Game::nodeAt(Vec2 position, float radius) const {
    Node* best = nullptr;
    float bestDistance = std::numeric_limits<float>::max();

    for (const Node& node : world.nodes) {
        if (!node.alive) {
            continue;
        }
        const float reach = radius + nodeRule(node.type).visualRadius;
        const float nodeDistance = distance(position, node.pos);
        if (nodeDistance <= reach && nodeDistance < bestDistance) {
            bestDistance = nodeDistance;
            best = const_cast<Node*>(&node);
        }
    }
    return best;
}

Animal* Game::animalAt(Vec2 position, float radius) const {
    Animal* best = nullptr;
    float bestDistance = std::numeric_limits<float>::max();

    for (const Animal& animal : world.animals) {
        if (!animal.alive) {
            continue;
        }
        const float reach = radius + animalRule(animal.type).visualRadius;
        const float animalDistance = distance(position, animal.pos);
        if (animalDistance <= reach && animalDistance < bestDistance) {
            bestDistance = animalDistance;
            best = const_cast<Animal*>(&animal);
        }
    }
    return best;
}

ClickTarget Game::targetAt(Vec2 position, float radius) const {
    ClickTarget result;

    const Node* node = nodeAt(position, radius);
    const Animal* animal = animalAt(position, radius);

    if (node == nullptr && animal == nullptr) {
        return result;
    }
    if (node == nullptr) {
        result.animalId = animal->id;
        return result;
    }
    if (animal == nullptr) {
        result.nodeId = node->id;
        return result;
    }

    const float nodeDistance = distance(position, node->pos);
    const float animalDistance = distance(position, animal->pos);
    if (animalDistance <= nodeDistance) {
        result.animalId = animal->id;
    } else {
        result.nodeId = node->id;
    }
    return result;
}

} // namespace cr
