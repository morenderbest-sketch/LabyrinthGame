#pragma once
#include "Item.h"

class Key : public Item {
public:
    // targetLocationId: которую открывает этот ключ
    Key(int id, const std::string& name, int targetLocationId);

    int getTargetLocationId() const;
    std::string describe() const override;

private:
    int targetLocationId_;
};
