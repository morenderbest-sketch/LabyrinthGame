#include "Potion.h"

Potion::Potion(int id, const std::string& name, int healAmount)
    : Item(id, name, ItemType::POTION), healAmount_(healAmount) {}

int Potion::getHealAmount() const { return healAmount_; }

std::string Potion::describe() const {
    return "[Зелье] " + name_ + " — восстанавливает " + std::to_string(healAmount_) + " HP";
}
