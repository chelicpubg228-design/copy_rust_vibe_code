#pragma once

#include "core/GameTypes.h"

#include <string>

namespace ui {

// Every user-facing string lives in Localization.cpp.  The simulation only
// emits stable keys, which keeps the core independent from the UI language.
std::string tr(const std::string& key, cr::Language language);

std::string resourceName(cr::ResourceType type, cr::Language language);
std::string toolName(cr::ToolType type, cr::Language language);
std::string armorName(cr::ArmorType type, cr::Language language);
std::string nodeName(cr::NodeType type, cr::Language language);
std::string animalName(cr::AnimalType type, cr::Language language);
std::string statName(cr::StatType type, cr::Language language);
std::string recipeName(int index, cr::Language language);
std::string recipeDescription(int index, cr::Language language);

// "06:45" for a time-of-day value in [0, 1).
std::string clockString(float timeOfDay);

} // namespace ui
