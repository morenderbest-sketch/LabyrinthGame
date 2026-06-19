#pragma once
#include "Common.h"
#include "Item.h"

// Outcome of a dialog leaf
struct DialogOutcome {
    std::string message;
    int giveItemId  = -1;       // -1 = none
    int openLocationId = -1;    // -1 = none
    bool makeHostile  = false;
    int healPlayer    = 0;
};

struct DialogNode {
    int id;
    std::string text;                               // NPC line or prompt
    std::vector<std::pair<std::string, int>> choices; // {choice text, next node id}
    bool isLeaf = false;
    DialogOutcome outcome;
};

class NPC {
public:
    NPC(int id, const std::string& name, NPCState state,
        int hp, int damage,
        std::vector<DialogNode> dialog,
        std::vector<std::shared_ptr<Item>> giveItems = {});

    int getId() const;
    const std::string& getName() const;
    NPCState getState() const;
    int getHp() const;
    int getDamage() const;
    bool isAlive() const;

    void setHostile();
    int takeDamage(int amount);  // returns remaining hp

    // Returns outcome (item ids to give, location to open, etc.)
    DialogOutcome runDialog();

    // Give item by id (for outcome processing)
    std::shared_ptr<Item> takeItem(int itemId);
    // Returns all remaining items as loot on death
    std::vector<std::shared_ptr<Item>> takeLoot();

    void displayStatus() const;

    // Serialization helpers
    void setHp(int hp);
    void setState(NPCState s);
    void setAlive(bool alive);

private:
    int id_;
    std::string name_;
    NPCState state_;
    int hp_;
    int maxHp_;
    int damage_;
    bool alive_;
    std::vector<DialogNode> dialog_;
    std::vector<std::shared_ptr<Item>> giveItems_;

    const DialogNode* findNode(int id) const;
};
