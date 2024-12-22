#ifndef SHIP_MANAGER_H
#define SHIP_MANAGER_H

#include <memory>
#include <vector>

#include "ship.h"

class ShipManager
{
public:
    ShipManager(const std::vector<int>& shipSizes);

    bool isValid() const;

    Ship* getShip(size_t index) const;
    size_t getShipCount() const;
    void updateShip(Ship* ship);

    int getShipRemaining() const;
    bool areAllShipsSunk() const;

private:
    static const int MIN_LENGTH = 1;
    static const int MAX_LENGTH = 4;
    std::vector<std::unique_ptr<Ship>> ships;
    std::vector<bool> shipSunkStatus;
    int shipsRemaining;

    ShipManager(const ShipManager&) = delete;
    ShipManager& operator=(const ShipManager&) = delete;

    bool valid;
};

#endif
