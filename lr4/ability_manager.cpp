#include "ability_manager.h"
#include "game_field.h"
#include "barrage_ability.h"
#include "double_damage_ability.h"
#include "scanner_ability.h"
#include "exceptions.h"
#include <random>

AbilityManager::AbilityManager()
{
    addRandomAbility();
}

void AbilityManager::addAbility(std::unique_ptr<Ability> ability)
{
    abilities.push_back(std::move(ability));
}

void AbilityManager::useAbility(GameField& field)
{
    if (abilities.empty())
        throw AbilityUnavailableException();

    std::unique_ptr<Ability> ability = std::move(abilities.front());
    abilities.pop_front();

    ability->setParameters();
    ability->apply(field);
}

bool AbilityManager::hasAbilities() const
{
    return !abilities.empty();
}

void AbilityManager::addRandomAbility() {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> abilityDist(0, 2);
    int abilityIndex = abilityDist(rng);

    switch (abilityIndex) {
        case 0:
            addAbility(std::make_unique<DoubleDamageAbility>());
            break;
        case 1:
            addAbility(std::make_unique<ScannerAbility>());
            break;
        case 2:
            addAbility(std::make_unique<BarrageAbility>());
            break;
    }
}

std::vector<int> AbilityManager::getAbilityCounts() const {
    std::vector<int> counts(3, 0);
    
    for (const auto& ability : abilities) {
        switch (ability->getType()) {
            case AbilityType::DoubleDamage:
                ++counts[0];
                break;
            case AbilityType::Scanner:
                ++counts[1];
                break;
            case AbilityType::Barrage:
                ++counts[2];
                break;
        }
    }
    
    return counts;
}

void AbilityManager::setAbilitiesFromCounts(const std::vector<int>& counts) {
    abilities.clear();
    
    if (counts.size() >= 3) {
        for (int i = 0; i < counts[0]; ++i) {
            addAbility(std::make_unique<DoubleDamageAbility>());
        }
        for (int i = 0; i < counts[1]; ++i) {
            addAbility(std::make_unique<ScannerAbility>());
        }
        for (int i = 0; i < counts[2]; ++i) {
            addAbility(std::make_unique<BarrageAbility>());
        }
    }
}

std::string AbilityManager::abilityTypeToString(AbilityType type) {
    switch (type) {
        case AbilityType::DoubleDamage:
            return "Двойной урон";
        case AbilityType::Scanner:
            return "Сканер";
        case AbilityType::Barrage:
            return "Бомбардировщик";
        default:
            return "Неизвестная способность";
    }
}

std::string AbilityManager::getFirstAbilityName() const {
    if (abilities.empty()) {
        return "нет доступных";
    }
    return abilityTypeToString(abilities.front()->getType());
}