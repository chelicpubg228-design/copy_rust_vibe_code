// ---------------------------------------------------------------------------
//  Copy Rust 2D - core simulation tests
//  No external test framework is required: run the executable and it prints a
//  summary.  CTest also runs it as a normal test.
// ---------------------------------------------------------------------------

#include "../src/core/Game.h"

#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

using namespace cr;

namespace {

int g_failures = 0;
int g_checks = 0;

void check(bool condition, const char* expression, const char* file, int line) {
    ++g_checks;
    if (!condition) {
        ++g_failures;
        std::cerr << "FAIL " << file << ":" << line << "  " << expression
                  << "\n";
    }
}

void checkNear(float actual, float expected, float tolerance,
               const char* expression, const char* file, int line) {
    ++g_checks;
    if (std::fabs(actual - expected) > tolerance) {
        ++g_failures;
        std::cerr << "FAIL " << file << ":" << line << "  " << expression
                  << " (actual " << actual << ", expected " << expected
                  << " +/- " << tolerance << ")\n";
    }
}

#define CHECK(expression) check((expression), #expression, __FILE__, __LINE__)
#define CHECK_NEAR(actual, expected, tolerance) \
    checkNear((actual), (expected), (tolerance), #actual " ~= " #expected, \
              __FILE__, __LINE__)

bool hasEvent(const Game& game, const std::string& key) {
    for (const GameEvent& event : game.events) {
        if (event.key == key) {
            return true;
        }
    }
    return false;
}

void clearCooldown(Game& game) {
    game.player.interactCooldown = 0.0f;
    game.player.attackCooldown = 0.0f;
}

void removeAllTools(Game& game) {
    for (int i = 0; i < kToolCount; ++i) {
        game.inventory.tools[static_cast<std::size_t>(i)].clear();
    }
    game.inventory.selectedTool = ToolType::Count;
}

// ---------------------------------------------------------------------------

void testWorldGeneration() {
    Game game(123456u, "Tester");

    CHECK(game.world.nodes.size() >= 230);
    CHECK(game.world.animals.size() >= 35);
    CHECK(game.world.seed == 123456u);
    CHECK_NEAR(game.world.radius, kWorldRadius, 0.1f);

    for (const Node& node : game.world.nodes) {
        CHECK(node.alive);
        CHECK_NEAR(node.hp, nodeRule(node.type).maxHp, 0.01f);
        CHECK(!game.inBaseSafeZone(node.pos));
        CHECK(node.pos.length() <= game.world.radius - 60.0f);
    }

    for (const Animal& animal : game.world.animals) {
        CHECK(animal.alive);
        CHECK(!game.inBaseSafeZone(animal.pos));
        CHECK(animal.pos.length() <= game.world.radius - 60.0f);
    }

    // Same seed must produce the same world.
    Game twin(123456u, "Tester");
    CHECK(twin.world.nodes.size() == game.world.nodes.size());
    CHECK(twin.world.animals.size() == game.world.animals.size());
    if (!game.world.nodes.empty() && !twin.world.nodes.empty()) {
        CHECK_NEAR(game.world.nodes[0].pos.x, twin.world.nodes[0].pos.x, 0.001f);
        CHECK_NEAR(game.world.nodes[0].pos.y, twin.world.nodes[0].pos.y, 0.001f);
    }

    // The starting area contains useful resources within a short walk.
    int nearTrees = 0;
    int nearBerries = 0;
    for (const Node& node : game.world.nodes) {
        if (node.pos.length() < 450.0f) {
            if (node.type == NodeType::TreeSmall ||
                node.type == NodeType::TreeNormal ||
                node.type == NodeType::TreeBig) {
                ++nearTrees;
            }
            if (node.type == NodeType::Berry) {
                ++nearBerries;
            }
        }
    }
    CHECK(nearTrees >= 3);
    CHECK(nearBerries >= 1);
}

void testStartingKit() {
    Game game(1u, "Starter");
    CHECK(game.hasTool(ToolType::StoneAxe));
    CHECK(game.hasTool(ToolType::Torch));
    CHECK(game.toolCount(ToolType::StoneAxe) == 1);
    CHECK(game.toolCount(ToolType::IronAxe) == 0);
    CHECK(game.inventory.selectedTool == ToolType::StoneAxe);
    CHECK(game.resource(ResourceType::Wood) == 0);
    CHECK_NEAR(game.player.hp, 100.0f, 0.01f);
    CHECK_NEAR(game.player.maxHp, 100.0f, 0.01f);
    CHECK(game.nearBase());
}

void testGathering() {
    Game game(2u, "Gatherer");
    const int nodeId = game.addNode(
        NodeType::TreeSmall, game.player.pos + Vec2{0.0f, 30.0f});
    Node* node = game.nodeById(nodeId);
    CHECK(node != nullptr);

    const int woodBefore = game.resource(ResourceType::Wood);
    int hits = 0;
    while (node != nullptr && node->alive && hits < 20) {
        clearCooldown(game);
        CHECK(game.gatherNode(*node));
        ++hits;
    }
    CHECK(hits > 0);
    CHECK(hits < 20);
    CHECK(!node->alive);
    CHECK(game.resource(ResourceType::Wood) > woodBefore);
    CHECK(game.stats.gathered > 0);
    CHECK(hasEvent(game, "node.hit"));
    CHECK(hasEvent(game, "node.depleted"));
    CHECK_NEAR(node->respawnTimer, nodeRule(NodeType::TreeSmall).respawnSeconds,
               0.01f);

    // The node comes back after its respawn timer.
    for (int i = 0; i < 400; ++i) {
        game.update(0.25f);
    }
    CHECK(node->alive);
    CHECK_NEAR(node->hp, node->maxHp, 0.01f);
    CHECK(hasEvent(game, "node.respawned"));
}

void testGatheringNeedsTool() {
    Game game(3u, "Hands");
    removeAllTools(game);
    const int nodeId = game.addNode(
        NodeType::TreeBig, game.player.pos + Vec2{30.0f, 0.0f});
    Node* node = game.nodeById(nodeId);

    const GatherPlan plan = game.planGather(*node);
    CHECK(!plan.possible);
    CHECK(plan.needsTool);
    CHECK(plan.tool != ToolType::Count);

    const int woodBefore = game.resource(ResourceType::Wood);
    clearCooldown(game);
    CHECK(!game.gatherNode(*node));
    CHECK(game.resource(ResourceType::Wood) == woodBefore);
    CHECK(hasEvent(game, "gather.need_tool"));
    CHECK(node->alive);

    // Berries can be gathered by hand.
    const int berryId = game.addNode(
        NodeType::Berry, game.player.pos + Vec2{-30.0f, 0.0f});
    Node* berry = game.nodeById(berryId);
    const GatherPlan berryPlan = game.planGather(*berry);
    CHECK(berryPlan.possible);
    CHECK(berryPlan.tool == ToolType::Count);
    const int berriesBefore = game.resource(ResourceType::Berry);
    clearCooldown(game);
    CHECK(game.gatherNode(*berry));
    CHECK(game.resource(ResourceType::Berry) > berriesBefore);
}

void testMovement() {
    Game game(4u, "Runner");
    const Vec2 start = game.player.pos;

    game.player.energy = 100.0f;
    game.movePlayer({1.0f, 0.0f}, 1.0f, false);
    CHECK_NEAR(distance(start, game.player.pos), kPlayerSpeed, 1.0f);
    CHECK_NEAR(game.player.energy, 100.0f - 0.22f, 0.01f);

    game.player.pos = {0.0f, 0.0f};
    game.player.energy = 100.0f;
    game.movePlayer({0.0f, 1.0f}, 1.0f, true);
    CHECK_NEAR(distance({0.0f, 0.0f}, game.player.pos), kPlayerSprintSpeed,
               1.0f);
    CHECK_NEAR(game.player.energy, 100.0f - 7.5f, 0.01f);

    game.player.pos = {game.world.radius - 10.0f, 0.0f};
    game.movePlayer({1.0f, 0.0f}, 1.0f, false);
    CHECK(game.player.pos.length() <= game.world.radius - 34.0f + 0.01f);

    game.player.pos = {0.0f, 100.0f};
    game.dead = true;
    game.movePlayer({1.0f, 0.0f}, 1.0f, false);
    CHECK_NEAR(game.player.pos.x, 0.0f, 0.01f);
    game.dead = false;
}

void testCrafting() {
    Game game(5u, "Crafter");
    const std::vector<Recipe>& recipes = game.recipes();
    CHECK(recipes.size() >= 9);

    // Stone axe: 5 wood + 1 stone.
    game.addResource(ResourceType::Wood, 10);
    game.addResource(ResourceType::Stone, 2);
    const int axesBefore = game.toolCount(ToolType::StoneAxe);
    CHECK(game.craft(0));
    CHECK(game.toolCount(ToolType::StoneAxe) == axesBefore + 1);
    CHECK(game.resource(ResourceType::Wood) == 5);
    CHECK(game.resource(ResourceType::Stone) == 1);
    CHECK(game.stats.crafted == 1);
    CHECK(hasEvent(game, "craft.ok"));

    // Iron axe cannot be crafted without iron.
    CHECK(!game.canCraft(recipes[1]));
    CHECK(!game.craft(1));
    CHECK(hasEvent(game, "craft.missing"));
    CHECK(game.toolCount(ToolType::IronAxe) == 0);

    // Threads: 5 cloth -> 5 thread.
    game.addResource(ResourceType::Cloth, 5);
    const int threadIndex = 4;
    CHECK(recipes[threadIndex].kind == RecipeKind::Resource);
    CHECK(game.craft(threadIndex));
    CHECK(game.resource(ResourceType::Thread) == 5);
    CHECK(game.resource(ResourceType::Cloth) == 0);

    // Invalid recipe index is rejected without crashing.
    CHECK(!game.craft(-1));
    CHECK(!game.craft(9999));
    CHECK(hasEvent(game, "craft.unknown"));
}

void testArmor() {
    Game game(6u, "Knight");
    game.addResource(ResourceType::Cloth, 10);
    game.addResource(ResourceType::Thread, 10);
    game.addResource(ResourceType::Leather, 10);

    const int helmetIndex = 5;
    CHECK(game.craft(helmetIndex));
    CHECK(game.inventory.armorItems[idx(ArmorType::Helmet)] == 1);

    CHECK(game.equipArmor(ArmorType::Helmet));
    CHECK(game.inventory.armorWorn[idx(ArmorType::Helmet)]);
    CHECK_NEAR(game.player.maxHp, 110.0f, 0.01f);
    CHECK_NEAR(game.player.hp, 110.0f, 0.01f);
    CHECK_NEAR(game.armorDefense(), 1.0f, 0.01f);

    CHECK(game.unequipArmor(ArmorType::Helmet));
    CHECK(!game.inventory.armorWorn[idx(ArmorType::Helmet)]);
    CHECK_NEAR(game.player.maxHp, 100.0f, 0.01f);
    CHECK_NEAR(game.player.hp, 100.0f, 0.01f);

    // Armor reduces damage and wears out.
    game.inventory.armorItems[idx(ArmorType::Bib)] = 1;
    CHECK(game.equipArmor(ArmorType::Bib));
    game.player.hp = 100.0f;
    game.damagePlayer(20.0f);
    CHECK_NEAR(game.player.hp, 82.4f, 0.05f);
    CHECK(game.inventory.armorDurability[idx(ArmorType::Bib)] < 50.0f);

    // A nearly broken piece breaks and is removed.
    game.inventory.armorDurability[idx(ArmorType::Bib)] = 0.1f;
    game.player.hp = 100.0f;
    game.damagePlayer(10.0f);
    CHECK(!game.inventory.armorWorn[idx(ArmorType::Bib)]);
    CHECK(game.inventory.armorItems[idx(ArmorType::Bib)] == 0);
    CHECK(hasEvent(game, "armor.broken"));
    CHECK_NEAR(game.player.maxHp, 100.0f, 0.01f);
}

void testCombatAndLoot() {
    Game game(7u, "Hunter");
    const int mouseId = game.addAnimal(
        AnimalType::Mouse, game.player.pos + Vec2{0.0f, 35.0f});
    Animal* mouse = game.animalById(mouseId);
    CHECK(mouse != nullptr);

    clearCooldown(game);
    CHECK(game.attackAnimal(*mouse));
    CHECK(!mouse->alive);
    CHECK(game.stats.kills == 1);
    CHECK(game.resource(ResourceType::Meat) > 0);
    CHECK(hasEvent(game, "animal.hit"));
    CHECK(hasEvent(game, "animal.killed"));

    bool foundLoot = false;
    for (const GameEvent& event : game.events) {
        if (event.key == "animal.killed" && !event.loot.empty()) {
            foundLoot = true;
        }
    }
    CHECK(foundLoot);

    // Aggressive animals attack outside the safe camp.
    game.player.pos = {500.0f, 0.0f};
    game.player.hp = 100.0f;
    const int wolfId = game.addAnimal(AnimalType::Wolf,
                                      {530.0f, 0.0f});
    Animal* wolf = game.animalById(wolfId);
    game.clearEvents();
    game.update(0.25f);
    CHECK(wolf->aggro);
    CHECK(game.player.hp < 100.0f);
    CHECK(hasEvent(game, "animal.attack"));

    // Death and respawn keep the inventory.
    const int meatBefore = game.resource(ResourceType::Meat);
    game.player.hp = 1.0f;
    wolf->attackTimer = 0.0f;
    game.update(0.25f);
    CHECK(game.dead);
    CHECK(game.totalDeaths == 1);
    CHECK(hasEvent(game, "player.died"));

    game.respawn();
    CHECK(!game.dead);
    CHECK_NEAR(game.player.hp, game.player.maxHp, 0.01f);
    CHECK(game.nearBase());
    CHECK(game.resource(ResourceType::Meat) == meatBefore);
    CHECK(hasEvent(game, "respawned"));
}

void testAnimalRespawn() {
    Game game(8u, "Respawner");
    const int mouseId = game.addAnimal(
        AnimalType::Mouse, game.player.pos + Vec2{0.0f, 35.0f});
    Animal* mouse = game.animalById(mouseId);
    clearCooldown(game);
    CHECK(game.attackAnimal(*mouse));
    CHECK(!mouse->alive);
    mouse->respawnTimer = 0.1f;
    game.update(0.25f);
    CHECK(mouse->alive);
    CHECK(hasEvent(game, "animal.respawned"));
}

void testSurvivalStats() {
    Game game(9u, "Survivor");
    game.player.hp = 100.0f;
    game.player.food = 100.0f;
    game.player.water = 100.0f;
    game.player.energy = 100.0f;
    game.clearEvents();

    for (int i = 0; i < 4; ++i) {
        game.update(0.25f);
    }
    CHECK_NEAR(game.player.food, 100.0f - 0.055f, 0.01f);
    CHECK_NEAR(game.player.water, 100.0f - 0.085f, 0.01f);
    CHECK_NEAR(game.player.energy, 100.0f - 0.018f, 0.01f);

    // Regeneration while well fed.
    game.player.hp = 50.0f;
    game.player.food = 100.0f;
    game.player.water = 100.0f;
    for (int i = 0; i < 4; ++i) {
        game.update(0.25f);
    }
    CHECK(game.player.hp > 50.0f);
    CHECK(game.player.hp < 52.0f);

    // Starvation bypasses armor and eventually kills.
    game.player.hp = 50.0f;
    game.player.food = 0.0f;
    game.player.water = 0.0f;
    game.update(0.25f);
    CHECK(game.player.hp < 49.0f);

    for (int i = 0; i < 80 && !game.dead; ++i) {
        game.update(0.25f);
    }
    CHECK(game.dead);
    CHECK(hasEvent(game, "player.died"));
}

void testLowStatWarnings() {
    Game game(10u, "Warnings");
    game.player.food = 24.0f;
    game.player.water = 100.0f;
    game.player.energy = 100.0f;
    game.clearEvents();
    game.update(0.25f);
    CHECK(hasEvent(game, "stat.low"));

    game.player.food = 50.0f;
    game.update(0.25f);
    game.clearEvents();
    game.player.food = 20.0f;
    game.update(0.25f);
    CHECK(hasEvent(game, "stat.low"));
}

void testRestAndEating() {
    Game game(11u, "Camper");
    game.player.energy = 50.0f;
    game.clearEvents();
    CHECK(game.restAtBase());
    CHECK(game.player.energy > 50.0f);
    CHECK(hasEvent(game, "rest.ok"));

    game.player.energy = 100.0f;
    CHECK(!game.restAtBase());
    CHECK(hasEvent(game, "rest.full"));

    game.player.pos = {1000.0f, 0.0f};
    game.player.energy = 50.0f;
    CHECK(!game.restAtBase());
    CHECK(hasEvent(game, "rest.need_base"));

    game.player.pos = {0.0f, 92.0f};
    game.clearEvents();
    CHECK(!game.eatBerry());
    CHECK(hasEvent(game, "eat.none"));

    game.addResource(ResourceType::Berry, 2);
    const int berries = game.resource(ResourceType::Berry);
    const float food = game.player.food;
    CHECK(game.eatBerry());
    CHECK(game.resource(ResourceType::Berry) == berries - 1);
    CHECK(game.player.food > food);
    CHECK(hasEvent(game, "ate.berry"));
}

void testToolBreakage() {
    Game game(12u, "Breaker");
    game.inventory.tools[idx(ToolType::StoneAxe)][0] = 0.5f;
    const int nodeId = game.addNode(
        NodeType::TreeSmall, game.player.pos + Vec2{0.0f, 30.0f});
    Node* node = game.nodeById(nodeId);
    clearCooldown(game);
    CHECK(game.gatherNode(*node));
    CHECK(!game.hasTool(ToolType::StoneAxe));
    CHECK(hasEvent(game, "tool.broken"));
    // The torch is still owned, so it becomes the selected tool.
    CHECK(game.inventory.selectedTool == ToolType::Torch);
}

void testSelection() {
    Game game(13u, "Selector");
    CHECK(!game.selectTool(ToolType::IronAxe));
    CHECK(hasEvent(game, "tool.not_owned"));
    CHECK(game.selectTool(ToolType::Torch));
    CHECK(game.inventory.selectedTool == ToolType::Torch);
    CHECK_NEAR(game.selectedWeaponDamage(),
               toolRule(ToolType::Torch).damage, 0.01f);
    game.cycleTool(1);
    CHECK(game.inventory.selectedTool == ToolType::StoneAxe);
    game.cycleTool(-1);
    CHECK(game.inventory.selectedTool == ToolType::Torch);
}

void testSaveLoad() {
    const std::string path = "test_save_roundtrip.json";
    std::filesystem::remove(path);

    Game game(777u, "Saver");
    game.addResource(ResourceType::Wood, 42);
    game.addResource(ResourceType::Iron, 7);
    game.giveTool(ToolType::IronAxe, 90.0f);
    game.player.pos = {300.0f, 120.0f};
    game.player.food = 71.0f;
    game.day = 4;
    game.totalDeaths = 2;

    const int nodeId = game.addNode(NodeType::TreeSmall, {500.0f, 0.0f});
    Node* node = game.nodeById(nodeId);
    clearCooldown(game);
    CHECK(game.gatherNode(*node));
    const float nodeHp = node->hp;
    const std::size_t nodeCount = game.world.nodes.size();
    const std::size_t animalCount = game.world.animals.size();

    CHECK(game.saveToFile(path));
    CHECK(std::filesystem::exists(path));
    CHECK(hasEvent(game, "saved"));

    Game loaded(1u, "Other");
    CHECK(loaded.loadFromFile(path));
    CHECK(loaded.playerName == "Saver");
    CHECK(loaded.world.seed == 777u);
    CHECK(loaded.resource(ResourceType::Wood) >= 42);
    CHECK(loaded.resource(ResourceType::Iron) == 7);
    CHECK(loaded.toolCount(ToolType::IronAxe) == 1);
    CHECK_NEAR(loaded.player.pos.x, 300.0f, 0.01f);
    CHECK_NEAR(loaded.player.pos.y, 120.0f, 0.01f);
    CHECK_NEAR(loaded.player.food, 71.0f, 0.5f);
    CHECK(loaded.day == 4);
    CHECK(loaded.totalDeaths == 2);
    CHECK(loaded.world.nodes.size() == nodeCount);
    CHECK(loaded.world.animals.size() == animalCount);
    Node* loadedNode = loaded.nodeById(nodeId);
    CHECK(loadedNode != nullptr);
    CHECK_NEAR(loadedNode->hp, nodeHp, 0.01f);
    CHECK(hasEvent(loaded, "loaded"));

    // Corrupt files are rejected without destroying the current game.
    {
        std::ofstream broken(path, std::ios::binary | std::ios::trunc);
        broken << "{ this is not json";
    }
    Game untouched(2u, "Untouched");
    CHECK(!untouched.loadFromFile(path));
    CHECK(untouched.playerName == "Untouched");
    CHECK(!untouched.lastError.empty());

    // Missing file is rejected gracefully.
    std::filesystem::remove(path);
    CHECK(!untouched.loadFromFile(path));
    CHECK(untouched.lastError == "load.not_found");

    std::filesystem::remove(path);
}

void testLegacyMigration() {
    const std::string path = "test_legacy_save.json";
    {
        std::ofstream legacy(path, std::ios::binary | std::ios::trunc);
        legacy << R"({
            "tree": 12, "stone": 3, "iron": 2, "sera": 1, "tkan": 4,
            "barries": 5, "meet": 2, "bone": 1, "kosha": 6, "nitki": 7,
            "hp": 64, "eda": 55, "water": 66, "energ": 77,
            "name_user": "Legacy", "x": 120, "y": -40,
            "axe": 1, "iron_axe": 1, "pick_iron": 0,
            "hp_stone_axe": 33, "hp_iron_axe": 120, "hp_iron_pick": 150
        })";
    }

    Game game(3u, "Modern");
    CHECK(game.loadFromFile(path));
    CHECK(game.playerName == "Legacy");
    CHECK(game.resource(ResourceType::Wood) == 12);
    CHECK(game.resource(ResourceType::Stone) == 3);
    CHECK(game.resource(ResourceType::Iron) == 2);
    CHECK(game.resource(ResourceType::Sulfur) == 1);
    CHECK(game.resource(ResourceType::Cloth) == 4);
    CHECK(game.resource(ResourceType::Berry) == 5);
    CHECK(game.resource(ResourceType::Leather) == 6);
    CHECK(game.resource(ResourceType::Thread) == 7);
    CHECK_NEAR(game.player.hp, 64.0f, 0.01f);
    CHECK_NEAR(game.player.food, 55.0f, 0.01f);
    CHECK_NEAR(game.player.water, 66.0f, 0.01f);
    CHECK_NEAR(game.player.energy, 77.0f, 0.01f);
    CHECK_NEAR(game.player.pos.x, 120.0f, 0.01f);
    CHECK_NEAR(game.player.pos.y, -40.0f, 0.01f);
    CHECK(game.toolCount(ToolType::StoneAxe) == 1);
    CHECK(game.toolCount(ToolType::IronAxe) == 1);
    CHECK_NEAR(game.toolDurability(ToolType::StoneAxe), 33.0f, 0.01f);
    CHECK_NEAR(game.toolDurability(ToolType::IronAxe), 120.0f, 0.01f);

    std::filesystem::remove(path);
}

void testTargeting() {
    Game game(14u, "Aim");
    const int nodeId = game.addNode(NodeType::Stone,
                                    game.player.pos + Vec2{20.0f, 0.0f});
    const ClickTarget target =
        game.targetAt(game.player.pos + Vec2{20.0f, 0.0f}, 5.0f);
    CHECK(target.nodeId == nodeId);
    CHECK(target.animalId == -1);

    const int animalId = game.addAnimal(
        AnimalType::Deer, game.player.pos + Vec2{-30.0f, 0.0f});
    const ClickTarget animalTarget =
        game.targetAt(game.player.pos + Vec2{-30.0f, 0.0f}, 5.0f);
    CHECK(animalTarget.animalId == animalId);
    CHECK(animalTarget.nodeId == -1);
}

void testEventsAndFormatting() {
    Game game(15u, "Events");
    game.clearEvents();
    CHECK(game.events.empty());
    GameEvent& event = game.emit("unit.test");
    event.amount = 42;
    CHECK(game.events.size() == 1);
    CHECK(game.events[0].key == "unit.test");
    CHECK(game.events[0].amount == 42);
    CHECK(formatNumber(12.345f, 1) == "12.3");
    CHECK(formatNumber(7.0f, 0) == "7");
}

} // namespace

int main() {
    std::cout << "Copy Rust 2D - core tests\n";

    testWorldGeneration();
    testStartingKit();
    testGathering();
    testGatheringNeedsTool();
    testMovement();
    testCrafting();
    testArmor();
    testCombatAndLoot();
    testAnimalRespawn();
    testSurvivalStats();
    testLowStatWarnings();
    testRestAndEating();
    testToolBreakage();
    testSelection();
    testSaveLoad();
    testLegacyMigration();
    testTargeting();
    testEventsAndFormatting();

    std::cout << "\n" << (g_checks - g_failures) << "/" << g_checks
              << " checks passed\n";
    if (g_failures == 0) {
        std::cout << "ALL TESTS PASSED\n";
        return 0;
    }
    std::cerr << g_failures << " TEST(S) FAILED\n";
    return 1;
}
