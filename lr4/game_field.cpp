#include "game_field.h"

#include <stdexcept>
#include <unordered_set>

#include "ability_manager.h"
#include "exceptions.h"
#include "ship.h"
#include "ship_manager.h"

GameField::GameField(int width, int height) : width(width), height(height), valid(true)
{
    if (width <= 0 || height <= 0)
    {
        valid = false;
        this->width = DEFAULT_WIDTH;
        this->height = DEFAULT_HEIGHT;
    }

    field.resize(height, std::vector<CellStatus>(width, CellStatus::Empty));
    shipGrid.resize(height, std::vector<Ship*>(width, nullptr));
    segmentIndexGrid.resize(height, std::vector<int>(width, -1));
}

bool GameField::isValid() const
{
    return valid;
}

GameField::GameField(const GameField& other)
{
    copyFrom(other);
}

GameField::GameField(GameField&& other) noexcept
    : width(other.width), height((other.height)), field(std::move(other.field)), shipGrid(std::move(other.shipGrid)), segmentIndexGrid(std::move(other.segmentIndexGrid))
{
    other.width = 0;
    other.height = 0;
}

GameField& GameField::operator=(const GameField& other)
{
    if (this != &other)
        copyFrom(other);

    return *this;
}

GameField& GameField::operator=(GameField&& other) noexcept
{
    if (this != &other)
    {
        width = other.width;
        height = other.height;
        field = std::move(other.field);
        shipGrid = std::move(other.shipGrid);
        segmentIndexGrid = std::move(other.segmentIndexGrid);
        other.width = 0;
        other.height = 0;
    }
    return *this;
}

GameField::~GameField() = default;

void GameField::copyFrom(const GameField& other)
{
    width = other.width;
    height = other.height;
    field = other.field;
    shipGrid = other.shipGrid;
    segmentIndexGrid = other.segmentIndexGrid;
}

bool GameField::isValidPosition(int x, int y) const
{
    return x >= 0 && x < width && y >= 0 && y < width;
}

bool GameField::canPlaceShip(Ship* ship, int x, int y, Orientation orientation) const
{
    if (!ship)
        return false;

    int dx = (orientation == Orientation::Horizontal) ? 1 : 0;
    int dy = (orientation == Orientation::Vertical) ? 1 : 0;
    int length = ship->getLength();

    for (int i = 0; i < length; ++i)
    {
        int nx = x + i * dx;
        int ny = y + i * dy;

        if (!isValidPosition(nx, ny) || field[ny][nx] != CellStatus::Empty || isAdjacentToShip(nx, ny))
            return false;
    }
    return true;
}

bool GameField::isAdjacentToShip(int x, int y) const
{
    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            int adjX = x + dx;
            int adjY = y + dy;
            if (isValidPosition(adjX, adjY) && field[adjY][adjX] == CellStatus::Ship)
                return true;
        }
    }
    return false;
}

void GameField::placeShip(Ship* ship, int x, int y, Orientation orientation)
{
    if (!ship)
        throw std::invalid_argument("Ship pointer is null.");

    if (!canPlaceShip(ship, x, y, orientation))
        throw ShipPlacementException();

    ship->setOrientation(orientation);

    int dx = (orientation == Orientation::Horizontal) ? 1 : 0;
    int dy = (orientation == Orientation::Vertical) ? 1 : 0;
    int length = ship->getLength();

    for (int i = 0; i < length; ++i)
    {
        int nx = x + i * dx;
        int ny = y + i * dy;

        field[ny][nx] = CellStatus::Ship;
        shipGrid[ny][nx] = ship;
        segmentIndexGrid[ny][nx] = i;
    }
    shipsInField.insert(ship);
}

CellStatus GameField::getCellStatus(int x, int y) const
{
    if (!isValidPosition(x, y))
        throw std::out_of_range("Cell position out of range.");

    return field[y][x];
}

void GameField::activateDoubleDamage()
{
    doubleDamageActivate = true;
}

bool GameField::attackCell(int x, int y, ShipManager &shipManager)
{
    if (!isValidPosition(x, y))
        throw std::out_of_range("Cell position out of range.");

    bool shipSunk = false;

    if (field[y][x] == CellStatus::Ship)
    {
        Ship* ship = shipGrid[y][x];
        if (!ship)
            throw std::runtime_error("Unexpected null ship pointer in ship grid.");

        int segmentIndex = segmentIndexGrid[y][x];

        int damage = 1;
        if (doubleDamageActivate)
        {
            damage *= 2;
            doubleDamageActivate = false;
        }
        ship->applyDamage(segmentIndex, damage);
        shipManager.updateShip(ship);

        if (ship->isSunk())
            shipSunk = true;
    }
    else if (field[y][x] == CellStatus::Empty || field[y][x] == CellStatus::Unknown)
    {
        field[y][x] = CellStatus::Miss;
    }

    return shipSunk;
}

char GameField::getDisplayCharAt(int x, int y) const
{
    if (!isValidPosition(x, y))
        throw std::out_of_range("Cell position out of range.");

    switch (field[y][x])
    {
        case CellStatus::Unknown:
            return '?';
        case CellStatus::Empty:
            return '.';
        case CellStatus::Miss:
            return 'O';
        case CellStatus::Ship:
        {
            Ship* ship = shipGrid[y][x];
            if (!ship)
                throw std::runtime_error("Unexpected null ship pointer in ship grid.");

            int segmentIndex = segmentIndexGrid[y][x];
            SegmentStatus segmentStatus = ship->getSegmentStatus(segmentIndex);
            switch (segmentStatus)
            {
                case SegmentStatus::Intact:
                    return 'S';
                case SegmentStatus::Damaged:
                    return 'D';
                case SegmentStatus::Destroyed:
                    return 'X';
            }
        }
    }
    return '?';
}

const std::unordered_set<Ship*>& GameField::getAllShips() const
{
    return shipsInField;
}
