#include "Player.h"

Player::Player(const std::string& name)
    : name_(name), hp_(100), maxHp_(100), baseDamage_(1), locationId_(1) {}

const std::string& Player::getName() const { return name_; }
int Player::getHp() const { return hp_; }
int Player::getMaxHp() const { return maxHp_; }
int Player::getBaseDamage() const { return baseDamage_; }
int Player::getCurrentLocationId() const { return locationId_; }
Inventory& Player::getInventory() { return inventory_; }
bool Player::isAlive() const { return hp_ > 0; }

void Player::setLocationId(int id) { locationId_ = id; }

void Player::takeDamage(int amount) {
    hp_ -= amount;
    if (hp_ < 0) hp_ = 0;
}

void Player::heal(int amount) {
    hp_ += amount;
    if (hp_ > maxHp_) hp_ = maxHp_;
}

void Player::equipWeapon(int weaponId) {
    if (inventory_.equip(weaponId))
        std::cout << "Оружие экипировано!\n";
    else
        std::cout << "Оружие не найдено в инвентаре.\n";
}

void Player::unequipWeapon() {
    inventory_.unequip();
    std::cout << "Оружие снято.\n";
}

int Player::usePotionFromInventory(int index) {
    int heal = inventory_.usePotion(index);
    if (heal > 0) {
        this->heal(heal);
        std::cout << "Вы выпили зелье и восстановили " << heal << " HP!\n";
    }
    return heal;
}

int Player::getTotalDamage() const {
    int bonus = 0;
    auto w = inventory_.getEquipped();
    if (w) bonus = w->getBonusDamage();
    return baseDamage_ + bonus;
}

void Player::displayStatus() const {
    std::cout << "\n╔══════════ СТАТУС ══════════╗\n";
    std::cout << "║ Игрок: " << name_;
    int pad = 19 - (int)name_.size();
    for (int i = 0; i < pad; ++i) std::cout << ' ';
    std::cout << "║\n";
    std::cout << "║ HP:    " << hp_ << "/" << maxHp_;
    int hpad = 17 - (int)std::to_string(hp_).size() - (int)std::to_string(maxHp_).size();
    for (int i = 0; i < hpad; ++i) std::cout << ' ';
    std::cout << "║\n";
    std::cout << "║ Урон:  " << getTotalDamage();
    int dpad = 19 - (int)std::to_string(getTotalDamage()).size();
    for (int i = 0; i < dpad; ++i) std::cout << ' ';
    std::cout << "║\n";
    auto w = inventory_.getEquipped();
    if (w) {
        std::cout << "║ Оружие: " << w->getName();
        int wpad = 18 - (int)w->getName().size();
        for (int i = 0; i < wpad; ++i) std::cout << ' ';
        std::cout << "║\n";
    }
    std::cout << "╚═════════════════════════════╝\n";
}

std::string Player::serialize() const {
    std::ostringstream oss;
    oss << name_ << "|" << hp_ << "|" << maxHp_ << "|" << baseDamage_
        << "|" << locationId_ << "|" << inventory_.serialize();
    return oss.str();
}

void Player::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string part;
    std::vector<std::string> parts;
    while (std::getline(iss, part, '|')) parts.push_back(part);
    if (parts.size() < 6) return;
    name_       = parts[0];
    hp_         = std::stoi(parts[1]);
    maxHp_      = std::stoi(parts[2]);
    baseDamage_ = std::stoi(parts[3]);
    locationId_ = std::stoi(parts[4]);
    inventory_.deserialize(parts[5]);
}
