#include "ConsoleApp.h"

#include "ui/Localization.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace console {

namespace {

std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) {
                       return static_cast<char>(std::tolower(c));
                   });
    return value;
}

std::string formatInt(const std::string& pattern, const std::string& key,
                      int value) {
    std::string result = pattern;
    const std::string token = "{" + key + "}";
    const std::size_t position = result.find(token);
    if (position != std::string::npos) {
        result.replace(position, token.size(), std::to_string(value));
    }
    return result;
}

std::string formatText(const std::string& pattern, const std::string& key,
                       const std::string& value) {
    std::string result = pattern;
    const std::string token = "{" + key + "}";
    std::size_t position = 0;
    while ((position = result.find(token, position)) != std::string::npos) {
        result.replace(position, token.size(), value);
        position += value.size();
    }
    return result;
}

std::string compass(cr::Vec2 delta) {
    const float ax = std::fabs(delta.x);
    const float ay = std::fabs(delta.y);
    if (ax < 0.001f && ay < 0.001f) {
        return "here";
    }
    if (ax > ay * 1.8f) {
        return delta.x > 0.0f ? "east" : "west";
    }
    if (ay > ax * 1.8f) {
        return delta.y > 0.0f ? "south" : "north";
    }
    if (delta.x > 0.0f) {
        return delta.y > 0.0f ? "south-east" : "north-east";
    }
    return delta.y > 0.0f ? "south-west" : "north-west";
}

} // namespace

ConsoleApp::ConsoleApp() {
    if (std::filesystem::exists(savePath_)) {
        game_.loadFromFile(savePath_);
        game_.clearEvents();
    } else {
        game_.newRun(cr::Game::makeSeed(), "Player");
    }
}

int ConsoleApp::run() {
    printHeader();
    printHelp();
    printStatus();

    std::string line;
    while (running_) {
        std::cout << "\n> " << std::flush;
        if (!std::getline(std::cin, line)) {
            break;
        }
        if (line.empty()) {
            continue;
        }
        handle(line);
    }

    save();
    return 0;
}

std::string ConsoleApp::tr(const std::string& key) const {
    return ui::tr(key, game_.settings.language);
}

std::string ConsoleApp::resource(cr::ResourceType type) const {
    return ui::resourceName(type, game_.settings.language);
}

std::string ConsoleApp::tool(cr::ToolType type) const {
    return ui::toolName(type, game_.settings.language);
}

std::string ConsoleApp::armor(cr::ArmorType type) const {
    return ui::armorName(type, game_.settings.language);
}

std::string ConsoleApp::node(cr::NodeType type) const {
    return ui::nodeName(type, game_.settings.language);
}

std::string ConsoleApp::animal(cr::AnimalType type) const {
    return ui::animalName(type, game_.settings.language);
}

void ConsoleApp::printHeader() const {
    std::cout << "==============================================\n";
    std::cout << "  Copy Rust 2D - console edition\n";
    std::cout << "  " << tr("help.goal") << "\n";
    std::cout << "==============================================\n";
}

void ConsoleApp::printHelp() const {
    std::cout << "\n" << tr("controls.title") << ":\n";
    std::cout << "  status | st        - " << tr("hud.stats") << "\n";
    std::cout << "  look | l           - " << tr("hud.objective") << "\n";
    std::cout << "  go <n|s|e|w>       - " << tr("controls.move") << "\n";
    std::cout << "  gather | g         - " << tr("prompt.gather") << "\n";
    std::cout << "  attack | a         - " << tr("prompt.attack") << "\n";
    std::cout << "  inv | i            - " << tr("inv.title") << "\n";
    std::cout << "  craft | c          - " << tr("craft.title") << "\n";
    std::cout << "  craft <1-9>        - " << tr("craft.created") << "\n";
    std::cout << "  tool <1-4>|cycle   - " << tr("controls.tools") << "\n";
    std::cout << "  equip/unequip <p>  - " << tr("inv.armor") << "\n";
    std::cout << "  eat | f            - " << tr("hud.eat") << "\n";
    std::cout << "  rest | r           - " << tr("hud.rest") << "\n";
    std::cout << "  save | load        - F5 / F9\n";
    std::cout << "  lang ru|en         - " << tr("menu.language") << "\n";
    std::cout << "  new | respawn      - " << tr("menu.new_game") << "\n";
    std::cout << "  quit | q           - " << tr("menu.quit") << "\n";
}

void ConsoleApp::printStatus() const {
    std::cout << "\n----------------------------------------------\n";
    std::cout << game_.playerName << "   " << tr("hud.day") << " " << game_.day
              << "   " << ui::clockString(game_.timeOfDay) << "\n";

    const auto bar = [](const char* label, float value, float maximum) {
        const int width = 18;
        const int filled = static_cast<int>(
            std::round(std::clamp(value / maximum, 0.0f, 1.0f) * width));
        std::cout << "  " << label << " [";
        for (int i = 0; i < width; ++i) {
            std::cout << (i < filled ? '#' : '.');
        }
        std::cout << "] " << static_cast<int>(std::round(value)) << "/"
                  << static_cast<int>(std::round(maximum)) << "\n";
    };
    bar(tr("hud.hp").c_str(), game_.player.hp, game_.player.maxHp);
    bar(tr("hud.food").c_str(), game_.player.food, cr::kMaxStat);
    bar(tr("hud.water").c_str(), game_.player.water, cr::kMaxStat);
    bar(tr("hud.energy").c_str(), game_.player.energy, cr::kMaxStat);

    std::printf("  %s: %.0f, %.0f   %s\n", tr("hud.position").c_str(),
                game_.player.pos.x, game_.player.pos.y,
                game_.inBaseSafeZone(game_.player.pos) ? tr("hud.safe").c_str()
                                                       : tr("hud.wild").c_str());
    std::cout << "  " << tr("hud.selected") << ": "
              << (game_.inventory.selectedTool == cr::ToolType::Count
                      ? tr("ui.none")
                      : tool(game_.inventory.selectedTool))
              << "   " << tr("hud.armor") << ": "
              << static_cast<int>(std::round(game_.armorDefense())) << "\n";
    std::cout << "  " << tr("hud.kills") << ": " << game_.stats.kills << "   "
              << tr("hud.gathered") << ": " << game_.stats.gathered << "   "
              << tr("hud.crafted") << ": " << game_.stats.crafted << "\n";
    if (game_.dead) {
        std::cout << "  *** " << tr("death.title") << " ***\n";
        std::cout << "  " << tr("death.hint") << "\n";
    }
}

void ConsoleApp::printLook() const {
    std::cout << "\n" << tr("hud.objective") << ":\n";
    const cr::Node* nearestNode = game_.nearestNode(game_.player.pos, 420.0f);
    if (nearestNode != nullptr) {
        const float d = game_.distanceToPlayer(nearestNode->pos);
        std::cout << "  " << node(nearestNode->type) << "  "
                  << static_cast<int>(std::round(d)) << " m "
                  << compass(nearestNode->pos - game_.player.pos) << "\n";
    } else {
        std::cout << "  " << tr("notify.nothing") << "\n";
    }

    const cr::Animal* nearestAnimal =
        game_.nearestAnimal(game_.player.pos, 420.0f);
    if (nearestAnimal != nullptr) {
        const float d = game_.distanceToPlayer(nearestAnimal->pos);
        std::cout << "  " << animal(nearestAnimal->type) << "  "
                  << static_cast<int>(std::round(d)) << " m "
                  << compass(nearestAnimal->pos - game_.player.pos) << "\n";
    }

    if (game_.nearBase()) {
        std::cout << "  " << tr("hud.campfire") << " - " << tr("hud.rest")
                  << "\n";
    }
}

void ConsoleApp::printInventory() const {
    std::cout << "\n" << tr("inv.title") << ":\n";
    bool any = false;
    for (int i = 0; i < cr::kResourceCount; ++i) {
        const cr::ResourceType type = static_cast<cr::ResourceType>(i);
        if (game_.resource(type) > 0) {
            std::cout << "  " << resource(type) << ": "
                      << game_.resource(type) << "\n";
            any = true;
        }
    }
    if (!any) {
        std::cout << "  " << tr("inv.empty") << "\n";
    }

    std::cout << "  " << tr("inv.tools") << ": ";
    bool anyTool = false;
    for (int i = 0; i < cr::kToolCount; ++i) {
        const cr::ToolType type = static_cast<cr::ToolType>(i);
        if (game_.hasTool(type)) {
            std::cout << tool(type) << " ("
                      << static_cast<int>(game_.toolDurability(type)) << ") ";
            anyTool = true;
        }
    }
    if (!anyTool) {
        std::cout << tr("ui.none");
    }
    std::cout << "\n";

    std::cout << "  " << tr("inv.armor") << ": ";
    bool anyArmor = false;
    for (int i = 0; i < cr::kArmorCount; ++i) {
        const cr::ArmorType type = static_cast<cr::ArmorType>(i);
        if (game_.inventory.armorItems[cr::idx(type)] > 0) {
            std::cout << armor(type)
                      << (game_.inventory.armorWorn[cr::idx(type)]
                              ? " [" + tr("inv.equipped") + "]"
                              : "")
                      << " ";
            anyArmor = true;
        }
    }
    if (!anyArmor) {
        std::cout << tr("ui.none");
    }
    std::cout << "\n";
}

void ConsoleApp::printRecipes() const {
    std::cout << "\n" << tr("craft.title") << ":\n";
    const std::vector<cr::Recipe>& recipes = game_.recipes();
    for (int i = 0; i < static_cast<int>(recipes.size()); ++i) {
        const cr::Recipe& recipe = recipes[static_cast<std::size_t>(i)];
        std::cout << "  " << (i + 1) << ") "
                  << ui::recipeName(i, game_.settings.language) << "  [";
        for (std::size_t c = 0; c < recipe.cost.size(); ++c) {
            if (c > 0) {
                std::cout << ", ";
            }
            std::cout << recipe.cost[c].amount << " "
                      << resource(recipe.cost[c].resource);
        }
        std::cout << "]  "
                  << (game_.canCraft(recipe) ? tr("craft.available")
                                             : tr("craft.missing"))
                  << "\n";
    }
}

void ConsoleApp::tick(float seconds) {
    float remaining = seconds;
    while (remaining > 0.0f) {
        const float step = std::min(0.25f, remaining);
        game_.update(step);
        remaining -= step;
    }
    notifyEvents();
}

void ConsoleApp::notifyEvents() {
    for (const cr::GameEvent& event : game_.events) {
        const std::string& key = event.key;
        if (key == "node.hit") {
            std::cout << "  +" << event.amount << " "
                      << resource(event.resource) << "\n";
        } else if (key == "node.depleted") {
            std::cout << "  "
                      << formatText(tr("notify.depleted"), "node",
                                    node(event.node))
                      << "\n";
        } else if (key == "gather.need_tool") {
            std::cout << "  "
                      << formatText(formatText(tr("notify.need_tool"), "tool",
                                               tool(event.tool)),
                                    "node", node(event.node))
                      << "\n";
        } else if (key == "nothing_near") {
            std::cout << "  " << tr("notify.nothing") << "\n";
        } else if (key == "animal.hit") {
            std::cout << "  "
                      << formatInt(formatText(tr("notify.hit_animal"), "animal",
                                              animal(event.animal)),
                                   "amount", event.amount)
                      << "\n";
        } else if (key == "animal.killed") {
            std::cout << "  "
                      << formatText(tr("notify.killed"), "animal",
                                    animal(event.animal))
                      << "\n";
        } else if (key == "animal.attack") {
            std::cout << "  "
                      << formatInt(
                             formatText(tr("notify.animal_attack"), "animal",
                                        animal(event.animal)),
                             "amount", event.amount)
                      << "\n";
        } else if (key == "player.hurt") {
            std::cout << "  "
                      << formatInt(tr("notify.hurt"), "amount", event.amount)
                      << "\n";
        } else if (key == "player.died") {
            std::cout << "  " << tr("notify.died") << "\n";
        } else if (key == "player.healed") {
            std::cout << "  "
                      << formatInt(tr("notify.healed"), "amount", event.amount)
                      << "\n";
        } else if (key == "stat.low") {
            std::cout << "  "
                      << formatText(tr("notify.low"), "stat",
                                    ui::statName(event.stat,
                                                 game_.settings.language))
                      << "\n";
        } else if (key == "craft.ok") {
            std::cout << "  "
                      << formatText(tr("notify.crafted"), "recipe",
                                    ui::recipeName(event.recipe,
                                                   game_.settings.language))
                      << "\n";
        } else if (key == "craft.missing") {
            std::cout << "  "
                      << formatText(
                             formatInt(tr("notify.missing"), "amount",
                                       event.amount),
                             "resource", resource(event.resource))
                      << "\n";
        } else if (key == "armor.equipped") {
            std::cout << "  "
                      << formatText(tr("notify.equipped"), "armor",
                                    armor(event.armor))
                      << "\n";
        } else if (key == "armor.unequipped") {
            std::cout << "  "
                      << formatText(tr("notify.unequipped"), "armor",
                                    armor(event.armor))
                      << "\n";
        } else if (key == "armor.broken") {
            std::cout << "  "
                      << formatText(tr("notify.armor_broken"), "armor",
                                    armor(event.armor))
                      << "\n";
        } else if (key == "tool.broken") {
            std::cout << "  "
                      << formatText(tr("notify.tool_broken"), "tool",
                                    tool(event.tool))
                      << "\n";
        } else if (key == "tool.selected") {
            std::cout << "  "
                      << formatText(tr("notify.tool_selected"), "tool",
                                    tool(event.tool))
                      << "\n";
        } else if (key == "tool.not_owned") {
            std::cout << "  "
                      << formatText(tr("notify.tool_not_owned"), "tool",
                                    tool(event.tool))
                      << "\n";
        } else if (key == "eat.none") {
            std::cout << "  " << tr("notify.no_berries") << "\n";
        } else if (key == "ate.berry") {
            std::cout << "  " << tr("notify.ate") << "\n";
        } else if (key == "rest.ok") {
            std::cout << "  "
                      << formatInt(tr("notify.rest"), "amount", event.amount)
                      << "\n";
        } else if (key == "rest.full") {
            std::cout << "  " << tr("notify.rest_full") << "\n";
        } else if (key == "rest.need_base") {
            std::cout << "  " << tr("notify.rest_base") << "\n";
        } else if (key == "new_day") {
            std::cout << "  "
                      << formatInt(tr("notify.new_day"), "amount", event.amount)
                      << "\n";
        } else if (key == "saved") {
            std::cout << "  " << tr("notify.saved") << "\n";
        } else if (key == "loaded") {
            std::cout << "  " << tr("notify.loaded") << "\n";
        } else if (key == "save.failed") {
            std::cout << "  " << tr("notify.save_failed") << "\n";
        } else if (key == "load.failed") {
            std::cout << "  " << tr("notify.load_failed") << "\n";
        } else if (key == "respawned") {
            std::cout << "  " << tr("notify.respawn") << "\n";
        } else if (key == "node.respawned") {
            std::cout << "  "
                      << formatText(tr("notify.respawned"), "node",
                                    node(event.node))
                      << "\n";
        }
    }
    game_.clearEvents();
}

void ConsoleApp::save() {
    game_.saveToFile(savePath_);
    notifyEvents();
}

void ConsoleApp::load() {
    if (game_.loadFromFile(savePath_)) {
        notifyEvents();
    } else {
        std::cout << "  " << tr("menu.no_save") << "\n";
        game_.clearEvents();
    }
}

bool ConsoleApp::handle(const std::string& line) {
    std::istringstream stream(line);
    std::string command;
    stream >> command;
    command = lower(command);

    if (command == "quit" || command == "exit" || command == "q") {
        running_ = false;
        return false;
    }
    if (command == "help" || command == "h" || command == "?") {
        printHelp();
        return true;
    }
    if (command == "status" || command == "st") {
        printStatus();
        return true;
    }
    if (command == "look" || command == "l") {
        printLook();
        return true;
    }
    if (command == "inv" || command == "inventory" || command == "i") {
        printInventory();
        return true;
    }
    if (command == "craft" || command == "c") {
        int index = 0;
        if (stream >> index) {
            game_.craft(index - 1);
            tick(0.4f);
        } else {
            printRecipes();
        }
        return true;
    }
    if (command == "gather" || command == "g") {
        cr::Node* target =
            game_.nearestNode(game_.player.pos, cr::kInteractRange);
        if (target == nullptr) {
            std::cout << "  " << tr("notify.nothing") << "\n";
        } else {
            game_.gatherNode(*target);
            tick(0.4f);
        }
        return true;
    }
    if (command == "attack" || command == "a") {
        cr::Animal* target =
            game_.nearestAnimal(game_.player.pos, cr::kAttackRange);
        if (target == nullptr) {
            std::cout << "  " << tr("notify.nothing") << "\n";
        } else {
            game_.attackAnimal(*target);
            tick(0.4f);
        }
        return true;
    }
    if (command == "go" || command == "move") {
        std::string direction;
        stream >> direction;
        direction = lower(direction);
        cr::Vec2 delta{};
        if (direction == "n" || direction == "north") {
            delta = {0.0f, -1.0f};
        } else if (direction == "s" || direction == "south") {
            delta = {0.0f, 1.0f};
        } else if (direction == "e" || direction == "east") {
            delta = {1.0f, 0.0f};
        } else if (direction == "w" || direction == "west") {
            delta = {-1.0f, 0.0f};
        } else {
            std::cout << "  " << tr("controls.move") << "\n";
            return true;
        }
        game_.movePlayer(delta, 0.8f, false);
        tick(0.8f);
        return true;
    }
    if (command == "tool") {
        int index = 0;
        if (stream >> index) {
            game_.selectTool(static_cast<cr::ToolType>(index - 1));
            notifyEvents();
        }
        return true;
    }
    if (command == "cycle") {
        game_.cycleTool(1);
        notifyEvents();
        return true;
    }
    if (command == "equip" || command == "unequip") {
        std::string name;
        stream >> name;
        name = lower(name);
        cr::ArmorType type = cr::ArmorType::Helmet;
        if (name == "helmet" || name == "shlem") {
            type = cr::ArmorType::Helmet;
        } else if (name == "bib" || name == "chest" || name == "nagrudnik") {
            type = cr::ArmorType::Bib;
        } else if (name == "leggings" || name == "pants") {
            type = cr::ArmorType::Leggings;
        } else if (name == "boots") {
            type = cr::ArmorType::Boots;
        } else {
            std::cout << "  helmet | bib | leggings | boots\n";
            return true;
        }
        if (command == "equip") {
            game_.equipArmor(type);
        } else {
            game_.unequipArmor(type);
        }
        notifyEvents();
        return true;
    }
    if (command == "eat" || command == "f") {
        game_.eatBerry();
        tick(0.2f);
        return true;
    }
    if (command == "rest" || command == "r") {
        game_.restAtBase();
        tick(0.2f);
        return true;
    }
    if (command == "save") {
        save();
        return true;
    }
    if (command == "load") {
        load();
        return true;
    }
    if (command == "lang" || command == "language") {
        std::string value;
        stream >> value;
        value = lower(value);
        if (value == "en" || value == "english") {
            game_.settings.language = cr::Language::English;
        } else if (value == "ru" || value == "russian") {
            game_.settings.language = cr::Language::Russian;
        }
        std::cout << "  " << tr("menu.language") << ": "
                  << tr(game_.settings.language == cr::Language::Russian
                            ? "menu.language_ru"
                            : "menu.language_en")
                  << "\n";
        return true;
    }
    if (command == "new") {
        game_.newRun(cr::Game::makeSeed(), game_.playerName);
        game_.clearEvents();
        std::cout << "  " << tr("menu.new_started") << "\n";
        return true;
    }
    if (command == "respawn") {
        if (game_.dead) {
            game_.respawn();
            notifyEvents();
        }
        return true;
    }
    if (command == "space" || command == "interact") {
        game_.interact();
        tick(0.4f);
        return true;
    }

    std::cout << "  ?  " << command << "\n";
    return true;
}

} // namespace console
