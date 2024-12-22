#include "ship_manager.h"

#include <memory>
#include <stdexcept>

#include "ship.h"

ShipManager::ShipManager(const std::vector<int>& shipSizes) : shipsRemaining(0), valid(true)
{
    for (int size : shipSizes)
    {
        auto ship = std::make_unique<Ship>(size, Orientation::Horizontal);
        if (!ship->isValid())
        {
            valid = false;
            break;
        }
        ships.push_back(std::move(ship));
        shipSunkStatus.push_back(false);
        shipsRemaining++;
    }
}

bool ShipManager::isValid() const
{
    return valid;
}

Ship* ShipManager::getShip(size_t index) const
{
    if (index >= ships.size())
        throw std::out_of_range("Ship index out of range.");

    return ships[index].get();
}

size_t ShipManager::getShipCount() const
{
    return ships.size();
}

void ShipManager::updateShip(Ship* ship)
{
    if (!ship)
        throw std::invalid_argument("Ship pointer is null.");

    for (size_t i = 0; i < ships.size(); ++i)
    {
        if (ships[i].get() == ship)
        {
            if (!shipSunkStatus[i] && ship->isSunk())
            {
                shipSunkStatus[i] = true;
                shipsRemaining--;
            }
            return;
        }
    }
    throw std::invalid_argument("Ship not found in manager in manager.");
}

int ShipManager::getShipRemaining() const
{
    return shipsRemaining;
}

bool ShipManager::areAllShipsSunk() const
{
    return shipsRemaining == 0;
}
