#include "NPC.h"

NPC::NPC(int id, const std::string& name, NPCState state,
         int hp, int damage,
         std::vector<DialogNode> dialog,
         std::vector<std::shared_ptr<Item>> giveItems)
    : id_(id), name_(name), state_(state),
      hp_(hp), maxHp_(hp), damage_(damage), alive_(true),
      dialog_(std::move(dialog)), giveItems_(std::move(giveItems)) {}

int NPC::getId() const { return id_; }
const std::string& NPC::getName() const { return name_; }
NPCState NPC::getState() const { return state_; }
int NPC::getHp() const { return hp_; }
int NPC::getDamage() const { return damage_; }
bool NPC::isAlive() const { return alive_; }

void NPC::setHostile() { state_ = NPCState::HOSTILE; }
void NPC::setHp(int hp) { hp_ = hp; }
void NPC::setState(NPCState s) { state_ = s; }
void NPC::setAlive(bool alive) { alive_ = alive; }

int NPC::takeDamage(int amount) {
    hp_ -= amount;
    if (hp_ <= 0) { hp_ = 0; alive_ = false; }
    return hp_;
}

std::shared_ptr<Item> NPC::takeItem(int itemId) {
    for (auto it = giveItems_.begin(); it != giveItems_.end(); ++it) {
        if ((*it)->getId() == itemId) {
            auto item = *it;
            giveItems_.erase(it);
            return item;
        }
    }
    return nullptr;
}

const DialogNode* NPC::findNode(int id) const {
    for (const auto& node : dialog_)
        if (node.id == id) return &node;
    return nullptr;
}

DialogOutcome NPC::runDialog() {
    if (dialog_.empty()) {
        std::cout << name_ << ": ...\n";
        return {};
    }
    int currentId = 0;
    while (true) {
        const DialogNode* node = findNode(currentId);
        if (!node) break;

        std::cout << "\n" << name_ << ": \"" << node->text << "\"\n";

        if (node->isLeaf) {
            if (!node->outcome.message.empty())
                std::cout << "[" << node->outcome.message << "]\n";
            return node->outcome;
        }

        if (node->choices.empty()) break;

        for (size_t i = 0; i < node->choices.size(); ++i)
            std::cout << "  " << (i+1) << ") " << node->choices[i].first << "\n";

        std::cout << "Ваш выбор: ";
        int choice;
        while (!(std::cin >> choice) || choice < 1 || choice > (int)node->choices.size()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Введите число от 1 до " << node->choices.size() << ": ";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        currentId = node->choices[choice-1].second;
    }
    return {};
}

void NPC::displayStatus() const {
    std::string stateStr = (state_ == NPCState::FRIENDLY) ? "дружелюбен" : "враждебен";
    std::cout << name_ << " [" << stateStr << "] HP: " << hp_ << "/" << maxHp_ << "\n";
}

std::vector<std::shared_ptr<Item>> NPC::takeLoot() {
    std::vector<std::shared_ptr<Item>> loot;
    loot.swap(giveItems_);
    return loot;
}
