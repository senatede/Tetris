#include "Renderer.h"
#include "Button.h"
#include "GameEngine/InputHandler.h"
#include "GameEngine/ScoreManagement/ScoreManager.h"
#include <iostream>
#include <filesystem>

Renderer::Renderer()
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Tetris!"),
      inputHandler(InputHandler::getInstance()),
      scoreManager(ScoreManager::getInstance()),
      gameEngine(GameEngine::getInstance(BOARD_WIDTH, BOARD_HEIGHT, inputHandler, scoreManager)),
      currentScreen(ScreenState::MAIN_MENU),
      gameLoadedSuccessfully(false),
      gameSavedSuccessfully(false)
{
    gameEngine.setObserver(this);
    window.setFramerateLimit(FPS);
    loadFont();
    loadTextures();
}

Renderer::~Renderer() {
    gameEngine.setObserver(nullptr);
}

void Renderer::onStateChanged() {
    renderFlag.store(true);
}

void Renderer::loadFont() {
    std::string path = std::filesystem::path(__FILE__).parent_path();
    path += "/res/fonts/momo.ttf";
    if (!font.loadFromFile(path)) {
        std::cerr << "Error: Could not load font from " << path << std::endl;
        exit(1);
    }
}

void Renderer::loadTextures() {
    std::string folder_path = std::filesystem::path(__FILE__).parent_path();
    folder_path += "/res/images/";

    if (!logoTexture.loadFromFile(folder_path + "logo.png"))
        std::cerr << "Error: Could not load logo.png" << std::endl;

    logoSprite.setTexture(logoTexture);
    logoSprite.setPosition(WINDOW_WIDTH / 2.0 - logoTexture.getSize().x / 2.f, 60.f);

    if (!backgroundTexture.loadFromFile(folder_path + "bg.png")) {
        std::cerr << "Error: Could not load background image" << std::endl;
    }
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setScale(
        static_cast<float>(WINDOW_WIDTH) / backgroundTexture.getSize().x,
        static_cast<float>(WINDOW_HEIGHT) / backgroundTexture.getSize().y
    );
    backgroundSprite.setPosition(0.f, 0.f);


    if (!blockTexture.loadFromFile(folder_path + "cells.png")) {
        std::cerr << "Error: Could not load texture from cells.png" << std::endl;
        exit(1);
    }
    int s = static_cast<int>(cellSize);

    textureMap[Cell::I] = sf::IntRect(s * 0, 0, s, s);
    textureMap[Cell::J] = sf::IntRect(s * 1, 0, s, s);
    textureMap[Cell::L] = sf::IntRect(s * 2, 0, s, s);
    textureMap[Cell::O] = sf::IntRect(s * 3, 0, s, s);
    textureMap[Cell::S] = sf::IntRect(s * 4, 0, s, s);
    textureMap[Cell::T] = sf::IntRect(s * 5, 0, s, s);
    textureMap[Cell::Z] = sf::IntRect(s * 6, 0, s, s);

    sf::IntRect ghostRect(s * 7, 0, s, s);
    textureMap[Cell::GhostI] = ghostRect;
    textureMap[Cell::GhostJ] = ghostRect;
    textureMap[Cell::GhostL] = ghostRect;
    textureMap[Cell::GhostO] = ghostRect;
    textureMap[Cell::GhostS] = ghostRect;
    textureMap[Cell::GhostT] = ghostRect;
    textureMap[Cell::GhostZ] = ghostRect;


    std::vector<std::pair<Cell, std::string>> imageFiles = {
        {Cell::I, folder_path + "blocks/I.png"},
        {Cell::J, folder_path + "blocks/J.png"},
        {Cell::L, folder_path + "blocks/L.png"},
        {Cell::O, folder_path + "blocks/O.png"},
        {Cell::S, folder_path + "blocks/S.png"},
        {Cell::T, folder_path + "blocks/T.png"},
        {Cell::Z, folder_path + "blocks/Z.png"}
    };

    for (auto& [type, filename] : imageFiles) {
        sf::Texture texture;
        if (!texture.loadFromFile(filename)) {
            std::cerr << "Error: Could not load " << filename << std::endl;
            continue;
        }
        pieceImages[type] = texture;
        const sf::Sprite sprite(pieceImages[type]);
        pieceSprites[type] = sprite;
    }
}

void Renderer::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Renderer::processEvents() {
    sf::Event event{};
    while (window.pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
            window.close();
            break;
        case sf::Event::MouseButtonPressed:
            if (event.mouseButton.button == sf::Mouse::Left)
                handleMouseClick(sf::Mouse::getPosition(window));
            break;
        case sf::Event::TextEntered:
            handleTextInput(event.text.unicode);
            break;
        case sf::Event::KeyPressed:
            handleKeyboardInput(event.key.code);
            break;
        case sf::Event::Resized:
            handleResize(event.size.width, event.size.height);
            break;
        default:
            break;
        }
    }
}

void Renderer::handleMouseClick(sf::Vector2i mousePos) {
    sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);
    gameSavedSuccessfully = false;

    switch (currentScreen) {
        case ScreenState::MAIN_MENU: {
            Button playBtn("Play", font, {300, 250}, {200, 50});
            Button loadBtn("Load Game", font, {300, 320}, {200, 50});
            Button leaderBtn("Leaderboard", font, {300, 390}, {200, 50});
            Button quitBtn("Quit", font, {300, 460}, {200, 50});

            if (playBtn.isClicked(mousePosF)) {
                gameEngine.startNewGame();
                currentScreen = ScreenState::PLAYING;
            } else if (loadBtn.isClicked(mousePosF)) {
                currentScreen = ScreenState::LOAD_GAME;
            } else if (leaderBtn.isClicked(mousePosF)) {
                currentScreen = ScreenState::LEADERBOARD;
            } else if (quitBtn.isClicked(mousePosF)) {
                currentScreen = ScreenState::QUITTING;
            }
            break;
        }
        case ScreenState::PAUSED: {
            Button resumeBtn("Resume", font, {300, 250}, {200, 50});
            Button saveBtn("Save Game", font, {300, 320}, {200, 50});
            Button leaderBtn("Leaderboard", font, {300, 390}, {200, 50});
            Button quitBtn("Quit to Menu", font, {300, 460}, {200, 50});

            if (resumeBtn.isClicked(mousePosF)) {
                inputHandler.handleKey(KeyType::RESUME);
                currentScreen = ScreenState::PLAYING;
            } else if (saveBtn.isClicked(mousePosF)) {
                inputHandler.handleKey(KeyType::SAVE);
                gameSavedSuccessfully = true;
            } else if (leaderBtn.isClicked(mousePosF)) {
                currentScreen = ScreenState::LEADERBOARD;
            } else if (quitBtn.isClicked(mousePosF)) {
                currentScreen = ScreenState::MAIN_MENU;
            }
            break;
        }
        case ScreenState::GAME_OVER: {
            Button playAgainBtn("Play Again", font, {300, 350}, {200, 50});
            Button menuBtn("Main Menu", font, {300, 420}, {200, 50});

            if (playAgainBtn.isClicked(mousePosF)) {
                gameEngine.startNewGame();
                currentScreen = ScreenState::PLAYING;
            } else if (menuBtn.isClicked(mousePosF)) {
                currentScreen = ScreenState::MAIN_MENU;
            }
            break;
        }
        case ScreenState::LEADERBOARD: {
            Button backBtn("Back", font, {300, 700}, {200, 50});
            if (backBtn.isClicked(mousePosF)) {
                if (gameEngine.getGameState() == GameState::PAUSED)
                    currentScreen = ScreenState::PAUSED;
                else
                    currentScreen = ScreenState::MAIN_MENU;
            }
            break;
    }
    case ScreenState::LOAD_GAME: {
        Button playBtn("Play", font, {300, 350}, {200, 50});
        Button backBtn("Back", font, {300, 420}, {200, 50});

        if (gameLoadedSuccessfully && playBtn.isClicked(mousePosF)) {
            gameEngine.startGame();
            currentScreen = ScreenState::PLAYING;
        } else if (backBtn.isClicked(mousePosF)) {
            currentScreen = ScreenState::MAIN_MENU;
        }
        break;
    }
    case ScreenState::QUITTING: {
        Button yesBtn("Yes", font, {250, 350}, {100, 50});
        Button noBtn("No", font, {450, 350}, {100, 50});

        if (yesBtn.isClicked(mousePosF)) {
            window.close();
        } else if (noBtn.isClicked(mousePosF)) {
            if (gameEngine.getGameState() == GameState::PAUSED) {
                inputHandler.handleKey(KeyType::RESUME);
                currentScreen = ScreenState::PAUSED;
            } else {
                currentScreen = ScreenState::MAIN_MENU;
            }
        }
        break;
    }
    default:
        break;
    }
}

void Renderer::handleTextInput(sf::Uint32 unicode) {
    if (currentScreen != ScreenState::SAVE_SCORE) return;

    if (unicode == 8 && !playerNameInput.empty()) {
        playerNameInput.pop_back();
    }
    else if (unicode >= 32 && unicode < 128 && playerNameInput.length() < 15) {
        playerNameInput += static_cast<char>(unicode);
    }
}

void Renderer::handleKeyboardInput(sf::Keyboard::Key key) {
    if (currentScreen == ScreenState::SAVE_SCORE && key == sf::Keyboard::Enter) {
        if (playerNameInput.empty()) playerNameInput = "Player";
        scoreManager.saveScore(playerNameInput);
        currentScreen = ScreenState::LEADERBOARD;
        return;
    }

    if (currentScreen != ScreenState::PLAYING) return;

    switch (key) {
    case sf::Keyboard::W:
    case sf::Keyboard::Up:
        inputHandler.handleKey(KeyType::ROTATE_CW);
        break;
    case sf::Keyboard::Q:
    case sf::Keyboard::Z:
        inputHandler.handleKey(KeyType::ROTATE_CCW);
        break;
    case sf::Keyboard::Space:
        inputHandler.handleKey(KeyType::HARD_DROP);
        break;
    case sf::Keyboard::H:
    case sf::Keyboard::C:
        inputHandler.handleKey(KeyType::HOLD);
        break;
    case sf::Keyboard::P:
    case sf::Keyboard::Escape:
        inputHandler.handleKey(KeyType::PAUSE);
        currentScreen = ScreenState::PAUSED;
        break;
    default:
        break;
    }
}

void Renderer::handleContinuousInput() {
    if (currentScreen != ScreenState::PLAYING) return;

    if (keyHeldClock.getElapsedTime().asMilliseconds() > 100) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            inputHandler.handleKey(KeyType::LEFT);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            inputHandler.handleKey(KeyType::RIGHT);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            inputHandler.handleKey(KeyType::SOFT_DROP);
        }
        keyHeldClock.restart();
    }
}


void Renderer::handleResize(const size_t width, const size_t height) {
    windowHeight = height;
    windowWidth = width;
}

void Renderer::update() {
    handleContinuousInput();

    switch (currentScreen) {
        case ScreenState::PLAYING:
            if (renderFlag.exchange(false)) {
                if (gameEngine.getGameState() == GameState::GAME_OVER) {
                    if (scoreManager.isAGoodScore()) {
                        playerNameInput = "";
                        currentScreen = ScreenState::SAVE_SCORE;
                    } else {
                        currentScreen = ScreenState::GAME_OVER;
                    }
                }
            }
            break;
        case ScreenState::LOAD_GAME:
            if (!gameLoadedSuccessfully) {
                inputHandler.handleKey(KeyType::LOAD);
                if (gameEngine.getGameState() == GameState::LOADED) {
                    gameLoadedSuccessfully = true;
                }
            }
            break;
        default:
            break;
    }
}

void Renderer::render() {
    window.setView(window.getDefaultView());
    window.draw(backgroundSprite);

    sf::View fixedView(sf::FloatRect(0, 0, windowWidth, windowHeight));
    fixedView.setCenter(sf::Vector2f(WINDOW_WIDTH / 2.0, WINDOW_HEIGHT / 2.0));
    window.setView(fixedView);

    switch (currentScreen) {
    case ScreenState::MAIN_MENU:    renderMainMenu();     break;
    case ScreenState::PLAYING:      renderPlaying();      break;
    case ScreenState::PAUSED:       renderPaused();       break;
    case ScreenState::GAME_OVER:    renderGameOver();     break;
    case ScreenState::LEADERBOARD:  renderLeaderboard();  break;
    case ScreenState::SAVE_SCORE:   renderSaveScore();    break;
    case ScreenState::LOAD_GAME:    renderLoadGame();     break;
    case ScreenState::QUITTING:     renderQuitting();     break;
    }

    window.display();
}

void Renderer::renderMainMenu() {
    window.draw(logoSprite);

    Button playBtn("Play", font, {300, 250}, {200, 50});
    Button loadBtn("Load Game", font, {300, 320}, {200, 50});
    Button leaderBtn("Leaderboard", font, {300, 390}, {200, 50});
    Button quitBtn("Quit", font, {300, 460}, {200, 50});

    playBtn.draw(window);
    loadBtn.draw(window);
    leaderBtn.draw(window);
    quitBtn.draw(window);
}

void Renderer::renderPlaying() {
    const auto renderData = gameEngine.getRenderData();
    const auto grid = renderData.grid;
    const auto score = renderData.score;
    const auto level = renderData.level;
    const auto totalLinesCleared = renderData.totalLinesCleared;
    const auto holdPiece = renderData.holdType;
    const auto nextPieces = renderData.nextTypes;

    sf::Vector2f boardPosition{240.f, 75.f};
    sf::Vector2f holdPosition{90.f, 150.f};
    sf::Vector2f nextPosition{590.f, 150.f};

    sf::Text holdText("HOLD", font, 24);
    holdText.setPosition(holdPosition.x, holdPosition.y - 40);
    window.draw(holdText);
    sf::RectangleShape holdBox(sf::Vector2f(4 * cellSize, 4 * cellSize));
    holdBox.setPosition(holdPosition);
    holdBox.setFillColor(sf::Color(0, 0, 0, 0));
    holdBox.setOutlineColor(sf::Color::White);
    holdBox.setOutlineThickness(1.f);
    window.draw(holdBox);
    if (holdPiece != Cell::Empty && pieceSprites.count(holdPiece)) {
        sf::Sprite sprite = pieceSprites[holdPiece];
        sf::FloatRect spriteBounds = sprite.getLocalBounds();
        float centerX = holdPosition.x + (holdBox.getSize().x - spriteBounds.width) / 2.f;
        float centerY = holdPosition.y + (holdBox.getSize().y - spriteBounds.height) / 2.f;
        sprite.setPosition(centerX, centerY);
        window.draw(sprite);
    }

    sf::RectangleShape boardBox(sf::Vector2f(BOARD_WIDTH * cellSize, (BOARD_HEIGHT) * cellSize));
    boardBox.setPosition(boardPosition);
    boardBox.setFillColor(sf::Color(0, 0, 0, 0));
    boardBox.setOutlineColor(sf::Color::White);
    boardBox.setOutlineThickness(1.f);
    window.draw(boardBox);
    drawGrid(window, grid, boardPosition);

    sf::Text nextText("NEXT", font, 24);
    nextText.setPosition(nextPosition.x, nextPosition.y - 40);
    window.draw(nextText);
    sf::RectangleShape nextBox(sf::Vector2f(4 * cellSize, 10 * cellSize));
    nextBox.setPosition(nextPosition);
    nextBox.setFillColor(sf::Color(0, 0, 0, 0));
    nextBox.setOutlineColor(sf::Color::White);
    nextBox.setOutlineThickness(1.f);
    window.draw(nextBox);
    for (int i = 0; i < std::min((int)nextPieces.size(), 3); ++i) {
        Cell nextType = nextPieces[i];
        if (nextType != Cell::Empty && pieceSprites.count(nextType)) {
            sf::Sprite sprite = pieceSprites[nextType];
            sf::FloatRect spriteBounds = sprite.getLocalBounds();
            float centerX = nextPosition.x + (nextBox.getSize().x - spriteBounds.width) / 2.f;
            float centerY = nextPosition.y + i * 3 * cellSize + (holdBox.getSize().y - spriteBounds.height) / 2.f;
            sprite.setPosition(centerX, centerY);
            window.draw(sprite);
        }
    }

    sf::Text scoreText("Score: " + std::to_string(score), font, 24);
    scoreText.setPosition(boardPosition.x, 35);
    window.draw(scoreText);

    sf::Text levelText("Level: " + std::to_string(level), font, 24);
    levelText.setPosition(nextPosition.x, 500);
    window.draw(levelText);

    sf::Text linesText("Lines: " + std::to_string(totalLinesCleared), font, 24);
    linesText.setPosition(nextPosition.x, 550);
    window.draw(linesText);
}

void Renderer::renderPaused() {
    sf::Text title("PAUSED", font, 70);
    centerText(title, {400, 120});
    title.setFillColor(sf::Color::Yellow);
    window.draw(title);

    Button resumeBtn("Resume", font, {300, 250}, {200, 50});
    Button saveBtn("Save Game", font, {300, 320}, {200, 50});
    Button leaderBtn("Leaderboard", font, {300, 390}, {200, 50});
    Button quitBtn("Quit to Menu", font, {300, 460}, {200, 50});

    resumeBtn.draw(window);
    saveBtn.draw(window);
    leaderBtn.draw(window);
    quitBtn.draw(window);

    if (gameSavedSuccessfully) {
        sf::Text savedText("Game Saved!", font, 24);
        savedText.setFillColor(sf::Color::Green);
        centerText(savedText, {400, 530});
        window.draw(savedText);
    }
}

void Renderer::renderGameOver() {
    sf::Text title("GAME OVER", font, 70);
    centerText(title, {400, 120});
    title.setFillColor(sf::Color::Red);
    window.draw(title);

    sf::Text scoreText("Final Score: " + std::to_string(scoreManager.getScore()), font, 30);
    centerText(scoreText, {400, 250});
    window.draw(scoreText);

    Button playAgainBtn("Play Again", font, {300, 350}, {200, 50});
    Button menuBtn("Main Menu", font, {300, 420}, {200, 50});

    playAgainBtn.draw(window);
    menuBtn.draw(window);
}

void Renderer::renderLeaderboard() {
    sf::Text title("LEADERBOARD", font, 50);
    centerText(title, {400, 80});
    title.setFillColor(sf::Color::Yellow);
    window.draw(title);

    auto leaderboard = scoreManager.getLeaderboard();
    
    sf::Text rankHeader("Rank", font, 24);
    rankHeader.setPosition(150, 150);
    window.draw(rankHeader);

    sf::Text nameHeader("Name", font, 24);
    nameHeader.setPosition(250, 150);
    window.draw(nameHeader);

    sf::Text scoreHeader("Score", font, 24);
    scoreHeader.setPosition(550, 150);
    window.draw(scoreHeader);
    
    float yPos = 200.f;
    for (size_t i = 0; i < leaderboard.size(); ++i) {
        const auto& entry = leaderboard[i];
        
        sf::Text rank(std::to_string(i + 1), font, 22);
        rank.setPosition(150, yPos);
        window.draw(rank);

        sf::Text name(entry.playerName, font, 22);
        name.setPosition(250, yPos);
        window.draw(name);

        sf::Text score(std::to_string(entry.score), font, 22);
        score.setPosition(550, yPos);
        window.draw(score);

        yPos += 40.f;
    }

    Button backBtn("Back", font, {300, 700}, {200, 50});
    backBtn.draw(window);
}

void Renderer::renderSaveScore() {
    sf::Text title(scoreManager.isANewRecord() ? "NEW RECORD!" : "NEW HIGH SCORE!", font, 50);
    centerText(title, {400, 150});
    title.setFillColor(sf::Color::Green);
    window.draw(title);

    sf::Text prompt("Enter your name:", font, 24);
    centerText(prompt, {400, 250});
    window.draw(prompt);

    sf::RectangleShape inputBox({300, 50});
    inputBox.setOrigin(150, 25);
    inputBox.setPosition(400, 320);
    inputBox.setFillColor(sf::Color::Black);
    inputBox.setOutlineColor(sf::Color::White);
    inputBox.setOutlineThickness(2.f);
    window.draw(inputBox);

    sf::Text nameText(playerNameInput, font, 24);
    nameText.setPosition(260, 305);
    window.draw(nameText);

    static sf::Clock cursorClock;
    if (static_cast<int>(cursorClock.getElapsedTime().asSeconds() * 2.f) % 2 == 0) {
        float textWidth = nameText.getGlobalBounds().width;
        sf::RectangleShape cursor({2, 30});
        cursor.setPosition(260 + textWidth + 5, 305);
        cursor.setFillColor(sf::Color::White);
        window.draw(cursor);
    }
    
    sf::Text savePrompt("Press ENTER to save", font, 20);
    centerText(savePrompt, {400, 400});
    window.draw(savePrompt);
}

void Renderer::renderLoadGame() {
    sf::Text title("LOAD GAME", font, 70);
    centerText(title, {400, 120});
    title.setFillColor(sf::Color::Cyan);
    window.draw(title);

    if (gameLoadedSuccessfully) {
        sf::Text successText("Loaded Successfully!", font, 30);
        successText.setFillColor(sf::Color::Green);
        centerText(successText, {400, 250});
        window.draw(successText);

        Button playBtn("Play", font, {300, 350}, {200, 50});
        playBtn.draw(window);
    } else {
        sf::Text failText("Could not load save file.", font, 30);
        failText.setFillColor(sf::Color::Red);
        centerText(failText, {400, 250});
        window.draw(failText);
    }

    Button backBtn("Back", font, {300, 420}, {200, 50});
    backBtn.draw(window);
}

void Renderer::renderQuitting() {
    sf::Text title("Are you sure you want to quit?", font, 40);
    centerText(title, {400, 250});
    title.setFillColor(sf::Color::Yellow);
    window.draw(title);

    Button yesBtn("Yes", font, {250, 350}, {100, 50});
    Button noBtn("No", font, {450, 350}, {100, 50});

    yesBtn.draw(window);
    noBtn.draw(window);
}

void Renderer::drawGrid(sf::RenderTarget& target, const std::vector<std::vector<Cell>>& grid, sf::Vector2f position) const {
    sf::Sprite blockSprite(blockTexture);

    for (size_t y = 0; y < grid.size(); ++y) {
        for (size_t x = 0; x < grid[grid.size() - y - 1].size(); ++x) {
            Cell cell = grid[grid.size() - y - 1][x];

            if (cell != Cell::Empty) {
                sf::IntRect textureRect = textureMap.at(cell);
                blockSprite.setTextureRect(textureRect);
                blockSprite.setPosition(position.x + x * cellSize, position.y + y * cellSize);

                target.draw(blockSprite);
            }
        }
    }
}

void Renderer::centerText(sf::Text& text, const sf::Vector2f centerPos) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(centerPos);
}

void Renderer::centerTextInRect(sf::Text& text, const sf::FloatRect rect) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(rect.left + rect.width / 2.0f, rect.top + rect.height / 2.0f);
}