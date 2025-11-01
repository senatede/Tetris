#include "CLI.h"
#include "../GameEngine/InputHandler.h"
#include "../GameEngine/ScoreManagement/ScoreManager.h"
#include "../GameEngine/Board/Cell.h"

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <csignal>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>

#include "Commands/Command.h"


CLI::CLI() : inputHandler(InputHandler::getInstance()),
    scoreManager(ScoreManager::getInstance()),
    gameEngine(GameEngine::getInstance(BOARD_WIDTH, BOARD_HEIGHT, inputHandler, scoreManager)) {}

void CLI::setTerminalRawBlocking() {
    if (tcgetattr(STDIN_FILENO, &old_term) == -1) {
        perror("tcgetattr");
        return;
    }
    termios newt = old_term;
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) == -1) {
        perror("tcsetattr");
    }
    if (int flags = fcntl(STDIN_FILENO, F_GETFL, 0); flags != -1) {
        flags &= ~O_NONBLOCK;
        fcntl(STDIN_FILENO, F_SETFL, flags);
    }
}
void CLI::restoreTerminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
}
void CLI::handleSigint(int)  {
    (void)int();
    running.store(false);
    restoreTerminal();
    std::cout << "\nExiting...\n";
    exit(0);
}

void CLI::inputThreadFunc() {
    pollfd pfd{};
    pfd.fd = STDIN_FILENO;
    pfd.events = POLLIN;

    while (running.load()) {
        int ret = poll(&pfd, 1, 10);
        if (ret > 0) {
            if (pfd.revents & POLLIN) {
                char c = 0;
                ssize_t n = read(STDIN_FILENO, &c, 1);
                if (n > 0) {
                    pushKey(c);
                }
            }
        } else if (ret == 0) {
        } else {
            if (errno == EINTR) continue;
            perror("poll");
            break;
        }
    }
}
void CLI::pushKey(const char c) {
    std::lock_guard lg(input_mutex);
    input_queue.push(c);
    input_cv.notify_one();
}
bool CLI::popKey(char &out) {
    std::lock_guard lg(input_mutex);
    if (input_queue.empty()) return false;
    out = input_queue.front();
    input_queue.pop();
    return true;
}

void CLI::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    std::cout << "\033[2J\033[1;1H" << std::flush;
#endif
}
int CLI::getTerminalWidth() {
    winsize w{};
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        return 80;
    }
    return w.ws_col;
}
std::string CLI::repeat(const std::string& s, const size_t n) {
    std::string result;
    for (int i = 0; i < n; ++i) result += s;
    return result;
}
void CLI::print_in_the_center(const std::string& s, size_t len) {
    if (len == 0) len = s.length();
    const int leftPadding = (getTerminalWidth() - len) / 2;
    std::string pad(leftPadding > 0 ? leftPadding : 0, ' ');
    std::cout << pad << s << std::endl;
}

void CLI::run() {
    std::signal(SIGINT, handleSigint);
    setTerminalRawBlocking();
    running.store(true);
    input_worker = std::thread(&CLI::inputThreadFunc, this);

    while (running.load()) {
        startScreen();
    }

    if (input_worker.joinable()) input_worker.join();
    restoreTerminal();
    std::cout << "\nThanks for playing!\n";
}

void CLI::startScreen() const {
    clearScreen();
    std::cout << "\n\n\n\n\n";
    print_in_the_center( " _____     _        _        _ ");
    print_in_the_center( "/__   \\___| |_ _ __(_)___   / \\");
    print_in_the_center(  "  / /\\/ _ \\ __| '__| / __| /  /");
    print_in_the_center(R"( / / |  __/ |_| |  | \__ \/\_/ )");
    print_in_the_center(R"( \/   \___|\__|_|  |_|___/\/   )");
    std::cout << std::endl;
    print_in_the_center("Controls:");
    std::cout << std::endl;
    print_in_the_center("(A)Left   (D)Right  (H)Hold ");
    print_in_the_center("(W)RotateCW    (Q)RotateCCW");
    print_in_the_center("(Space)HardDrop (S)SoftDrop");
    print_in_the_center("(P)Pause  (R)Resume (X)Quit");
    print_in_the_center("(L)Leaderboard (G)Load game");
    std::cout << std::endl;
    print_in_the_center("PRESS 'Enter' TO START");
    std::cout << "\n\n\n\n\n";
    char c;
    while (true) {
        if (popKey(c)) {
            if (c == '\n') {
                gameEngine.startNewGame();
                playLoop();
                return;
            }
            if (tolower(c) == 'l') {
                leaderboardScreen();
                return;
            }
            if (tolower(c) == 'g') {
                loadGameScreen();
                return;
            }
            if (tolower(c) == 'x') {
                quittingScreen();
                return;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
void CLI::playLoop() const {
    auto nextFrameTime = std::chrono::steady_clock::now();
    while (running.load()) {
        char k = '\0';
        std::unique_lock ul(input_mutex);
        if (input_cv.wait_for(ul, std::chrono::milliseconds(10), []{ return !input_queue.empty(); })) {
            k = input_queue.front();
            input_queue.pop();
        }
        ul.unlock();

        switch (tolower(k)) {
            case 'a': inputHandler.handleKey(KeyType::LEFT); break;
            case 'd': inputHandler.handleKey(KeyType::RIGHT); break;
            case 's': inputHandler.handleKey(KeyType::SOFT_DROP); break;
            case 'w': inputHandler.handleKey(KeyType::ROTATE_CW); break;
            case 'q': inputHandler.handleKey(KeyType::ROTATE_CCW); break;
            case ' ': inputHandler.handleKey(KeyType::HARD_DROP); break;
            case 'h': inputHandler.handleKey(KeyType::HOLD); break;
            case 'p':
                inputHandler.handleKey(KeyType::PAUSE);
                pausedScreen();
                break;
            case 'x':
                inputHandler.handleKey(KeyType::PAUSE);
                quittingScreen();
                break;
            default: break;
        }

        if (gameEngine.needsRender()) {
            std::lock_guard lock(console_mutex);

            switch (gameEngine.getGameState()) {
                case GameState::RUNNING:
                    drawBoard(); break;
                case GameState::GAME_OVER:
                    saveScoreScreen();
                    gameOverScreen();
                    return;
                default: break;
            }
            gameEngine.markRender();
        }
        nextFrameTime += std::chrono::microseconds(1000000 / FPS);
        std::this_thread::sleep_until(nextFrameTime);
    }
}
void CLI::pausedScreen() const {
    bool saved = false;
    while (true) {
        clearScreen();
        std::cout << "\n\n\n\n\n";
        print_in_the_center("▄▖         ▄▖         ▌", 20);
        print_in_the_center("▌ ▀▌▛▛▌█▌  ▙▌▀▌▌▌▛▘█▌▛▌", 20);
        print_in_the_center("▙▌█▌▌▌▌▙▖  ▌ █▌▙▌▄▌▙▖▙▌", 20);
        std::cout << std::endl;
        print_in_the_center("Current Stats:");
        std::cout << std::endl;
        print_in_the_center("Score: " + std::to_string(scoreManager.getScore()));
        print_in_the_center("Level: " + std::to_string(scoreManager.getLevel()));
        print_in_the_center("Lines: " + std::to_string(scoreManager.getTotalLinesCleared()));
        std::cout << "\n\n";
        print_in_the_center("Controls:");
        print_in_the_center("(R)esume   (S)ave game");
        print_in_the_center("(L)eaderboard  (X)quit");
        std::cout << "\n\n";
        if (saved) print_in_the_center("Saved!");

        char c;
        while (true) {
            if (popKey(c)) {
                if (tolower(c) == 'r') {
                    inputHandler.handleKey(KeyType::RESUME);
                    return;
                }
                if (tolower(c) == 's') {
                    inputHandler.handleKey(KeyType::SAVE);
                    saved = true;
                    break;
                }
                if (tolower(c) == 'l') {
                    leaderboardScreen();
                    break;
                }
                if (tolower(c) == 'x') {
                    quittingScreen();
                    break;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}
void CLI::gameOverScreen() const {
    clearScreen();
    std::cout << "\n\n\n\n\n";
    print_in_the_center("▄▖         ▄▖      ▌", 20);
    print_in_the_center("▌ ▀▌▛▛▌█▌  ▌▌▌▌█▌▛▘▌", 20);
    print_in_the_center("▙▌█▌▌▌▌▙▖  ▙▌▚▘▙▖▌ ▖", 20);
    std::cout << std::endl;
    print_in_the_center("Final Score: " + std::to_string(scoreManager.getScore()));
    print_in_the_center("Level: " + std::to_string(scoreManager.getLevel()));
    print_in_the_center("Lines Cleared: " + std::to_string(scoreManager.getTotalLinesCleared()));
    std::cout << std::endl;
    print_in_the_center("Play again?");
    print_in_the_center("(Y)es  (N)o");
    std::cout << "\n\n\n\n\n";
    char c;
    while (true) {
        if (popKey(c)) {
            if (tolower(c) == 'y') {
                return;
            }
            if (tolower(c) == 'n') {
                quittingScreen();
                return;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
void CLI::quittingScreen() const {
    clearScreen();
    std::cout << "\n\n\n\n\n";
    print_in_the_center("▖▖                        ", 26);
    print_in_the_center("▌▌▛▌▌▌  ▛▘▌▌▛▘█▌  ▌▌▛▌▌▌  ", 26);
    print_in_the_center("▐ ▙▌▙▌  ▄▌▙▌▌ ▙▖  ▙▌▙▌▙▌  ", 26);
    print_in_the_center("                  ▄▌      ", 26);
    print_in_the_center("       ▗   ▗         ▘▗ ▄▖", 26);
    print_in_the_center("▌▌▌▀▌▛▌▜▘  ▜▘▛▌  ▛▌▌▌▌▜▘▗▘", 26);
    print_in_the_center("▚▚▘█▌▌▌▐▖  ▐▖▙▌  ▙▌▙▌▌▐▖▗ ", 26);
    print_in_the_center("                  ▌       ", 26);
    std::cout << std::endl;
    print_in_the_center("(Y)es   (N)o");
    std::cout << "\n\n\n\n\n";

    char c;
    while (true) {
        if (popKey(c)) {
            if (tolower(c) == 'y') {
                running.store(false);
                restoreTerminal();
                std::cout << "\nThanks for playing!\n";
                exit(0);
            }
            if (tolower(c) == 'n') {
                inputHandler.handleKey(KeyType::RESUME);
                return;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

}
void CLI::leaderboardScreen() const {
    clearScreen();
    std::cout << "\n\n\n\n\n";
    print_in_the_center("▖ ▄▖▄▖▄ ▄▖▄▖▄ ▄▖▄▖▄▖▄ ", 22);
    print_in_the_center("▌ ▙▖▌▌▌▌▙▖▙▘▙▘▌▌▌▌▙▘▌▌", 22);
    print_in_the_center("▙▖▙▖▛▌▙▘▙▖▌▌▙▘▙▌▛▌▌▌▙▘", 22);

    auto leaderboard = scoreManager.getLeaderboard();
    size_t longest_name = 0, longest_score = 0;
    for (const auto& [name, score] : leaderboard) {
        longest_name = std::max(longest_name, name.length());
        longest_score = std::max(longest_score, std::to_string(score).length());
    }

    size_t width = 5+longest_name+1+longest_score+1;
    print_in_the_center("╔═══╦" + repeat("═", longest_name) + + "╦" + repeat("═", longest_score) + "╗", width);
    for (size_t index = 0; index < leaderboard.size(); index++) {
        std::string name = leaderboard[index].playerName;
        std::string score = std::to_string(leaderboard[index].score);

        std::string line = "║ " + std::to_string(index+1) + " ║" + name + repeat(" ", longest_name - name.length()) + "║" + repeat(" ", longest_score - score.length()) + score + "║";
        print_in_the_center(line, width);
        if (index != leaderboard.size() - 1) print_in_the_center("╠═══╬" + repeat("═", longest_name) + "╬" + repeat("═", longest_score) + "╣", width);
    }
    print_in_the_center("╚═══╩" + repeat("═", longest_name) + "╩" + repeat("═", longest_score) + "╝", width);

    print_in_the_center("Press 'b' to go back.");
    print_in_the_center("Or 'x' to quit.");
    std::cout << "\n\n\n\n\n";

    char c;
    while (true) {
        if (popKey(c)) {
            if (tolower(c) == 'b') {
                return;
            }
            if (tolower(c) == 'x') {
                quittingScreen();
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
void CLI::saveScoreScreen() const {
    if (!scoreManager.isAGoodScore()) return;
    clearScreen();
    std::cout << "\n\n\n";
    if (scoreManager.isANewRecord()) {
        print_in_the_center(" _  _ _____      __");
        print_in_the_center(R"(| \| | __\ \    / /)");
        print_in_the_center(R"(| .` | _| \ \/\/ / )");
        print_in_the_center(R"(|_|\_|___| \_/\_/  )");
        std::cout << std::endl;
        print_in_the_center("___ ___ ___ ___  ___ ___  _ ");
        print_in_the_center(R"(| _ \ __/ __/ _ \| _ \   \| |)");
        print_in_the_center(R"(|   / _| (_| (_) |   / |) |_|)");
        print_in_the_center(R"(|_|_\___\___\___/|_|_\___/(_))");
    }
    else {
        print_in_the_center(" _  _ _____      __");
        print_in_the_center(R"(| \| | __\ \    / /)");
        print_in_the_center(R"(| .` | _| \ \/\/ / )");
        print_in_the_center(R"(|_|\_|___| \_/\_/  )");
        std::cout << std::endl;
        print_in_the_center("_  _ ___ ___ _  _ ");
        print_in_the_center(R"(| || |_ _/ __| || |)");
        print_in_the_center(R"(| __ || | (_ | __ |)");
        print_in_the_center(R"(|_||_|___\___|_||_|)");
        std::cout << std::endl;
        print_in_the_center("___  ___ ___  ___ ___ _ ");
        print_in_the_center(R"(/ __|/ __/ _ \| _ \ __| |)");
        print_in_the_center(R"(\__ \ (_| (_) |   / _||_|)");
        print_in_the_center(R"(|___/\___\___/|_|_\___(_))");
    }
    std::cout << "\n  Enter your name: " << std::flush;;
    std::string name;
    char c;
    while (name.length() < 15) {
        if (popKey(c)) {
            if (c == '\n') break;
            if (c == 127 || c == 8) {
                if (!name.empty()) {
                    name.pop_back();
                    std::cout << "\b \b" << std::flush;
                }
            }
            else {
                name += c;
                std::cout << c << std::flush;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << std::flush;

    scoreManager.saveScore(name);
    leaderboardScreen();
}
void CLI::loadGameScreen() const {
    clearScreen();

    inputHandler.handleKey(KeyType::LOAD);
    if (gameEngine.getGameState() == GameState::LOADED) {
        std::cout << "\n\n\n\n\n";
        print_in_the_center("Loaded Successfully");
        print_in_the_center("PRESS 'Enter' TO START");
        print_in_the_center("OR 'X' to quit.");
        std::cout << "\n\n\n\n\n";
        char c;
        while (true) {
            if (popKey(c)) {
                if (c == '\n') {
                    gameEngine.startGame();
                    playLoop();
                    return;
                }
                if (tolower(c) == 'x') {
                    quittingScreen();
                    return;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    std::cout << "\n\n\n\n\n";
    print_in_the_center("INCORRECT FILE");
    print_in_the_center("(B)Back (X)Quit");
    std::cout << "\n\n\n\n\n";
    char c;
    while (true) {
        if (popKey(c)) {
            if (c == 'b') return;
            if (tolower(c) == 'x') {
                quittingScreen();
                return;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}



std::string CLI::getCellChar(Cell cell) {
    const auto full_cell = "██";
    const auto ghost_cell = "░░";

    switch (cell) {
        case Cell::I: return std::string(CYAN) + full_cell + RESET;
        case Cell::GhostI: return std::string(CYAN) + ghost_cell + RESET;
        case Cell::O: return std::string(YELLOW) + full_cell + RESET;
        case Cell::GhostO: return std::string(YELLOW) + ghost_cell + RESET;
        case Cell::T: return std::string(MAGENTA) + full_cell + RESET;
        case Cell::GhostT: return std::string(MAGENTA) + ghost_cell + RESET;
        case Cell::L: return std::string(ORANGE) + full_cell + RESET;
        case Cell::GhostL: return std::string(ORANGE) + ghost_cell + RESET;
        case Cell::J: return std::string(BLUE) + full_cell + RESET;
        case Cell::GhostJ: return std::string(BLUE) + ghost_cell + RESET;
        case Cell::S: return std::string(GREEN) + full_cell + RESET;
        case Cell::GhostS: return std::string(GREEN) + ghost_cell + RESET;
        case Cell::Z: return std::string(RED) + full_cell + RESET;
        case Cell::GhostZ: return std::string(RED) + ghost_cell + RESET;
        case Cell::Empty:
        default:
            return "  ";
    }
}
void CLI::drawBoard() const {
    clearScreen();

    auto grid = gameEngine.getRenderGrid();
    auto hold_grid = gameEngine.getRenderHold();
    auto next_grid = gameEngine.getRenderNext();
    auto score = scoreManager.getScore();
    auto level = scoreManager.getLevel();
    auto totalLinesCleared = scoreManager.getTotalLinesCleared();

    int termWidth = getTerminalWidth();
    int boardDisplayWidth = BOARD_WIDTH * 2 + 2;
    int leftPadding = (termWidth - boardDisplayWidth) / 2;
    std::string pad(leftPadding > 0 ? leftPadding : 0, ' ');
    int holdWidth = 10;
    int nextWidth = 10;

    int holdLPadding = leftPadding - holdWidth - 1;
    std::string holdPad(holdLPadding > 0 ? holdLPadding : 0, ' ');

    std::cout << pad << "╔" << repeat("═", BOARD_WIDTH * 2) << "╗\n";

    auto score_line = std::to_string(score);
    auto c = boardDisplayWidth - score_line.length() - 10;
    std::cout << pad << "║ Score:"  << std::string(c > 0 ? c: 0, ' ') << score_line << " ║\n";

    std::cout << holdPad << "╔" << repeat("═", holdWidth)  << "╬" << repeat("═", BOARD_WIDTH * 2)  << "╬" << repeat("═", nextWidth)  << "╗\n";

    for (int LINE_INDEX = 0; LINE_INDEX < BOARD_HEIGHT; LINE_INDEX++) {
        int y = BOARD_HEIGHT - LINE_INDEX - 1;

        switch (LINE_INDEX) {
            case 0:
                std::cout << holdPad << "║ Hold:    ║";
                for (int x = 0; x < BOARD_WIDTH; ++x) std::cout << getCellChar(grid[y][x]);
                std::cout << "║ Next:    ║\n";
                break;
            case 1:
                std::cout << holdPad << "╠" << repeat("═", holdWidth) << "╣";
                for (int x = 0; x < BOARD_WIDTH; ++x) std::cout << getCellChar(grid[y][x]);
                std::cout << "╠" << repeat("═", nextWidth) << "╣\n";
                break;
            case 2:
            case 5:
                std::cout << holdPad << "║" << std::string(holdWidth, ' ') << "║";
                for (int x = 0; x < BOARD_WIDTH; ++x) std::cout << getCellChar(grid[y][x]);
                std::cout << "║" << std::string(nextWidth, ' ') << "║\n";
                break;
            case 3:
            case 4: {
                int i = 4 - LINE_INDEX;
                std::cout << holdPad << "║ ";
                for (int x = 0; x < 4; ++x) std::cout << getCellChar(hold_grid[i][x]);
                std::cout << " ║";
                for (int x = 0; x < BOARD_WIDTH; ++x) std::cout << getCellChar(grid[y][x]);
                std::cout << "║ ";
                for (int x = 0; x < 4; ++x) std::cout << getCellChar(next_grid[0][i][x]);
                std::cout << " ║\n";
                break;
            }
            case 6:
                std::cout << holdPad << "╠" << repeat("═", holdWidth) << "╣";
                for (int x = 0; x < BOARD_WIDTH; ++x) std::cout << getCellChar(grid[y][x]);
                std::cout << "╠" << repeat("═", nextWidth) << "╣\n";
                break;
            case 7:
                std::cout << holdPad << "║ Level:   ║";
                for (int x = 0; x < BOARD_WIDTH; ++x) std::cout << getCellChar(grid[y][x]);
                std::cout << "║" << repeat(" ", nextWidth) << "║\n";
                break;
            case 8: {
                auto level_line = std::to_string(level);
                std::cout << holdPad << "║" << std::string(9 - level_line.length(), ' ') << level_line << " ║";
                for (int x = 0; x < BOARD_WIDTH; ++x) std::cout << getCellChar(grid[y][x]);
                std::cout << "║ ";
                int i = 9 - LINE_INDEX;
                for (int x = 0; x < 4; ++x) std::cout << getCellChar(next_grid[1][i][x]);
                std::cout << " ║\n";
                break;
            }
            case 9: {
                std::cout << holdPad << "╠" << repeat("═", holdWidth) << "╣";
                for (int x = 0; x < BOARD_WIDTH; ++x) std::cout << getCellChar(grid[y][x]);
                std::cout << "║ ";
                int i = 9 - LINE_INDEX;
                for (int x = 0; x < 4; ++x) std::cout << getCellChar(next_grid[1][i][x]);
                std::cout << " ║\n";
                break;
            }
            case 10:
                std::cout << holdPad << "║ Lines:   ║";
                for (int x = 0; x < BOARD_WIDTH; ++x) std::cout << getCellChar(grid[y][x]);
                std::cout << "║" << repeat(" ", nextWidth) << "║\n";
                break;
            case 11: {
                auto lines_line = std::to_string(totalLinesCleared);
                std::cout << holdPad << "║" << std::string(9 - lines_line.length(), ' ') << lines_line << " ║";
                for (int x = 0; x < BOARD_WIDTH; ++x) std::cout << getCellChar(grid[y][x]);
                std::cout << "╠" << repeat("═", nextWidth) << "╣\n";
                break;
            }
            case 12:
                std::cout << holdPad << "╚═" << repeat("═", holdWidth - 2) << "═╣";
                for (int x = 0; x < BOARD_WIDTH; ++x) std::cout << getCellChar(grid[y][x]);
                std::cout << "║" << repeat(" ", nextWidth) << "║\n";
                break;
            case 13:
            case 14: {
                std::cout << pad << "║";
                for (int x = 0; x < BOARD_WIDTH; ++x) std::cout << getCellChar(grid[y][x]);
                std::cout << "║ ";
                int i = 14 - LINE_INDEX;
                for (int x = 0; x < 4; ++x) std::cout << getCellChar(next_grid[2][i][x]);
                std::cout << " ║\n";
                break;
            }
            case 15:
                std::cout << pad << "║";
                for (int x = 0; x < BOARD_WIDTH; ++x) std::cout << getCellChar(grid[y][x]);
                std::cout << "║" << repeat(" ", nextWidth) << "║\n";
                break;
            case 16:
                std::cout << pad << "║";
                for (int x = 0; x < BOARD_WIDTH; ++x) std::cout << getCellChar(grid[y][x]);
                std::cout << "║" << repeat("═", nextWidth) << "╝\n";
                break;
            default:
                std::cout << pad << "║";
                for (int x = 0; x < BOARD_WIDTH; ++x) std::cout << getCellChar(grid[y][x]);
                std::cout << "║\n";
                break;
        }
    }
    std::cout << pad << "╚" << repeat("═", BOARD_WIDTH * 2) << "╝\n";
    std::cout << std::endl;

    print_in_the_center("Controls:");
    print_in_the_center("(a)Left (d)Right (h)Hold (w)RotateCW (q)RotateCCW");
    print_in_the_center("(Space)HardDrop (s)SoftDrop (p)Pause (r)Resume (x)Quit");
}
