#include "Game.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <limits>
#include <random>
#include <sstream>
#include <system_error>

namespace cr {

// ---------------------------------------------------------------------------
//  Tuning constants.  Keeping them here makes the simulation easy to test and
//  easy to tweak without touching the front-ends.
// ---------------------------------------------------------------------------

namespace {

constexpr float kFoodDrainPerSecond        = 0.055f;
constexpr float kWaterDrainPerSecond       = 0.085f;
constexpr float kEnergyDrainPerSecond      = 0.018f;
constexpr float kWalkEnergyDrainPerSecond  = 0.22f;
constexpr float kSprintEnergyDrainPerSecond = 7.5f;
constexpr float kRegenHpPerSecond          = 1.6f;
constexpr float kRegenFoodCostPerSecond    = 0.15f;
constexpr float kRegenWaterCostPerSecond   = 0.15f;
constexpr float kStarvationDamagePerSecond = 2.2f;
constexpr float kDehydrationDamagePerSecond = 3.2f;
constexpr float kArmorFlatReduction        = 0.60f;
constexpr float kArmorWearPerDamage        = 0.20f;
constexpr float kLowStatThreshold          = 25.0f;
constexpr float kLowStatResetThreshold     = 40.0f;
constexpr float kFistDamage                = 5.0f;

NodeType nodeFromId(const std::string& id) {
    for (int i = 0; i < kNodeCount; ++i) {
        const NodeType type = static_cast<NodeType>(i);
        const std::string full = nodeKey(type);
        if (id == full) {
            return type;
        }
        // Accept both "node.tree_big" and "tree_big".
        const std::string shortId = full.rfind("node.", 0) == 0
                                        ? full.substr(5)
                                        : full;
        if (id == shortId) {
            return type;
        }
    }
    return NodeType::TreeSmall;
}

AnimalType animalFromId(const std::string& id) {
    for (int i = 0; i < kAnimalCount; ++i) {
        const AnimalType type = static_cast<AnimalType>(i);
        const std::string full = animalKey(type);
        if (id == full) {
            return type;
        }
        const std::string shortId = full.rfind("animal.", 0) == 0
                                        ? full.substr(7)
                                        : full;
        if (id == shortId) {
            return type;
        }
    }
    return AnimalType::Mouse;
}

const char* languageId(Language language) {
    return language == Language::English ? "en" : "ru";
}

Language languageFromId(const std::string& id) {
    return id == "en" || id == "english" ? Language::English
                                         : Language::Russian;
}

nlohmann::json pointToJson(Vec2 point) {
    return nlohmann::json{{"x", point.x}, {"y", point.y}};
}

Vec2 pointFromJson(const nlohmann::json& value, Vec2 fallback) {
    if (!value.is_object()) {
        return fallback;
    }
    return {value.value("x", fallback.x), value.value("y", fallback.y)};
}

} // namespace

// ---------------------------------------------------------------------------
//  Lifecycle
// ---------------------------------------------------------------------------

Game::Game() : Game(makeSeed(), "Player") {}

Game::Game(std::uint32_t seed, const std::string& name) {
    newRun(seed, name);
}

std::uint32_t Game::makeSeed() {
    std::random_device device;
    const auto now = static_cast<std::uint64_t>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uint64_t value = (static_cast<std::uint64_t>(device()) << 32) ^ now;
    value ^= value >> 33;
    value *= 0xff51afd7ed558ccdULL;
    value ^= value >> 33;
    return static_cast<std::uint32_t>(value);
}

void Game::newRun(std::uint32_t seed, const std::string& name) {
    events.clear();
    playerName = name.empty() ? "Player" : name;
    dead = false;
    day = 1;
    timeOfDay = 0.28f;
    playTime = 0.0f;
    saveCooldown = 0.0f;
    totalDeaths = 0;
    lastError.clear();
    stats = {};
    inventory = {};
    lowWarning_.fill(0.0f);
    lastHurtSource_ = 0.0f;

    resetPlayerState();
    rng_.seed(seed ^ 0x9e3779b9u);

    generateWorld(seed);

    // A small starter kit: enough to gather the first resources without a
    // frustrating "no tool" dead end.
    giveTool(ToolType::StoneAxe);
    giveTool(ToolType::Torch);
    inventory.selectedTool = ToolType::StoneAxe;

    recalcMaxHp();
}

void Game::respawn() {
    if (!dead) {
        return;
    }

    // Death costs progress and time, but the island itself stays the same.
    // Regenerating the whole world made death feel like an unrelated new run.
    player.pos = {0.0f, 92.0f};
    player.facing = {0.0f, -1.0f};
    player.interactCooldown = 0.0f;
    player.attackCooldown = 0.0f;
    player.restCooldown = 0.0f;
    player.attackAnim = 0.0f;
    player.hurtFlash = 0.0f;
    player.stepPhase = 0.0f;
    player.moving = false;
    player.sprinting = false;

    recalcMaxHp();
    player.hp = player.maxHp;
    player.food = kMaxStat;
    player.water = kMaxStat;
    player.energy = kMaxStat;
    lowWarning_.fill(0.0f);
    lastHurtSource_ = 0.0f;
    dead = false;
    emit("respawned");
}

void Game::resetPlayerState() {
    player = {};
    player.pos = {0.0f, 92.0f};
    player.facing = {0.0f, -1.0f};
    player.hp = 100.0f;
    player.maxHp = 100.0f;
    player.food = kMaxStat;
    player.water = kMaxStat;
    player.energy = kMaxStat;
}

void Game::die() {
    if (dead) {
        return;
    }
    dead = true;
    player.hp = 0.0f;
    ++totalDeaths;
    emit("player.died");
}

// ---------------------------------------------------------------------------
//  Main update
// ---------------------------------------------------------------------------

void Game::update(float dt) {
    if (dead) {
        player.attackAnim = std::max(0.0f, player.attackAnim - dt);
        player.hurtFlash = std::max(0.0f, player.hurtFlash - dt);
        return;
    }

    dt = clampf(dt, 0.0f, 0.25f);
    if (dt <= 0.0f) {
        return;
    }

    playTime += dt;
    player.interactCooldown = std::max(0.0f, player.interactCooldown - dt);
    player.attackCooldown = std::max(0.0f, player.attackCooldown - dt);
    player.restCooldown = std::max(0.0f, player.restCooldown - dt);
    player.attackAnim = std::max(0.0f, player.attackAnim - dt);
    player.hurtFlash = std::max(0.0f, player.hurtFlash - dt);
    saveCooldown = std::max(0.0f, saveCooldown - dt);

    timeOfDay += dt / kDayLengthSeconds;
    if (timeOfDay >= 1.0f) {
        timeOfDay -= 1.0f;
        ++day;
        GameEvent& event = emit("new_day");
        event.amount = day;
    }

    updateStats(dt);
    if (dead) {
        return;
    }
    updateAnimals(dt);
    if (dead) {
        return;
    }
    updateNodes(dt);
}

void Game::movePlayer(Vec2 direction, float dt, bool sprint) {
    if (dead) {
        return;
    }

    const float length = direction.length();
    if (length < 0.0001f) {
        player.moving = false;
        player.sprinting = false;
        return;
    }

    direction = direction / length;
    player.facing = direction;
    player.moving = true;

    const bool canSprint = sprint && player.energy > 0.5f;
    float speed = canSprint ? kPlayerSprintSpeed : kPlayerSpeed;
    if (player.energy <= 0.0f) {
        speed *= 0.62f;
    }

    player.sprinting = canSprint;
    player.pos += direction * speed * dt;
    player.stepPhase += speed * dt * 0.055f;

    const float limit = world.radius - 34.0f;
    const float radius = player.pos.length();
    if (radius > limit) {
        player.pos = player.pos.normalized() * limit;
    }

    const float drain = canSprint ? kSprintEnergyDrainPerSecond
                                  : kWalkEnergyDrainPerSecond;
    player.energy = clampf(player.energy - drain * dt, 0.0f, kMaxStat);
}

// ---------------------------------------------------------------------------
//  Interaction
// ---------------------------------------------------------------------------

bool Game::interact() {
    if (dead || player.interactCooldown > 0.0f) {
        return false;
    }

    Animal* animal = nearestAnimal(player.pos, kAttackRange);
    Node* node = nearestNode(player.pos, kInteractRange);

    float animalDistance = std::numeric_limits<float>::max();
    float nodeDistance = std::numeric_limits<float>::max();
    if (animal != nullptr) {
        animalDistance = distanceToPlayer(animal->pos);
    }
    if (node != nullptr) {
        nodeDistance = distanceToPlayer(node->pos) -
                       nodeRule(node->type).visualRadius * 0.35f;
    }

    if (animal != nullptr && (node == nullptr || animalDistance <= nodeDistance)) {
        return attackAnimal(*animal);
    }
    if (node != nullptr) {
        return gatherNode(*node);
    }

    player.interactCooldown = 0.30f;
    emit("nothing_near");
    return false;
}

GatherPlan Game::planGather(const Node& node) const {
    GatherPlan plan;
    if (!node.alive) {
        plan.failKey = "gather.depleted";
        return plan;
    }

    const NodeRule& rule = nodeRule(node.type);
    if (rule.hands) {
        plan.possible = true;
        plan.damage = rule.handDamage;
        plan.tool = ToolType::Count;
        return plan;
    }

    for (int i = 0; i < rule.toolCount; ++i) {
        const ToolType tool = rule.tools[static_cast<std::size_t>(i)];
        if (tool == ToolType::Count) {
            continue;
        }
        if (hasTool(tool)) {
            plan.possible = true;
            plan.tool = tool;
            plan.damage = toolRule(tool).damage;
            return plan;
        }
    }

    plan.possible = false;
    plan.needsTool = true;
    plan.failKey = "gather.need_tool";
    for (int i = 0; i < rule.toolCount; ++i) {
        if (rule.tools[static_cast<std::size_t>(i)] != ToolType::Count) {
            plan.tool = rule.tools[static_cast<std::size_t>(i)];
            break;
        }
    }
    return plan;
}

bool Game::gatherNode(Node& node) {
    if (dead || player.interactCooldown > 0.0f) {
        return false;
    }

    const GatherPlan plan = planGather(node);
    const NodeRule& rule = nodeRule(node.type);

    if (!plan.possible) {
        player.interactCooldown = 0.35f;
        GameEvent& event = emit(plan.failKey);
        event.node = node.type;
        event.tool = plan.tool;
        event.resource = rule.resource;
        event.pos = node.pos;
        return false;
    }

    player.interactCooldown = 0.35f;
    player.attackAnim = 0.22f;
    node.shake = 0.40f;

    const int amount = std::max(1, rule.yieldPerHit);
    addResource(rule.resource, amount);
    stats.gathered += amount;
    node.hp -= plan.damage;

    GameEvent& hit = emit("node.hit");
    hit.node = node.type;
    hit.resource = rule.resource;
    hit.amount = amount;
    hit.pos = node.pos;

    if (plan.tool != ToolType::Count) {
        wearTool(plan.tool, 1.0f);
    }

    if (node.hp <= 0.0f) {
        node.hp = 0.0f;
        node.alive = false;
        node.respawnTimer = rule.respawnSeconds;
        GameEvent& depleted = emit("node.depleted");
        depleted.node = node.type;
        depleted.resource = rule.resource;
        depleted.pos = node.pos;
    }

    return true;
}

bool Game::attackAnimal(Animal& animal) {
    if (dead || player.interactCooldown > 0.0f) {
        return false;
    }

    const AnimalRule& rule = animalRule(animal.type);
    if (distanceToPlayer(animal.pos) > kAttackRange + rule.visualRadius) {
        player.interactCooldown = 0.20f;
        GameEvent& event = emit("animal.too_far");
        event.animal = animal.type;
        event.pos = animal.pos;
        return false;
    }

    player.interactCooldown = 0.35f;
    player.attackCooldown = 0.28f;
    player.attackAnim = 0.28f;
    player.facing = (animal.pos - player.pos).normalized();

    const float damage = selectedWeaponDamage();
    animal.hp -= damage;
    animal.hurtFlash = 0.30f;
    if (rule.aggressive) {
        animal.aggro = true;
    }

    GameEvent& hit = emit("animal.hit");
    hit.animal = animal.type;
    hit.amount = static_cast<int>(std::round(damage));
    hit.pos = animal.pos;

    if (inventory.selectedTool != ToolType::Count &&
        hasTool(inventory.selectedTool)) {
        wearTool(inventory.selectedTool, 1.0f);
    }

    if (animal.hp <= 0.0f) {
        animal.hp = 0.0f;
        animal.alive = false;
        animal.aggro = false;
        animal.respawnTimer = 55.0f + randomRange(0.0f, 35.0f);
        ++stats.kills;

        GameEvent& killed = emit("animal.killed");
        killed.animal = animal.type;
        killed.pos = animal.pos;

        for (int i = 0; i < rule.dropCount; ++i) {
            const DropRule& drop = rule.drops[static_cast<std::size_t>(i)];
            const int amount = randomInt(drop.minAmount, drop.maxAmount);
            if (amount <= 0) {
                continue;
            }
            addResource(drop.resource, amount);
            killed.loot.push_back(ResourceAmount{drop.resource, amount});
        }
    }

    return true;
}

// ---------------------------------------------------------------------------
//  Inventory / crafting
// ---------------------------------------------------------------------------

const std::vector<Recipe>& Game::recipes() const {
    return recipeCatalog();
}

bool Game::canCraft(const Recipe& recipe, ResourceType* missing,
                    int* missingAmount) const {
    for (const Ingredient& ingredient : recipe.cost) {
        const int available = resource(ingredient.resource);
        if (available < ingredient.amount) {
            if (missing != nullptr) {
                *missing = ingredient.resource;
            }
            if (missingAmount != nullptr) {
                *missingAmount = ingredient.amount - available;
            }
            return false;
        }
    }
    return true;
}

bool Game::craft(int recipeIndex) {
    const std::vector<Recipe>& catalog = recipes();
    if (recipeIndex < 0 || recipeIndex >= static_cast<int>(catalog.size())) {
        emit("craft.unknown");
        return false;
    }

    const Recipe& recipe = catalog[static_cast<std::size_t>(recipeIndex)];
    ResourceType missing = ResourceType::Wood;
    int missingAmount = 0;
    if (!canCraft(recipe, &missing, &missingAmount)) {
        GameEvent& event = emit("craft.missing");
        event.recipe = recipeIndex;
        event.resource = missing;
        event.amount = missingAmount;
        return false;
    }

    for (const Ingredient& ingredient : recipe.cost) {
        addResource(ingredient.resource, -ingredient.amount);
    }

    switch (recipe.kind) {
    case RecipeKind::Tool:
        giveTool(recipe.tool);
        break;
    case RecipeKind::Armor:
        inventory.armorItems[idx(recipe.armor)] += 1;
        if (inventory.armorDurability[idx(recipe.armor)] <= 0.0f) {
            inventory.armorDurability[idx(recipe.armor)] =
                armorRule(recipe.armor).maxDurability;
        }
        break;
    case RecipeKind::Resource:
        addResource(recipe.resource, recipe.amount);
        break;
    }

    ++stats.crafted;
    GameEvent& event = emit("craft.ok");
    event.recipe = recipeIndex;
    event.tool = recipe.tool;
    event.armor = recipe.armor;
    event.resource = recipe.resource;
    event.amount = recipe.amount;
    return true;
}

bool Game::equipArmor(ArmorType type) {
    const int index = idx(type);
    if (index < 0 || index >= kArmorCount) {
        return false;
    }
    if (inventory.armorWorn[static_cast<std::size_t>(index)]) {
        emit("armor.already");
        return false;
    }
    if (inventory.armorItems[static_cast<std::size_t>(index)] <= 0) {
        emit("armor.none");
        return false;
    }

    if (inventory.armorDurability[static_cast<std::size_t>(index)] <= 0.0f) {
        inventory.armorDurability[static_cast<std::size_t>(index)] =
            armorRule(type).maxDurability;
    }

    inventory.armorWorn[static_cast<std::size_t>(index)] = true;
    player.hp += armorRule(type).hpBonus;
    recalcMaxHp();

    GameEvent& event = emit("armor.equipped");
    event.armor = type;
    return true;
}

bool Game::unequipArmor(ArmorType type) {
    const int index = idx(type);
    if (index < 0 || index >= kArmorCount) {
        return false;
    }
    if (!inventory.armorWorn[static_cast<std::size_t>(index)]) {
        emit("armor.not_worn");
        return false;
    }

    inventory.armorWorn[static_cast<std::size_t>(index)] = false;
    recalcMaxHp();

    GameEvent& event = emit("armor.unequipped");
    event.armor = type;
    return true;
}

bool Game::eatBerry() {
    if (resource(ResourceType::Berry) <= 0) {
        emit("eat.none");
        return false;
    }

    addResource(ResourceType::Berry, -1);
    player.food = clampf(player.food + 18.0f, 0.0f, kMaxStat);
    player.water = clampf(player.water + 4.0f, 0.0f, kMaxStat);
    player.hp = clampf(player.hp + 2.0f, 0.0f, player.maxHp);
    player.energy = clampf(player.energy + 1.0f, 0.0f, kMaxStat);
    emit("ate.berry");
    return true;
}

bool Game::restAtBase() {
    if (dead) {
        return false;
    }
    if (!nearBase()) {
        emit("rest.need_base");
        return false;
    }
    if (player.energy >= 99.5f) {
        emit("rest.full");
        return false;
    }

    const float before = player.energy;
    player.energy = clampf(player.energy + 45.0f, 0.0f, kMaxStat);
    player.food = clampf(player.food - 2.0f, 0.0f, kMaxStat);
    player.water = clampf(player.water - 3.0f, 0.0f, kMaxStat);
    player.hp = clampf(player.hp + 5.0f, 0.0f, player.maxHp);
    player.restCooldown = 1.0f;

    timeOfDay += 0.035f;
    if (timeOfDay >= 1.0f) {
        timeOfDay -= 1.0f;
        ++day;
    }

    GameEvent& event = emit("rest.ok");
    event.amount = static_cast<int>(std::round(player.energy - before));
    return true;
}

bool Game::hasTool(ToolType type) const {
    const int index = idx(type);
    if (index < 0 || index >= kToolCount) {
        return false;
    }
    return !inventory.tools[static_cast<std::size_t>(index)].empty();
}

int Game::toolCount(ToolType type) const {
    const int index = idx(type);
    if (index < 0 || index >= kToolCount) {
        return 0;
    }
    return static_cast<int>(
        inventory.tools[static_cast<std::size_t>(index)].size());
}

float Game::toolDurability(ToolType type) const {
    const int index = idx(type);
    if (index < 0 || index >= kToolCount) {
        return 0.0f;
    }
    float best = 0.0f;
    for (float value : inventory.tools[static_cast<std::size_t>(index)]) {
        best = std::max(best, value);
    }
    return best;
}

bool Game::selectTool(ToolType type) {
    if (type == ToolType::Count) {
        inventory.selectedTool = ToolType::Count;
        emit("tool.selected");
        return true;
    }
    if (!hasTool(type)) {
        GameEvent& event = emit("tool.not_owned");
        event.tool = type;
        return false;
    }
    inventory.selectedTool = type;
    GameEvent& event = emit("tool.selected");
    event.tool = type;
    return true;
}

void Game::cycleTool(int direction) {
    std::vector<ToolType> owned;
    for (int i = 0; i < kToolCount; ++i) {
        const ToolType type = static_cast<ToolType>(i);
        if (hasTool(type)) {
            owned.push_back(type);
        }
    }
    if (owned.empty()) {
        inventory.selectedTool = ToolType::Count;
        return;
    }

    int current = -1;
    for (int i = 0; i < static_cast<int>(owned.size()); ++i) {
        if (owned[static_cast<std::size_t>(i)] == inventory.selectedTool) {
            current = i;
            break;
        }
    }

    const int step = direction >= 0 ? 1 : -1;
    const int count = static_cast<int>(owned.size());
    const int next = current < 0 ? 0 : (current + step + count) % count;
    inventory.selectedTool = owned[static_cast<std::size_t>(next)];

    GameEvent& event = emit("tool.selected");
    event.tool = inventory.selectedTool;
}

float Game::selectedWeaponDamage() const {
    if (inventory.selectedTool == ToolType::Count ||
        !hasTool(inventory.selectedTool)) {
        return kFistDamage;
    }
    return toolRule(inventory.selectedTool).damage;
}

float Game::armorDefense() const {
    float total = 0.0f;
    for (int i = 0; i < kArmorCount; ++i) {
        if (inventory.armorWorn[static_cast<std::size_t>(i)] &&
            inventory.armorDurability[static_cast<std::size_t>(i)] > 0.0f) {
            total += armorRule(static_cast<ArmorType>(i)).defense;
        }
    }
    return total;
}

int Game::resource(ResourceType type) const {
    const int index = idx(type);
    if (index < 0 || index >= kResourceCount) {
        return 0;
    }
    return inventory.resources[static_cast<std::size_t>(index)];
}

void Game::addResource(ResourceType type, int amount) {
    const int index = idx(type);
    if (index < 0 || index >= kResourceCount) {
        return;
    }
    int& value = inventory.resources[static_cast<std::size_t>(index)];
    value = clampi(value + amount, 0, kMaxStack);
}

void Game::giveTool(ToolType type, float durability) {
    const int index = idx(type);
    if (index < 0 || index >= kToolCount) {
        return;
    }
    const float value = durability < 0.0f ? toolRule(type).maxDurability
                                          : clampf(durability, 0.1f,
                                                   toolRule(type).maxDurability);
    inventory.tools[static_cast<std::size_t>(index)].push_back(value);
    if (inventory.selectedTool == ToolType::Count) {
        inventory.selectedTool = type;
    }
}

void Game::wearTool(ToolType type, float amount) {
    const int index = idx(type);
    if (index < 0 || index >= kToolCount) {
        return;
    }
    std::vector<float>& tools = inventory.tools[static_cast<std::size_t>(index)];
    if (tools.empty()) {
        return;
    }

    // Wear the strongest copy first so the best tool is not silently lost
    // while a nearly broken duplicate sits in the bag.
    auto best = std::max_element(tools.begin(), tools.end());
    *best -= std::max(0.0f, amount);
    if (*best > 0.0f) {
        return;
    }

    tools.erase(best);
    GameEvent& event = emit("tool.broken");
    event.tool = type;

    if (inventory.selectedTool == type && tools.empty()) {
        inventory.selectedTool = ToolType::Count;
        for (int i = 0; i < kToolCount; ++i) {
            if (hasTool(static_cast<ToolType>(i))) {
                inventory.selectedTool = static_cast<ToolType>(i);
                break;
            }
        }
    }
}

// ---------------------------------------------------------------------------
//  Health
// ---------------------------------------------------------------------------

void Game::damagePlayer(float rawDamage) {
    if (dead || rawDamage <= 0.0f) {
        return;
    }

    const float defense = armorDefense();
    float reduced = rawDamage - defense * kArmorFlatReduction;
    // Armor should never make a hit completely harmless.
    reduced = std::max(reduced, rawDamage * 0.15f);
    player.hp -= reduced;
    player.hurtFlash = 0.40f;
    lastHurtSource_ = rawDamage;

    wearArmor(rawDamage);

    GameEvent& event = emit("player.hurt");
    event.amount = static_cast<int>(std::round(reduced));

    if (player.hp <= 0.0f) {
        die();
    }
}

void Game::wearArmor(float rawDamage) {
    for (int i = 0; i < kArmorCount; ++i) {
        const ArmorType type = static_cast<ArmorType>(i);
        if (!inventory.armorWorn[static_cast<std::size_t>(i)]) {
            continue;
        }
        inventory.armorDurability[static_cast<std::size_t>(i)] -=
            rawDamage * kArmorWearPerDamage;
        if (inventory.armorDurability[static_cast<std::size_t>(i)] <= 0.0f) {
            breakArmor(type);
        }
    }
}

void Game::breakArmor(ArmorType type) {
    const int index = idx(type);
    if (index < 0 || index >= kArmorCount) {
        return;
    }
    if (!inventory.armorWorn[static_cast<std::size_t>(index)]) {
        return;
    }

    inventory.armorWorn[static_cast<std::size_t>(index)] = false;
    inventory.armorDurability[static_cast<std::size_t>(index)] = 0.0f;
    inventory.armorItems[static_cast<std::size_t>(index)] =
        std::max(0, inventory.armorItems[static_cast<std::size_t>(index)] - 1);
    recalcMaxHp();

    GameEvent& event = emit("armor.broken");
    event.armor = type;
}

void Game::healPlayer(float amount) {
    if (dead || amount <= 0.0f) {
        return;
    }
    const float before = player.hp;
    player.hp = clampf(player.hp + amount, 0.0f, player.maxHp);
    const float healed = player.hp - before;
    if (healed > 0.0f) {
        GameEvent& event = emit("player.healed");
        event.amount = static_cast<int>(std::round(healed));
    }
}

void Game::recalcMaxHp() {
    float bonus = 0.0f;
    for (int i = 0; i < kArmorCount; ++i) {
        if (inventory.armorWorn[static_cast<std::size_t>(i)] &&
            inventory.armorDurability[static_cast<std::size_t>(i)] > 0.0f) {
            bonus += armorRule(static_cast<ArmorType>(i)).hpBonus;
        }
    }
    player.maxHp = 100.0f + bonus;
    player.hp = clampf(player.hp, 0.0f, player.maxHp);
}

// ---------------------------------------------------------------------------
//  Simulation helpers
// ---------------------------------------------------------------------------

void Game::updateNodes(float dt) {
    for (Node& node : world.nodes) {
        node.shake = std::max(0.0f, node.shake - dt);
        if (node.alive) {
            continue;
        }
        node.respawnTimer -= dt;
        if (node.respawnTimer <= 0.0f) {
            respawnNode(node);
            GameEvent& event = emit("node.respawned");
            event.node = node.type;
            event.pos = node.pos;
        }
    }
}

void Game::updateAnimals(float dt) {
    for (Animal& animal : world.animals) {
        const AnimalRule& rule = animalRule(animal.type);

        if (!animal.alive) {
            animal.respawnTimer -= dt;
            if (animal.respawnTimer <= 0.0f) {
                respawnAnimal(animal);
                GameEvent& event = emit("animal.respawned");
                event.animal = animal.type;
                event.pos = animal.pos;
            }
            continue;
        }

        animal.hurtFlash = std::max(0.0f, animal.hurtFlash - dt);
        animal.attackTimer = std::max(0.0f, animal.attackTimer - dt);

        const float playerDistance = distance(animal.pos, player.pos);
        const bool playerSafe = inBaseSafeZone(player.pos);

        if (rule.aggressive && !playerSafe &&
            playerDistance <= rule.aggroRadius) {
            animal.aggro = true;
        }
        if (animal.aggro &&
            (playerSafe || playerDistance > rule.aggroRadius * 1.7f ||
             dead)) {
            animal.aggro = false;
        }

        if (animal.aggro && !dead) {
            if (playerDistance > rule.attackRange * 0.80f) {
                const Vec2 direction = (player.pos - animal.pos).normalized();
                animal.pos += direction * rule.speed * dt;
            } else if (animal.attackTimer <= 0.0f) {
                animal.attackTimer = rule.attackCooldown;
                damagePlayer(rule.damage);
                GameEvent& event = emit("animal.attack");
                event.animal = animal.type;
                event.amount = static_cast<int>(std::round(rule.damage));
                event.pos = animal.pos;
                if (dead) {
                    break;
                }
            }
        } else {
            animal.wanderTimer -= dt;
            if (animal.wanderTimer <= 0.0f) {
                animal.wanderTimer = randomRange(1.5f, 4.0f);
                animal.wanderDir = Vec2{randomRange(-1.0f, 1.0f),
                                        randomRange(-1.0f, 1.0f)}
                                       .normalized();
                if (animal.wanderDir.lengthSquared() < 0.01f) {
                    animal.wanderDir = {0.0f, 1.0f};
                }
            }

            // Harmless animals run away when the player gets too close; this
            // makes hunting feel alive instead of walking up to statues.
            const bool flee = !rule.aggressive && playerDistance < 150.0f;
            Vec2 direction = animal.wanderDir;
            float speedScale = 0.35f;
            if (flee) {
                direction = (animal.pos - player.pos).normalized();
                speedScale = 1.0f;
            }
            animal.pos += direction * rule.speed * speedScale * dt;
        }

        // Keep animals inside the island and outside the camp safe zone.
        const float limit = world.radius - 40.0f;
        if (animal.pos.length() > limit) {
            animal.pos = animal.pos.normalized() * limit;
            animal.wanderDir = animal.wanderDir * -1.0f;
        }
        if (inBaseSafeZone(animal.pos)) {
            Vec2 outward = animal.pos.normalized();
            if (outward.lengthSquared() < 0.01f) {
                outward = {0.0f, 1.0f};
            }
            animal.pos = outward * (kBaseSafeRadius + 18.0f);
            animal.aggro = false;
        }
    }
}

void Game::updateStats(float dt) {
    player.food = clampf(player.food - kFoodDrainPerSecond * dt, 0.0f,
                         kMaxStat);
    player.water = clampf(player.water - kWaterDrainPerSecond * dt, 0.0f,
                          kMaxStat);
    player.energy = clampf(player.energy - kEnergyDrainPerSecond * dt, 0.0f,
                           kMaxStat);

    // Passive regeneration only works when the player is well fed and
    // hydrated.  It costs a little food and water, so it is not free healing.
    if (player.food > 55.0f && player.water > 55.0f &&
        player.hp < player.maxHp) {
        player.hp = clampf(player.hp + kRegenHpPerSecond * dt, 0.0f,
                           player.maxHp);
        player.food = clampf(player.food - kRegenFoodCostPerSecond * dt, 0.0f,
                             kMaxStat);
        player.water = clampf(player.water - kRegenWaterCostPerSecond * dt,
                              0.0f, kMaxStat);
    }

    float starvation = 0.0f;
    if (player.food <= 0.01f) {
        starvation += kStarvationDamagePerSecond;
    }
    if (player.water <= 0.01f) {
        starvation += kDehydrationDamagePerSecond;
    }
    if (starvation > 0.0f) {
        player.hp -= starvation * dt;
        player.hurtFlash = std::max(player.hurtFlash, 0.15f);
    }

    player.hp = clampf(player.hp, 0.0f, player.maxHp);

    const StatType watched[3] = {StatType::Food, StatType::Water,
                                 StatType::Energy};
    for (int i = 0; i < 3; ++i) {
        float value = 0.0f;
        switch (watched[i]) {
        case StatType::Food:
            value = player.food;
            break;
        case StatType::Water:
            value = player.water;
            break;
        case StatType::Energy:
            value = player.energy;
            break;
        default:
            break;
        }

        if (value <= kLowStatThreshold &&
            lowWarning_[static_cast<std::size_t>(i)] <= 0.0f) {
            lowWarning_[static_cast<std::size_t>(i)] = 1.0f;
            GameEvent& event = emit("stat.low");
            event.stat = watched[i];
            event.amount = static_cast<int>(std::round(value));
        } else if (value >= kLowStatResetThreshold) {
            lowWarning_[static_cast<std::size_t>(i)] = 0.0f;
        }
    }

    if (player.hp <= 0.0f) {
        die();
    }
}

// ---------------------------------------------------------------------------
//  Events
// ---------------------------------------------------------------------------

void Game::clearEvents() {
    events.clear();
}

GameEvent& Game::emit(const std::string& key) {
    GameEvent event;
    event.key = key;
    events.push_back(std::move(event));
    return events.back();
}

// ---------------------------------------------------------------------------
//  Persistence
// ---------------------------------------------------------------------------

bool Game::saveToFile(const std::string& path) {
    using nlohmann::json;

    json root;
    root["version"] = 2;
    root["name"] = playerName;
    root["seed"] = world.seed;
    root["day"] = day;
    root["timeOfDay"] = timeOfDay;
    root["playTime"] = playTime;
    root["totalDeaths"] = totalDeaths;
    root["dead"] = dead;

    root["player"] = {
        {"pos", pointToJson(player.pos)},
        {"facing", pointToJson(player.facing)},
        {"hp", player.hp},
        {"maxHp", player.maxHp},
        {"food", player.food},
        {"water", player.water},
        {"energy", player.energy},
        {"selectedTool", inventory.selectedTool == ToolType::Count
                             ? "none"
                             : toolToId(inventory.selectedTool)},
    };

    json resources = json::object();
    for (int i = 0; i < kResourceCount; ++i) {
        resources[resourceKey(static_cast<ResourceType>(i))] =
            inventory.resources[static_cast<std::size_t>(i)];
    }
    root["resources"] = std::move(resources);

    json tools = json::object();
    for (int i = 0; i < kToolCount; ++i) {
        const ToolType type = static_cast<ToolType>(i);
        json copies = json::array();
        for (float durability : inventory.tools[static_cast<std::size_t>(i)]) {
            copies.push_back(durability);
        }
        tools[toolToId(type)] = std::move(copies);
    }
    root["tools"] = std::move(tools);

    json armor = json::object();
    for (int i = 0; i < kArmorCount; ++i) {
        const ArmorType type = static_cast<ArmorType>(i);
        armor[armorToId(type)] = {
            {"count", inventory.armorItems[static_cast<std::size_t>(i)]},
            {"worn", inventory.armorWorn[static_cast<std::size_t>(i)]},
            {"durability",
             inventory.armorDurability[static_cast<std::size_t>(i)]},
        };
    }
    root["armor"] = std::move(armor);

    root["stats"] = {
        {"kills", stats.kills},
        {"gathered", stats.gathered},
        {"crafted", stats.crafted},
    };
    root["settings"] = {
        {"language", languageId(settings.language)},
        {"fullscreen", settings.fullscreen},
        {"tutorial", settings.tutorial},
    };

    json nodes = json::array();
    for (const Node& node : world.nodes) {
        nodes.push_back({
            {"id", node.id},
            {"type", nodeKey(node.type)},
            {"pos", pointToJson(node.pos)},
            {"hp", node.hp},
            {"maxHp", node.maxHp},
            {"alive", node.alive},
            {"respawn", node.respawnTimer},
        });
    }

    json animals = json::array();
    for (const Animal& animal : world.animals) {
        animals.push_back({
            {"id", animal.id},
            {"type", animalKey(animal.type)},
            {"pos", pointToJson(animal.pos)},
            {"hp", animal.hp},
            {"maxHp", animal.maxHp},
            {"alive", animal.alive},
            {"aggro", animal.aggro},
            {"respawn", animal.respawnTimer},
        });
    }

    root["world"] = {
        {"radius", world.radius},
        {"nodes", std::move(nodes)},
        {"animals", std::move(animals)},
    };

    const std::string temporary = path + ".tmp";
    {
        std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
        if (!output) {
            lastError = "save.open_failed";
            emit("save.failed");
            return false;
        }
        output << root.dump(2);
        if (!output) {
            lastError = "save.write_failed";
            emit("save.failed");
            return false;
        }
    }

    std::error_code error;
    std::filesystem::remove(path, error);
    error.clear();
    std::filesystem::rename(temporary, path, error);
    if (error) {
        // Some file systems refuse rename over an existing file; fall back to
        // a copy and clean up the temporary file.
        error.clear();
        std::filesystem::copy_file(temporary, path,
                                   std::filesystem::copy_options::overwrite_existing,
                                   error);
        std::filesystem::remove(temporary, error);
        if (error) {
            lastError = "save.rename_failed";
            emit("save.failed");
            return false;
        }
    }

    lastError.clear();
    saveCooldown = 3.0f;
    emit("saved");
    return true;
}

bool Game::loadFromFile(const std::string& path) {
    using nlohmann::json;

    std::ifstream input(path, std::ios::binary);
    if (!input) {
        lastError = "load.not_found";
        emit("load.failed");
        return false;
    }

    json root;
    try {
        input >> root;
    } catch (...) {
        lastError = "load.corrupt";
        emit("load.failed");
        return false;
    }
    if (!root.is_object()) {
        lastError = "load.corrupt";
        emit("load.failed");
        return false;
    }

    try {
        Game loaded = *this;
        loaded.events.clear();

        // ---- legacy save migration -------------------------------------
        if (!root.contains("version") && root.contains("hp")) {
            loaded.inventory = {};
            loaded.playerName = root.value("name_user", "Player");
            loaded.inventory.resources[idx(ResourceType::Wood)] =
                root.value("tree", 0);
            loaded.inventory.resources[idx(ResourceType::Stone)] =
                root.value("stone", 0);
            loaded.inventory.resources[idx(ResourceType::Iron)] =
                root.value("iron", 0);
            loaded.inventory.resources[idx(ResourceType::Sulfur)] =
                root.value("sera", 0);
            loaded.inventory.resources[idx(ResourceType::Cloth)] =
                root.value("tkan", 0);
            loaded.inventory.resources[idx(ResourceType::Berry)] =
                root.value("barries", 0);
            loaded.inventory.resources[idx(ResourceType::Meat)] =
                root.value("meet", 0);
            loaded.inventory.resources[idx(ResourceType::Bone)] =
                root.value("bone", 0);
            loaded.inventory.resources[idx(ResourceType::Leather)] =
                root.value("kosha", 0);
            loaded.inventory.resources[idx(ResourceType::Thread)] =
                root.value("nitki", 0);
            loaded.inventory.resources[idx(ResourceType::Scrap)] =
                root.value("skrap", 0);
            loaded.inventory.resources[idx(ResourceType::Water)] =
                root.value("but_water", 0);

            const int stoneAxes = root.value("axe", 0);
            const int ironAxes = root.value("iron_axe", 0);
            const int ironPicks = root.value("pick_iron", 0);
            for (int i = 0; i < stoneAxes; ++i) {
                loaded.giveTool(ToolType::StoneAxe,
                                root.value("hp_stone_axe", 100.0f));
            }
            for (int i = 0; i < ironAxes; ++i) {
                loaded.giveTool(ToolType::IronAxe,
                                root.value("hp_iron_axe", 140.0f));
            }
            for (int i = 0; i < ironPicks; ++i) {
                loaded.giveTool(ToolType::IronPick,
                                root.value("hp_iron_pick", 150.0f));
            }

            loaded.player.hp = root.value("hp", 100.0f);
            loaded.player.food = root.value("eda", 100.0f);
            loaded.player.water = root.value("water", 100.0f);
            loaded.player.energy = root.value("energ", 100.0f);
            loaded.player.pos = {static_cast<float>(root.value("x", 0)),
                                 static_cast<float>(root.value("y", 92))};
            loaded.day = 1;
            loaded.timeOfDay = 0.28f;
            loaded.totalDeaths = 0;
            loaded.dead = false;
            loaded.inventory.selectedTool =
                loaded.hasTool(ToolType::StoneAxe) ? ToolType::StoneAxe
                                                   : ToolType::Count;
            loaded.recalcMaxHp();
            loaded.player.hp =
                clampf(loaded.player.hp, 0.0f, loaded.player.maxHp);
            loaded.player.food =
                clampf(loaded.player.food, 0.0f, kMaxStat);
            loaded.player.water =
                clampf(loaded.player.water, 0.0f, kMaxStat);
            loaded.player.energy =
                clampf(loaded.player.energy, 0.0f, kMaxStat);

            *this = std::move(loaded);
            lastError.clear();
            emit("loaded");
            return true;
        }

        // ---- current format --------------------------------------------
        loaded.playerName = root.value("name", loaded.playerName);
        loaded.world.seed = root.value("seed", loaded.world.seed);
        loaded.day = std::max(1, root.value("day", 1));
        loaded.timeOfDay = clampf(root.value("timeOfDay", 0.28f), 0.0f,
                                  0.999f);
        loaded.playTime = std::max(0.0f, root.value("playTime", 0.0f));
        loaded.totalDeaths = std::max(0, root.value("totalDeaths", 0));
        loaded.dead = false;

        if (root.contains("player") && root["player"].is_object()) {
            const json& p = root["player"];
            loaded.player.pos = pointFromJson(p.value("pos", json::object()),
                                              loaded.player.pos);
            loaded.player.facing =
                pointFromJson(p.value("facing", json::object()),
                              loaded.player.facing);
            loaded.player.hp = p.value("hp", 100.0f);
            loaded.player.food = p.value("food", 100.0f);
            loaded.player.water = p.value("water", 100.0f);
            loaded.player.energy = p.value("energy", 100.0f);
            const std::string selected =
                p.value("selectedTool", "stone_axe");
            loaded.inventory.selectedTool =
                selected == "none" ? ToolType::Count
                                   : toolFromString(selected);
        }

        loaded.inventory.resources.fill(0);
        if (root.contains("resources") && root["resources"].is_object()) {
            for (int i = 0; i < kResourceCount; ++i) {
                const ResourceType type = static_cast<ResourceType>(i);
                const std::string key = resourceKey(type);
                loaded.inventory.resources[static_cast<std::size_t>(i)] =
                    clampi(root["resources"].value(key, 0), 0, kMaxStack);
            }
        }

        for (int i = 0; i < kToolCount; ++i) {
            loaded.inventory.tools[static_cast<std::size_t>(i)].clear();
        }
        if (root.contains("tools") && root["tools"].is_object()) {
            for (int i = 0; i < kToolCount; ++i) {
                const ToolType type = static_cast<ToolType>(i);
                const std::string key = toolToId(type);
                if (!root["tools"].contains(key) ||
                    !root["tools"][key].is_array()) {
                    continue;
                }
                for (const json& value : root["tools"][key]) {
                    if (!value.is_number()) {
                        continue;
                    }
                    const float durability = clampf(
                        value.get<float>(), 0.1f, toolRule(type).maxDurability);
                    loaded.inventory.tools[static_cast<std::size_t>(i)]
                        .push_back(durability);
                }
            }
        }

        for (int i = 0; i < kArmorCount; ++i) {
            loaded.inventory.armorItems[static_cast<std::size_t>(i)] = 0;
            loaded.inventory.armorWorn[static_cast<std::size_t>(i)] = false;
            loaded.inventory.armorDurability[static_cast<std::size_t>(i)] =
                0.0f;
        }
        if (root.contains("armor") && root["armor"].is_object()) {
            for (int i = 0; i < kArmorCount; ++i) {
                const ArmorType type = static_cast<ArmorType>(i);
                const std::string key = armorToId(type);
                if (!root["armor"].contains(key) ||
                    !root["armor"][key].is_object()) {
                    continue;
                }
                const json& a = root["armor"][key];
                loaded.inventory.armorItems[static_cast<std::size_t>(i)] =
                    std::max(0, a.value("count", 0));
                loaded.inventory.armorWorn[static_cast<std::size_t>(i)] =
                    a.value("worn", false);
                loaded.inventory.armorDurability[static_cast<std::size_t>(i)] =
                    clampf(a.value("durability", 0.0f), 0.0f,
                           armorRule(type).maxDurability);
                if (loaded.inventory.armorWorn[static_cast<std::size_t>(i)] &&
                    loaded.inventory.armorDurability[static_cast<std::size_t>(
                        i)] <= 0.0f) {
                    loaded.inventory.armorWorn[static_cast<std::size_t>(i)] =
                        false;
                }
            }
        }

        if (root.contains("stats") && root["stats"].is_object()) {
            const json& s = root["stats"];
            loaded.stats.kills = std::max(0, s.value("kills", 0));
            loaded.stats.gathered = std::max(0, s.value("gathered", 0));
            loaded.stats.crafted = std::max(0, s.value("crafted", 0));
        }

        if (root.contains("settings") && root["settings"].is_object()) {
            const json& s = root["settings"];
            loaded.settings.language =
                languageFromId(s.value("language", "ru"));
            loaded.settings.fullscreen = s.value("fullscreen", false);
            loaded.settings.tutorial = s.value("tutorial", true);
        }

        loaded.world.radius = clampf(root.value("world", json::object())
                                          .value("radius", kWorldRadius),
                                      600.0f, 5000.0f);

        bool hasWorld = false;
        if (root.contains("world") && root["world"].is_object() &&
            root["world"].contains("nodes") &&
            root["world"]["nodes"].is_array() &&
            !root["world"]["nodes"].empty()) {
            loaded.world.nodes.clear();
            int id = 0;
            for (const json& n : root["world"]["nodes"]) {
                if (!n.is_object()) {
                    continue;
                }
                Node node;
                node.id = id++;
                node.type = nodeFromId(n.value("type", "node.tree_small"));
                node.pos = pointFromJson(n.value("pos", json::object()),
                                         {0.0f, 0.0f});
                node.maxHp = nodeRule(node.type).maxHp;
                node.hp = clampf(n.value("hp", node.maxHp), 0.0f, node.maxHp);
                node.alive = n.value("alive", true);
                node.respawnTimer =
                    std::max(0.0f, n.value("respawn", 0.0f));
                if (!node.alive && node.respawnTimer <= 0.0f) {
                    node.respawnTimer = nodeRule(node.type).respawnSeconds;
                }
                loaded.world.nodes.push_back(node);
            }
            hasWorld = !loaded.world.nodes.empty();
        }

        if (root.contains("world") && root["world"].is_object() &&
            root["world"].contains("animals") &&
            root["world"]["animals"].is_array()) {
            loaded.world.animals.clear();
            int id = 0;
            for (const json& a : root["world"]["animals"]) {
                if (!a.is_object()) {
                    continue;
                }
                Animal animal;
                animal.id = id++;
                animal.type = animalFromId(a.value("type", "animal.mouse"));
                animal.pos = pointFromJson(a.value("pos", json::object()),
                                           {0.0f, 0.0f});
                animal.maxHp = animalRule(animal.type).maxHp;
                animal.hp =
                    clampf(a.value("hp", animal.maxHp), 0.0f, animal.maxHp);
                animal.alive = a.value("alive", true);
                animal.aggro = a.value("aggro", false);
                animal.respawnTimer =
                    std::max(0.0f, a.value("respawn", 0.0f));
                if (!animal.alive && animal.respawnTimer <= 0.0f) {
                    animal.respawnTimer = 45.0f;
                }
                animal.wanderTimer = randomRange(0.5f, 3.0f);
                animal.wanderDir = Vec2{randomRange(-1.0f, 1.0f),
                                        randomRange(-1.0f, 1.0f)}
                                       .normalized();
                if (animal.wanderDir.lengthSquared() < 0.01f) {
                    animal.wanderDir = {0.0f, 1.0f};
                }
                loaded.world.animals.push_back(animal);
            }
        }

        if (!hasWorld) {
            loaded.generateWorld(loaded.world.seed);
        }

        loaded.world.seed = root.value("seed", loaded.world.seed);
        loaded.rng_.seed(loaded.world.seed ^ 0x9e3779b9u);
        loaded.recalcMaxHp();

        // Sanitize values that may have been edited by hand.
        const float limit = loaded.world.radius - 34.0f;
        if (loaded.player.pos.length() > limit) {
            loaded.player.pos = loaded.player.pos.normalized() * limit;
        }
        loaded.player.food = clampf(loaded.player.food, 0.0f, kMaxStat);
        loaded.player.water = clampf(loaded.player.water, 0.0f, kMaxStat);
        loaded.player.energy = clampf(loaded.player.energy, 0.0f, kMaxStat);
        loaded.player.hp =
            clampf(loaded.player.hp, 0.0f, loaded.player.maxHp);
        if (loaded.player.hp <= 0.0f) {
            loaded.player.hp = loaded.player.maxHp;
        }
        if (loaded.inventory.selectedTool != ToolType::Count &&
            !loaded.hasTool(loaded.inventory.selectedTool)) {
            loaded.inventory.selectedTool = ToolType::Count;
            for (int i = 0; i < kToolCount; ++i) {
                if (loaded.hasTool(static_cast<ToolType>(i))) {
                    loaded.inventory.selectedTool = static_cast<ToolType>(i);
                    break;
                }
            }
        }

        loaded.lastError.clear();
        loaded.dead = false;
        *this = std::move(loaded);
        lastError.clear();
        emit("loaded");
        return true;
    } catch (...) {
        lastError = "load.invalid_data";
        emit("load.failed");
        return false;
    }
}

// ---------------------------------------------------------------------------
//  Misc
// ---------------------------------------------------------------------------

float Game::randomRange(float low, float high) {
    if (high <= low) {
        return low;
    }
    std::uniform_real_distribution<float> distribution(low, high);
    return distribution(rng_);
}

int Game::randomInt(int low, int high) {
    if (high <= low) {
        return low;
    }
    std::uniform_int_distribution<int> distribution(low, high);
    return distribution(rng_);
}

std::string formatNumber(float value, int decimals) {
    std::ostringstream stream;
    stream.setf(std::ios::fixed);
    stream.precision(decimals);
    stream << value;
    return stream.str();
}

} // namespace cr
