#ifndef SHIP_PLACEMENT_HANDLER_H
#define SHIP_PLACEMENT_HANDLER_H

#include "game_field.h"
#include "ship_manager.h"
#include "game_display.h"

class ShipPlacementHandler {
public:
    static void placeUserShips(GameField& field, ShipManager& shipManager, IGameObserver& observer);

private:
    static void getPlacementCoordinates(int& x, int& y, char& orientationChar);
    static bool validateCoordinates(int x, int y);
    static Orientation charToOrientation(char orientationChar);
};

#endif