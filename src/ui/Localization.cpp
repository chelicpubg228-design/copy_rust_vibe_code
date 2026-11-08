#include "Localization.h"

#include <cstdio>
#include <unordered_map>

namespace ui {

namespace {

struct Entry {
    const char* ru;
    const char* en;
};

const std::unordered_map<std::string, Entry>& strings() {
    static const std::unordered_map<std::string, Entry> table = {
        // ---- resources -------------------------------------------------
        {"res.wood", {"Дерево", "Wood"}},
        {"res.stone", {"Камень", "Stone"}},
        {"res.iron", {"Железо", "Iron"}},
        {"res.sulfur", {"Сера", "Sulfur"}},
        {"res.cloth", {"Ткань", "Cloth"}},
        {"res.berry", {"Ягоды", "Berries"}},
        {"res.meat", {"Мясо", "Meat"}},
        {"res.bone", {"Кости", "Bone"}},
        {"res.leather", {"Кожа", "Leather"}},
        {"res.thread", {"Нитки", "Thread"}},
        {"res.scrap", {"Металлолом", "Scrap"}},
        {"res.water", {"Вода", "Water"}},

        // ---- tools -----------------------------------------------------
        {"tool.stone_axe", {"Каменный топор", "Stone axe"}},
        {"tool.iron_axe", {"Железный топор", "Iron axe"}},
        {"tool.iron_pick", {"Железная кирка", "Iron pick"}},
        {"tool.torch", {"Факел", "Torch"}},
        {"tool.stone_axe.desc",
         {"Надёжный топор для дерева и камня.", "A reliable axe for wood and stone."}},
        {"tool.iron_axe.desc",
         {"Быстрее и прочнее каменного.", "Faster and sturdier than stone."}},
        {"tool.iron_pick.desc",
         {"Добывает железо и серу.", "Mines iron and sulfur."}},
        {"tool.torch.desc",
         {"Освещает ночь и годится как оружие.", "Lights the night and works as a weapon."}},

        // ---- armor -----------------------------------------------------
        {"armor.helmet", {"Кожаный шлем", "Leather helmet"}},
        {"armor.bib", {"Кожаный нагрудник", "Leather chestplate"}},
        {"armor.leggings", {"Кожаные поножи", "Leather leggings"}},
        {"armor.boots", {"Кожаные ботинки", "Leather boots"}},

        // ---- nodes -----------------------------------------------------
        {"node.tree_big", {"Большое дерево", "Big tree"}},
        {"node.tree_normal", {"Дерево", "Tree"}},
        {"node.tree_small", {"Маленькое дерево", "Small tree"}},
        {"node.stone", {"Каменная глыба", "Stone outcrop"}},
        {"node.iron", {"Железная руда", "Iron ore"}},
        {"node.sulfur", {"Сера", "Sulfur deposit"}},
        {"node.cloth", {"Волокна ткани", "Cloth plant"}},
        {"node.berry", {"Ягодный куст", "Berry bush"}},

        // ---- animals ---------------------------------------------------
        {"animal.mouse", {"Мышь", "Mouse"}},
        {"animal.deer", {"Олень", "Deer"}},
        {"animal.wolf", {"Волк", "Wolf"}},
        {"animal.bear", {"Медведь", "Bear"}},

        // ---- stats -----------------------------------------------------
        {"stat.hp", {"Здоровье", "Health"}},
        {"stat.food", {"Еда", "Food"}},
        {"stat.water", {"Вода", "Water"}},
        {"stat.energy", {"Энергия", "Energy"}},

        // ---- recipes ---------------------------------------------------
        {"recipe.stone_axe", {"Каменный топор", "Stone axe"}},
        {"recipe.stone_axe.desc",
         {"5 дерева + 1 камень", "5 wood + 1 stone"}},
        {"recipe.iron_axe", {"Железный топор", "Iron axe"}},
        {"recipe.iron_axe.desc",
         {"5 дерева + 5 железа", "5 wood + 5 iron"}},
        {"recipe.iron_pick", {"Железная кирка", "Iron pick"}},
        {"recipe.iron_pick.desc",
         {"3 дерева + 5 железа", "3 wood + 5 iron"}},
        {"recipe.torch", {"Факел", "Torch"}},
        {"recipe.torch.desc",
         {"1 дерево + 1 ткань", "1 wood + 1 cloth"}},
        {"recipe.threads", {"Нитки", "Threads"}},
        {"recipe.threads.desc",
         {"5 ткани → 5 ниток", "5 cloth → 5 thread"}},
        {"recipe.helmet", {"Кожаный шлем", "Leather helmet"}},
        {"recipe.helmet.desc",
         {"3 ткани + 3 ниток + 3 кожи", "3 cloth + 3 thread + 3 leather"}},
        {"recipe.bib", {"Кожаный нагрудник", "Leather chestplate"}},
        {"recipe.bib.desc",
         {"5 ткани + 6 ниток + 10 кожи", "5 cloth + 6 thread + 10 leather"}},
        {"recipe.leggings", {"Кожаные поножи", "Leather leggings"}},
        {"recipe.leggings.desc",
         {"4 ткани + 5 ниток + 4 кожи", "4 cloth + 5 thread + 4 leather"}},
        {"recipe.boots", {"Кожаные ботинки", "Leather boots"}},
        {"recipe.boots.desc",
         {"3 ткани + 3 ниток + 5 кожи", "3 cloth + 3 thread + 5 leather"}},

        // ---- common ----------------------------------------------------
        {"ui.on", {"Вкл", "On"}},
        {"ui.off", {"Выкл", "Off"}},
        {"ui.yes", {"Да", "Yes"}},
        {"ui.no", {"Нет", "No"}},
        {"ui.none", {"Нет", "None"}},
        {"ui.empty", {"Пусто", "Empty"}},
        {"ui.close", {"Закрыть", "Close"}},
        {"ui.back", {"Назад", "Back"}},
        {"ui.continue", {"Продолжить", "Continue"}},
        {"ui.min", {"мин", "min"}},

        // ---- HUD -------------------------------------------------------
        {"hud.hp", {"Здоровье", "Health"}},
        {"hud.food", {"Еда", "Food"}},
        {"hud.water", {"Вода", "Water"}},
        {"hud.energy", {"Энергия", "Energy"}},
        {"hud.day", {"День", "Day"}},
        {"hud.time", {"Время", "Time"}},
        {"hud.base", {"Лагерь", "Camp"}},
        {"hud.safe", {"Безопасная зона", "Safe zone"}},
        {"hud.wild", {"Дикая зона", "Wilderness"}},
        {"hud.tools", {"Инструменты", "Tools"}},
        {"hud.inventory", {"Инвентарь", "Inventory"}},
        {"hud.craft", {"Крафт", "Crafting"}},
        {"hud.help", {"Помощь", "Help"}},
        {"hud.rest", {"Отдых", "Rest"}},
        {"hud.eat", {"Съесть ягоду", "Eat berry"}},
        {"hud.selected", {"Выбрано", "Selected"}},
        {"hud.durability", {"Прочность", "Durability"}},
        {"hud.position", {"Позиция", "Position"}},
        {"hud.stats", {"Статистика", "Statistics"}},
        {"hud.kills", {"Убийства", "Kills"}},
        {"hud.gathered", {"Собрано", "Gathered"}},
        {"hud.crafted", {"Создано", "Crafted"}},
        {"hud.deaths", {"Смерти", "Deaths"}},
        {"hud.autosave", {"Автосохранение", "Autosave"}},
        {"hud.notifications", {"Уведомления", "Notifications"}},
        {"hud.controls", {"Управление", "Controls"}},
        {"hud.paused", {"Пауза", "Paused"}},
        {"hud.saved", {"Сохранено", "Saved"}},
        {"hud.loaded", {"Загружено", "Loaded"}},
        {"hud.no_save", {"Нет сохранения", "No save file"}},
        {"hud.survival", {"Выживание", "Survival"}},
        {"hud.armor", {"Броня", "Armor"}},
        {"hud.defense", {"Защита", "Defense"}},
        {"hud.world", {"Мир", "World"}},
        {"hud.minimap", {"Карта", "Map"}},
        {"hud.objective", {"Цель", "Objective"}},
        {"hud.hint", {"Подсказка", "Hint"}},
        {"hud.campfire", {"Костёр", "Campfire"}},

        // ---- interaction prompts --------------------------------------
        {"prompt.gather", {"Space — собрать", "Space — gather"}},
        {"prompt.attack", {"Space — атаковать", "Space — attack"}},
        {"prompt.too_far", {"Слишком далеко", "Too far away"}},
        {"prompt.rest", {"R — отдохнуть у костра", "R — rest at the campfire"}},
        {"prompt.craft", {"C — открыть крафт", "C — open crafting"}},
        {"prompt.inventory", {"I — инвентарь", "I — inventory"}},
        {"prompt.click", {"ЛКМ — идти / собрать / атаковать",
                          "LMB — walk / gather / attack"}},
        {"prompt.move", {"WASD — движение", "WASD — move"}},
        {"prompt.space", {"Space — взаимодействие", "Space — interact"}},
        {"prompt.mouse", {"ЛКМ — действие", "LMB — action"}},
        {"prompt.map", {"M — карта", "M — map"}},

        // ---- controls overlay -----------------------------------------
        {"controls.title", {"Управление", "Controls"}},
        {"controls.move", {"WASD / стрелки — движение", "WASD / arrows — move"}},
        {"controls.sprint", {"Shift — бег (тратит энергию)",
                             "Shift — sprint (uses energy)"}},
        {"controls.interact", {"Space / E — собрать или атаковать",
                               "Space / E — gather or attack"}},
        {"controls.attack", {"ЛКМ — идти к цели и атаковать",
                             "LMB — walk to target and attack"}},
        {"controls.craft", {"C — крафт", "C — crafting"}},
        {"controls.inventory", {"I — инвентарь", "I — inventory"}},
        {"controls.rest", {"R — отдых у лагеря", "R — rest at camp"}},
        {"controls.eat", {"F — съесть ягоду", "F — eat a berry"}},
        {"controls.tools", {"1–4 — выбрать инструмент", "1–4 — select tool"}},
        {"controls.cycle", {"Tab — сменить инструмент", "Tab — cycle tool"}},
        {"controls.map", {"M — карта", "M — map"}},
        {"controls.save", {"F5 — сохранить", "F5 — save"}},
        {"controls.load", {"F9 — загрузить", "F9 — load"}},
        {"controls.help", {"H — помощь", "H — help"}},
        {"controls.pause", {"Esc — пауза", "Esc — pause"}},
        {"controls.fullscreen", {"F11 — полный экран", "F11 — fullscreen"}},
        {"controls.mouse", {"Колесо мыши — зум", "Wheel — zoom"}},
        {"controls.close", {"Esc / H — закрыть", "Esc / H — close"}},
        {"controls.tutorial", {"F1 — подсказки", "F1 — hints"}},

        // ---- tutorial --------------------------------------------------
        {"tutorial.title", {"Первые шаги", "First steps"}},
        {"tutorial.step1",
         {"Подойдите к дереву и нажмите Space, чтобы собрать древесину.",
          "Walk to a tree and press Space to gather wood."}},
        {"tutorial.step2",
         {"Соберите 5 дерева и 1 камень. Откройте крафт (C) и создайте каменный топор.",
          "Gather 5 wood and 1 stone. Open crafting (C) and make a stone axe."}},
        {"tutorial.step3",
         {"Найдите ягодный куст и съешьте ягоду (F), чтобы восстановить еду.",
          "Find a berry bush and eat a berry (F) to restore food."}},
        {"tutorial.step4",
         {"Охотьтесь на оленей и волков, затем создайте кожаную броню.",
          "Hunt deer and wolves, then craft leather armor."}},
        {"tutorial.step5",
         {"Возвращайтесь в лагерь (R), чтобы восстановить энергию.",
          "Return to camp (R) to restore energy."}},
        {"tutorial.step6",
         {"Исследуйте остров. Опасные звери живут дальше от лагеря.",
          "Explore the island. Dangerous animals live farther from camp."}},
        {"tutorial.done", {"Обучение завершено. Удачи!",
                           "Tutorial complete. Good luck!"}},
        {"tutorial.skip", {"F1 — скрыть подсказки", "F1 — hide hints"}},

        // ---- pause menu ------------------------------------------------
        {"menu.title", {"Пауза", "Paused"}},
        {"menu.resume", {"Продолжить", "Resume"}},
        {"menu.new_game", {"Новая игра", "New game"}},
        {"menu.save", {"Сохранить", "Save"}},
        {"menu.load", {"Загрузить", "Load"}},
        {"menu.help", {"Помощь", "Help"}},
        {"menu.settings", {"Настройки", "Settings"}},
        {"menu.quit", {"Выход", "Quit"}},
        {"menu.language", {"Язык", "Language"}},
        {"menu.fullscreen", {"Полный экран", "Fullscreen"}},
        {"menu.tutorial", {"Обучение", "Tutorial"}},
        {"menu.back", {"Назад", "Back"}},
        {"menu.confirm_new",
         {"Начать новую игру? Несохранённый прогресс будет потерян.",
          "Start a new game? Unsaved progress will be lost."}},
        {"menu.confirm_yes", {"Да, начать", "Yes, start"}},
        {"menu.confirm_no", {"Отмена", "Cancel"}},
        {"menu.saved", {"Игра сохранена", "Game saved"}},
        {"menu.loaded", {"Игра загружена", "Game loaded"}},
        {"menu.save_failed", {"Не удалось сохранить", "Save failed"}},
        {"menu.load_failed", {"Не удалось загрузить", "Load failed"}},
        {"menu.no_save", {"Файл сохранения не найден", "Save file not found"}},
        {"menu.new_started", {"Новая игра начата", "New game started"}},
        {"menu.settings_title", {"Настройки", "Settings"}},
        {"menu.language_ru", {"Русский", "Russian"}},
        {"menu.language_en", {"Английский", "English"}},

        // ---- crafting panel -------------------------------------------
        {"craft.title", {"Крафт", "Crafting"}},
        {"craft.hint",
         {"1–9 или ЛКМ — создать · C/Esc — закрыть",
          "1–9 or LMB — craft · C/Esc — close"}},
        {"craft.cost", {"Стоимость", "Cost"}},
        {"craft.available", {"Доступно", "Available"}},
        {"craft.missing", {"Не хватает", "Missing"}},
        {"craft.empty", {"Нет рецептов", "No recipes"}},
        {"craft.created", {"Создано", "Created"}},
        {"craft.select", {"Выберите рецепт", "Select a recipe"}},
        {"craft.tool", {"Инструмент", "Tool"}},
        {"craft.armor", {"Броня", "Armor"}},
        {"craft.resource", {"Ресурс", "Resource"}},

        // ---- inventory panel ------------------------------------------
        {"inv.title", {"Инвентарь", "Inventory"}},
        {"inv.resources", {"Ресурсы", "Resources"}},
        {"inv.tools", {"Инструменты", "Tools"}},
        {"inv.armor", {"Броня и защита", "Armor & protection"}},
        {"inv.equipped", {"Надето", "Equipped"}},
        {"inv.not_equipped", {"Не надето", "Not equipped"}},
        {"inv.durability", {"Прочность", "Durability"}},
        {"inv.select", {"Выбрать", "Select"}},
        {"inv.equip", {"Надеть", "Equip"}},
        {"inv.unequip", {"Снять", "Unequip"}},
        {"inv.empty", {"Пусто", "Empty"}},
        {"inv.hint",
         {"ЛКМ по броне — надеть/снять · I/Esc — закрыть",
          "Click armor to equip/unequip · I/Esc — close"}},
        {"inv.defense", {"Защита", "Defense"}},
        {"inv.maxhp", {"Макс. HP", "Max HP"}},

        // ---- help ------------------------------------------------------
        {"help.title", {"Помощь", "Help"}},
        {"help.goal",
         {"Цель: выжить, собрать ресурсы, создать инструменты и броню.",
          "Goal: survive, gather resources, craft tools and armor."}},
        {"help.survival",
         {"Голод и жажда медленно падают. Ешьте ягоды и отдыхайте у лагеря.",
          "Hunger and thirst fall slowly. Eat berries and rest at camp."}},
        {"help.safe",
         {"В безопасной зоне у лагеря звери не атакуют.",
          "Animals do not attack inside the camp safe zone."}},
        {"help.tools",
         {"Инструменты изнашиваются. Создавайте новые.",
          "Tools wear out. Craft new ones."}},
        {"help.save",
         {"Игра сохраняется автоматически каждые 60 секунд и при выходе.",
          "The game autosaves every 60 seconds and on exit."}},
        {"help.close", {"H / Esc — закрыть", "H / Esc — close"}},

        // ---- notifications --------------------------------------------
        {"notify.gather", {"+{amount} {resource}", "+{amount} {resource}"}},
        {"notify.depleted", {"{node} истощено", "{node} depleted"}},
        {"notify.respawned", {"{node} выросло снова", "{node} regrew"}},
        {"notify.need_tool",
         {"Нужен {tool}, чтобы добыть {node}",
          "Need {tool} to gather {node}"}},
        {"notify.nothing", {"Рядом ничего нет", "Nothing nearby"}},
        {"notify.hit_animal", {"Удар по {animal}: {amount}",
                               "Hit {animal} for {amount}"}},
        {"notify.killed", {"Убит {animal}", "Killed {animal}"}},
        {"notify.animal_attack", {"{animal} нанёс {amount} урона",
                                  "{animal} hit you for {amount}"}},
        {"notify.too_far", {"{animal} слишком далеко", "{animal} is too far"}},
        {"notify.animal_respawn", {"{animal} вернулся", "{animal} returned"}},
        {"notify.hurt", {"-{amount} HP", "-{amount} HP"}},
        {"notify.died",
         {"Вы погибли! Нажмите R, чтобы возродиться",
          "You died! Press R to respawn"}},
        {"notify.respawn", {"Вы возродились в лагере",
                            "You respawned at camp"}},
        {"notify.healed", {"+{amount} HP", "+{amount} HP"}},
        {"notify.low", {"{stat} на исходе!", "{stat} is low!"}},
        {"notify.crafted", {"Создано: {recipe}", "Crafted: {recipe}"}},
        {"notify.missing", {"Не хватает {amount} × {resource}",
                            "Need {amount} more {resource}"}},
        {"notify.unknown_recipe", {"Неизвестный рецепт", "Unknown recipe"}},
        {"notify.equipped", {"Надето: {armor}", "Equipped: {armor}"}},
        {"notify.unequipped", {"Снято: {armor}", "Removed: {armor}"}},
        {"notify.armor_broken", {"Сломано: {armor}", "Broke: {armor}"}},
        {"notify.already_equipped", {"Уже надето", "Already equipped"}},
        {"notify.no_armor", {"Нет предмета: {armor}",
                             "No {armor} in inventory"}},
        {"notify.not_worn", {"Это не надето", "That is not worn"}},
        {"notify.tool_broken", {"Сломан: {tool}", "Broke: {tool}"}},
        {"notify.tool_selected", {"Выбрано: {tool}", "Selected: {tool}"}},
        {"notify.tool_not_owned", {"Нет инструмента: {tool}",
                                   "You don't own {tool}"}},
        {"notify.no_berries", {"Нет ягод", "No berries"}},
        {"notify.ate", {"Съедена ягода", "Ate a berry"}},
        {"notify.rest", {"Отдых: +{amount} энергии",
                         "Rested: +{amount} energy"}},
        {"notify.rest_full", {"Энергия уже полная", "Energy is already full"}},
        {"notify.rest_base", {"Отдыхать можно только в лагере",
                              "Rest only works at camp"}},
        {"notify.new_day", {"День {amount}", "Day {amount}"}},
        {"notify.saved", {"Игра сохранена", "Game saved"}},
        {"notify.loaded", {"Игра загружена", "Game loaded"}},
        {"notify.save_failed", {"Ошибка сохранения", "Save failed"}},
        {"notify.load_failed", {"Ошибка загрузки", "Load failed"}},

        // ---- map -------------------------------------------------------
        {"map.title", {"Карта острова", "Island map"}},
        {"map.you", {"Вы", "You"}},
        {"map.camp", {"Лагерь", "Camp"}},
        {"map.legend", {"Легенда", "Legend"}},
        {"map.nodes", {"Ресурсы", "Resources"}},
        {"map.animals", {"Животные", "Animals"}},

        // ---- death -----------------------------------------------------
        {"death.title", {"Вы погибли", "You died"}},
        {"death.hint", {"Нажмите R, чтобы возродиться в лагере",
                        "Press R to respawn at camp"}},
        {"death.respawn", {"Возродиться", "Respawn"}},
        {"death.stats", {"Собрано: {gathered} · Убийств: {kills}",
                         "Gathered: {gathered} · Kills: {kills}"}},
    };
    return table;
}

const char* pick(const Entry& entry, cr::Language language) {
    return language == cr::Language::English ? entry.en : entry.ru;
}

std::string keyOf(cr::ResourceType type) { return cr::resourceKey(type); }
std::string keyOf(cr::ToolType type) { return cr::toolKey(type); }
std::string keyOf(cr::ArmorType type) { return cr::armorKey(type); }
std::string keyOf(cr::NodeType type) { return cr::nodeKey(type); }
std::string keyOf(cr::AnimalType type) { return cr::animalKey(type); }
std::string keyOf(cr::StatType type) { return cr::statKey(type); }

} // namespace

std::string tr(const std::string& key, cr::Language language) {
    const auto& table = strings();
    const auto it = table.find(key);
    if (it == table.end()) {
        return key;
    }
    return pick(it->second, language);
}

std::string resourceName(cr::ResourceType type, cr::Language language) {
    return tr(keyOf(type), language);
}

std::string toolName(cr::ToolType type, cr::Language language) {
    return tr(keyOf(type), language);
}

std::string armorName(cr::ArmorType type, cr::Language language) {
    return tr(keyOf(type), language);
}

std::string nodeName(cr::NodeType type, cr::Language language) {
    return tr(keyOf(type), language);
}

std::string animalName(cr::AnimalType type, cr::Language language) {
    return tr(keyOf(type), language);
}

std::string statName(cr::StatType type, cr::Language language) {
    return tr(keyOf(type), language);
}

std::string recipeName(int index, cr::Language language) {
    const std::vector<cr::Recipe>& recipes = cr::recipeCatalog();
    if (index < 0 || index >= static_cast<int>(recipes.size())) {
        return {};
    }
    return tr(recipes[static_cast<std::size_t>(index)].nameKey, language);
}

std::string recipeDescription(int index, cr::Language language) {
    const std::vector<cr::Recipe>& recipes = cr::recipeCatalog();
    if (index < 0 || index >= static_cast<int>(recipes.size())) {
        return {};
    }
    return tr(recipes[static_cast<std::size_t>(index)].descriptionKey,
              language);
}

std::string clockString(float timeOfDay) {
    float value = timeOfDay - static_cast<int>(timeOfDay);
    if (value < 0.0f) {
        value += 1.0f;
    }
    const int totalMinutes = static_cast<int>(value * 24.0f * 60.0f) % (24 * 60);
    const int hours = totalMinutes / 60;
    const int minutes = totalMinutes % 60;
    char buffer[8] = {};
    std::snprintf(buffer, sizeof(buffer), "%02d:%02d", hours, minutes);
    return buffer;
}

} // namespace ui
