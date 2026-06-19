#include "Location.h"

Location::Location(int id, const std::string& name, const std::string& description,
                   std::vector<std::string> asciiMap, LocationState state,
                   std::shared_ptr<Puzzle> puzzle, int puzzleDamage,
                   std::vector<DoorInfo> doors, bool isFinal,
                   int keyItemId, const std::string& hint)
    : id_(id), name_(name), description_(description),
      asciiMap_(std::move(asciiMap)), state_(state),
      puzzle_(std::move(puzzle)), puzzleDamage_(puzzleDamage),
      doors_(std::move(doors)), isFinal_(isFinal),
      keyItemId_(keyItemId), hint_(hint) {}

int Location::getId() const { return id_; }
const std::string& Location::getName() const { return name_; }
const std::string& Location::getDescription() const { return description_; }
LocationState Location::getState() const { return state_; }
bool Location::isOpen() const { return state_ == LocationState::OPEN; }
bool Location::isFinal() const { return isFinal_; }
const std::vector<DoorInfo>& Location::getDoors() const { return doors_; }
int Location::getPuzzleDamage() const { return puzzleDamage_; }
Puzzle* Location::getPuzzle() const { return puzzle_.get(); }
int Location::getKeyItemId() const { return keyItemId_; }
const std::string& Location::getHint() const { return hint_; }
std::vector<std::shared_ptr<Chest>>& Location::getChests() { return chests_; }
std::vector<std::shared_ptr<NPC>>&   Location::getNPCs()   { return npcs_; }

std::vector<int> Location::getConnectedIds() const {
    std::vector<int> ids;
    for (auto& d : doors_) ids.push_back(d.targetLocationId);
    return ids;
}

void Location::addChest(std::shared_ptr<Chest> chest, int row, int col) {
    ObjectPos p; p.row = row; p.col = col; p.id = chest->getId();
    chestPositions_.push_back(p);
    chests_.push_back(std::move(chest));
}
void Location::addNPC(std::shared_ptr<NPC> npc, int row, int col) {
    ObjectPos p; p.row = row; p.col = col; p.id = npc->getId();
    npcPositions_.push_back(p);
    npcs_.push_back(std::move(npc));
}

void Location::getSpawn(int& row, int& col) {
    for (int r = 0; r < (int)asciiMap_.size(); ++r)
        for (int c = 0; c < (int)asciiMap_[r].size(); ++c)
            if (asciiMap_[r][c] == '@') {
                row = r; col = c;
                asciiMap_[r][c] = '.'; // erase so displayMap won't double-draw
                return;
            }
    row = 1; col = 1;
}

char Location::cellAt(int r, int c) const {
    if (r < 0 || r >= (int)asciiMap_.size()) return '#';
    if (c < 0 || c >= (int)asciiMap_[r].size()) return '#';
    return asciiMap_[r][c];
}
void Location::setCell(int r, int c, char ch) {
    if (r < 0 || r >= (int)asciiMap_.size()) return;
    if (c < 0 || c >= (int)asciiMap_[r].size()) return;
    asciiMap_[r][c] = ch;
}

bool Location::tryMove(int pr, int pc, int dr, int dc, int& nr, int& nc) const {
    nr = pr + dr; nc = pc + dc;
    char cell = cellAt(nr, nc);
    if (cell == '#' || cell == 'D' || cell == 'C' || cell == 'N') return false;
    return true;
}

const DoorInfo* Location::getDoorAt(int r, int c) const {
    for (auto& d : doors_) if (d.row == r && d.col == c) return &d;
    return nullptr;
}
Chest* Location::getChestAt(int r, int c) const {
    for (size_t i = 0; i < chestPositions_.size(); ++i)
        if (chestPositions_[i].row == r && chestPositions_[i].col == c)
            return chests_[i].get();
    return nullptr;
}
NPC* Location::getNPCAt(int r, int c) const {
    for (size_t i = 0; i < npcPositions_.size(); ++i)
        if (npcPositions_[i].row == r && npcPositions_[i].col == c)
            return npcs_[i].get();
    return nullptr;
}

CellType Location::checkAdjacent(int pr, int pc, int dr, int dc, int& outId) const {
    int nr = pr + dr, nc = pc + dc;
    outId = -1;
    char cell = cellAt(nr, nc);
    if (cell == '#') return CellType::WALL;
    auto* door = getDoorAt(nr, nc);
    if (door) { outId = door->targetLocationId; return CellType::DOOR; }
    auto* chest = getChestAt(nr, nc);
    if (chest) { outId = chest->getId(); return CellType::CHEST; }
    auto* npc = getNPCAt(nr, nc);
    if (npc && npc->isAlive()) { outId = npc->getId(); return CellType::NPC; }
    return CellType::FLOOR;
}

bool Location::tryUnlock(const std::string& answer) {
    if (!puzzle_) { state_ = LocationState::OPEN; return true; }
    if (puzzle_->checkAnswer(answer)) {
        state_ = LocationState::OPEN; puzzle_ = nullptr; return true;
    }
    return false;
}
void Location::forceOpen() { state_ = LocationState::OPEN; puzzle_ = nullptr; }

void Location::displayMap(int playerRow, int playerCol) const {
    std::cout << "\n";
    int mapW = asciiMap_.empty() ? 12 : (int)asciiMap_[0].size();
    std::cout << "  +";
    for (int i = 0; i < mapW; ++i) std::cout << "-";
    std::cout << "+\n";

    for (int r = 0; r < (int)asciiMap_.size(); ++r) {
        std::cout << "  |";
        for (int c = 0; c < (int)asciiMap_[r].size(); ++c) {
            if (r == playerRow && c == playerCol) { std::cout << '@'; continue; }
            // Chest: show 'C' or '.' if opened
            bool printed = false;
            for (size_t k = 0; k < chestPositions_.size(); ++k) {
                if (chestPositions_[k].row == r && chestPositions_[k].col == c) {
                    std::cout << (chests_[k]->isOpened() ? '.' : 'C');
                    printed = true; break;
                }
            }
            if (!printed) {
                for (size_t k = 0; k < npcPositions_.size(); ++k) {
                    if (npcPositions_[k].row == r && npcPositions_[k].col == c) {
                        std::cout << (npcs_[k]->isAlive() ? 'N' : '.');
                        printed = true; break;
                    }
                }
            }
            if (!printed) std::cout << asciiMap_[r][c];
        }
        std::cout << "|\n";
    }
    std::cout << "  +";
    for (int i = 0; i < mapW; ++i) std::cout << "-";
    std::cout << "+\n";
}

void Location::displayInfo() const {
    std::cout << "\n=== " << name_ << " ===\n" << description_ << "\n";
}
