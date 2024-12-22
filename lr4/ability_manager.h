#ifndef ABILITY_MANAGER_H
#define ABILITY_MANAGER_H

#include <memory>
#include <deque>
#include <vector>
#include "ability.h"

class GameField;

class AbilityManager {
public:
    AbilityManager();
    void addAbility(std::unique_ptr<Ability> ability);
    void useAbility(GameField& field);
    bool hasAbilities() const;
    void addRandomAbility();

    std::vector<int> getAbilityCounts() const;
    void setAbilitiesFromCounts(const std::vector<int>& counts);

    std::string getFirstAbilityName() const;

private:
    std::deque<std::unique_ptr<Ability>> abilities;

    static std::string abilityTypeToString(AbilityType type);
};

#endif