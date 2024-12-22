#ifndef ABILITY_H
#define ABILITY_H

#include "game_field.h"

enum class AbilityType {
    DoubleDamage,
    Scanner,
    Barrage
};

class Ability {
public:
    virtual void apply(GameField& field) = 0;
    virtual void setParameters() {};
    virtual AbilityType getType() const = 0;
    virtual ~Ability() = default;
};

#endif
