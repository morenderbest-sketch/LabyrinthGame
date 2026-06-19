#include "Item.h"

Item::Item(int id, const std::string& name, ItemType type)
    : id_(id), name_(name), type_(type) {}

int Item::getId() const { return id_; }
const std::string& Item::getName() const { return name_; }
ItemType Item::getType() const { return type_; }
