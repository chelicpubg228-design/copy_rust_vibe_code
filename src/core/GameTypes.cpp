#include "GameTypes.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace cr {

// ---------------------------------------------------------------------------
//  Vec2 / small helpers
// ---------------------------------------------------------------------------

float Vec2::length() const {
    return std::sqrt(lengthSquared());
}

Vec2 Vec2::normalized() const {
    const float len = length();
    if (len < 0.00001f) {
        return {0.0f, 0.0f};
    }
    return {x / len, y / len};
}

float distance(const Vec2& a, const Vec2& b) {
    return (a - b).length();
}

float clampf(float value, float low, float high) {
    return std::min(high, std::max(low, value));
}

int clampi(int value, int low, int high) {
    return std::min(high, std::max(low, value));
}

// ---------------------------------------------------------------------------
//  Metadata tables
// ---------------------------------------------------------------------------

namespace {

const NodeRule kNodeRules[kNodeCount] = {
    // key              resource             hp    yield respawn radius hands handDmg tools
    {"node.tree_big",    ResourceType::Wood,   110.0f, 5, 120.0f, 38.0f, false, 5.0f,
     {ToolType::StoneAxe, ToolType::IronAxe, ToolType::Count}, 2},
    {"node.tree_normal", ResourceType::Wood,    70.0f, 3,  95.0f, 30.0f, false, 5.0f,
     {ToolType::StoneAxe, ToolType::IronAxe, ToolType::Count}, 2},
    {"node.tree_small",  ResourceType::Wood,    40.0f, 2,  80.0f, 23.0f, false, 5.0f,
     {ToolType::StoneAxe, ToolType::IronAxe, ToolType::Count}, 2},
    {"node.stone",       ResourceType::Stone,   60.0f, 2, 100.0f, 20.0f, false, 5.0f,
     {ToolType::StoneAxe, ToolType::IronPick, ToolType::Count}, 2},
    {"node.iron",        ResourceType::Iron,    80.0f, 2, 120.0f, 20.0f, false, 5.0f,
     {ToolType::IronPick, ToolType::Count, ToolType::Count}, 1},
    {"node.sulfur",      ResourceType::Sulfur,  70.0f, 2, 120.0f, 18.0f, false, 5.0f,
     {ToolType::IronPick, ToolType::Count, ToolType::Count}, 1},
    {"node.cloth",       ResourceType::Cloth,   25.0f, 1,  70.0f, 17.0f, true,  5.0f,
     {ToolType::Count, ToolType::Count, ToolType::Count}, 0},
    {"node.berry",       ResourceType::Berry,   15.0f, 2,  55.0f, 15.0f, true,  5.0f,
     {ToolType::Count, ToolType::Count, ToolType::Count}, 0},
};

const ToolRule kToolRules[kToolCount] = {
    {"tool.stone_axe", 100.0f, 12.0f, "tool.stone_axe.desc"},
    {"tool.iron_axe",  140.0f, 22.0f, "tool.iron_axe.desc"},
    {"tool.iron_pick", 150.0f, 16.0f, "tool.iron_pick.desc"},
    {"tool.torch",      60.0f, 10.0f, "tool.torch.desc"},
};

const ArmorRule kArmorRules[kArmorCount] = {
    {"armor.helmet",    25.0f, 10.0f, 1.0f},
    {"armor.bib",       50.0f, 20.0f, 4.0f},
    {"armor.leggings",  30.0f, 15.0f, 3.0f},
    {"armor.boots",     18.0f,  5.0f, 1.0f},
};

const AnimalRule kAnimalRules[kAnimalCount] = {
    {"animal.mouse",  8.0f,  0.0f, 150.0f,   0.0f,  0.0f, 1.0f,  9.0f, false,
     {DropRule{ResourceType::Meat, 1, 1}, DropRule{ResourceType::Bone, 1, 1}}, 2},
    {"animal.deer",  34.0f,  0.0f, 255.0f,   0.0f,  0.0f, 1.0f, 16.0f, false,
     {DropRule{ResourceType::Meat, 3, 4}, DropRule{ResourceType::Bone, 1, 2},
      DropRule{ResourceType::Leather, 1, 1}, DropRule{ResourceType::Cloth, 1, 1}}, 4},
    {"animal.wolf",  55.0f, 10.0f, 205.0f, 300.0f, 56.0f, 1.3f, 18.0f, true,
     {DropRule{ResourceType::Meat, 2, 3}, DropRule{ResourceType::Bone, 2, 2},
      DropRule{ResourceType::Leather, 1, 2}, DropRule{ResourceType::Cloth, 1, 1}}, 4},
    {"animal.bear", 120.0f, 18.0f, 155.0f, 250.0f, 64.0f, 1.6f, 25.0f, true,
     {DropRule{ResourceType::Meat, 4, 5}, DropRule{ResourceType::Bone, 3, 3},
      DropRule{ResourceType::Leather, 2, 3}, DropRule{ResourceType::Cloth, 1, 2}}, 4},
};

const char* const kResourceKeys[kResourceCount] = {
    "res.wood", "res.stone", "res.iron", "res.sulfur",
    "res.cloth", "res.berry", "res.meat", "res.bone",
    "res.leather", "res.thread", "res.scrap", "res.water",
};

const char* const kToolKeys[kToolCount] = {
    "tool.stone_axe", "tool.iron_axe", "tool.iron_pick", "tool.torch",
};

const char* const kArmorKeys[kArmorCount] = {
    "armor.helmet", "armor.bib", "armor.leggings", "armor.boots",
};

const char* const kNodeKeys[kNodeCount] = {
    "node.tree_big", "node.tree_normal", "node.tree_small", "node.stone",
    "node.iron", "node.sulfur", "node.cloth", "node.berry",
};

const char* const kAnimalKeys[kAnimalCount] = {
    "animal.mouse", "animal.deer", "animal.wolf", "animal.bear",
};

const char* const kStatKeys[4] = {
    "stat.hp", "stat.food", "stat.water", "stat.energy",
};

const std::vector<Recipe>& buildRecipes() {
    static const std::vector<Recipe> recipes = [] {
        std::vector<Recipe> out;

        Recipe stoneAxe;
        stoneAxe.nameKey = "recipe.stone_axe";
        stoneAxe.descriptionKey = "recipe.stone_axe.desc";
        stoneAxe.cost = {{ResourceType::Wood, 5}, {ResourceType::Stone, 1}};
        stoneAxe.kind = RecipeKind::Tool;
        stoneAxe.tool = ToolType::StoneAxe;
        out.push_back(stoneAxe);

        Recipe ironAxe;
        ironAxe.nameKey = "recipe.iron_axe";
        ironAxe.descriptionKey = "recipe.iron_axe.desc";
        ironAxe.cost = {{ResourceType::Wood, 5}, {ResourceType::Iron, 5}};
        ironAxe.kind = RecipeKind::Tool;
        ironAxe.tool = ToolType::IronAxe;
        out.push_back(ironAxe);

        Recipe ironPick;
        ironPick.nameKey = "recipe.iron_pick";
        ironPick.descriptionKey = "recipe.iron_pick.desc";
        ironPick.cost = {{ResourceType::Wood, 3}, {ResourceType::Iron, 5}};
        ironPick.kind = RecipeKind::Tool;
        ironPick.tool = ToolType::IronPick;
        out.push_back(ironPick);

        Recipe torch;
        torch.nameKey = "recipe.torch";
        torch.descriptionKey = "recipe.torch.desc";
        torch.cost = {{ResourceType::Wood, 1}, {ResourceType::Cloth, 1}};
        torch.kind = RecipeKind::Tool;
        torch.tool = ToolType::Torch;
        out.push_back(torch);

        Recipe threads;
        threads.nameKey = "recipe.threads";
        threads.descriptionKey = "recipe.threads.desc";
        threads.cost = {{ResourceType::Cloth, 5}};
        threads.kind = RecipeKind::Resource;
        threads.resource = ResourceType::Thread;
        threads.amount = 5;
        out.push_back(threads);

        Recipe helmet;
        helmet.nameKey = "recipe.helmet";
        helmet.descriptionKey = "recipe.helmet.desc";
        helmet.cost = {{ResourceType::Cloth, 3}, {ResourceType::Thread, 3},
                       {ResourceType::Leather, 3}};
        helmet.kind = RecipeKind::Armor;
        helmet.armor = ArmorType::Helmet;
        out.push_back(helmet);

        Recipe bib;
        bib.nameKey = "recipe.bib";
        bib.descriptionKey = "recipe.bib.desc";
        bib.cost = {{ResourceType::Cloth, 5}, {ResourceType::Thread, 6},
                    {ResourceType::Leather, 10}};
        bib.kind = RecipeKind::Armor;
        bib.armor = ArmorType::Bib;
        out.push_back(bib);

        Recipe leggings;
        leggings.nameKey = "recipe.leggings";
        leggings.descriptionKey = "recipe.leggings.desc";
        leggings.cost = {{ResourceType::Cloth, 4}, {ResourceType::Thread, 5},
                         {ResourceType::Leather, 4}};
        leggings.kind = RecipeKind::Armor;
        leggings.armor = ArmorType::Leggings;
        out.push_back(leggings);

        Recipe boots;
        boots.nameKey = "recipe.boots";
        boots.descriptionKey = "recipe.boots.desc";
        boots.cost = {{ResourceType::Cloth, 3}, {ResourceType::Thread, 3},
                      {ResourceType::Leather, 5}};
        boots.kind = RecipeKind::Armor;
        boots.armor = ArmorType::Boots;
        out.push_back(boots);

        return out;
    }();
    return recipes;
}

} // namespace

// ---------------------------------------------------------------------------
//  Rule accessors
// ---------------------------------------------------------------------------

const NodeRule& nodeRule(NodeType type) {
    const int value = clampi(idx(type), 0, kNodeCount - 1);
    return kNodeRules[value];
}

const ToolRule& toolRule(ToolType type) {
    const int value = clampi(idx(type), 0, kToolCount - 1);
    return kToolRules[value];
}

const ArmorRule& armorRule(ArmorType type) {
    const int value = clampi(idx(type), 0, kArmorCount - 1);
    return kArmorRules[value];
}

const AnimalRule& animalRule(AnimalType type) {
    const int value = clampi(idx(type), 0, kAnimalCount - 1);
    return kAnimalRules[value];
}

const char* resourceKey(ResourceType type) {
    return kResourceKeys[clampi(idx(type), 0, kResourceCount - 1)];
}

const char* toolKey(ToolType type) {
    return kToolKeys[clampi(idx(type), 0, kToolCount - 1)];
}

const char* armorKey(ArmorType type) {
    return kArmorKeys[clampi(idx(type), 0, kArmorCount - 1)];
}

const char* nodeKey(NodeType type) {
    return kNodeKeys[clampi(idx(type), 0, kNodeCount - 1)];
}

const char* animalKey(AnimalType type) {
    return kAnimalKeys[clampi(idx(type), 0, kAnimalCount - 1)];
}

const char* statKey(StatType type) {
    return kStatKeys[clampi(idx(type), 0, 3)];
}

// ---------------------------------------------------------------------------
//  String ids used by the save file
// ---------------------------------------------------------------------------

namespace {

const char* const kResourceIds[kResourceCount] = {
    "wood", "stone", "iron", "sulfur", "cloth", "berry",
    "meat", "bone", "leather", "thread", "scrap", "water",
};

struct NamedValue {
    const char* id;
    int value;
};

const NamedValue kToolIds[kToolCount] = {
    {"stone_axe", idx(ToolType::StoneAxe)},
    {"iron_axe", idx(ToolType::IronAxe)},
    {"iron_pick", idx(ToolType::IronPick)},
    {"torch", idx(ToolType::Torch)},
};

const NamedValue kArmorIds[kArmorCount] = {
    {"helmet", idx(ArmorType::Helmet)},
    {"bib", idx(ArmorType::Bib)},
    {"leggings", idx(ArmorType::Leggings)},
    {"boots", idx(ArmorType::Boots)},
};

} // namespace

ResourceType resourceFromString(const std::string& value) {
    for (int i = 0; i < kResourceCount; ++i) {
        if (value == kResourceIds[i]) {
            return static_cast<ResourceType>(i);
        }
    }
    return ResourceType::Wood;
}

ToolType toolFromString(const std::string& value) {
    for (const NamedValue& entry : kToolIds) {
        if (value == entry.id) {
            return static_cast<ToolType>(entry.value);
        }
    }
    return ToolType::StoneAxe;
}

ArmorType armorFromString(const std::string& value) {
    for (const NamedValue& entry : kArmorIds) {
        if (value == entry.id) {
            return static_cast<ArmorType>(entry.value);
        }
    }
    return ArmorType::Helmet;
}

const char* toolToId(ToolType type) {
    return kToolIds[clampi(idx(type), 0, kToolCount - 1)].id;
}

const char* armorToId(ArmorType type) {
    return kArmorIds[clampi(idx(type), 0, kArmorCount - 1)].id;
}

const std::vector<Recipe>& recipeCatalog() {
    return buildRecipes();
}

} // namespace cr
