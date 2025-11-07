#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <vector>

#include "../GameEngine/GameEngine.h"
#include "../GameEngine/Board/Cell.h"

constexpr int BOARD_WIDTH = 10;
constexpr int BOARD_HEIGHT = 20;
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 800;
constexpr int FPS = 60;

enum class ScreenState {
    MAIN_MENU,
    PLAYING,
    PAUSED,
    GAME_OVER,
    LEADERBOARD,
    SAVE_SCORE,
    LOAD_GAME,
    QUITTING
};

class Renderer final : public IObserver, public std::enable_shared_from_this<Renderer> {
public:
    Renderer();
    ~Renderer() override;
    void initializeObserver();
    void run();

private:
    sf::RenderWindow window;
    sf::Font font;

    size_t windowWidth = 800;
    size_t windowHeight = 800;

    InputHandler& inputHandler;
    ScoreManager& scoreManager;
    GameEngine& gameEngine;
    std::atomic<bool> renderFlag{false};

    ScreenState currentScreen;
    std::string playerNameInput;
    bool gameLoadedSuccessfully;
    bool gameSavedSuccessfully;
    sf::Clock keyHeldClock;

    sf::Texture logoTexture;
    sf::Sprite logoSprite;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Texture blockTexture;
    std::map<Cell, sf::IntRect> textureMap;
    const float cellSize = 32.f;
    std::unordered_map<Cell, sf::Texture> pieceImages;
    std::unordered_map<Cell, sf::Sprite> pieceSprites;

    void onStateChanged() override;

    void processEvents();
    void update();
    void render();

    void handleMouseClick(sf::Vector2i mousePos);
    void handleTextInput(sf::Uint32 unicode);
    void handleKeyboardInput(sf::Keyboard::Key key);
    void handleContinuousInput();
    void handleResize(size_t width, size_t height);

    void renderMainMenu();
    void renderPlaying();
    void renderPaused();
    void renderGameOver();
    void renderLeaderboard();
    void renderSaveScore();
    void renderLoadGame();
    void renderQuitting();

    void loadFont();
    void loadTextures();
    void drawGrid(sf::RenderTarget& target, const std::vector<std::vector<Cell>>& grid, sf::Vector2f position) const;
    static void centerText(sf::Text& text, sf::Vector2f centerPos);
    static void centerTextInRect(sf::Text& text, sf::FloatRect rect);
};