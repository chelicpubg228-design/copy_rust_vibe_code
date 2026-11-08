#pragma once

#include "GameTypes.h"

#include <array>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

namespace cr {

// ---------------------------------------------------------------------------
//  Game
//  Owns the complete simulation.  Rendering and text are completely outside;
//  this class can be tested without any window or console.
// ---------------------------------------------------------------------------

struct GatherPlan {
    bool possible = false;
    bool needsTool = false;
    ToolType tool = ToolType::Count;
    float damage = 0.0f;
    const char* failKey = "";
};

struct ClickTarget {
    int nodeId = -1;
    int animalId = -1;
};

class Game {
public:
    Game();
    Game(std::uint32_t seed, const std::string& name);

    // ---- run lifecycle ---------------------------------------------------
    void newRun(std::uint32_t seed, const std::string& name);
    void respawn();
    void update(float dt);
    static std::uint32_t makeSeed();

    // ---- world -----------------------------------------------------------
    void generateWorld(std::uint32_t seed);
    int addNode(NodeType type, Vec2 pos);
    int addAnimal(AnimalType type, Vec2 pos);

    // ---- player ----------------------------------------------------------
    void movePlayer(Vec2 direction, float dt, bool sprint);
    bool interact();
    bool gatherNode(Node& node);
    bool attackAnimal(Animal& animal);
    GatherPlan planGather(const Node& node) const;

    Node* nodeById(int id);
    Animal* animalById(int id);
    const Node* nodeById(int id) const;
    const Animal* animalById(int id) const;
    Node* nearestNode(Vec2 position, float maxDistance) const;
    Animal* nearestAnimal(Vec2 position, float maxDistance) const;
    Node* nodeAt(Vec2 position, float radius) const;
    Animal* animalAt(Vec2 position, float radius) const;
    ClickTarget targetAt(Vec2 position, float radius) const;
    float distanceToPlayer(Vec2 position) const;
    float distanceToBase() const;
    bool nearBase() const;
    bool inBaseSafeZone(Vec2 position) const;
    static Vec2 basePosition();

    // ---- crafting / inventory -------------------------------------------
    const std::vector<Recipe>& recipes() const;
    bool canCraft(const Recipe& recipe, ResourceType* missing = nullptr,
                  int* missingAmount = nullptr) const;
    bool craft(int recipeIndex);
    bool equipArmor(ArmorType type);
    bool unequipArmor(ArmorType type);
    bool eatBerry();
    bool restAtBase();

    bool hasTool(ToolType type) const;
    int toolCount(ToolType type) const;
    float toolDurability(ToolType type) const;
    bool selectTool(ToolType type);
    void cycleTool(int direction);
    float selectedWeaponDamage() const;
    float armorDefense() const;

    int resource(ResourceType type) const;
    void addResource(ResourceType type, int amount);
    void giveTool(ToolType type, float durability = -1.0f);

    // ---- health ----------------------------------------------------------
    void damagePlayer(float rawDamage);
    void healPlayer(float amount);
    void recalcMaxHp();

    // ---- persistence -----------------------------------------------------
    bool saveToFile(const std::string& path);
    bool loadFromFile(const std::string& path);

    // ---- events ----------------------------------------------------------
    void clearEvents();
    GameEvent& emit(const std::string& key);

    // ---- state (readable by both front-ends) -----------------------------
    PlayerState player;
    WorldState world;
    InventoryState inventory;
    RunStats stats;
    Settings settings;

    std::string playerName;
    std::string lastError;
    std::vector<GameEvent> events;

    bool dead = false;
    int day = 1;
    int totalDeaths = 0;
    float timeOfDay = 0.28f;
    float playTime = 0.0f;
    float saveCooldown = 0.0f;

private:
    void resetPlayerState();
    void die();
    void wearTool(ToolType type, float amount = 1.0f);
    void wearArmor(float rawDamage);
    void breakArmor(ArmorType type);
    void updateAnimals(float dt);
    void updateNodes(float dt);
    void updateStats(float dt);
    void respawnAnimal(Animal& animal);
    void respawnNode(Node& node);
    void placeAnimal(AnimalType type, float minRadius, float maxRadius,
                     std::mt19937& rng);
    int placeNode(NodeType type, float minRadius, float maxRadius,
                  std::mt19937& rng, float minSeparation);
    float randomRange(float low, float high);
    int randomInt(int low, int high);

    std::mt19937 rng_;
    std::array<float, 3> lowWarning_{};
    float lastHurtSource_ = 0.0f;
};

// Small utility used by both front-ends.
std::string formatNumber(float value, int decimals = 0);

} // namespace cr
