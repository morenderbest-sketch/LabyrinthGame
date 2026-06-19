#include "Weapon.h"

Weapon::Weapon(int id, const std::string& name, int bonusDamage)
    : Item(id, name, ItemType::WEAPON), bonusDamage_(bonusDamage) {}

int Weapon::getBonusDamage() const { return bonusDamage_; }

std::string Weapon::describe() const {
    return "[Оружие] " + name_ + " — урон +" + std::to_string(bonusDamage_);
}
