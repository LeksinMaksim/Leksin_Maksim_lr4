#include "game.h"
#include "exceptions.h"
#include "ship_placement_handler.h"
#include <iostream>
#include <iomanip>

Game::Game() : gameOver(false), rng(std::random_device{}()) {
}

void Game::initializeGame() {
    userField = std::make_unique<GameField>();
    computerField = std::make_unique<GameField>();
    userAbilityManager = std::make_unique<AbilityManager>();
    userField->setAbilityManager(userAbilityManager.get());
    userShipManager = std::make_unique<ShipManager>(shipSizes);
    computerShipManager = std::make_unique<ShipManager>(shipSizes);

    if (!userShipManager->isValid() || !computerShipManager->isValid()) {
        std::cerr << "Ошибка при инициализации флота.\n";
        std::exit(EXIT_FAILURE);
    }

    placeComputerShips();
    notifyFieldUpdate();

    ShipPlacementHandler::placeUserShips(*userField, *userShipManager, *(observers_.front()));
    notifyFieldUpdate();
}

void Game::registerObserver(IGameObserver* observer) {
    if (observer) {
        observers_.push_back(observer);
    }
}

void Game::unregisterObserver(IGameObserver* observer) {
    observers_.erase(
        std::remove(observers_.begin(), observers_.end(), observer),
        observers_.end()
    );
}

void Game::notifyFieldUpdate() {
    for (auto* observer : observers_) {
        if (observer) observer->onFieldUpdate();
    }
}

void Game::notifyAbilityUsed() {
    for (auto* observer : observers_) {
        if (observer) observer->onAbilityUsed();
    }
}

void Game::notifyGameOver() {
    for (auto* observer : observers_) {
        if (observer) observer->onGameOver();
    }
}

void Game::notifyShipDestroyed() {
    for (auto* observer : observers_) {
        if (observer) observer->onShipDestroyed();
    }
}

void Game::placeComputerShips() {
    std::uniform_int_distribution<int> coordDistX(0, GameField::DEFAULT_WIDTH - 1);
    std::uniform_int_distribution<int> coordDistY(0, GameField::DEFAULT_HEIGHT - 1);
    std::uniform_int_distribution<int> orientationDist(0, 1);

    for (size_t i = 0; i < computerShipManager->getShipCount(); ++i) {
        Ship* ship = computerShipManager->getShip(i);
        bool placed = false;
        while (!placed) {
            try {
                int x = coordDistX(rng);
                int y = coordDistY(rng);
                Orientation orientation = (orientationDist(rng) == 0) ? 
                                       Orientation::Horizontal : 
                                       Orientation::Vertical;
                computerField->placeShip(ship, x, y, orientation);
                placed = true;
            } catch (const ShipPlacementException&) {
            }
        }
    }
}

void Game::computerTurn() {
    std::uniform_int_distribution<int> coordDistX(0, GameField::DEFAULT_WIDTH - 1);
    std::uniform_int_distribution<int> coordDistY(0, GameField::DEFAULT_HEIGHT - 1);

    bool validAttack = false;
    int x, y;

    while (!validAttack) {
        x = coordDistX(rng);
        y = coordDistY(rng);

        CellStatus currentStatus = userField->getCellStatus(x, y);
        Ship* ship = userField->getShipAt(x, y);
        if (currentStatus == CellStatus::Miss || 
            (ship && ship->getSegmentStatus(userField->getSegmentIndexAt(x, y)) != SegmentStatus::Intact)) {
            continue;
        }
        validAttack = true;
    }

    bool shipSunk = userField->attackCell(x, y, *userShipManager);
    notifyFieldUpdate();

    if (shipSunk) {
        notifyShipDestroyed();
    }

    GameResult result = checkWin();
    if (result != GameResult::NoWin) {
        handleGameResult(result);
    }
}

GameResult Game::checkWin() {
    if (computerShipManager->areAllShipsSunk()) {
        return GameResult::PlayerWin;
    }
    if (userShipManager->areAllShipsSunk()) {
        return GameResult::ComputerWin;
    }
    return GameResult::NoWin;
}

void Game::handleGameResult(GameResult result) {
    if (result == GameResult::PlayerWin) {
        startNewRound();
    } else if (result == GameResult::ComputerWin) {
        resetGame();
    }
    notifyGameOver();
}

void Game::resetGame() {
    gameOver = true;
}

void Game::startNewRound() {
    computerField = std::make_unique<GameField>();
    computerShipManager = std::make_unique<ShipManager>(shipSizes);

    if (!computerShipManager->isValid()) {
        std::cerr << "Ошибка при инициализации флота компьютера.\n";
        std::exit(EXIT_FAILURE);
    }

    placeComputerShips();
    notifyFieldUpdate();
}

void Game::saveGame(const std::string& filename) {
    try {
        GameState::saveToFile(filename, *userField, *computerField, *userAbilityManager);
    } catch (const std::exception& e) {
        throw;
    }
}

void Game::loadGame(const std::string& filename) {
    try {
        GameField newUserField;
        GameField newComputerField;
        AbilityManager newAbilityManager;
        
        auto newUserShipManager = std::make_unique<ShipManager>(shipSizes);
        auto newComputerShipManager = std::make_unique<ShipManager>(shipSizes);
        
        if (!newUserShipManager->isValid() || !newComputerShipManager->isValid()) {
            throw std::runtime_error("Ошибка создания менеджеров кораблей");
        }
        
        GameState::loadFromFile(filename, 
                              newUserField, 
                              newComputerField,
                              *newUserShipManager, 
                              *newComputerShipManager, 
                              newAbilityManager);
        
        if (!newUserField.isValid() || !newComputerField.isValid()) {
            throw std::runtime_error("Ошибка загрузки полей");
        }
        
        userField = std::make_unique<GameField>(std::move(newUserField));
        computerField = std::make_unique<GameField>(std::move(newComputerField));
        userShipManager = std::move(newUserShipManager);
        computerShipManager = std::move(newComputerShipManager);
        userAbilityManager = std::make_unique<AbilityManager>(std::move(newAbilityManager));
        
        userField->setAbilityManager(userAbilityManager.get());
        
        notifyFieldUpdate();
    } catch (const std::exception& e) {
        throw;
    }
}