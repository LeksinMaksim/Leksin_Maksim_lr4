#include "game_controller.h"
#include "ship_placement_handler.h"
#include <iostream>
#include <iomanip>
#include <limits>

MenuChoice DefaultCommandHandler::showStartMenu() const {
    std::cout << "\n=== Морской бой ===\n";
    std::cout << "1. Новая игра\n";
    std::cout << "2. Загрузить игру\n";
    std::cout << "3. Выход\n";
    std::cout << "\nВыберите действие (1-3): ";

    int choice;
    while (!(std::cin >> choice) || choice < 1 || choice > 3) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Пожалуйста, введите число от 1 до 3: ";
    }

    switch (choice) {
        case 1: return MenuChoice::NewGame;
        case 2: return MenuChoice::LoadGame;
        case 3: return MenuChoice::Exit;
        default: return MenuChoice::Exit;
    }
}

void DefaultCommandHandler::handleStartMenu(Game& game) {
    MenuChoice choice = showStartMenu();
    
    switch (choice) {
        case MenuChoice::NewGame:
            game.initializeGame();
            break;
            
        case MenuChoice::LoadGame: {
            std::string filename;
            std::cout << "Введите имя файла для загрузки: ";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::getline(std::cin, filename);
            
            try {
                game.loadGame(filename);
            } catch (const std::exception& e) {
                std::cout << "Ошибка при загрузке: " << e.what() << "\n";
                std::cout << "Начинаем новую игру...\n";
                game.initializeGame();
            }
            break;
        }
        
        case MenuChoice::Exit:
            game.setGameOver(true);
            std::cout << "Спасибо за игру!\n";
            break;
    }
}

void DefaultCommandHandler::handleCommand(Command cmd, Game& game) {
    try {
        switch(cmd) {
            case Command::Attack:
            {
                int x, y;
                std::cout << "Введите координату X для атаки (0-" << GameField::DEFAULT_WIDTH - 1 << "): ";
                while (!(std::cin >> x) || x < 0 || x >= GameField::DEFAULT_WIDTH) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Пожалуйста, введите корректную координату X (0-" 
                            << GameField::DEFAULT_WIDTH - 1 << "): ";
                }

                std::cout << "Введите координату Y для атаки (0-" << GameField::DEFAULT_HEIGHT - 1 << "): ";
                while (!(std::cin >> y) || y < 0 || y >= GameField::DEFAULT_HEIGHT) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Пожалуйста, введите корректную координату Y (0-" 
                            << GameField::DEFAULT_HEIGHT - 1 << "): ";
                }

                GameField& computerField = game.getComputerField();
                ShipManager* computerShipManager = game.getComputerShipManager();

                bool shipSunk = computerField.attackCell(x, y, *computerShipManager);
                if (shipSunk) {
                    game.getUserAbilityManager()->addRandomAbility();
                }

                game.computerTurn();
                break;
            }

            case Command::UseAbility:
            {
                if (game.getUserAbilityManager()->hasAbilities()) {
                    game.getUserAbilityManager()->useAbility(game.getComputerField());
                    std::cout << "Способность использована успешно.\n";
                } else {
                    std::cout << "У вас нет доступных способностей.\n";
                }
                break;
            }

            case Command::SaveGame:
            {
                std::string filename;
                std::cout << "Введите имя файла для сохранения: ";
                std::getline(std::cin, filename);
                
                if (filename.empty()) {
                    std::cout << "Имя файла не может быть пустым.\n";
                    break;
                }
                
                if (filename.find(".sav") == std::string::npos) {
                    filename += ".sav";
                }
                
                try {
                    game.saveGame(filename);
                    std::cout << "Игра успешно сохранена в файл: " << filename << "\n";
                } catch (const std::exception& e) {
                    std::cerr << "Ошибка при сохранении: " << e.what() << "\n";
                }
                break;
            }

            case Command::LoadGame:
            {
                std::string filename;
                std::cout << "Введите имя файла для загрузки: ";
                std::getline(std::cin, filename);
                
                if (filename.empty()) {
                    std::cout << "Имя файла не может быть пустым.\n";
                    break;
                }
                
                if (filename.find(".sav") == std::string::npos) {
                    filename += ".sav";
                }
                
                try {
                    game.loadGame(filename);
                    std::cout << "Игра успешно загружена из файла: " << filename << "\n";
                } catch (const std::exception& e) {
                    std::cerr << "Ошибка при загрузке: " << e.what() << "\n";
                }
                break;
            }

            case Command::DisplayFields:
            {
                observer_->onFieldUpdate();
                break;
            }

            case Command::Quit:
                game.setGameOver(true);
                std::cout << "Спасибо за игру!\n";
                break;

            default:
                std::cout << "Неизвестная команда\n";
                break;
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при выполнении команды: " << e.what() << "\n";
    }
}