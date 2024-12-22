#ifndef SCANNER_ABILITY_H
#define SCANNER_ABILITY_H

#include "ability.h"

class ScannerAbility : public Ability {
public:
    void apply(GameField& field) override;
    void setParameters() override;
    AbilityType getType() const override { return AbilityType::Scanner; }
private:
    int x = 0;
    int y = 0;
};

#endif
