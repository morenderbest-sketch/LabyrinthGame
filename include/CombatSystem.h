#pragma once
#include "Common.h"
#include "Player.h"
#include "NPC.h"

class CombatSystem {
public:
    // Returns true if player wins, false if player dies or flees
    static bool fight(Player& player, NPC& npc);

private:
    static void displayCombatStatus(const Player& player, const NPC& npc);
    static bool askFlee();
    static bool askUsePotion(Player& player);
};
