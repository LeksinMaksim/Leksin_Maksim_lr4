#ifndef TERMINAL_INPUT_H
#define TERMINAL_INPUT_H

#include <map>
#include <string>
#include "game_controller.h"

class TerminalInputProcessor {
public:
    explicit TerminalInputProcessor(const std::string& configFile = "keymap.txt");
    Command getCommand();
    const std::map<char, Command>& getKeymap() const;
    std::string getCommandDescription(char key) const;

private:
    std::map<char, Command> keymap;
    std::map<Command, bool> usedCommands;

    void loadDefaultKeymap();
    void loadKeymap(const std::string& filename);
    void validateKeymap();
    Command stringToCommand(const std::string& str);
    std::string commandToString(Command cmd) const;
};

#endif
