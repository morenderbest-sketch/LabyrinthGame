#pragma once
#include "Item.h"

class Weapon : public Item {
public:
    Weapon(int id, const std::string& name, int bonusDamage);

    int getBonusDamage() const;
    std::string describe() const override;

private:
    int bonusDamage_;
};
