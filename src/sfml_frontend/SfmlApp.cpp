#include "SfmlApp.h"

#include "ui/Localization.h"
#include "Ui.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace ui {

namespace {

constexpr float kPi = 3.14159265358979323846f;
constexpr float kAutosaveSeconds = 60.0f;
constexpr float kClickRadius = 26.0f;

sf::Vector2f toSf(cr::Vec2 value) {
    return {value.x, value.y};
}

sf::Color withAlpha(sf::Color color, int alpha) {
    return sf::Color(color.r, color.g, color.b,
                     static_cast<std::uint8_t>(std::clamp(alpha, 0, 255)));
}

float textWidth(const sf::Font& font, const std::string& value, unsigned size,
                bool bold = false) {
    return makeText(font, value, size, sf::Color::White, bold)
        .getLocalBounds()
        .size.x;
}

sf::Color resourceColor(cr::ResourceType type) {
    switch (type) {
    case cr::ResourceType::Wood:
        return {156, 112, 66};
    case cr::ResourceType::Stone:
        return {150, 152, 158};
    case cr::ResourceType::Iron:
        return {182, 188, 200};
    case cr::ResourceType::Sulfur:
        return {214, 206, 96};
    case cr::ResourceType::Cloth:
        return {202, 188, 150};
    case cr::ResourceType::Berry:
        return {196, 64, 92};
    case cr::ResourceType::Meat:
        return {188, 92, 92};
    case cr::ResourceType::Bone:
        return {224, 220, 200};
    case cr::ResourceType::Leather:
        return {150, 104, 68};
    case cr::ResourceType::Thread:
        return {206, 206, 196};
    case cr::ResourceType::Scrap:
        return {130, 138, 148};
    case cr::ResourceType::Water:
        return {72, 148, 214};
    default:
        return {180, 180, 180};
    }
}

sf::Color animalColor(cr::AnimalType type) {
    switch (type) {
    case cr::AnimalType::Mouse:
        return {158, 154, 146};
    case cr::AnimalType::Deer:
        return {176, 126, 76};
    case cr::AnimalType::Wolf:
        return {126, 138, 152};
    case cr::AnimalType::Bear:
        return {150, 96, 62};
    default:
        return {160, 160, 160};
    }
}

cr::Vec2 readMoveAxis() {
    float x = 0.0f;
    float y = 0.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        x -= 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        x += 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        y -= 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
        y += 1.0f;
    }
    return {x, y};
}

bool isSprintHeld() {
    return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ||
           sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);
}

int numberFromKey(sf::Keyboard::Key key) {
    const int first = static_cast<int>(sf::Keyboard::Key::Num1);
    const int last = static_cast<int>(sf::Keyboard::Key::Num9);
    const int value = static_cast<int>(key);
    if (value >= first && value <= last) {
        return value - first + 1;
    }
    const int padFirst = static_cast<int>(sf::Keyboard::Key::Numpad1);
    const int padLast = static_cast<int>(sf::Keyboard::Key::Numpad9);
    if (value >= padFirst && value <= padLast) {
        return value - padFirst + 1;
    }
    return 0;
}

} // namespace

// ---------------------------------------------------------------------------
//  Lifecycle
// ---------------------------------------------------------------------------

SfmlApp::SfmlApp() = default;

bool SfmlApp::init() {
    window_.create(sf::VideoMode({1280, 720}), "Copy Rust 2D",
                   sf::Style::Default, sf::State::Windowed);
    if (!window_.isOpen()) {
        return false;
    }
    window_.setVerticalSyncEnabled(true);
    window_.setKeyRepeatEnabled(false);

    const char* fontCandidates[] = {
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "C:/Windows/Fonts/tahoma.ttf",
        "assets/font.ttf",
    };
    bool fontLoaded = false;
    for (const char* path : fontCandidates) {
        if (std::filesystem::exists(path) && font_.openFromFile(path)) {
            fontLoaded = true;
            break;
        }
    }
    if (!fontLoaded) {
        return false;
    }

    bool loadedSave = false;
    if (std::filesystem::exists(savePath_)) {
        loadedSave = game_.loadFromFile(savePath_);
    }
    if (!loadedSave) {
        game_.newRun(cr::Game::makeSeed(), "Player");
    }
    game_.clearEvents();

    fullscreen_ = game_.settings.fullscreen;
    if (fullscreen_) {
        setFullscreen(true);
    }

    cameraCenter_ = toSf(game_.player.pos);
    updateViewSize();
    frameClock_.restart();

    if (loadedSave) {
        pushNote(tr("notify.loaded", language()), theme().success);
    } else if (std::filesystem::exists(savePath_)) {
        pushNote(tr("menu.load_failed", language()), theme().danger);
    }
    pushNote(tr("help.goal", language()), theme().accent, 6.0f);
    return true;
}

int SfmlApp::run() {
    while (window_.isOpen() && running_) {
        float dt = frameClock_.restart().asSeconds();
        dt = std::clamp(dt, 0.0f, 0.1f);

        while (const std::optional event = window_.pollEvent()) {
            handleEvent(*event);
        }

        update(dt);
        render();

        if (dt > 0.0001f) {
            fps_ = fps_ * 0.92f + (1.0f / dt) * 0.08f;
        }
    }
    return 0;
}

void SfmlApp::handleEvent(const sf::Event& event) {
    if (event.is<sf::Event::Closed>()) {
        saveGame();
        running_ = false;
        window_.close();
        return;
    }
    if (const auto* resized = event.getIf<sf::Event::Resized>()) {
        (void)resized;
        updateViewSize();
        return;
    }
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        handleKey(key->code, key->shift, key->control);
        return;
    }
    if (const auto* mouse = event.getIf<sf::Event::MouseButtonPressed>()) {
        handleMousePressed(mouse->button, mouse->position);
        return;
    }
    if (const auto* wheel = event.getIf<sf::Event::MouseWheelScrolled>()) {
        if (wheel->wheel == sf::Mouse::Wheel::Vertical) {
            handleWheel(wheel->delta);
        }
        return;
    }
    if (const auto* moved = event.getIf<sf::Event::MouseMoved>()) {
        mouseScreen_ = {static_cast<float>(moved->position.x),
                        static_cast<float>(moved->position.y)};
    }
}

void SfmlApp::handleKey(sf::Keyboard::Key key, bool shift, bool control) {
    (void)control;
    const bool typingOverlay = overlay_ != Overlay::None;

    if (key == sf::Keyboard::Key::Escape) {
        if (confirmNew_) {
            confirmNew_ = false;
            return;
        }
        if (typingOverlay) {
            closeOverlay();
        } else {
            overlay_ = Overlay::Pause;
        }
        return;
    }

    if (key == sf::Keyboard::Key::F11) {
        toggleFullscreen();
        return;
    }
    if (key == sf::Keyboard::Key::F1) {
        game_.settings.tutorial = !game_.settings.tutorial;
        pushNote(tr("menu.tutorial", language()) + ": " +
                     tr(game_.settings.tutorial ? "ui.on" : "ui.off",
                        language()),
                 theme().accent);
        return;
    }

    if (confirmNew_) {
        if (key == sf::Keyboard::Key::Enter) {
            newGame();
        }
        return;
    }

    if (overlay_ == Overlay::Craft) {
        const int number = numberFromKey(key);
        if (number >= 1 && number <= 9) {
            game_.craft(number - 1);
            processGameEvents();
        } else if (key == sf::Keyboard::Key::C) {
            closeOverlay();
        }
        return;
    }
    if (overlay_ == Overlay::Pause) {
        if (key == sf::Keyboard::Key::Enter) {
            handlePauseAction("resume");
        }
        return;
    }
    if (overlay_ == Overlay::Help) {
        if (key == sf::Keyboard::Key::H) {
            closeOverlay();
        }
        return;
    }
    if (overlay_ == Overlay::Map) {
        if (key == sf::Keyboard::Key::M) {
            closeOverlay();
        }
        return;
    }
    if (overlay_ == Overlay::Inventory) {
        if (key == sf::Keyboard::Key::I) {
            closeOverlay();
        }
        return;
    }

    if (game_.dead) {
        if (key == sf::Keyboard::Key::F5) {
            saveGame();
        } else if (key == sf::Keyboard::Key::F9) {
            loadGame();
        } else if (key == sf::Keyboard::Key::R ||
                   key == sf::Keyboard::Key::Enter) {
            respawn();
        }
        return;
    }

    // ---- world controls ---------------------------------------------------
    if (key == sf::Keyboard::Key::H) {
        overlay_ = Overlay::Help;
        return;
    }
    if (key == sf::Keyboard::Key::C) {
        overlay_ = Overlay::Craft;
        return;
    }
    if (key == sf::Keyboard::Key::I) {
        overlay_ = Overlay::Inventory;
        return;
    }
    if (key == sf::Keyboard::Key::M) {
        overlay_ = Overlay::Map;
        return;
    }
    if (key == sf::Keyboard::Key::F5) {
        saveGame();
        return;
    }
    if (key == sf::Keyboard::Key::F9) {
        loadGame();
        return;
    }

    if (key == sf::Keyboard::Key::R) {
        game_.restAtBase();
        processGameEvents();
        return;
    }
    if (key == sf::Keyboard::Key::F) {
        game_.eatBerry();
        processGameEvents();
        return;
    }
    if (key == sf::Keyboard::Key::Space || key == sf::Keyboard::Key::E) {
        game_.interact();
        processGameEvents();
        return;
    }
    if (key == sf::Keyboard::Key::Tab) {
        game_.cycleTool(shift ? -1 : 1);
        processGameEvents();
        return;
    }
    const int number = numberFromKey(key);
    if (number >= 1 && number <= 4) {
        game_.selectTool(static_cast<cr::ToolType>(number - 1));
        processGameEvents();
        return;
    }
}

void SfmlApp::handleMousePressed(sf::Mouse::Button button,
                                 sf::Vector2i position) {
    const sf::Vector2f point{static_cast<float>(position.x),
                             static_cast<float>(position.y)};

    if (confirmNew_) {
        handleConfirmClick(point);
        return;
    }
    if (overlay_ == Overlay::Craft) {
        handleCraftClick(point);
        return;
    }
    if (overlay_ == Overlay::Inventory) {
        handleInventoryClick(point);
        return;
    }
    if (overlay_ == Overlay::Pause) {
        handlePauseClick(point);
        return;
    }
    if (overlay_ == Overlay::Help || overlay_ == Overlay::Map) {
        if (!contains(overlay_ == Overlay::Help ? helpPanelRect()
                                                : mapPanelRect(),
                      point)) {
            closeOverlay();
        }
        return;
    }

    if (button == sf::Mouse::Button::Right) {
        hasMoveTarget_ = false;
        autoNodeId_ = -1;
        autoAnimalId_ = -1;
        return;
    }
    if (button != sf::Mouse::Button::Left || game_.dead) {
        return;
    }

    const float width = static_cast<float>(window_.getSize().x);
    const float height = static_cast<float>(window_.getSize().y);

    // Hotbar: clicking a slot selects that tool.
    const float slotSize = 62.0f;
    const float gap = 8.0f;
    const float total = 4.0f * slotSize + 3.0f * gap;
    const float startX = (width - total) * 0.5f;
    const float startY = height - slotSize - 18.0f;
    for (int i = 0; i < cr::kToolCount; ++i) {
        const sf::FloatRect slot({startX + i * (slotSize + gap), startY},
                                 {slotSize, slotSize});
        if (contains(slot, point)) {
            game_.selectTool(static_cast<cr::ToolType>(i));
            processGameEvents();
            return;
        }
    }

    // Minimap: clicking it opens the full island map.
    const sf::Vector2f mapCenter{width - 124.0f, height - 124.0f};
    const sf::Vector2f mapDelta = point - mapCenter;
    if (mapDelta.x * mapDelta.x + mapDelta.y * mapDelta.y <= 104.0f * 104.0f) {
        overlay_ = Overlay::Map;
        return;
    }

    // HUD panels should not move the player.
    if (contains(sf::FloatRect({18.0f, 18.0f}, {292.0f, 238.0f}), point) ||
        contains(sf::FloatRect({width - 260.0f, 18.0f}, {242.0f, 152.0f}),
                 point)) {
        return;
    }

    mouseHeld_ = true;
    const sf::Vector2f worldPoint =
        window_.mapPixelToCoords(position, worldView_);
    const cr::Vec2 world{worldPoint.x, worldPoint.y};
    const cr::ClickTarget target =
        game_.targetAt(world, kClickRadius / zoom_);

    if (target.nodeId >= 0) {
        autoNodeId_ = target.nodeId;
        autoAnimalId_ = -1;
        hasMoveTarget_ = false;
        autoActionTimer_ = 0.0f;
    } else if (target.animalId >= 0) {
        autoAnimalId_ = target.animalId;
        autoNodeId_ = -1;
        hasMoveTarget_ = false;
        autoActionTimer_ = 0.0f;
    } else {
        hasMoveTarget_ = true;
        moveTarget_ = world;
        autoNodeId_ = -1;
        autoAnimalId_ = -1;
    }
}

void SfmlApp::handleWheel(float delta) {
    if (overlay_ != Overlay::None) {
        return;
    }
    const float factor = delta > 0.0f ? 1.12f : 0.89f;
    zoom_ = std::clamp(zoom_ * factor, 0.55f, 2.4f);
    updateViewSize();
}

// ---------------------------------------------------------------------------
//  Update
// ---------------------------------------------------------------------------

void SfmlApp::update(float dt) {
    elapsed_ += dt;

    const bool paused = overlay_ != Overlay::None || confirmNew_;
    if (!paused && !game_.dead) {
        const cr::Vec2 axis = readMoveAxis();
        if (axis.lengthSquared() > 0.001f) {
            hasMoveTarget_ = false;
            autoNodeId_ = -1;
            autoAnimalId_ = -1;
            moveWithInput(dt);
        } else {
            game_.player.moving = false;
            game_.player.sprinting = false;
            updateAutoAction(dt);
        }
    } else {
        game_.player.moving = false;
        game_.player.sprinting = false;
    }

    game_.update(paused ? 0.0f : dt);
    processGameEvents();

    if (!paused && !game_.dead) {
        autosaveTimer_ += dt;
        if (autosaveTimer_ >= kAutosaveSeconds) {
            autosaveTimer_ = 0.0f;
            saveGame();
        }
    }

    updateCamera(dt);
    updateMouseHover();

    for (Note& note : notes_) {
        note.ttl -= dt;
    }
    while (!notes_.empty() && notes_.front().ttl <= 0.0f) {
        notes_.pop_front();
    }

    shake_ = std::max(0.0f, shake_ - dt);
    autoActionTimer_ = std::max(0.0f, autoActionTimer_ - dt);
}

void SfmlApp::updateViewSize() {
    const sf::Vector2u size = window_.getSize();
    const float width = std::max(640.0f, static_cast<float>(size.x));
    const float height = std::max(480.0f, static_cast<float>(size.y));
    worldView_.setSize({width / zoom_, height / zoom_});
    worldView_.setCenter(cameraCenter_);
}

void SfmlApp::updateCamera(float dt) {
    const sf::Vector2f target = toSf(game_.player.pos);
    const float blend = 1.0f - std::exp(-7.0f * dt);
    cameraCenter_ += (target - cameraCenter_) * blend;

    sf::Vector2f offset{0.0f, 0.0f};
    if (shake_ > 0.0f) {
        const float strength = shake_ * 8.0f;
        offset.x = std::sin(elapsed_ * 61.0f) * strength;
        offset.y = std::cos(elapsed_ * 53.0f) * strength;
    }
    worldView_.setCenter(cameraCenter_ + offset);
    worldView_.setSize({std::max(640.0f, static_cast<float>(
                                       window_.getSize().x)) /
                            zoom_,
                        std::max(480.0f, static_cast<float>(
                                             window_.getSize().y)) /
                            zoom_});
}

void SfmlApp::updateMouseHover() {
    mouseScreen_ = {static_cast<float>(sf::Mouse::getPosition(window_).x),
                    static_cast<float>(sf::Mouse::getPosition(window_).y)};
    const sf::Vector2f worldPoint =
        window_.mapPixelToCoords(sf::Vector2i(static_cast<int>(mouseScreen_.x),
                                             static_cast<int>(mouseScreen_.y)),
                                 worldView_);
    mouseWorld_ = {worldPoint.x, worldPoint.y};

    hoverNodeId_ = -1;
    hoverAnimalId_ = -1;
    if (overlay_ != Overlay::None || game_.dead) {
        return;
    }
    const cr::ClickTarget target =
        game_.targetAt(mouseWorld_, kClickRadius / zoom_);
    hoverNodeId_ = target.nodeId;
    hoverAnimalId_ = target.animalId;
}

void SfmlApp::moveWithInput(float dt) {
    const cr::Vec2 axis = readMoveAxis();
    if (axis.lengthSquared() < 0.001f) {
        game_.player.moving = false;
        game_.player.sprinting = false;
        return;
    }
    game_.movePlayer(axis, dt, isSprintHeld());
}

void SfmlApp::updateAutoAction(float dt) {
    if (game_.dead || overlay_ != Overlay::None) {
        return;
    }

    if (autoNodeId_ >= 0) {
        cr::Node* node = game_.nodeById(autoNodeId_);
        if (node == nullptr || !node->alive) {
            autoNodeId_ = -1;
            return;
        }
        const float reach =
            cr::kInteractRange + cr::nodeRule(node->type).visualRadius * 0.35f -
            10.0f;
        if (game_.distanceToPlayer(node->pos) > reach) {
            const cr::Vec2 direction = (node->pos - game_.player.pos).normalized();
            game_.movePlayer(direction, dt, false);
        } else {
            game_.player.moving = false;
            game_.player.sprinting = false;
            if (autoActionTimer_ <= 0.0f) {
                game_.gatherNode(*node);
                autoActionTimer_ = 0.30f;
            }
        }
        return;
    }

    if (autoAnimalId_ >= 0) {
        cr::Animal* animal = game_.animalById(autoAnimalId_);
        if (animal == nullptr || !animal->alive) {
            autoAnimalId_ = -1;
            return;
        }
        const float reach =
            cr::kAttackRange + cr::animalRule(animal->type).visualRadius - 12.0f;
        if (game_.distanceToPlayer(animal->pos) > reach) {
            const cr::Vec2 direction =
                (animal->pos - game_.player.pos).normalized();
            game_.movePlayer(direction, dt, false);
        } else {
            game_.player.moving = false;
            game_.player.sprinting = false;
            if (autoActionTimer_ <= 0.0f) {
                game_.attackAnimal(*animal);
                autoActionTimer_ = 0.30f;
            }
        }
        return;
    }

    if (hasMoveTarget_) {
        const cr::Vec2 difference = moveTarget_ - game_.player.pos;
        if (difference.length() <= 16.0f) {
            hasMoveTarget_ = false;
            game_.player.moving = false;
            return;
        }
        game_.movePlayer(difference.normalized(), dt, false);
    }
}

// ---------------------------------------------------------------------------
//  Rendering
// ---------------------------------------------------------------------------

void SfmlApp::render() {
    window_.clear(theme().background);

    window_.setView(worldView_);
    renderWorld();

    window_.setView(window_.getDefaultView());
    renderHud();
    renderHotbar();
    renderMinimap({static_cast<float>(window_.getSize().x) - 124.0f,
                   static_cast<float>(window_.getSize().y) - 124.0f},
                  96.0f);
    renderPrompt();
    renderTutorial();
    renderNotifications();

    if (game_.dead) {
        renderDeath();
    }
    if (overlay_ != Overlay::None) {
        renderOverlays();
    }
    if (confirmNew_) {
        renderConfirmNew();
    }

    window_.display();
}

void SfmlApp::renderWorld() {
    RenderFocus focus;
    focus.nodeId = hoverNodeId_;
    focus.animalId = hoverAnimalId_;
    focus.hasMoveTarget = hasMoveTarget_;
    focus.moveTarget = moveTarget_;
    renderer_.draw(window_, game_, elapsed_, mouseWorld_, focus);
}

void SfmlApp::renderHud() {
    const float width = static_cast<float>(window_.getSize().x);
    const float height = static_cast<float>(window_.getSize().y);

    // ---- player panel -----------------------------------------------------
    const sf::FloatRect panel({18.0f, 18.0f}, {292.0f, 238.0f});
    drawPanel(window_, panel, withAlpha(theme().panel, 232));

    drawText(window_, font_, game_.playerName, 20, theme().text,
             {panel.position.x + 16.0f, panel.position.y + 12.0f}, Align::Left,
             true);

    struct BarInfo {
        const char* label;
        float value;
        float maximum;
        sf::Color color;
    };
    const BarInfo bars[] = {
        {"hud.hp", game_.player.hp, game_.player.maxHp, theme().hp},
        {"hud.food", game_.player.food, cr::kMaxStat, theme().food},
        {"hud.water", game_.player.water, cr::kMaxStat, theme().waterStat},
        {"hud.energy", game_.player.energy, cr::kMaxStat, theme().energy},
    };

    float y = panel.position.y + 44.0f;
    for (const BarInfo& bar : bars) {
        drawText(window_, font_, tr(bar.label, language()), 13, theme().textDim,
                 {panel.position.x + 16.0f, y});
        const std::string value =
            std::to_string(static_cast<int>(std::round(bar.value))) + " / " +
            std::to_string(static_cast<int>(std::round(bar.maximum)));
        drawText(window_, font_, value, 12, theme().textDim,
                 {panel.position.x + panel.size.x - 16.0f, y}, Align::Right);
        drawBar(window_, {panel.position.x + 16.0f, y + 17.0f},
                {panel.size.x - 32.0f, 11.0f}, bar.value, bar.maximum,
                bar.color);
        y += 36.0f;
    }

    int wornPieces = 0;
    for (int i = 0; i < cr::kArmorCount; ++i) {
        if (game_.inventory.armorWorn[static_cast<std::size_t>(i)]) {
            ++wornPieces;
        }
    }
    const std::string armor =
        tr("hud.armor", language()) + ": " + std::to_string(wornPieces) +
        "  ·  " + tr("hud.defense", language()) + ": " +
        std::to_string(static_cast<int>(std::round(game_.armorDefense())));
    drawText(window_, font_, armor, 13, theme().accent,
             {panel.position.x + 16.0f, panel.position.y + panel.size.y - 26.0f});

    // ---- world panel ------------------------------------------------------
    const sf::FloatRect worldPanel({width - 260.0f, 18.0f}, {242.0f, 152.0f});
    drawPanel(window_, worldPanel, withAlpha(theme().panel, 232));

    const std::string dayLine =
        tr("hud.day", language()) + " " + std::to_string(game_.day) + "   " +
        clockString(game_.timeOfDay);
    drawText(window_, font_, dayLine, 18, theme().text,
             {worldPanel.position.x + 16.0f, worldPanel.position.y + 12.0f},
             Align::Left, true);

    const std::string zone =
        game_.inBaseSafeZone(game_.player.pos)
            ? tr("hud.safe", language())
            : tr("hud.wild", language());
    drawText(window_, font_, zone, 14,
             game_.inBaseSafeZone(game_.player.pos) ? theme().success
                                                    : theme().textDim,
             {worldPanel.position.x + 16.0f, worldPanel.position.y + 40.0f});

    char positionText[96] = {};
    std::snprintf(positionText, sizeof(positionText), "%s: %.0f, %.0f",
                  tr("hud.position", language()).c_str(), game_.player.pos.x,
                  game_.player.pos.y);
    drawText(window_, font_, positionText, 12, theme().textDim,
             {worldPanel.position.x + 16.0f, worldPanel.position.y + 62.0f});

    char statsText[128] = {};
    std::snprintf(statsText, sizeof(statsText), "%s %d · %s %d · %s %d",
                  tr("hud.kills", language()).c_str(), game_.stats.kills,
                  tr("hud.gathered", language()).c_str(), game_.stats.gathered,
                  tr("hud.crafted", language()).c_str(), game_.stats.crafted);
    drawText(window_, font_, statsText, 12, theme().textDim,
             {worldPanel.position.x + 16.0f, worldPanel.position.y + 82.0f});

    char miscText[128] = {};
    std::snprintf(miscText, sizeof(miscText), "%s %d · %s %.0f",
                  tr("hud.deaths", language()).c_str(), game_.totalDeaths,
                  "FPS", fps_);
    drawText(window_, font_, miscText, 12, theme().textDim,
             {worldPanel.position.x + 16.0f, worldPanel.position.y + 102.0f});

    drawText(window_, font_, tr("controls.help", language()), 11,
             withAlpha(theme().textDim, 190),
             {worldPanel.position.x + 16.0f, worldPanel.position.y + 126.0f});

    (void)height;
}

void SfmlApp::renderHotbar() {
    const float width = static_cast<float>(window_.getSize().x);
    const float height = static_cast<float>(window_.getSize().y);
    const float slotSize = 62.0f;
    const float gap = 8.0f;
    const float total = 4.0f * slotSize + 3.0f * gap;
    const float startX = (width - total) * 0.5f;
    const float startY = height - slotSize - 18.0f;

    for (int i = 0; i < cr::kToolCount; ++i) {
        const cr::ToolType tool = static_cast<cr::ToolType>(i);
        const bool owned = game_.hasTool(tool);
        const bool selected = game_.inventory.selectedTool == tool;
        const sf::FloatRect slot({startX + i * (slotSize + gap), startY},
                                 {slotSize, slotSize});

        drawPanel(window_, slot,
                  selected ? withAlpha(theme().panelLight, 245)
                           : withAlpha(theme().panel, 220),
                  selected ? theme().accent : theme().border,
                  selected ? 2.0f : 1.0f);

        drawText(window_, font_, std::to_string(i + 1), 12,
                 selected ? theme().accent : theme().textDim,
                 {slot.position.x + 6.0f, slot.position.y + 4.0f}, Align::Left,
                 true);

        drawToolIcon(window_, tool,
                     {slot.position.x + slotSize * 0.5f,
                      slot.position.y + slotSize * 0.5f + 2.0f},
                     0.72f, owned);

        if (owned) {
            const float durability = game_.toolDurability(tool);
            const float maximum = cr::toolRule(tool).maxDurability;
            drawBar(window_,
                    {slot.position.x + 8.0f, slot.position.y + slotSize - 10.0f},
                    {slotSize - 16.0f, 4.0f}, durability, maximum,
                    durability / maximum < 0.25f ? theme().danger
                                                 : theme().success,
                    false);
            drawText(window_, font_, std::to_string(static_cast<int>(durability)),
                     10, theme().textDim,
                     {slot.position.x + slotSize - 6.0f,
                      slot.position.y + slotSize - 24.0f},
                     Align::Right);
        }
    }

    const std::string selected =
        game_.inventory.selectedTool == cr::ToolType::Count
            ? tr("ui.none", language())
            : toolLabel(game_.inventory.selectedTool);
    drawText(window_, font_, selected, 12, theme().textDim,
             {width * 0.5f, startY - 20.0f}, Align::Center);
}

void SfmlApp::renderMinimap(sf::Vector2f center, float radius) {
    sf::CircleShape background(radius + 6.0f);
    background.setOrigin({radius + 6.0f, radius + 6.0f});
    background.setPosition(center);
    background.setFillColor(withAlpha(theme().panel, 220));
    background.setOutlineThickness(1.0f);
    background.setOutlineColor(theme().border);
    window_.draw(background);

    const float scale = (radius - 4.0f) / game_.world.radius;

    sf::CircleShape island(radius - 4.0f);
    island.setOrigin({radius - 4.0f, radius - 4.0f});
    island.setPosition(center);
    island.setFillColor(withAlpha(theme().grassDark, 200));
    island.setOutlineThickness(1.0f);
    island.setOutlineColor(withAlpha(theme().sand, 170));
    window_.draw(island);

    const auto mapPoint = [&](cr::Vec2 position) {
        return sf::Vector2f{center.x + position.x * scale,
                            center.y + position.y * scale};
    };

    for (const cr::Node& node : game_.world.nodes) {
        if (!node.alive) {
            continue;
        }
        sf::CircleShape dot(2.4f);
        dot.setOrigin({2.4f, 2.4f});
        dot.setPosition(mapPoint(node.pos));
        dot.setFillColor(withAlpha(resourceColor(cr::nodeRule(node.type).resource),
                                   220));
        window_.draw(dot);
    }

    for (const cr::Animal& animal : game_.world.animals) {
        if (!animal.alive) {
            continue;
        }
        sf::CircleShape dot(cr::animalRule(animal.type).aggressive ? 3.0f : 2.0f);
        dot.setOrigin({dot.getRadius(), dot.getRadius()});
        dot.setPosition(mapPoint(animal.pos));
        dot.setFillColor(
            cr::animalRule(animal.type).aggressive
                ? withAlpha(theme().danger, 220)
                : withAlpha(animalColor(animal.type), 200));
        window_.draw(dot);
    }

    sf::CircleShape camp(4.0f);
    camp.setOrigin({4.0f, 4.0f});
    camp.setPosition(center);
    camp.setFillColor(theme().accent);
    window_.draw(camp);

    sf::CircleShape player(4.5f);
    player.setOrigin({4.5f, 4.5f});
    player.setPosition(mapPoint(game_.player.pos));
    player.setFillColor(sf::Color::White);
    player.setOutlineThickness(2.0f);
    player.setOutlineColor(theme().accent);
    window_.draw(player);

    drawText(window_, font_, tr("hud.minimap", language()), 11, theme().textDim,
             {center.x, center.y - radius - 18.0f}, Align::Center);
}

void SfmlApp::renderPrompt() {
    if (overlay_ != Overlay::None || game_.dead || confirmNew_) {
        return;
    }
    sf::Color color = theme().text;
    const std::string prompt = currentPrompt(&color);
    if (prompt.empty()) {
        return;
    }

    const float width = textWidth(font_, prompt, 16, true) + 34.0f;
    const sf::FloatRect rect(
        {static_cast<float>(window_.getSize().x) * 0.5f - width * 0.5f,
         static_cast<float>(window_.getSize().y) - 116.0f},
        {width, 34.0f});
    drawPanel(window_, rect, withAlpha(theme().panel, 235), color, 1.0f);
    drawText(window_, font_, prompt, 16, color,
             {rect.position.x + rect.size.x * 0.5f,
              rect.position.y + 7.0f},
             Align::Center, true);
}

void SfmlApp::renderTutorial() {
    if (!game_.settings.tutorial || game_.dead) {
        return;
    }
    const std::string text = tutorialText();
    if (text.empty()) {
        return;
    }

    const float width = 300.0f;
    const std::vector<std::string> lines =
        wrapText(font_, text, 14, width - 32.0f);
    const float height = 54.0f + static_cast<float>(lines.size()) * 19.0f;
    const sf::FloatRect rect({18.0f, 252.0f}, {width, height});
    drawPanel(window_, rect, withAlpha(theme().panel, 226),
              withAlpha(theme().accent, 150));

    drawText(window_, font_, tr("tutorial.title", language()), 15,
             theme().accent, {rect.position.x + 16.0f, rect.position.y + 10.0f},
             Align::Left, true);

    float y = rect.position.y + 34.0f;
    for (const std::string& line : lines) {
        drawText(window_, font_, line, 14, theme().text,
                 {rect.position.x + 16.0f, y});
        y += 19.0f;
    }
    drawText(window_, font_, tr("tutorial.skip", language()), 11,
             theme().textDim,
             {rect.position.x + 16.0f, rect.position.y + height - 18.0f});
}

void SfmlApp::renderNotifications() {
    if (notes_.empty()) {
        return;
    }
    float y = static_cast<float>(window_.getSize().y) - 104.0f;
    const int count = std::min<int>(static_cast<int>(notes_.size()), 5);
    for (int i = count - 1; i >= 0; --i) {
        const Note& note = notes_[static_cast<std::size_t>(i)];
        const float alpha =
            std::clamp(note.ttl / std::min(0.8f, note.maxTtl), 0.0f, 1.0f);
        const float width =
            std::min(420.0f, textWidth(font_, note.text, 14) + 26.0f);
        const sf::FloatRect rect({18.0f, y - 28.0f}, {width, 28.0f});
        drawPanel(window_, rect,
                  withAlpha(theme().panel, static_cast<int>(215 * alpha)),
                  withAlpha(note.color, static_cast<int>(180 * alpha)));
        drawText(window_, font_, note.text, 14,
                 withAlpha(note.color, static_cast<int>(255 * alpha)),
                 {rect.position.x + 13.0f, rect.position.y + 5.0f});
        y -= 34.0f;
    }
}

void SfmlApp::renderDeath() {
    const sf::Vector2f size{static_cast<float>(window_.getSize().x),
                            static_cast<float>(window_.getSize().y)};
    sf::RectangleShape veil(size);
    veil.setFillColor(sf::Color(24, 6, 8, 190));
    window_.draw(veil);

    const sf::FloatRect panel = centered({560.0f, 250.0f});
    drawPanel(window_, panel, withAlpha(theme().panel, 248), theme().danger,
              2.0f);

    drawText(window_, font_, tr("death.title", language()), 36, theme().danger,
             {panel.position.x + panel.size.x * 0.5f,
              panel.position.y + 30.0f},
             Align::Center, true);
    drawText(window_, font_, tr("death.hint", language()), 16, theme().text,
             {panel.position.x + panel.size.x * 0.5f,
              panel.position.y + 96.0f},
             Align::Center);
    const std::string stats = format(
        tr("death.stats", language()),
        {{"gathered", std::to_string(game_.stats.gathered)},
         {"kills", std::to_string(game_.stats.kills)}});
    drawText(window_, font_, stats, 15, theme().textDim,
             {panel.position.x + panel.size.x * 0.5f,
              panel.position.y + 132.0f},
             Align::Center);

    const sf::FloatRect resumeRect(
        {centered({180.0f, 40.0f}).position.x, panel.position.y + 176.0f},
        {180.0f, 40.0f});
    drawPanel(window_, resumeRect, withAlpha(theme().panelLight, 245),
              theme().accent);
    drawText(window_, font_, "R — " + tr("death.respawn", language()), 16,
             theme().accent,
             {resumeRect.position.x + 90.0f,
              resumeRect.position.y + 8.0f},
             Align::Center, true);
}

void SfmlApp::renderOverlays() {
    const sf::Vector2f size{static_cast<float>(window_.getSize().x),
                            static_cast<float>(window_.getSize().y)};
    sf::RectangleShape veil(size);
    veil.setFillColor(sf::Color(6, 9, 12, 165));
    window_.draw(veil);

    switch (overlay_) {
    case Overlay::Craft:
        renderCraft();
        break;
    case Overlay::Inventory:
        renderInventory();
        break;
    case Overlay::Pause:
        renderPause();
        break;
    case Overlay::Help:
        renderHelp();
        break;
    case Overlay::Map:
        renderMap();
        break;
    case Overlay::None:
        break;
    }
}

void SfmlApp::renderCraft() {
    const sf::FloatRect panel = craftPanelRect();
    drawPanel(window_, panel, withAlpha(theme().panel, 248), theme().border);

    drawText(window_, font_, tr("craft.title", language()), 26, theme().accent,
             {panel.position.x + 24.0f, panel.position.y + 18.0f}, Align::Left,
             true);
    drawText(window_, font_, tr("craft.hint", language()), 12, theme().textDim,
             {panel.position.x + panel.size.x - 24.0f,
              panel.position.y + 26.0f},
             Align::Right);

    const std::vector<cr::Recipe>& recipes = game_.recipes();
    const sf::Vector2f mouse = mouseScreen_;
    for (int i = 0; i < static_cast<int>(recipes.size()); ++i) {
        const sf::FloatRect row = craftRowRect(i);
        const cr::Recipe& recipe = recipes[static_cast<std::size_t>(i)];
        const bool available = game_.canCraft(recipe);
        const bool hovered = contains(row, mouse);

        drawPanel(window_, row,
                  hovered ? withAlpha(theme().panelLight, 245)
                          : withAlpha(theme().panelLight, 170),
                  hovered ? theme().accent : withAlpha(theme().border, 150));

        drawText(window_, font_, std::to_string(i + 1), 14,
                 available ? theme().accent : theme().textDim,
                 {row.position.x + 12.0f, row.position.y + 12.0f}, Align::Left,
                 true);

        drawText(window_, font_, recipeName(i, language()), 16, theme().text,
                 {row.position.x + 38.0f, row.position.y + 7.0f}, Align::Left,
                 true);
        drawText(window_, font_, formatRecipeCost(recipe), 12,
                 available ? theme().success : theme().textDim,
                 {row.position.x + 38.0f, row.position.y + 28.0f});

        const std::string status =
            available ? tr("craft.available", language())
                      : tr("craft.missing", language());
        drawText(window_, font_, status, 12,
                 available ? theme().success : theme().danger,
                 {row.position.x + row.size.x - 14.0f,
                  row.position.y + 18.0f},
                 Align::Right, true);
    }

    drawText(window_, font_, tr("craft.select", language()),
             11, theme().textDim,
             {panel.position.x + 24.0f,
              panel.position.y + panel.size.y - 20.0f});
}

void SfmlApp::renderInventory() {
    const sf::FloatRect panel = inventoryPanelRect();
    drawPanel(window_, panel, withAlpha(theme().panel, 248), theme().border);

    drawText(window_, font_, tr("inv.title", language()), 26, theme().accent,
             {panel.position.x + 24.0f, panel.position.y + 18.0f}, Align::Left,
             true);
    drawText(window_, font_, tr("inv.hint", language()), 12, theme().textDim,
             {panel.position.x + panel.size.x - 24.0f,
              panel.position.y + 26.0f},
             Align::Right);

    const float leftWidth = panel.size.x * 0.46f;
    const float columnX = panel.position.x + 24.0f;
    drawText(window_, font_, tr("inv.resources", language()), 15,
             theme().text, {columnX, panel.position.y + 62.0f}, Align::Left,
             true);

    for (int i = 0; i < cr::kResourceCount; ++i) {
        const cr::ResourceType type = static_cast<cr::ResourceType>(i);
        const int column = i % 2;
        const int row = i / 2;
        const sf::FloatRect cell(
            {columnX + column * (leftWidth * 0.5f),
             panel.position.y + 88.0f + row * 48.0f},
            {leftWidth * 0.5f - 8.0f, 40.0f});
        drawPanel(window_, cell, withAlpha(theme().panelLight, 160),
                  withAlpha(theme().border, 120));
        drawResourceDot(window_, type,
                        {cell.position.x + 16.0f,
                         cell.position.y + cell.size.y * 0.5f},
                        6.0f);
        drawText(window_, font_, resourceName(type, language()), 13,
                 theme().text, {cell.position.x + 30.0f,
                                cell.position.y + 11.0f});
        drawText(window_, font_, std::to_string(game_.resource(type)), 14,
                 game_.resource(type) > 0 ? theme().accent : theme().textDim,
                 {cell.position.x + cell.size.x - 12.0f,
                  cell.position.y + 10.0f},
                 Align::Right, true);
    }

    const float toolsX = panel.position.x + panel.size.x * 0.50f;
    drawText(window_, font_, tr("inv.tools", language()), 15, theme().text,
             {toolsX, panel.position.y + 62.0f}, Align::Left, true);
    for (int i = 0; i < cr::kToolCount; ++i) {
        const cr::ToolType tool = static_cast<cr::ToolType>(i);
        const sf::FloatRect row = toolRowRect(i);
        const bool owned = game_.hasTool(tool);
        const bool selected = game_.inventory.selectedTool == tool;
        const bool hovered = contains(row, mouseScreen_);
        drawPanel(window_, row,
                  hovered ? withAlpha(theme().panelLight, 240)
                          : withAlpha(theme().panelLight, 165),
                  selected ? theme().accent : withAlpha(theme().border, 120));
        drawToolIcon(window_, tool,
                     {row.position.x + 24.0f, row.position.y + row.size.y * 0.5f},
                     0.55f, owned);
        drawText(window_, font_, toolName(tool, language()), 12,
                 owned ? theme().text : theme().textDim,
                 {row.position.x + 44.0f, row.position.y + 6.0f});
        if (owned) {
            drawBar(window_,
                    {row.position.x + 44.0f, row.position.y + row.size.y - 14.0f},
                    {row.size.x - 56.0f, 4.0f}, game_.toolDurability(tool),
                    cr::toolRule(tool).maxDurability,
                    game_.toolDurability(tool) /
                                cr::toolRule(tool).maxDurability <
                            0.25f
                        ? theme().danger
                        : theme().success,
                    false);
            drawText(window_, font_,
                     std::to_string(static_cast<int>(game_.toolDurability(tool))),
                     10, theme().textDim,
                     {row.position.x + row.size.x - 6.0f,
                      row.position.y + row.size.y - 18.0f},
                     Align::Right);
        }
    }

    const float armorX = panel.position.x + panel.size.x * 0.74f;
    drawText(window_, font_, tr("inv.armor", language()), 15, theme().text,
             {armorX, panel.position.y + 62.0f}, Align::Left, true);
    for (int i = 0; i < cr::kArmorCount; ++i) {
        const cr::ArmorType type = static_cast<cr::ArmorType>(i);
        const sf::FloatRect row = armorRowRect(i);
        const bool owned = game_.inventory.armorItems[cr::idx(type)] > 0;
        const bool worn = game_.inventory.armorWorn[cr::idx(type)];
        const bool hovered = contains(row, mouseScreen_);
        drawPanel(window_, row,
                  hovered ? withAlpha(theme().panelLight, 240)
                          : withAlpha(theme().panelLight, 165),
                  worn ? theme().success : withAlpha(theme().border, 120));
        drawText(window_, font_, armorName(type, language()), 12,
                 owned ? theme().text : theme().textDim,
                 {row.position.x + 10.0f, row.position.y + 6.0f});
        const std::string state =
            worn ? tr("inv.equipped", language())
                 : (owned ? tr("inv.equip", language())
                          : tr("inv.empty", language()));
        drawText(window_, font_, state, 11,
                 worn ? theme().success
                      : (owned ? theme().accent : theme().textDim),
                 {row.position.x + 10.0f, row.position.y + 26.0f}, Align::Left,
                 true);
        if (owned) {
            drawText(window_, font_,
                     std::to_string(game_.inventory.armorItems[cr::idx(type)]),
                     12, theme().textDim,
                     {row.position.x + row.size.x - 8.0f,
                      row.position.y + 6.0f},
                     Align::Right);
        }
    }

    const std::string summary =
        tr("inv.defense", language()) + " " +
        std::to_string(static_cast<int>(std::round(game_.armorDefense()))) +
        "   ·   " + tr("inv.maxhp", language()) + " " +
        std::to_string(static_cast<int>(std::round(game_.player.maxHp)));
    drawText(window_, font_, summary, 13, theme().accent,
             {panel.position.x + 24.0f,
              panel.position.y + panel.size.y - 26.0f});
}

void SfmlApp::renderPause() {
    const sf::FloatRect panel = pausePanelRect();
    drawPanel(window_, panel, withAlpha(theme().panel, 248), theme().border);

    drawText(window_, font_, tr("menu.title", language()), 30, theme().accent,
             {panel.position.x + panel.size.x * 0.5f,
              panel.position.y + 24.0f},
             Align::Center, true);

    for (const Button& button : pauseButtons()) {
        drawButton(button, contains(button.rect, mouseScreen_));
    }
}

void SfmlApp::renderHelp() {
    const sf::FloatRect panel = helpPanelRect();
    drawPanel(window_, panel, withAlpha(theme().panel, 248), theme().border);

    drawText(window_, font_, tr("help.title", language()), 26, theme().accent,
             {panel.position.x + 24.0f, panel.position.y + 18.0f}, Align::Left,
             true);
    drawText(window_, font_, tr("help.close", language()), 12, theme().textDim,
             {panel.position.x + panel.size.x - 24.0f,
              panel.position.y + 26.0f},
             Align::Right);

    float y = panel.position.y + 66.0f;
    const char* paragraphs[] = {"help.goal", "help.survival", "help.safe",
                                "help.tools", "help.save"};
    for (const char* key : paragraphs) {
        const std::vector<std::string> lines =
            wrapText(font_, tr(key, language()), 13, panel.size.x - 48.0f);
        for (const std::string& line : lines) {
            drawText(window_, font_, line, 13, theme().text,
                     {panel.position.x + 24.0f, y});
            y += 18.0f;
        }
        y += 6.0f;
    }

    y += 6.0f;
    drawText(window_, font_, tr("controls.title", language()), 16,
             theme().accent, {panel.position.x + 24.0f, y}, Align::Left, true);
    y += 26.0f;

    const char* controls[] = {
        "controls.move",   "controls.sprint", "controls.interact",
        "controls.attack", "controls.craft",  "controls.inventory",
        "controls.rest",   "controls.eat",    "controls.tools",
        "controls.cycle",  "controls.map",    "controls.save",
        "controls.load",   "controls.help",   "controls.pause",
        "controls.fullscreen", "controls.mouse", "controls.tutorial",
    };
    const int perColumn = 9;
    const float columnWidth = (panel.size.x - 48.0f) * 0.5f;
    for (int i = 0; i < 18; ++i) {
        const int column = i / perColumn;
        const int row = i % perColumn;
        drawText(window_, font_, tr(controls[i], language()), 12, theme().text,
                 {panel.position.x + 24.0f + column * columnWidth,
                  y + row * 20.0f});
    }
}

void SfmlApp::renderMap() {
    const sf::FloatRect panel = mapPanelRect();
    drawPanel(window_, panel, withAlpha(theme().panel, 248), theme().border);

    drawText(window_, font_, tr("map.title", language()), 26, theme().accent,
             {panel.position.x + 24.0f, panel.position.y + 18.0f}, Align::Left,
             true);
    drawText(window_, font_, tr("controls.map", language()), 12, theme().textDim,
             {panel.position.x + panel.size.x - 24.0f,
              panel.position.y + 26.0f},
             Align::Right);

    const float radius = std::min(panel.size.y * 0.34f, panel.size.x * 0.28f);
    const sf::Vector2f center{panel.position.x + radius + 34.0f,
                              panel.position.y + panel.size.y * 0.56f};

    sf::CircleShape water(radius + 12.0f);
    water.setOrigin({radius + 12.0f, radius + 12.0f});
    water.setPosition(center);
    water.setFillColor(withAlpha(theme().water, 220));
    window_.draw(water);

    sf::CircleShape island(radius);
    island.setOrigin({radius, radius});
    island.setPosition(center);
    island.setFillColor(withAlpha(theme().grassDark, 230));
    island.setOutlineThickness(2.0f);
    island.setOutlineColor(withAlpha(theme().sand, 200));
    window_.draw(island);

    const float scale = radius / game_.world.radius;
    const auto mapPoint = [&](cr::Vec2 position) {
        return sf::Vector2f{center.x + position.x * scale,
                            center.y + position.y * scale};
    };

    for (const cr::Node& node : game_.world.nodes) {
        if (!node.alive) {
            continue;
        }
        drawResourceDot(window_, cr::nodeRule(node.type).resource,
                        mapPoint(node.pos), 2.6f);
    }
    for (const cr::Animal& animal : game_.world.animals) {
        if (!animal.alive) {
            continue;
        }
        sf::CircleShape dot(cr::animalRule(animal.type).aggressive ? 3.2f : 2.2f);
        dot.setOrigin({dot.getRadius(), dot.getRadius()});
        dot.setPosition(mapPoint(animal.pos));
        dot.setFillColor(
            cr::animalRule(animal.type).aggressive
                ? withAlpha(theme().danger, 220)
                : withAlpha(animalColor(animal.type), 210));
        window_.draw(dot);
    }

    sf::CircleShape camp(5.0f);
    camp.setOrigin({5.0f, 5.0f});
    camp.setPosition(center);
    camp.setFillColor(theme().accent);
    window_.draw(camp);

    sf::CircleShape player(5.0f);
    player.setOrigin({5.0f, 5.0f});
    player.setPosition(mapPoint(game_.player.pos));
    player.setFillColor(sf::Color::White);
    player.setOutlineThickness(2.0f);
    player.setOutlineColor(theme().accent);
    window_.draw(player);

    // Legend.
    const float legendX = center.x + radius + 42.0f;
    float legendY = panel.position.y + 82.0f;
    drawText(window_, font_, tr("map.legend", language()), 16, theme().accent,
             {legendX, legendY}, Align::Left, true);
    legendY += 30.0f;

    const auto legendRow = [&](sf::Color color, const std::string& label) {
        sf::CircleShape dot(4.0f);
        dot.setOrigin({4.0f, 4.0f});
        dot.setPosition({legendX + 6.0f, legendY + 9.0f});
        dot.setFillColor(color);
        window_.draw(dot);
        drawText(window_, font_, label, 13, theme().text,
                 {legendX + 20.0f, legendY});
        legendY += 26.0f;
    };

    legendRow(theme().accent, tr("map.camp", language()));
    legendRow(sf::Color::White, tr("map.you", language()));
    legendRow(resourceColor(cr::ResourceType::Wood),
              tr("map.nodes", language()));
    legendRow(theme().danger, tr("map.animals", language()));

    legendY += 10.0f;
    for (int i = 0; i < cr::kResourceCount; ++i) {
        const cr::ResourceType type = static_cast<cr::ResourceType>(i);
        const int count = game_.resource(type);
        if (count <= 0) {
            continue;
        }
        drawResourceDot(window_, type, {legendX + 6.0f, legendY + 8.0f}, 4.0f);
        drawText(window_, font_,
                 resourceName(type, language()) + "  " + std::to_string(count),
                 12, theme().textDim, {legendX + 20.0f, legendY});
        legendY += 21.0f;
    }
}

void SfmlApp::renderConfirmNew() {
    const sf::Vector2f size{static_cast<float>(window_.getSize().x),
                            static_cast<float>(window_.getSize().y)};
    sf::RectangleShape veil(size);
    veil.setFillColor(sf::Color(5, 8, 11, 185));
    window_.draw(veil);

    const sf::FloatRect panel = confirmPanelRect();
    drawPanel(window_, panel, withAlpha(theme().panel, 250), theme().warning,
              2.0f);

    drawText(window_, font_, tr("menu.new_game", language()), 22,
             theme().warning,
             {panel.position.x + panel.size.x * 0.5f,
              panel.position.y + 20.0f},
             Align::Center, true);

    float y = panel.position.y + 62.0f;
    const std::vector<std::string> lines = wrapText(
        font_, tr("menu.confirm_new", language()), 14, panel.size.x - 48.0f);
    for (const std::string& line : lines) {
        drawText(window_, font_, line, 14, theme().text,
                 {panel.position.x + panel.size.x * 0.5f, y}, Align::Center);
        y += 20.0f;
    }

    Button yes;
    yes.rect = confirmYesRect();
    yes.label = tr("menu.confirm_yes", language());
    yes.action = "yes";
    drawButton(yes, contains(yes.rect, mouseScreen_));

    Button no;
    no.rect = confirmNoRect();
    no.label = tr("menu.confirm_no", language());
    no.action = "no";
    drawButton(no, contains(no.rect, mouseScreen_));
}

void SfmlApp::drawButton(const Button& button, bool hovered) {
    drawPanel(window_, button.rect,
              hovered ? withAlpha(theme().panelLight, 250)
                      : withAlpha(theme().panelLight, 200),
              hovered ? theme().accent : theme().border,
              hovered ? 2.0f : 1.0f);
    drawText(window_, font_, button.label, 15,
             button.enabled ? theme().text : theme().textDim,
             {button.rect.position.x + button.rect.size.x * 0.5f,
              button.rect.position.y + button.rect.size.y * 0.5f - 10.0f},
             Align::Center, hovered);
}

void SfmlApp::drawToolIcon(sf::RenderTarget& target, cr::ToolType tool,
                           sf::Vector2f center, float scale, bool owned) {
    const int alpha = owned ? 255 : 90;
    const sf::Color handle =
        withAlpha(sf::Color(126, 90, 54), owned ? 255 : 70);
    const sf::Color stone = withAlpha(sf::Color(154, 158, 166), alpha);
    const sf::Color iron = withAlpha(sf::Color(196, 204, 216), alpha);
    const sf::Color flame = withAlpha(sf::Color(244, 168, 62), alpha);

    const auto addHandle = [&](float length, sf::Color color) {
        sf::RectangleShape shape({5.0f * scale, length * scale});
        shape.setOrigin({2.5f * scale, length * scale * 0.88f});
        shape.setPosition(center);
        shape.setRotation(sf::degrees(-38.0f));
        shape.setFillColor(color);
        target.draw(shape);
    };

    if (tool == cr::ToolType::Torch) {
        addHandle(34.0f, handle);
        sf::CircleShape glow(11.0f * scale);
        glow.setOrigin({11.0f * scale, 11.0f * scale});
        glow.setPosition({center.x + 10.0f * scale, center.y - 20.0f * scale});
        glow.setFillColor(withAlpha(flame, owned ? 70 : 24));
        target.draw(glow);
        sf::CircleShape core(5.0f * scale);
        core.setOrigin({5.0f * scale, 5.0f * scale});
        core.setPosition({center.x + 10.0f * scale, center.y - 20.0f * scale});
        core.setFillColor(flame);
        target.draw(core);
        return;
    }

    addHandle(32.0f, handle);

    sf::RectangleShape head({18.0f * scale, 9.0f * scale});
    head.setOrigin({9.0f * scale, 4.5f * scale});
    head.setPosition({center.x + 8.0f * scale, center.y - 15.0f * scale});
    head.setRotation(sf::degrees(-38.0f));
    head.setFillColor(tool == cr::ToolType::IronPick ? iron : stone);
    head.setOutlineThickness(1.0f);
    head.setOutlineColor(withAlpha(sf::Color(40, 44, 52), alpha));
    target.draw(head);

    if (tool == cr::ToolType::IronPick) {
        sf::RectangleShape tip({6.0f * scale, 18.0f * scale});
        tip.setOrigin({3.0f * scale, 9.0f * scale});
        tip.setPosition({center.x + 15.0f * scale, center.y - 16.0f * scale});
        tip.setRotation(sf::degrees(52.0f));
        tip.setFillColor(iron);
        target.draw(tip);
    }
}

void SfmlApp::drawResourceDot(sf::RenderTarget& target,
                              cr::ResourceType resource, sf::Vector2f position,
                              float radius) {
    sf::CircleShape dot(radius);
    dot.setOrigin({radius, radius});
    dot.setPosition(position);
    dot.setFillColor(resourceColor(resource));
    dot.setOutlineThickness(1.0f);
    dot.setOutlineColor(sf::Color(16, 20, 24, 200));
    target.draw(dot);
}

// ---------------------------------------------------------------------------
//  Actions
// ---------------------------------------------------------------------------

void SfmlApp::saveGame() {
    game_.saveToFile(savePath_);
    processGameEvents();
}

void SfmlApp::loadGame() {
    if (!game_.loadFromFile(savePath_)) {
        if (game_.lastError == "load.not_found") {
            // The core emits a generic load.failed event; replace it with a
            // clearer "no save file" notice.
            game_.clearEvents();
            pushNote(tr("menu.no_save", language()), theme().warning);
        }
    } else {
        cameraCenter_ = toSf(game_.player.pos);
        hasMoveTarget_ = false;
        autoNodeId_ = -1;
        autoAnimalId_ = -1;
        updateViewSize();
    }
    processGameEvents();
}

void SfmlApp::newGame() {
    const std::string name = game_.playerName;
    game_.newRun(cr::Game::makeSeed(), name);
    game_.settings.tutorial = true;
    cameraCenter_ = toSf(game_.player.pos);
    hasMoveTarget_ = false;
    autoNodeId_ = -1;
    autoAnimalId_ = -1;
    confirmNew_ = false;
    overlay_ = Overlay::None;
    notes_.clear();
    autosaveTimer_ = 0.0f;
    updateViewSize();
    pushNote(tr("menu.new_started", language()), theme().success);
}

void SfmlApp::respawn() {
    game_.respawn();
    cameraCenter_ = toSf(game_.player.pos);
    hasMoveTarget_ = false;
    autoNodeId_ = -1;
    autoAnimalId_ = -1;
    processGameEvents();
}

void SfmlApp::toggleFullscreen() {
    setFullscreen(!fullscreen_);
}

void SfmlApp::setFullscreen(bool enabled) {
    fullscreen_ = enabled;
    game_.settings.fullscreen = enabled;
    if (enabled) {
        window_.create(sf::VideoMode::getDesktopMode(), "Copy Rust 2D",
                       sf::State::Fullscreen);
    } else {
        window_.create(sf::VideoMode({1280, 720}), "Copy Rust 2D",
                       sf::Style::Default, sf::State::Windowed);
        window_.setVerticalSyncEnabled(true);
    }
    window_.setKeyRepeatEnabled(false);
    window_.setVerticalSyncEnabled(true);
    updateViewSize();
}

void SfmlApp::closeOverlay() {
    overlay_ = Overlay::None;
    confirmNew_ = false;
}

void SfmlApp::processGameEvents() {
    for (const cr::GameEvent& event : game_.events) {
        const std::string text = describeEvent(event);
        if (text.empty()) {
            continue;
        }

        sf::Color color = theme().text;
        const std::string& key = event.key;
        if (key == "player.hurt" || key == "animal.attack" ||
            key == "player.died" || key == "stat.low" ||
            key == "tool.broken" || key == "armor.broken" ||
            key == "save.failed" || key == "load.failed") {
            color = theme().danger;
            if (key == "player.hurt" || key == "animal.attack") {
                shake_ = 0.28f;
            }
        } else if (key == "node.hit" || key == "animal.hit" ||
                   key == "animal.killed" || key == "craft.ok" ||
                   key == "ate.berry" || key == "rest.ok" ||
                   key == "saved" || key == "loaded") {
            color = theme().success;
        } else if (key == "gather.need_tool" || key == "craft.missing" ||
                   key == "tool.not_owned" || key == "eat.none" ||
                   key == "rest.full") {
            color = theme().warning;
        } else if (key == "new_day") {
            color = theme().accent;
        } else {
            color = theme().textDim;
        }
        pushNote(text, color);
    }
    game_.clearEvents();
}

void SfmlApp::pushNote(const std::string& text, sf::Color color, float ttl) {
    if (text.empty()) {
        return;
    }
    Note note;
    note.text = text;
    note.color = color;
    note.ttl = ttl;
    note.maxTtl = ttl;
    notes_.push_back(std::move(note));
    while (notes_.size() > 8) {
        notes_.pop_front();
    }
}

std::string SfmlApp::describeEvent(const cr::GameEvent& event) const {
    const std::string& key = event.key;
    const cr::Language lang = language();

    if (key == "node.hit") {
        return format(tr("notify.gather", lang),
                      {{"amount", std::to_string(event.amount)},
                       {"resource", resourceName(event.resource, lang)}});
    }
    if (key == "node.depleted") {
        return format(tr("notify.depleted", lang),
                      {{"node", nodeName(event.node, lang)}});
    }
    if (key == "node.respawned") {
        return format(tr("notify.respawned", lang),
                      {{"node", nodeName(event.node, lang)}});
    }
    if (key == "gather.need_tool") {
        return format(tr("notify.need_tool", lang),
                      {{"tool", toolName(event.tool, lang)},
                       {"node", nodeName(event.node, lang)}});
    }
    if (key == "gather.depleted") {
        return format(tr("notify.depleted", lang),
                      {{"node", nodeName(event.node, lang)}});
    }
    if (key == "nothing_near") {
        return tr("notify.nothing", lang);
    }
    if (key == "animal.hit") {
        return format(tr("notify.hit_animal", lang),
                      {{"animal", animalName(event.animal, lang)},
                       {"amount", std::to_string(event.amount)}});
    }
    if (key == "animal.killed") {
        std::string text = format(tr("notify.killed", lang),
                                  {{"animal", animalName(event.animal, lang)}});
        for (const cr::ResourceAmount& loot : event.loot) {
            text += "  +" + std::to_string(loot.amount) + " " +
                    resourceName(loot.resource, lang);
        }
        return text;
    }
    if (key == "animal.attack") {
        return format(tr("notify.animal_attack", lang),
                      {{"animal", animalName(event.animal, lang)},
                       {"amount", std::to_string(event.amount)}});
    }
    if (key == "animal.too_far") {
        return format(tr("notify.too_far", lang),
                      {{"animal", animalName(event.animal, lang)}});
    }
    if (key == "animal.respawned") {
        return format(tr("notify.animal_respawn", lang),
                      {{"animal", animalName(event.animal, lang)}});
    }
    if (key == "player.hurt") {
        return formatInt(tr("notify.hurt", lang), "amount", event.amount);
    }
    if (key == "player.died") {
        return tr("notify.died", lang);
    }
    if (key == "player.healed") {
        return formatInt(tr("notify.healed", lang), "amount", event.amount);
    }
    if (key == "respawned") {
        return tr("notify.respawn", lang);
    }
    if (key == "stat.low") {
        return format(tr("notify.low", lang),
                      {{"stat", statName(event.stat, lang)}});
    }
    if (key == "craft.ok") {
        return format(tr("notify.crafted", lang),
                      {{"recipe", recipeName(event.recipe, lang)}});
    }
    if (key == "craft.missing") {
        return format(tr("notify.missing", lang),
                      {{"amount", std::to_string(event.amount)},
                       {"resource", resourceName(event.resource, lang)}});
    }
    if (key == "craft.unknown") {
        return tr("notify.unknown_recipe", lang);
    }
    if (key == "armor.equipped") {
        return format(tr("notify.equipped", lang),
                      {{"armor", armorName(event.armor, lang)}});
    }
    if (key == "armor.unequipped") {
        return format(tr("notify.unequipped", lang),
                      {{"armor", armorName(event.armor, lang)}});
    }
    if (key == "armor.already") {
        return tr("notify.already_equipped", lang);
    }
    if (key == "armor.none") {
        return format(tr("notify.no_armor", lang),
                      {{"armor", armorName(event.armor, lang)}});
    }
    if (key == "armor.not_worn") {
        return tr("notify.not_worn", lang);
    }
    if (key == "armor.broken") {
        return format(tr("notify.armor_broken", lang),
                      {{"armor", armorName(event.armor, lang)}});
    }
    if (key == "tool.broken") {
        return format(tr("notify.tool_broken", lang),
                      {{"tool", toolName(event.tool, lang)}});
    }
    if (key == "tool.selected") {
        return format(tr("notify.tool_selected", lang),
                      {{"tool", toolName(event.tool, lang)}});
    }
    if (key == "tool.not_owned") {
        return format(tr("notify.tool_not_owned", lang),
                      {{"tool", toolName(event.tool, lang)}});
    }
    if (key == "eat.none") {
        return tr("notify.no_berries", lang);
    }
    if (key == "ate.berry") {
        return tr("notify.ate", lang);
    }
    if (key == "rest.ok") {
        return formatInt(tr("notify.rest", lang), "amount", event.amount);
    }
    if (key == "rest.full") {
        return tr("notify.rest_full", lang);
    }
    if (key == "rest.need_base") {
        return tr("notify.rest_base", lang);
    }
    if (key == "new_day") {
        return formatInt(tr("notify.new_day", lang), "amount", event.amount);
    }
    if (key == "saved") {
        return tr("notify.saved", lang);
    }
    if (key == "loaded") {
        return tr("notify.loaded", lang);
    }
    if (key == "save.failed") {
        return tr("notify.save_failed", lang);
    }
    if (key == "load.failed") {
        return tr("notify.load_failed", lang);
    }
    return {};
}

std::string SfmlApp::tutorialText() const {
    if (game_.stats.gathered <= 0) {
        return tr("tutorial.step1", language());
    }
    if (game_.stats.crafted <= 0) {
        return tr("tutorial.step2", language());
    }
    if (game_.resource(cr::ResourceType::Berry) <= 0) {
        return tr("tutorial.step3", language());
    }
    if (game_.stats.kills <= 0) {
        return tr("tutorial.step4", language());
    }
    if (game_.player.energy < 55.0f) {
        return tr("tutorial.step5", language());
    }
    return tr("tutorial.step6", language());
}

std::string SfmlApp::currentPrompt(sf::Color* color) const {
    const cr::Language lang = language();

    cr::Animal* animal = game_.nearestAnimal(game_.player.pos, cr::kAttackRange);
    cr::Node* node = game_.nearestNode(game_.player.pos, cr::kInteractRange);

    if (animal != nullptr) {
        if (color != nullptr) {
            *color = cr::animalRule(animal->type).aggressive ? theme().danger
                                                             : theme().accent;
        }
        return tr("prompt.attack", lang) + "  ·  " +
               animalName(animal->type, lang);
    }
    if (node != nullptr) {
        const cr::GatherPlan plan = game_.planGather(*node);
        if (!plan.possible) {
            if (color != nullptr) {
                *color = theme().warning;
            }
            return format(tr("notify.need_tool", lang),
                          {{"tool", toolName(plan.tool, lang)},
                           {"node", nodeName(node->type, lang)}});
        }
        if (color != nullptr) {
            *color = theme().accent;
        }
        return tr("prompt.gather", lang) + "  ·  " +
               nodeName(node->type, lang);
    }
    if (game_.nearBase()) {
        if (color != nullptr) {
            *color = theme().success;
        }
        return tr("prompt.rest", lang);
    }
    if (color != nullptr) {
        *color = theme().textDim;
    }
    return tr("prompt.click", lang);
}

std::string SfmlApp::formatRecipeCost(const cr::Recipe& recipe) const {
    std::string result;
    for (std::size_t i = 0; i < recipe.cost.size(); ++i) {
        const cr::Ingredient& ingredient = recipe.cost[i];
        if (i > 0) {
            result += " · ";
        }
        result += std::to_string(ingredient.amount) + " " +
                  resourceName(ingredient.resource, language());
    }
    return result;
}

std::string SfmlApp::toolLabel(cr::ToolType tool) const {
    if (tool == cr::ToolType::Count) {
        return tr("ui.none", language());
    }
    return toolName(tool, language()) + " (" +
           std::to_string(static_cast<int>(game_.toolDurability(tool))) + ")";
}

// ---------------------------------------------------------------------------
//  Layout
// ---------------------------------------------------------------------------

sf::FloatRect SfmlApp::centered(sf::Vector2f size) const {
    const sf::Vector2f windowSize{
        static_cast<float>(window_.getSize().x),
        static_cast<float>(window_.getSize().y)};
    return {{(windowSize.x - size.x) * 0.5f, (windowSize.y - size.y) * 0.5f},
            size};
}

sf::FloatRect SfmlApp::craftPanelRect() const {
    const sf::Vector2f windowSize{
        static_cast<float>(window_.getSize().x),
        static_cast<float>(window_.getSize().y)};
    const sf::Vector2f size{std::min(760.0f, windowSize.x - 60.0f),
                            std::min(560.0f, windowSize.y - 60.0f)};
    return centered(size);
}

sf::FloatRect SfmlApp::craftRowRect(int index) const {
    const sf::FloatRect panel = craftPanelRect();
    const float rowHeight = (panel.size.y - 110.0f) / 9.0f;
    return {{panel.position.x + 18.0f,
             panel.position.y + 88.0f + index * rowHeight},
            {panel.size.x - 36.0f, rowHeight - 6.0f}};
}

sf::FloatRect SfmlApp::inventoryPanelRect() const {
    const sf::Vector2f windowSize{
        static_cast<float>(window_.getSize().x),
        static_cast<float>(window_.getSize().y)};
    const sf::Vector2f size{std::min(900.0f, windowSize.x - 60.0f),
                            std::min(560.0f, windowSize.y - 60.0f)};
    return centered(size);
}

sf::FloatRect SfmlApp::armorRowRect(int index) const {
    const sf::FloatRect panel = inventoryPanelRect();
    return {{panel.position.x + panel.size.x * 0.74f,
             panel.position.y + 88.0f + index * 66.0f},
            {panel.size.x * 0.23f, 58.0f}};
}

sf::FloatRect SfmlApp::toolRowRect(int index) const {
    const sf::FloatRect panel = inventoryPanelRect();
    return {{panel.position.x + panel.size.x * 0.50f,
             panel.position.y + 88.0f + index * 66.0f},
            {panel.size.x * 0.21f, 58.0f}};
}

sf::FloatRect SfmlApp::pausePanelRect() const {
    const sf::Vector2f windowSize{
        static_cast<float>(window_.getSize().x),
        static_cast<float>(window_.getSize().y)};
    const sf::Vector2f size{std::min(440.0f, windowSize.x - 60.0f),
                            std::min(560.0f, windowSize.y - 60.0f)};
    return centered(size);
}

sf::FloatRect SfmlApp::pauseButtonRect(int index) const {
    const sf::FloatRect panel = pausePanelRect();
    return {{panel.position.x + 36.0f,
             panel.position.y + 82.0f + index * 48.0f},
            {panel.size.x - 72.0f, 38.0f}};
}

sf::FloatRect SfmlApp::helpPanelRect() const {
    const sf::Vector2f windowSize{
        static_cast<float>(window_.getSize().x),
        static_cast<float>(window_.getSize().y)};
    const sf::Vector2f size{std::min(820.0f, windowSize.x - 60.0f),
                            std::min(600.0f, windowSize.y - 60.0f)};
    return centered(size);
}

sf::FloatRect SfmlApp::mapPanelRect() const {
    const sf::Vector2f windowSize{
        static_cast<float>(window_.getSize().x),
        static_cast<float>(window_.getSize().y)};
    const sf::Vector2f size{std::min(860.0f, windowSize.x - 60.0f),
                            std::min(640.0f, windowSize.y - 60.0f)};
    return centered(size);
}

sf::FloatRect SfmlApp::confirmPanelRect() const {
    return centered({540.0f, 230.0f});
}

sf::FloatRect SfmlApp::confirmYesRect() const {
    const sf::FloatRect panel = confirmPanelRect();
    return {{panel.position.x + 54.0f, panel.position.y + panel.size.y - 62.0f},
            {200.0f, 40.0f}};
}

sf::FloatRect SfmlApp::confirmNoRect() const {
    const sf::FloatRect panel = confirmPanelRect();
    return {{panel.position.x + panel.size.x - 254.0f,
             panel.position.y + panel.size.y - 62.0f},
            {200.0f, 40.0f}};
}

std::vector<SfmlApp::Button> SfmlApp::pauseButtons() const {
    const cr::Language lang = language();
    std::vector<Button> buttons;

    const auto add = [&](const std::string& label, const std::string& action) {
        Button button;
        button.rect = pauseButtonRect(static_cast<int>(buttons.size()));
        button.label = label;
        button.action = action;
        buttons.push_back(button);
    };

    add(tr("menu.resume", lang), "resume");
    add(tr("menu.new_game", lang), "new");
    add(tr("menu.save", lang), "save");
    add(tr("menu.load", lang), "load");
    add(tr("menu.help", lang), "help");
    add(tr("menu.language", lang) + ": " +
            tr(lang == cr::Language::Russian ? "menu.language_ru"
                                             : "menu.language_en",
               lang),
        "language");
    add(tr("menu.fullscreen", lang) + ": " +
            tr(fullscreen_ ? "ui.on" : "ui.off", lang),
        "fullscreen");
    add(tr("menu.tutorial", lang) + ": " +
            tr(game_.settings.tutorial ? "ui.on" : "ui.off", lang),
        "tutorial");
    add(tr("menu.quit", lang), "quit");
    return buttons;
}

void SfmlApp::handlePauseAction(const std::string& action) {
    if (action == "resume") {
        closeOverlay();
    } else if (action == "new") {
        confirmNew_ = true;
    } else if (action == "save") {
        saveGame();
    } else if (action == "load") {
        loadGame();
    } else if (action == "help") {
        overlay_ = Overlay::Help;
    } else if (action == "language") {
        game_.settings.language =
            language() == cr::Language::Russian ? cr::Language::English
                                                : cr::Language::Russian;
        pushNote(tr("menu.language", language()) + ": " +
                     tr(language() == cr::Language::Russian
                            ? "menu.language_ru"
                            : "menu.language_en",
                        language()),
                 theme().accent);
    } else if (action == "fullscreen") {
        toggleFullscreen();
    } else if (action == "tutorial") {
        game_.settings.tutorial = !game_.settings.tutorial;
    } else if (action == "quit") {
        saveGame();
        running_ = false;
        window_.close();
    }
}

void SfmlApp::handleCraftClick(sf::Vector2f point) {
    const std::vector<cr::Recipe>& recipes = game_.recipes();
    for (int i = 0; i < static_cast<int>(recipes.size()); ++i) {
        if (contains(craftRowRect(i), point)) {
            game_.craft(i);
            processGameEvents();
            return;
        }
    }
    if (!contains(craftPanelRect(), point)) {
        closeOverlay();
    }
}

void SfmlApp::handleInventoryClick(sf::Vector2f point) {
    for (int i = 0; i < cr::kArmorCount; ++i) {
        if (contains(armorRowRect(i), point)) {
            const cr::ArmorType type = static_cast<cr::ArmorType>(i);
            if (game_.inventory.armorWorn[cr::idx(type)]) {
                game_.unequipArmor(type);
            } else {
                game_.equipArmor(type);
            }
            processGameEvents();
            return;
        }
    }
    for (int i = 0; i < cr::kToolCount; ++i) {
        if (contains(toolRowRect(i), point)) {
            game_.selectTool(static_cast<cr::ToolType>(i));
            processGameEvents();
            return;
        }
    }
    if (!contains(inventoryPanelRect(), point)) {
        closeOverlay();
    }
}

void SfmlApp::handlePauseClick(sf::Vector2f point) {
    for (const Button& button : pauseButtons()) {
        if (contains(button.rect, point)) {
            handlePauseAction(button.action);
            return;
        }
    }
}

void SfmlApp::handleConfirmClick(sf::Vector2f point) {
    if (contains(confirmYesRect(), point)) {
        newGame();
    } else if (contains(confirmNoRect(), point)) {
        confirmNew_ = false;
    }
}

} // namespace ui
