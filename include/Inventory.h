#pragma once
#include "Common.h"
#include "Potion.h"
#include "Weapon.h"
#include "Key.h"

class Inventory {
public:
    Inventory();

    void addItem(std::shared_ptr<Item> item);
    void removeItem(int id);

    std::vector<std::shared_ptr<Potion>> getPotions() const;
    std::vector<std::shared_ptr<Weapon>> getWeapons() const;
    std::vector<std::shared_ptr<Key>>    getKeys()    const;

    // Find key for a specific location; nullptr if not found
    std::shared_ptr<Key> findKeyFor(int locationId) const;
    // Consume the key (remove from inventory after use)
    void consumeKey(int locationId);

    std::shared_ptr<Weapon> getEquipped() const;
    bool equip(int weaponId);
    void unequip();

    int usePotion(int index); // returns heal amount or -1

    void display() const;

    std::string serialize() const;
    void deserialize(const std::string& data);

private:
    std::vector<std::shared_ptr<Item>> items_;
    int equippedWeaponId_;
};
