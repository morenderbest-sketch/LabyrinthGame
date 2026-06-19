#pragma once
#include "Common.h"
#include "Item.h"
#include "Puzzle.h"

class Chest {
public:
    Chest(int id, std::vector<std::shared_ptr<Item>> items,
          std::shared_ptr<Puzzle> puzzle = nullptr);

    int getId() const;
    bool isLocked() const;
    bool isOpened() const;

    // Returns items if opened successfully; empty if locked/already opened
    std::vector<std::shared_ptr<Item>> tryOpen(const std::string& answer = "");
    // Open without puzzle
    std::vector<std::shared_ptr<Item>> open();

    Puzzle* getPuzzle() const;
    void display() const;

private:
    int id_;
    bool opened_;
    std::vector<std::shared_ptr<Item>> items_;
    std::shared_ptr<Puzzle> puzzle_;
};
