#ifndef GAME_DISPLAY_H
#define GAME_DISPLAY_H

#include <memory>

class Game;
class GameField;
class AbilityManager;

class IGameObserver {
public:
    virtual ~IGameObserver() = default;
    virtual void onFieldUpdate() = 0;
    virtual void onAbilityUsed() = 0;
    virtual void onGameOver() = 0;
    virtual void onShipDestroyed() = 0;
    virtual void renderShipPlacement(int shipLength, int shipNumber) = 0;
    virtual void renderStartMenu() = 0;
};

class IGameRenderer {
public:
    virtual ~IGameRenderer() = default;
    virtual void renderUserField(const GameField& field) = 0;
    virtual void renderComputerField(const GameField& field) = 0;
    virtual void renderAbilityStatus(const AbilityManager& manager) = 0;
    virtual void renderGameStatus(const Game& game) = 0;
    virtual void renderShipPlacement(int shipLength, int shipNumber) = 0;
    virtual void renderStartMenu() = 0;
};

template<typename Renderer>
class GameDisplay : public IGameObserver {
public:
    explicit GameDisplay(std::shared_ptr<Game> game);
    void onFieldUpdate() override;
    void onAbilityUsed() override;
    void onGameOver() override;
    void onShipDestroyed() override;
    void renderShipPlacement(int shipLength, int shipNumber) override;
    void renderStartMenu() override;
    void refresh();

private:
    std::shared_ptr<Game> game_;
    std::unique_ptr<Renderer> renderer_;
    void updateDisplay();
};

#endif