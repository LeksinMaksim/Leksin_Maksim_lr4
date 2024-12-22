#include "scanner_ability.h"

#include <iostream>

#include "exceptions.h"

void ScannerAbility::setParameters()
{
    std::cout << "Enter the starting X coordinate for scanning (0-" << GameField::DEFAULT_WIDTH - 1 << "): ";
    std::cin >> x;
    std::cout << "Enter the starting Y coordinate for scanning (0-" << GameField::DEFAULT_HEIGHT - 1 << "): ";
    std::cin >> y;
}

void ScannerAbility::apply(GameField& field)
{
    if (!field.isValidPosition(x, y))
        throw OutOfBoundsException();

    for (int dy = 0; dy < 2; ++dy)
    {
        for (int dx = 0; dx < 2; ++dx)
        {
            int nx = x + dx;
            int ny = y + dy;

            if (field.isValidPosition(nx, ny))
            {
                if (field.getCellStatus(nx, ny) == CellStatus::Ship)
                    std::cout << "Ship segment detectet at (" << nx << ", " << ny << ")\n";
                else
                    std::cout << "No ship at (" << nx << ", " << ny << ")\n";
            }
        }
    }
}
