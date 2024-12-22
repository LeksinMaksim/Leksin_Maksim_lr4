#ifndef GAME_H
#define GAME_H

#include <memory>
#include <random>
#include <vector>
#include <fstream>
#include "game_field.h"
#include "ship_manager.h"
#include "ability_manager.h"
#include "game_state.h"
#include "game_display.h"

enum class GameAction {
    Attack,
    UseAbility,
    SaveGame,
    LoadGame,
    DisplayFields,
    Quit
};

enum class GameResult {
    NoWin,
    PlayerWin,
    ComputerWin
};

struct AttackResult {
    bool hit;
    bool sunk;
};

class Game {
public:
    Game();

    void saveGame(const std::string& filename);
    void loadGame(const std::string& filename);
    void computerTurn();

    void registerObserver(IGameObserver* observer);
    void unregisterObserver(IGameObserver* observer);

    const GameField& getUserField() const { return *userField; }
    const GameField& getComputerField() const { return *computerField; }
    GameField& getUserField() { return *userField; }
    GameField& getComputerField() { return *computerField; }
    AbilityManager* getUserAbilityManager() const { return userAbilityManager.get(); }
    bool isGameOver() const { return gameOver; }
    void setGameOver(bool value) { gameOver = value; }
    ShipManager* getComputerShipManager() const { return computerShipManager.get(); }
    void initializeGame();

protected:
    void notifyFieldUpdate();
    void notifyAbilityUsed();
    void notifyGameOver();
    void notifyShipDestroyed();

private:
    void showStartMenu();
    AttackResult processAttackResult(Ship* ship, int segmentIndex);
    GameResult checkWin();
    void resetGame();
    void startNewRound();
    void handleGameResult(GameResult result);
    void placeComputerShips();

    std::unique_ptr<GameField> userField;
    std::unique_ptr<GameField> computerField;
    std::unique_ptr<ShipManager> userShipManager;
    std::unique_ptr<ShipManager> computerShipManager;
    std::unique_ptr<AbilityManager> userAbilityManager;
    bool gameOver;
    std::mt19937 rng;
    const std::vector<int> shipSizes = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
    std::vector<IGameObserver*> observers_;
};

#endif