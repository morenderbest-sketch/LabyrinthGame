#pragma once
#include "Common.h"

class Item {
public:
    Item(int id, const std::string& name, ItemType type);
    virtual ~Item() = default;

    int getId() const;
    const std::string& getName() const;
    ItemType getType() const;

    virtual std::string describe() const = 0;

protected:
    int id_;
    std::string name_;
    ItemType type_;
};
