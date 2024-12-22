#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "game.h"
#include <memory>

enum class Command {
    Attack,
    UseAbility,
    SaveGame,
    LoadGame,
    DisplayFields,
    Quit,
    Invalid
};

enum class MenuChoice {
    NewGame,
    LoadGame,
    Exit
};

class ICommandHandler {
public:
    virtual ~ICommandHandler() = default;
    virtual void handleCommand(Command cmd, Game& game) = 0;
    virtual void handleStartMenu(Game& game) = 0;
};

class DefaultCommandHandler : public ICommandHandler {
public:
    DefaultCommandHandler(std::shared_ptr<IGameObserver> observer) : observer_(observer) {}
    void handleCommand(Command cmd, Game& game) override;
    void handleStartMenu(Game& game) override;
private:
    MenuChoice showStartMenu() const;
    std::shared_ptr<IGameObserver> observer_;
};

template<typename InputProcessor>
class GameController {
public:
    explicit GameController(std::shared_ptr<Game> game, 
                          std::shared_ptr<ICommandHandler> handler = std::make_shared<DefaultCommandHandler>());
    void processInput();
    bool isGameOver() const;
    void run();

private:
    std::shared_ptr<Game> game_;
    std::shared_ptr<ICommandHandler> commandHandler_;
    InputProcessor inputProcessor_;
    bool gameInitialized_ = false;
};

template<typename InputProcessor>
GameController<InputProcessor>::GameController(std::shared_ptr<Game> game, 
                                            std::shared_ptr<ICommandHandler> handler)
    : game_(game), commandHandler_(handler) {}

template<typename InputProcessor>
void GameController<InputProcessor>::processInput() {
    if (!gameInitialized_) {
        commandHandler_->handleStartMenu(*game_);
        gameInitialized_ = true;
        return;
    }

    Command cmd = inputProcessor_.getCommand();
    if (cmd != Command::Invalid) {
        commandHandler_->handleCommand(cmd, *game_);
    }
}

template<typename InputProcessor>
bool GameController<InputProcessor>::isGameOver() const {
    return game_ ? game_->isGameOver() : true;
}

template<typename InputProcessor>
void GameController<InputProcessor>::run() {
    while (!isGameOver()) {
        processInput();
    }
}

#endif