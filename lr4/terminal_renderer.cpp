#include "terminal_renderer.h"
#include <iostream>
#include <iomanip>

void TerminalRenderer::renderUserField(const GameField& field) {
    std::cout << "\nВаше поле:\n";
    renderField(field, false);
}

void TerminalRenderer::renderComputerField(const GameField& field) {
    std::cout << "\nПоле компьютера:\n";
    renderField(field, true);
}

void TerminalRenderer::renderAbilityStatus(const AbilityManager& manager) {
    std::cout << "\nДоступные способности:\n";
    if (manager.hasAbilities()) {
        std::cout << "Следующая способность: " << manager.getFirstAbilityName() << "\n";
    } else {
        std::cout << "У вас нет доступных способностей\n";
    }
}

void TerminalRenderer::renderGameStatus(const Game& /* game */) {
    renderHeader();
    renderInstructions();
}

void TerminalRenderer::renderStartMenu() {
    std::cout << "\n=== Морской бой ===\n";
    std::cout << "1. Новая игра\n";
    std::cout << "2. Загрузить игру\n";
    std::cout << "3. Выход\n";
    std::cout << "\nВыберите действие (1-3): ";
}

void TerminalRenderer::renderShipPlacement(int shipLength, int shipNumber) {
    std::cout << "Размещение корабля " << shipNumber << " (Длина: " << shipLength << ")\n";
}

void TerminalRenderer::renderAttackResult(int x, int y, bool isHit, bool isSunk, bool isComputerAttack) {
    std::string attacker = isComputerAttack ? "Компьютер" : "Вы";
    std::cout << attacker << " атакует клетку (" << x << ", " << y << ")\n";
    
    if (isHit) {
        std::cout << attacker << " попал" << (isComputerAttack ? "" : "и") << " в корабль!\n";
        if (isSunk) {
            std::cout << attacker << " уничтожил" << (isComputerAttack ? "" : "и") << " корабль!\n";
        }
    } else {
        std::cout << attacker << " промахнул" << (isComputerAttack ? "ся" : "ись") << "!\n";
    }
}

void TerminalRenderer::renderField(const GameField& field, bool isComputer) const {
    std::cout << "   ";
    for (int x = 0; x < GameField::DEFAULT_WIDTH; ++x) {
        std::cout << x << " ";
    }
    std::cout << "\n";

    for (int y = 0; y < GameField::DEFAULT_HEIGHT; ++y) {
        std::cout << std::setw(2) << y << " ";
        for (int x = 0; x < GameField::DEFAULT_WIDTH; ++x) {
            char symbol = getFieldSymbol(field, x, y, isComputer);
            std::cout << symbol << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

char TerminalRenderer::getFieldSymbol(const GameField& field, int x, int y, bool isComputer) const {
    try {
        CellStatus status = field.getCellStatus(x, y);
        
        if (isComputer) {
            switch (status) {
                case CellStatus::Unknown:
                case CellStatus::Empty:
                    return '.';
                case CellStatus::Miss:
                    return 'O';
                case CellStatus::Ship: {
                    Ship* ship = field.getShipAt(x, y);
                    if (!ship) return '.';
                    
                    int segmentIndex = field.getSegmentIndexAt(x, y);
                    SegmentStatus segmentStatus = ship->getSegmentStatus(segmentIndex);
                    
                    switch (segmentStatus) {
                        case SegmentStatus::Intact:
                            return '.';
                        case SegmentStatus::Damaged:
                            return 'D';
                        case SegmentStatus::Destroyed:
                            return 'X';
                        default:
                            return '.';
                    }
                }
                default:
                    return '.';
            }
        } else {
            switch (status) {
                case CellStatus::Unknown:
                    return '?';
                case CellStatus::Empty:
                    return '.';
                case CellStatus::Miss:
                    return 'O';
                case CellStatus::Ship: {
                    Ship* ship = field.getShipAt(x, y);
                    if (!ship) return 'S';
                    
                    int segmentIndex = field.getSegmentIndexAt(x, y);
                    SegmentStatus segmentStatus = ship->getSegmentStatus(segmentIndex);
                    
                    switch (segmentStatus) {
                        case SegmentStatus::Intact:
                            return 'S';
                        case SegmentStatus::Damaged:
                            return 'D';
                        case SegmentStatus::Destroyed:
                            return 'X';
                        default:
                            return 'S';
                    }
                }
                default:
                    return '?';
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при получении символа поля: " << e.what() << "\n";
        return '?';
    }
}

void TerminalRenderer::renderHeader() const {
    std::cout << "\n=========================\n";
    std::cout << "     МОРСКОЙ БОЙ";
    std::cout << "\n=========================\n";
}

void TerminalRenderer::renderInstructions() const {
    std::cout << "\nОбозначения:\n";
    std::cout << ". - пустая клетка\n";
    std::cout << "O - промах\n";
    std::cout << "S - корабль\n";
    std::cout << "D - поврежденный корабль\n";
    std::cout << "X - уничтоженный корабль\n\n";
}