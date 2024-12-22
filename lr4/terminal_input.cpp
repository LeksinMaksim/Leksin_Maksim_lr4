#include "terminal_input.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

TerminalInputProcessor::TerminalInputProcessor(const std::string& configFile) {
    loadDefaultKeymap();
    try {
        loadKeymap(configFile);
    } catch (const std::exception& e) {
        std::cerr << "Warning: Could not load keymap from file: " << e.what() << "\n";
        std::cerr << "Using default keymap instead.\n";
    }
    validateKeymap();
}

Command TerminalInputProcessor::getCommand() {
    char input;
    std::cout << "Введите команду: ";
    std::cin >> input;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    input = std::tolower(static_cast<unsigned char>(input));

    auto it = keymap.find(input);
    if (it != keymap.end()) {
        return it->second;
    }
    
    std::cout << "Неизвестная команда. Используйте:\n";
    for (const auto& [key, cmd] : keymap) {
        std::cout << key << " - " << commandToString(cmd) << "\n";
    }
    return Command::Invalid;
}

const std::map<char, Command>& TerminalInputProcessor::getKeymap() const {
    return keymap;
}

std::string TerminalInputProcessor::getCommandDescription(char key) const {
    auto it = keymap.find(key);
    if (it != keymap.end()) {
        return commandToString(it->second);
    }
    return "Неизвестная команда";
}

void TerminalInputProcessor::loadDefaultKeymap() {
    keymap = {
        {'a', Command::Attack},
        {'s', Command::UseAbility},
        {'v', Command::SaveGame},
        {'l', Command::LoadGame},
        {'d', Command::DisplayFields},
        {'q', Command::Quit}
    };
}

void TerminalInputProcessor::loadKeymap(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open keymap file: " + filename);
    }

    keymap.clear();
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        if (line.empty() || line[0] == '#') continue;

        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            std::cerr << "Warning: Invalid format in line " << lineNumber << ": " << line << "\n";
            continue;
        }

        std::string keyStr = line.substr(0, pos);
        std::string commandStr = line.substr(pos + 1);

        keyStr.erase(std::remove_if(keyStr.begin(), keyStr.end(), ::isspace), keyStr.end());
        commandStr.erase(std::remove_if(commandStr.begin(), commandStr.end(), ::isspace), commandStr.end());

        if (keyStr.length() != 1) {
            std::cerr << "Warning: Invalid key in line " << lineNumber << ": " << keyStr << "\n";
            continue;
        }

        char key = std::tolower(static_cast<unsigned char>(keyStr[0]));
        Command cmd = stringToCommand(commandStr);
        
        if (cmd != Command::Invalid) {
            keymap[key] = cmd;
        } else {
            std::cerr << "Warning: Invalid command in line " << lineNumber << ": " << commandStr << "\n";
        }
    }
}

void TerminalInputProcessor::validateKeymap() {
    std::map<Command, bool> commandCovered;
    for (const auto& [key, cmd] : keymap) {
        if (commandCovered[cmd]) {
            throw std::runtime_error("Duplicate command mapping detected for command: " + 
                                   commandToString(cmd));
        }
        commandCovered[cmd] = true;
    }

    std::vector<Command> requiredCommands = {
        Command::Attack,
        Command::UseAbility,
        Command::SaveGame,
        Command::LoadGame,
        Command::DisplayFields,
        Command::Quit
    };

    for (Command cmd : requiredCommands) {
        if (!commandCovered[cmd]) {
            throw std::runtime_error("Missing required command: " + commandToString(cmd));
        }
    }

    std::map<char, Command> keyUsage;
    for (const auto& [key, cmd] : keymap) {
        if (keyUsage.find(key) != keyUsage.end()) {
            throw std::runtime_error("Multiple commands mapped to key: " + std::string(1, key));
        }
        keyUsage[key] = cmd;
    }
}

Command TerminalInputProcessor::stringToCommand(const std::string& str) {
    std::string upperStr = str;
    std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);

    if (upperStr == "ATTACK") return Command::Attack;
    if (upperStr == "ABILITY") return Command::UseAbility;
    if (upperStr == "SAVE") return Command::SaveGame;
    if (upperStr == "LOAD") return Command::LoadGame;
    if (upperStr == "DISPLAY") return Command::DisplayFields;
    if (upperStr == "QUIT") return Command::Quit;
    return Command::Invalid;
}

std::string TerminalInputProcessor::commandToString(Command cmd) const {
    switch (cmd) {
        case Command::Attack:
            return "attack";
        case Command::UseAbility:
            return "ability";
        case Command::SaveGame:
            return "save";
        case Command::LoadGame:
            return "load";
        case Command::DisplayFields:
            return "display";
        case Command::Quit:
            return "quit";
        default:
            return "INVALID";
    }
}