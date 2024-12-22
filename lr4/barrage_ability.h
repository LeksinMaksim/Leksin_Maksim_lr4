#ifndef BARRAGE_ABILITY_H
#define BARRAGE_ABILITY_H

#include "ability.h"

class BarrageAbility : public Ability {
public:
    void apply(GameField& field) override;
    AbilityType getType() const override { return AbilityType::Barrage; }
};

#endif
