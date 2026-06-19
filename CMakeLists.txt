cmake_minimum_required(VERSION 3.14)
project(Labyrinth CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/Logger.cpp
    src/Item.cpp
    src/Potion.cpp
    src/Weapon.cpp
    src/Puzzle.cpp
    src/Chest.cpp
    src/NPC.cpp
    src/Inventory.cpp
    src/Location.cpp
    src/Player.cpp
    src/CombatSystem.cpp
    src/SaveSystem.cpp
    src/GameWorld.cpp
    src/Game.cpp
)

add_executable(labyrinth ${SOURCES})

if(MSVC)
    target_compile_options(labyrinth PRIVATE /W4 /utf-8)
else()
    target_compile_options(labyrinth PRIVATE -Wall -Wextra -O2)
endif()
