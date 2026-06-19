CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude
TARGET   = labyrinth

SRCS = src/main.cpp \
       src/Logger.cpp \
       src/Item.cpp \
       src/Potion.cpp \
       src/Weapon.cpp \
       src/Key.cpp \
       src/Puzzle.cpp \
       src/Chest.cpp \
       src/NPC.cpp \
       src/Inventory.cpp \
       src/Location.cpp \
       src/Player.cpp \
       src/CombatSystem.cpp \
       src/SaveSystem.cpp \
       src/GameWorld.cpp \
       src/Game.cpp

OBJS = $(SRCS:src/%.cpp=build/%.o)

all: build_dir $(TARGET)

build_dir:
	mkdir -p build

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build $(TARGET) savegame.txt game_log.txt

.PHONY: all clean build_dir
