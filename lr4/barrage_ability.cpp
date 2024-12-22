#include "barrage_ability.h"

#include <random>
#include <vector>

void BarrageAbility::apply(GameField& field)
{
    const auto& shipsSet = field.getAllShips();

    std::vector<Ship*> ships(shipsSet.begin(), shipsSet.end());

    std::mt19937 rng(std::random_device {}());
    std::uniform_int_distribution<int> shipDist(0, ships.size() - 1);
    Ship* targetShip = ships[shipDist(rng)];

    int segmentCount = targetShip->getLength();
    std::uniform_int_distribution<int> segmentDist(0, segmentCount - 1);
    int targetSegment = segmentDist(rng);

    targetShip->applyDamage(targetSegment, 1);
}
