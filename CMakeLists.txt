cmake_minimum_required(VERSION 3.10)
project(TetrisEngine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Threads REQUIRED)

include_directories(
        .
        GameEngine
        CLI
)

add_executable(tetris
        main.cpp
        CLI/CLI.cpp
        GameEngine/GameEngine.cpp
        GameEngine/Timer.cpp
        GameEngine/InputHandler.cpp
        GameEngine/ScoreManagement/ScoreManager.cpp
        GameEngine/ScoreManagement/Leaderboard.cpp
        GameEngine/Commands/Command.cpp
        GameEngine/Commands/MoveRightCommand.cpp
        GameEngine/Commands/PauseCommand.cpp
        GameEngine/Commands/ResumeCommand.cpp
        GameEngine/Commands/RotateCCWCommand.cpp
        GameEngine/Commands/RotateCWCommand.cpp
        GameEngine/Commands/SoftDropCommand.cpp
        GameEngine/Commands/HardDropCommand.cpp
        GameEngine/Commands/HoldCommand.cpp
        GameEngine/Commands/MoveLeftCommand.cpp
        GameEngine/Board/Board.cpp
        GameEngine/BlockFactory/BagGenerator.cpp
        GameEngine/BlockFactory/BlockFactory.cpp
        GameEngine/Blocks/Block.cpp
        GameEngine/Blocks/IBlock.cpp
        GameEngine/Blocks/JBlock.cpp
        GameEngine/Blocks/LBlock.cpp
        GameEngine/Blocks/OBlock.cpp
        GameEngine/Blocks/SBlock.cpp
        GameEngine/Blocks/TBlock.cpp
        GameEngine/Blocks/ZBlock.cpp
)

target_link_libraries(tetris PRIVATE Threads::Threads)
