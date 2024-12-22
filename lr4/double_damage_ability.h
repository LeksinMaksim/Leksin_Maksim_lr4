#ifndef DOUBLE_DAMAGE_ABILITY_H
#define DOUBLE_DAMAGE_ABILITY_H

#include "ability.h"

class DoubleDamageAbility : public Ability {
public:
    void apply(GameField& field) override;
    AbilityType getType() const override { return AbilityType::DoubleDamage; }
};

#endif
