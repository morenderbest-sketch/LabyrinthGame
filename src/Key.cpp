#include "Key.h"

Key::Key(int id, const std::string& name, int targetLocationId)
    : Item(id, name, ItemType::KEY), targetLocationId_(targetLocationId) {}

int Key::getTargetLocationId() const { return targetLocationId_; }

std::string Key::describe() const {
    return "[Ключ] " + name_ + " — открывает путь дальше";
}
