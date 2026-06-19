#pragma once
#include "Common.h"
#include "Inventory.h"
#include "Location.h"

class Player {
public:
    Player(const std::string& name);

    const std::string& getName() const;
    int getHp() const;
    int getMaxHp() const;
    int getBaseDamage() const;
    int getTotalDamage() const;
    int getCurrentLocationId() const;
    Inventory& getInventory();
    bool isAlive() const;

    void setLocationId(int id);
    void takeDamage(int amount);
    void heal(int amount);
    void equipWeapon(int weaponId);
    void unequipWeapon();
    int usePotionFromInventory(int index);

    void displayStatus() const;

    std::string serialize() const;
    void deserialize(const std::string& data);

private:
    std::string name_;
    int hp_;
    int maxHp_;
    int baseDamage_;
    int locationId_;
    Inventory inventory_;
};
