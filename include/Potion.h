#pragma once
#include "Item.h"

class Potion : public Item {
public:
    Potion(int id, const std::string& name, int healAmount);

    int getHealAmount() const;
    std::string describe() const override;

private:
    int healAmount_;
};
