#include "ship_placement_handler.h"
#include <iostream>
#include <limits>

void ShipPlacementHandler::placeUserShips(GameField& field, ShipManager& shipManager, IGameObserver& observer) {
    for (size_t i = 0; i < shipManager.getShipCount(); ++i) {
        Ship* ship = shipManager.getShip(i);
        bool placed = false;
        
        while (!placed) {
            observer.renderShipPlacement(ship->getLength(), i + 1);
            observer.onFieldUpdate(); 

            int x = 0, y = 0;
            char orientationChar = 0;
            getPlacementCoordinates(x, y, orientationChar);
            
            try {
                field.placeShip(ship, x, y, charToOrientation(orientationChar));
                placed = true;
                std::cout << "Корабль размещен.\n";
            } catch (const ShipPlacementException& e) {
                std::cout << e.what() << " Попробуйте снова.\n";
            }
        }
    }
}

void ShipPlacementHandler::getPlacementCoordinates(int& x, int& y, char& orientationChar) {
    std::cout << "Введите координату X (0-" << GameField::DEFAULT_WIDTH - 1 << "): ";
    while (!(std::cin >> x) || !validateCoordinates(x, GameField::DEFAULT_WIDTH)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Пожалуйста, введите корректную координату X (0-" 
                << GameField::DEFAULT_WIDTH - 1 << "): ";
    }

    std::cout << "Введите координату Y (0-" << GameField::DEFAULT_HEIGHT - 1 << "): ";
    while (!(std::cin >> y) || !validateCoordinates(y, GameField::DEFAULT_HEIGHT)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Пожалуйста, введите корректную координату Y (0-" 
                << GameField::DEFAULT_HEIGHT - 1 << "): ";
    }

    std::cout << "Введите ориентацию (H - горизонтально, V - вертикально): ";
    while (!(std::cin >> orientationChar) || 
           (std::toupper(orientationChar) != 'H' && std::toupper(orientationChar) != 'V')) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Пожалуйста, введите H или V: ";
    }
}

bool ShipPlacementHandler::validateCoordinates(int coord, int maxValue) {
    return coord >= 0 && coord < maxValue;
}

Orientation ShipPlacementHandler::charToOrientation(char orientationChar) {
    return (std::toupper(orientationChar) == 'H') ? 
           Orientation::Horizontal : 
           Orientation::Vertical;
}