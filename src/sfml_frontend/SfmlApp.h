#pragma once

#include <SFML/Graphics.hpp>

#include "WorldRenderer.h"
#include "core/Game.h"

#include <deque>
#include <string>
#include <vector>

namespace ui {

class SfmlApp {
public:
    SfmlApp();
    bool init();
    int run();

private:
    enum class Overlay {
        None,
        Craft,
        Inventory,
        Help,
        Pause,
        Map
    };

    struct Note {
        std::string text;
        sf::Color color;
        float ttl = 4.0f;
        float maxTtl = 4.0f;
    };

    struct Button {
        sf::FloatRect rect;
        std::string label;
        std::string action;
        bool enabled = true;
    };

    // ---- lifecycle -----------------------------------------------------
    void handleEvent(const sf::Event& event);
    void handleKey(sf::Keyboard::Key key, bool shift, bool control);
    void handleMousePressed(sf::Mouse::Button button, sf::Vector2i position);
    void handleWheel(float delta);
    void update(float dt);
    void updateViewSize();
    void updateCamera(float dt);
    void updateMouseHover();
    void updateAutoAction(float dt);
    void moveWithInput(float dt);

    // ---- rendering -----------------------------------------------------
    void render();
    void renderWorld();
    void renderHud();
    void renderHotbar();
    void renderMinimap(sf::Vector2f center, float radius);
    void renderPrompt();
    void renderTutorial();
    void renderNotifications();
    void renderDeath();
    void renderOverlays();
    void renderCraft();
    void renderInventory();
    void renderPause();
    void renderHelp();
    void renderMap();
    void renderConfirmNew();
    void drawButton(const Button& button, bool hovered);
    void drawToolIcon(sf::RenderTarget& target, cr::ToolType tool,
                      sf::Vector2f center, float scale, bool owned);
    void drawResourceDot(sf::RenderTarget& target, cr::ResourceType resource,
                         sf::Vector2f position, float radius);

    // ---- actions -------------------------------------------------------
    void saveGame();
    void loadGame();
    void newGame();
    void respawn();
    void toggleFullscreen();
    void setFullscreen(bool enabled);
    void closeOverlay();
    void processGameEvents();
    void pushNote(const std::string& text, sf::Color color,
                  float ttl = 4.2f);
    std::string describeEvent(const cr::GameEvent& event) const;
    std::string tutorialText() const;
    std::string currentPrompt(sf::Color* color = nullptr) const;
    std::string formatRecipeCost(const cr::Recipe& recipe) const;
    std::string toolLabel(cr::ToolType tool) const;

    // ---- layout --------------------------------------------------------
    sf::FloatRect centered(sf::Vector2f size) const;
    sf::FloatRect craftPanelRect() const;
    sf::FloatRect craftRowRect(int index) const;
    sf::FloatRect inventoryPanelRect() const;
    sf::FloatRect armorRowRect(int index) const;
    sf::FloatRect toolRowRect(int index) const;
    sf::FloatRect pausePanelRect() const;
    sf::FloatRect pauseButtonRect(int index) const;
    sf::FloatRect helpPanelRect() const;
    sf::FloatRect mapPanelRect() const;
    sf::FloatRect confirmPanelRect() const;
    sf::FloatRect confirmYesRect() const;
    sf::FloatRect confirmNoRect() const;
    std::vector<Button> pauseButtons() const;
    void handlePauseAction(const std::string& action);
    void handleCraftClick(sf::Vector2f point);
    void handleInventoryClick(sf::Vector2f point);
    void handlePauseClick(sf::Vector2f point);
    void handleConfirmClick(sf::Vector2f point);

    cr::Language language() const { return game_.settings.language; }

    // ---- state ---------------------------------------------------------
    sf::RenderWindow window_;
    sf::Font font_;
    cr::Game game_;
    WorldRenderer renderer_;
    sf::View worldView_;
    sf::Clock frameClock_;

    sf::Vector2f cameraCenter_{0.0f, 0.0f};
    sf::Vector2f mouseScreen_{0.0f, 0.0f};
    cr::Vec2 mouseWorld_{};

    std::deque<Note> notes_;
    std::vector<Button> pauseButtonsCache_;

    std::string savePath_ = "savegame.json";
    float zoom_ = 1.0f;
    float elapsed_ = 0.0f;
    float shake_ = 0.0f;
    float autoActionTimer_ = 0.0f;
    float autosaveTimer_ = 0.0f;
    float fps_ = 0.0f;

    int hoverNodeId_ = -1;
    int hoverAnimalId_ = -1;
    int autoNodeId_ = -1;
    int autoAnimalId_ = -1;
    bool hasMoveTarget_ = false;
    cr::Vec2 moveTarget_{};
    bool mouseHeld_ = false;
    bool running_ = true;
    bool fullscreen_ = false;
    bool confirmNew_ = false;

    Overlay overlay_ = Overlay::None;
};

} // namespace ui
