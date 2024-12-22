#include "game.h"
#include "game_controller.h"
#include "game_display.h"
#include "game_display_impl.h"  
#include "terminal_input.h"
#include "terminal_renderer.h"
#include <memory>

int main() {
    auto game = std::make_shared<Game>();
    auto display = std::make_shared<GameDisplay<TerminalRenderer>>(game);
    game->registerObserver(display.get());
    auto handler = std::make_shared<DefaultCommandHandler>(display);
    GameController<TerminalInputProcessor> controller(game, handler);
    controller.run();
    
    return 0;
}