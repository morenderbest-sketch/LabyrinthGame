#include "Chest.h"

Chest::Chest(int id, std::vector<std::shared_ptr<Item>> items,
             std::shared_ptr<Puzzle> puzzle)
    : id_(id), opened_(false), items_(std::move(items)), puzzle_(std::move(puzzle)) {}

int Chest::getId() const { return id_; }
bool Chest::isLocked() const { return puzzle_ != nullptr && !opened_; }
bool Chest::isOpened() const { return opened_; }

std::vector<std::shared_ptr<Item>> Chest::tryOpen(const std::string& answer) {
    if (opened_) {
        std::cout << "Сундук уже открыт и пуст.\n";
        return {};
    }
    if (puzzle_) {
        puzzle_->display();
        if (!answer.empty()) {
            if (puzzle_->checkAnswer(answer)) {
                opened_ = true;
                std::cout << "Замок щёлкнул! Сундук открыт!\n";
                return items_;
            } else {
                std::cout << "Неверно! Сундук больно ударяет вас по руке!\n";
                return {}; // caller handles damage
            }
        }
        return {}; // answer not provided yet
    }
    return open();
}

std::vector<std::shared_ptr<Item>> Chest::open() {
    if (opened_) return {};
    opened_ = true;
    std::cout << "Сундук открыт!\n";
    return items_;
}

Puzzle* Chest::getPuzzle() const { return puzzle_.get(); }

void Chest::display() const {
    if (opened_) std::cout << "  [Сундук #" << id_ << " — пуст]\n";
    else if (puzzle_) std::cout << "  [Сундук #" << id_ << " — закрыт на замок]\n";
    else std::cout << "  [Сундук #" << id_ << " — открыт, внутри предметы]\n";
}
