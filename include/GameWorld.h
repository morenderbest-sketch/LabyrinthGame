#pragma once
#include "Common.h"
#include "Location.h"
#include "Potion.h"
#include "Weapon.h"
#include "Key.h"

class GameWorld {
public:
    GameWorld();

    Location* getLocation(int id);
    const std::vector<std::shared_ptr<Location>>& getLocations() const;
    Chest* getChest(int id);
    NPC* getNPC(int id);

private:
    std::vector<std::shared_ptr<Location>> locations_;

    void buildWorld();

    std::shared_ptr<Potion>  makePotion(int id, const std::string& name, int heal);
    std::shared_ptr<Weapon>  makeWeapon(int id, const std::string& name, int dmg);
    std::shared_ptr<Key>     makeKey(int id, const std::string& name, int targetLoc);
    std::shared_ptr<Puzzle>  makePuzzle(PuzzleType t, const std::string& desc,
                                        const std::string& answer, int dmg,
                                        std::vector<PuzzleOption> opts = {});
    std::shared_ptr<NPC> makeNPC_OldMan();
    std::shared_ptr<NPC> makeNPC_Guard();
    std::shared_ptr<NPC> makeNPC_Librarian();
    std::shared_ptr<NPC> makeNPC_Ghost();
};
