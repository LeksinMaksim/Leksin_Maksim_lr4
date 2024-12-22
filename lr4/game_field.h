#ifndef GAME_FIELD_H
#define GAME_FIELD_H

#include <unordered_set>
#include <vector>

#include "ship.h"
#include "ship_manager.h"
#include "exceptions.h"

class AbilityManager;

enum class CellStatus
{
    Unknown,
    Empty,
    Ship,
    Miss
};

class GameField
{
public:
    static const int DEFAULT_WIDTH = 10;
    static const int DEFAULT_HEIGHT = 10;

    bool isValid() const;

    GameField(int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT);
    GameField(const GameField& other);
    GameField(GameField&& other) noexcept;
    GameField& operator=(const GameField& other);
    GameField& operator=(GameField&& other) noexcept;
    ~GameField();

    void placeShip(Ship* ship, int x, int y, Orientation orientation);
    CellStatus getCellStatus(int x, int y) const;
    bool attackCell(int x, int y, ShipManager& shipManager);
    char getDisplayCharAt(int x, int y) const;

    bool isValidPosition(int x, int y) const;
    void activateDoubleDamage();
    const std::unordered_set<Ship*>& getAllShips() const;

    void setAbilityManager(AbilityManager* manager) { abilityManager = manager; }

    Ship* getShipAt(int x, int y) const { return shipGrid[y][x]; }
    int getSegmentIndexAt(int x, int y) const { return segmentIndexGrid[y][x]; }

    void setCellStatus(int x, int y, CellStatus status) {
    if (!isValidPosition(x, y))
        throw OutOfBoundsException();
    field[y][x] = status;
}

private:
    int width;
    int height;
    std::vector<std::vector<CellStatus>> field;
    std::vector<std::vector<Ship*>> shipGrid;
    std::vector<std::vector<int>> segmentIndexGrid;

    bool canPlaceShip(Ship* ship, int x, int y, Orientation orientation) const;
    bool isAdjacentToShip(int x, int y) const;
    void copyFrom(const GameField& other);

    bool valid;
    bool doubleDamageActivate = false;
    std::unordered_set<Ship*> shipsInField;

    AbilityManager* abilityManager = nullptr;
};

#endif
