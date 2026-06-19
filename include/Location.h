#pragma once
#include "Common.h"
#include "Chest.h"
#include "NPC.h"
#include "Puzzle.h"

struct DoorInfo {
    int row, col;
    int targetLocationId;
};

struct ObjectPos {
    int row, col;
    int id;
};

class Location {
public:
    Location(int id,
             const std::string& name,
             const std::string& description,
             std::vector<std::string> asciiMap,
             LocationState state,
             std::shared_ptr<Puzzle> puzzle,
             int puzzleDamage,
             std::vector<DoorInfo> doors,
             bool isFinal = false,
             // Key-locked: if keyId != -1, location opens only with that key item
             int keyItemId = -1,
             // Hint shown to player when they approach the locked door
             const std::string& hint = "");

    int getId() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    LocationState getState() const;
    bool isOpen() const;
    bool isFinal() const;
    const std::vector<DoorInfo>& getDoors() const;

    void addChest(std::shared_ptr<Chest> chest, int row, int col);
    void addNPC(std::shared_ptr<NPC> npc, int row, int col);
    std::vector<std::shared_ptr<Chest>>& getChests();
    std::vector<std::shared_ptr<NPC>>&   getNPCs();

    bool tryUnlock(const std::string& answer);
    void forceOpen();

    int getPuzzleDamage() const;
    Puzzle* getPuzzle() const;
    int getKeyItemId() const;           // -1 = no key required
    const std::string& getHint() const; // hint for player

    void getSpawn(int& row, int& col);
    bool tryMove(int pr, int pc, int dr, int dc, int& newRow, int& newCol) const;
    char cellAt(int r, int c) const;
    void setCell(int r, int c, char ch);

    CellType checkAdjacent(int pr, int pc, int dr, int dc, int& outId) const;
    const DoorInfo* getDoorAt(int r, int c) const;
    Chest* getChestAt(int r, int c) const;
    NPC*   getNPCAt(int r, int c) const;

    void displayMap(int playerRow, int playerCol) const;
    void displayInfo() const;

    std::vector<int> getConnectedIds() const;

private:
    int id_;
    std::string name_;
    std::string description_;
    std::vector<std::string> asciiMap_;
    LocationState state_;
    std::shared_ptr<Puzzle> puzzle_;
    int puzzleDamage_;
    std::vector<DoorInfo> doors_;
    bool isFinal_;
    int keyItemId_;
    std::string hint_;
    std::vector<std::shared_ptr<Chest>> chests_;
    std::vector<std::shared_ptr<NPC>>   npcs_;
    std::vector<ObjectPos> chestPositions_;
    std::vector<ObjectPos> npcPositions_;
};
