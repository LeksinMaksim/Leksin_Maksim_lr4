#ifndef GAME_DISPLAY_IMPL_H
#define GAME_DISPLAY_IMPL_H

#include "game_display.h"
#include "game.h"
#include "game_field.h"
#include "ability_manager.h"

template<typename Renderer>
GameDisplay<Renderer>::GameDisplay(std::shared_ptr<Game> game)
    : game_(game), renderer_(std::make_unique<Renderer>()) {
}

template<typename Renderer>
void GameDisplay<Renderer>::onFieldUpdate() {
    if (game_) {
        updateDisplay();
    }
}

template<typename Renderer>
void GameDisplay<Renderer>::onAbilityUsed() {
    if (game_) {
        renderer_->renderAbilityStatus(*game_->getUserAbilityManager());
    }
}

template<typename Renderer>
void GameDisplay<Renderer>::onGameOver() {
    if (game_) {
        renderer_->renderGameStatus(*game_);
    }
}

template<typename Renderer>
void GameDisplay<Renderer>::onShipDestroyed() {
    if (game_) {
        updateDisplay();
    }
}

template<typename Renderer>
void GameDisplay<Renderer>::renderStartMenu() {
    renderer_->renderStartMenu();
}

template<typename Renderer>
void GameDisplay<Renderer>::renderShipPlacement(int shipLength, int shipNumber) {
    renderer_->renderShipPlacement(shipLength, shipNumber);
}

template<typename Renderer>
void GameDisplay<Renderer>::refresh() {
    updateDisplay();
}

template<typename Renderer>
void GameDisplay<Renderer>::updateDisplay() {
    if (!game_) return;

    const GameField& userField = game_->getUserField();
    const GameField& computerField = game_->getComputerField();

    renderer_->renderUserField(userField);
    renderer_->renderComputerField(computerField);
    renderer_->renderAbilityStatus(*game_->getUserAbilityManager());
    renderer_->renderGameStatus(*game_);
}

#endif