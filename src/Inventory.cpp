#include "Inventory.h"

Inventory::Inventory() : equippedWeaponId_(-1) {}

void Inventory::addItem(std::shared_ptr<Item> item) {
    items_.push_back(std::move(item));
}

void Inventory::removeItem(int id) {
    items_.erase(std::remove_if(items_.begin(), items_.end(),
        [id](const std::shared_ptr<Item>& i){ return i->getId() == id; }),
        items_.end());
    if (equippedWeaponId_ == id) equippedWeaponId_ = -1;
}

std::vector<std::shared_ptr<Potion>> Inventory::getPotions() const {
    std::vector<std::shared_ptr<Potion>> r;
    for (auto& i : items_)
        if (i->getType() == ItemType::POTION)
            r.push_back(std::dynamic_pointer_cast<Potion>(i));
    return r;
}

std::vector<std::shared_ptr<Weapon>> Inventory::getWeapons() const {
    std::vector<std::shared_ptr<Weapon>> r;
    for (auto& i : items_)
        if (i->getType() == ItemType::WEAPON)
            r.push_back(std::dynamic_pointer_cast<Weapon>(i));
    return r;
}

std::vector<std::shared_ptr<Key>> Inventory::getKeys() const {
    std::vector<std::shared_ptr<Key>> r;
    for (auto& i : items_)
        if (i->getType() == ItemType::KEY)
            r.push_back(std::dynamic_pointer_cast<Key>(i));
    return r;
}

std::shared_ptr<Key> Inventory::findKeyFor(int locationId) const {
    for (auto& i : items_)
        if (i->getType() == ItemType::KEY) {
            auto k = std::dynamic_pointer_cast<Key>(i);
            if (k && k->getTargetLocationId() == locationId) return k;
        }
    return nullptr;
}

void Inventory::consumeKey(int locationId) {
    for (auto it = items_.begin(); it != items_.end(); ++it) {
        if ((*it)->getType() == ItemType::KEY) {
            auto k = std::dynamic_pointer_cast<Key>(*it);
            if (k && k->getTargetLocationId() == locationId) {
                items_.erase(it);
                return;
            }
        }
    }
}

std::shared_ptr<Weapon> Inventory::getEquipped() const {
    if (equippedWeaponId_ == -1) return nullptr;
    for (auto& i : items_)
        if (i->getId() == equippedWeaponId_ && i->getType() == ItemType::WEAPON)
            return std::dynamic_pointer_cast<Weapon>(i);
    return nullptr;
}

bool Inventory::equip(int weaponId) {
    for (auto& i : items_)
        if (i->getId() == weaponId && i->getType() == ItemType::WEAPON) {
            equippedWeaponId_ = weaponId; return true;
        }
    return false;
}

void Inventory::unequip() { equippedWeaponId_ = -1; }

int Inventory::usePotion(int index) {
    auto potions = getPotions();
    if (index < 0 || index >= (int)potions.size()) return -1;
    int heal = potions[index]->getHealAmount();
    removeItem(potions[index]->getId());
    return heal;
}

void Inventory::display() const {
    std::cout << "\n=== Инвентарь ===\n";
    auto potions = getPotions();
    auto weapons = getWeapons();
    auto keys    = getKeys();
    if (potions.empty() && weapons.empty() && keys.empty()) {
        std::cout << "  (пусто)\n=================\n"; return;
    }
    if (!potions.empty()) {
        std::cout << "Зелья:\n";
        for (size_t i = 0; i < potions.size(); ++i)
            std::cout << "  " << (i+1) << ") " << potions[i]->describe() << "\n";
    }
    if (!weapons.empty()) {
        std::cout << "Оружие:\n";
        for (auto& w : weapons) {
            std::cout << "  [" << w->getId() << "] " << w->describe();
            if (w->getId() == equippedWeaponId_) std::cout << " <ЭКИПИРОВАНО>";
            std::cout << "\n";
        }
    }
    if (!keys.empty()) {
        std::cout << "Ключи:\n";
        for (auto& k : keys)
            std::cout << "  " << k->describe() << "\n";
    }
    std::cout << "=================\n";
}

std::string Inventory::serialize() const {
    std::ostringstream oss;
    oss << equippedWeaponId_;
    for (auto& i : items_) {
        oss << "," << i->getId() << ":" << (int)i->getType();
        if (i->getType() == ItemType::POTION)
            oss << ":" << std::dynamic_pointer_cast<Potion>(i)->getHealAmount()
                << ":" << i->getName();
        else if (i->getType() == ItemType::WEAPON)
            oss << ":" << std::dynamic_pointer_cast<Weapon>(i)->getBonusDamage()
                << ":" << i->getName();
        else if (i->getType() == ItemType::KEY)
            oss << ":" << std::dynamic_pointer_cast<Key>(i)->getTargetLocationId()
                << ":" << i->getName();
    }
    return oss.str();
}

void Inventory::deserialize(const std::string& data) {
    items_.clear(); equippedWeaponId_ = -1;
    if (data.empty()) return;
    std::istringstream iss(data);
    std::string token;
    bool first = true;
    while (std::getline(iss, token, ',')) {
        if (first) { equippedWeaponId_ = std::stoi(token); first = false; continue; }
        std::istringstream ts(token);
        std::string part;
        std::vector<std::string> parts;
        while (std::getline(ts, part, ':')) parts.push_back(part);
        if (parts.size() < 4) continue;
        int id   = std::stoi(parts[0]);
        int type = std::stoi(parts[1]);
        int val  = std::stoi(parts[2]);
        std::string name = parts[3];
        if (type == (int)ItemType::POTION)
            items_.push_back(std::make_shared<Potion>(id, name, val));
        else if (type == (int)ItemType::WEAPON)
            items_.push_back(std::make_shared<Weapon>(id, name, val));
        else if (type == (int)ItemType::KEY)
            items_.push_back(std::make_shared<Key>(id, name, val));
    }
}
