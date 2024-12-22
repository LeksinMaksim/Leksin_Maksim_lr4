#ifndef TERMINAL_RENDERER_H
#define TERMINAL_RENDERER_H

#include "game_display.h"
#include "game_field.h"
#include "ability_manager.h"

class TerminalRenderer : public IGameRenderer {
public:
    void renderUserField(const GameField& field) override;
    void renderComputerField(const GameField& field) override;
    void renderAbilityStatus(const AbilityManager& manager) override;
    void renderGameStatus(const Game& game) override;
    void renderStartMenu() override;
    void renderShipPlacement(int shipLength, int shipNumber) override;
    void renderAttackResult(int x, int y, bool isHit, bool isSunk, bool isComputerAttack);

private:
    void renderField(const GameField& field, bool isComputer) const;
    char getFieldSymbol(const GameField& field, int x, int y, bool isComputer) const;
    void renderHeader() const;
    void renderInstructions() const;
};

#endif