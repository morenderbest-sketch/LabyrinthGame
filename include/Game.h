#pragma once
#include "Common.h"
#include "Player.h"
#include "GameWorld.h"
#include "CombatSystem.h"
#include "SaveSystem.h"
#include "Logger.h"

class Game {
public:
    Game();
    void run();

private:
    Player player_;
    GameWorld world_;
    GameState state_;
    bool running_;
    bool won_;
    bool locationDisplayedOnce_;
    std::map<int, std::pair<int,int>> playerPos_;

    void showTitle();
    void initOrLoad();
    void mainLoop();
    void handleMovement();

    std::pair<int,int>& currentPos();

    void doMove(Location* loc, int& row, int& col, int dr, int dc);
    void doInteract(Location* loc, int row, int col);
    void interactWith(Location* loc, int row, int col,
                      int dr, int dc, CellType type, int id);

    void handleChestInteraction(Chest* chest);
    void handleNPCInteraction(NPC* npc);
    void handleDoorInteraction(int targetLocId);
    void enterLocation(Location* target);
    void tryOpenLocation(Location* target);

    void actionInventory();
    void actionSave();

    void collectItems(const std::vector<std::shared_ptr<Item>>& items);
    void applyDamage(int amount, const std::string& reason);

    void syncStateToWorld();
    void syncWorldToState();

    std::string readLine();
    int readInt(int min, int max);
};
