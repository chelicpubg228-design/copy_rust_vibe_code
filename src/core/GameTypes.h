#pragma once

// ============================================================================
//  Copy Rust 2D - core types
//  This header has no UI dependencies (no SFML, no console).  Both the SFML
//  front-end and the console front-end use these types through Game.
// ============================================================================

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace cr {

constexpr int kResourceCount = 12;
constexpr int kToolCount     = 4;
constexpr int kArmorCount    = 4;
constexpr int kNodeCount     = 8;
constexpr int kAnimalCount   = 4;

// ---------------------------------------------------------------------------
//  Enumerations
// ---------------------------------------------------------------------------

enum class ResourceType : int {
    Wood = 0,
    Stone,
    Iron,
    Sulfur,
    Cloth,
    Berry,
    Meat,
    Bone,
    Leather,
    Thread,
    Scrap,
    Water,
    Count
};

enum class ToolType : int {
    StoneAxe = 0,
    IronAxe,
    IronPick,
    Torch,
    Count
};

enum class ArmorType : int {
    Helmet = 0,
    Bib,
    Leggings,
    Boots,
    Count
};

enum class NodeType : int {
    TreeBig = 0,
    TreeNormal,
    TreeSmall,
    Stone,
    Iron,
    Sulfur,
    Cloth,
    Berry,
    Count
};

enum class AnimalType : int {
    Mouse = 0,
    Deer,
    Wolf,
    Bear,
    Count
};

enum class StatType : int {
    Hp = 0,
    Food,
    Water,
    Energy,
    Count
};

enum class Language : int {
    Russian = 0,
    English = 1
};

constexpr int idx(ResourceType t) { return static_cast<int>(t); }
constexpr int idx(ToolType t)     { return static_cast<int>(t); }
constexpr int idx(ArmorType t)    { return static_cast<int>(t); }
constexpr int idx(NodeType t)     { return static_cast<int>(t); }
constexpr int idx(AnimalType t)   { return static_cast<int>(t); }
constexpr int idx(StatType t)     { return static_cast<int>(t); }

// ---------------------------------------------------------------------------
//  Basic math
// ---------------------------------------------------------------------------

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    constexpr Vec2() = default;
    constexpr Vec2(float xValue, float yValue) : x(xValue), y(yValue) {}

    Vec2 operator+(const Vec2& other) const { return {x + other.x, y + other.y}; }
    Vec2 operator-(const Vec2& other) const { return {x - other.x, y - other.y}; }
    Vec2 operator*(float scalar) const { return {x * scalar, y * scalar}; }
    Vec2 operator/(float scalar) const { return {x / scalar, y / scalar}; }

    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    Vec2& operator-=(const Vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    float lengthSquared() const { return x * x + y * y; }
    float length() const;
    Vec2 normalized() const;
    float dot(const Vec2& other) const { return x * other.x + y * other.y; }
};

float distance(const Vec2& a, const Vec2& b);
float clampf(float value, float low, float high);
int clampi(int value, int low, int high);

// ---------------------------------------------------------------------------
//  Rules and metadata
// ---------------------------------------------------------------------------

struct NodeRule {
    const char* key = "";
    ResourceType resource = ResourceType::Wood;
    float maxHp = 1.0f;
    int yieldPerHit = 1;
    float respawnSeconds = 90.0f;
    float visualRadius = 20.0f;
    bool hands = false;
    float handDamage = 5.0f;
    std::array<ToolType, 3> tools{};
    int toolCount = 0;
};

struct ToolRule {
    const char* key = "";
    float maxDurability = 100.0f;
    float damage = 5.0f;
    const char* descriptionKey = "";
};

struct ArmorRule {
    const char* key = "";
    float maxDurability = 20.0f;
    float hpBonus = 0.0f;
    float defense = 0.0f;
};

struct DropRule {
    ResourceType resource = ResourceType::Meat;
    int minAmount = 1;
    int maxAmount = 1;
};

struct AnimalRule {
    const char* key = "";
    float maxHp = 10.0f;
    float damage = 0.0f;
    float speed = 100.0f;
    float aggroRadius = 0.0f;
    float attackRange = 0.0f;
    float attackCooldown = 1.0f;
    float visualRadius = 10.0f;
    bool aggressive = false;
    std::array<DropRule, 4> drops{};
    int dropCount = 0;
};

const NodeRule& nodeRule(NodeType type);
const ToolRule& toolRule(ToolType type);
const ArmorRule& armorRule(ArmorType type);
const AnimalRule& animalRule(AnimalType type);

const char* resourceKey(ResourceType type);
const char* toolKey(ToolType type);
const char* armorKey(ArmorType type);
const char* nodeKey(NodeType type);
const char* animalKey(AnimalType type);
const char* statKey(StatType type);

ResourceType resourceFromString(const std::string& value);
ToolType toolFromString(const std::string& value);
ArmorType armorFromString(const std::string& value);
const char* toolToId(ToolType type);
const char* armorToId(ArmorType type);

// ---------------------------------------------------------------------------
//  Crafting
// ---------------------------------------------------------------------------

struct Ingredient {
    ResourceType resource = ResourceType::Wood;
    int amount = 1;
};

enum class RecipeKind {
    Tool,
    Armor,
    Resource
};

struct Recipe {
    std::string nameKey;
    std::string descriptionKey;
    std::vector<Ingredient> cost;
    RecipeKind kind = RecipeKind::Tool;
    ToolType tool = ToolType::StoneAxe;
    ArmorType armor = ArmorType::Helmet;
    ResourceType resource = ResourceType::Wood;
    int amount = 1;
};

const std::vector<Recipe>& recipeCatalog();

// ---------------------------------------------------------------------------
//  World objects
// ---------------------------------------------------------------------------

struct Node {
    int id = 0;
    NodeType type = NodeType::TreeSmall;
    Vec2 pos{};
    float hp = 1.0f;
    float maxHp = 1.0f;
    bool alive = true;
    float respawnTimer = 0.0f;
    float shake = 0.0f;
};

struct Animal {
    int id = 0;
    AnimalType type = AnimalType::Mouse;
    Vec2 pos{};
    Vec2 wanderDir{0.0f, 1.0f};
    float hp = 1.0f;
    float maxHp = 1.0f;
    bool alive = true;
    bool aggro = false;
    float respawnTimer = 0.0f;
    float wanderTimer = 0.0f;
    float attackTimer = 0.0f;
    float hurtFlash = 0.0f;
};

struct PlayerState {
    Vec2 pos{0.0f, 60.0f};
    Vec2 facing{0.0f, 1.0f};
    float hp = 100.0f;
    float maxHp = 100.0f;
    float food = 100.0f;
    float water = 100.0f;
    float energy = 100.0f;
    float interactCooldown = 0.0f;
    float attackCooldown = 0.0f;
    float restCooldown = 0.0f;
    float attackAnim = 0.0f;
    float hurtFlash = 0.0f;
    float stepPhase = 0.0f;
    bool moving = false;
    bool sprinting = false;
};

struct InventoryState {
    std::array<int, kResourceCount> resources{};
    // Durability of every owned tool of that type.  The best (max) one is used.
    std::array<std::vector<float>, kToolCount> tools{};
    std::array<int, kArmorCount> armorItems{};
    std::array<bool, kArmorCount> armorWorn{};
    std::array<float, kArmorCount> armorDurability{};
    ToolType selectedTool = ToolType::StoneAxe;
};

struct RunStats {
    int kills = 0;
    int gathered = 0;
    int crafted = 0;
};

struct WorldState {
    std::uint32_t seed = 0;
    float radius = 2600.0f;
    std::vector<Node> nodes;
    std::vector<Animal> animals;
};

struct Settings {
    Language language = Language::Russian;
    bool fullscreen = false;
    bool tutorial = true;
};

// ---------------------------------------------------------------------------
//  Events
//  The core never contains localized text.  Events carry a key and data;
//  each front-end turns them into text in the selected language.
// ---------------------------------------------------------------------------

struct ResourceAmount {
    ResourceType resource = ResourceType::Wood;
    int amount = 0;
};

struct GameEvent {
    std::string key;
    int amount = 0;
    int recipe = -1;
    ResourceType resource = ResourceType::Wood;
    ToolType tool = ToolType::StoneAxe;
    ArmorType armor = ArmorType::Helmet;
    AnimalType animal = AnimalType::Mouse;
    NodeType node = NodeType::TreeSmall;
    StatType stat = StatType::Hp;
    Vec2 pos{};
    std::vector<ResourceAmount> loot;
};

// ---------------------------------------------------------------------------
//  Gameplay constants
// ---------------------------------------------------------------------------

constexpr float kBaseRadius          = 150.0f;
constexpr float kBaseSafeRadius      = 210.0f;
constexpr float kInteractRange       = 92.0f;
constexpr float kAttackRange         = 104.0f;
constexpr float kPlayerRadius        = 16.0f;
constexpr float kPlayerSpeed         = 232.0f;
constexpr float kPlayerSprintSpeed   = 342.0f;
constexpr float kMaxStat             = 100.0f;
constexpr float kDayLengthSeconds    = 360.0f;
constexpr float kWorldRadius         = 2600.0f;
constexpr int   kMaxStack            = 9999;

} // namespace cr

// Some third-party headers leak min/max macros; keep this header self-contained.
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
