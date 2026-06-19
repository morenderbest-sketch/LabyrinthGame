#pragma once
#include "Common.h"
#include "Player.h"

struct GameState {
    std::map<int, bool> locationOpen;
    std::map<int, bool> chestOpened;
    struct NpcSave { bool alive; int hp; bool hostile; };
    std::map<int, NpcSave> npcStates;
    int playerRow = -1;  // saved position on map
    int playerCol = -1;
};

class SaveSystem {
public:
    static bool save(const Player& player, const GameState& state,
                     const std::string& filename = "savegame.txt");
    static bool load(Player& player, GameState& state,
                     const std::string& filename = "savegame.txt");
    static bool hasSave(const std::string& filename = "savegame.txt");
};
