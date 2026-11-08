#pragma once

#include "core/Game.h"

#include <string>

namespace console {

class ConsoleApp {
public:
    ConsoleApp();

    int run();

private:
    void printHeader() const;
    void printStatus() const;
    void printHelp() const;
    void printInventory() const;
    void printRecipes() const;
    void printLook() const;
    bool handle(const std::string& line);
    void tick(float seconds);
    void notifyEvents();
    void save();
    void load();

    std::string tr(const std::string& key) const;
    std::string resource(cr::ResourceType type) const;
    std::string tool(cr::ToolType type) const;
    std::string armor(cr::ArmorType type) const;
    std::string node(cr::NodeType type) const;
    std::string animal(cr::AnimalType type) const;

    cr::Game game_;
    std::string savePath_ = "savegame.json";
    bool running_ = true;
    bool showHelp_ = true;
};

} // namespace console
