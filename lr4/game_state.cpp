#include "game_state.h"
#include "game_field.h"
#include "ability_manager.h"
#include "ship_manager.h"
#include "ship.h"
#include "double_damage_ability.h"
#include "scanner_ability.h"
#include "barrage_ability.h"
#include <iostream>

std::ostream& operator<<(std::ostream& os, const GameState& state) {
    os << GameField::DEFAULT_WIDTH << ' ' << GameField::DEFAULT_HEIGHT << '\n';
    
    GameState::writeFieldData(os, state.userFieldData);
    GameState::writeFieldData(os, state.computerFieldData);
    GameState::writeAbilitiesData(os, state.abilitiesData);
    
    return os;
}

std::istream& operator>>(std::istream& is, GameState& state) {
    int width, height;
    if (!(is >> width >> height)) {
        throw std::runtime_error("Error reading dimensions");
    }
    
    if (width != GameField::DEFAULT_WIDTH || height != GameField::DEFAULT_HEIGHT) {
        throw std::runtime_error("Invalid dimensions in save file");
    }
    
    state.userFieldData = GameState::readFieldData(is);
    state.computerFieldData = GameState::readFieldData(is);
    state.abilitiesData = GameState::readAbilitiesData(is);
    
    return is;
}

void GameState::saveToFile(const std::string& filename,
                          const GameField& userField,
                          const GameField& computerField,
                          const AbilityManager& abilities) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Cannot open file for saving: " + filename);
    }

    try {
        GameState state;
        state.userFieldData = serializeField(userField);
        state.computerFieldData = serializeField(computerField);
        state.abilitiesData = serializeAbilities(abilities);

        out << state;

        out.flush();
        if (!out) {
            throw std::runtime_error("Error writing to file");
        }
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Error saving game state: ") + e.what());
    }
}

void GameState::loadFromFile(const std::string& filename,
                           GameField& userField,
                           GameField& computerField,
                           ShipManager& userShips,
                           ShipManager& computerShips,
                           AbilityManager& abilities) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Cannot open file for loading: " + filename);
    }

    try {
        GameState state;
        in >> state;

        deserializeField(userField, state.userFieldData, userShips);
        deserializeField(computerField, state.computerFieldData, computerShips);
        deserializeAbilities(abilities, state.abilitiesData);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Error loading game state: ") + e.what());
    }
}

FieldData GameState::serializeField(const GameField& field) {
    FieldData data;
    data.cells.resize(GameField::DEFAULT_HEIGHT, 
                     std::vector<CellStatus>(GameField::DEFAULT_WIDTH));

    for (int y = 0; y < GameField::DEFAULT_HEIGHT; ++y) {
        for (int x = 0; x < GameField::DEFAULT_WIDTH; ++x) {
            data.cells[y][x] = field.getCellStatus(x, y);
        }
    }

    for (int y = 0; y < GameField::DEFAULT_HEIGHT; ++y) {
        for (int x = 0; x < GameField::DEFAULT_WIDTH; ++x) {
            Ship* ship = field.getShipAt(x, y);
            if (ship && field.getSegmentIndexAt(x, y) == 0) {
                ShipData shipData;
                shipData.length = ship->getLength();
                shipData.orientation = ship->getOrientation();
                shipData.segmentStatus.clear();
                
                int dx = (shipData.orientation == Orientation::Horizontal) ? 1 : 0;
                int dy = (shipData.orientation == Orientation::Vertical) ? 1 : 0;
                
                for (int i = 0; i < ship->getLength(); ++i) {
                    int nx = x + i * dx;
                    int ny = y + i * dy;
                    if (field.isValidPosition(nx, ny)) {
                        shipData.segmentStatus.push_back(ship->getSegmentStatus(i));
                    }
                }

                data.ships.push_back(shipData);
                data.shipPositions.emplace_back(x, y);
                data.shipOrientations.push_back(ship->getOrientation());
            }
        }
    }
    
    return data;
}

void GameState::deserializeField(GameField& field, const FieldData& data, ShipManager& shipManager) {
    field = GameField();

    std::vector<size_t> shipIndices(data.ships.size());
    for (size_t i = 0; i < shipIndices.size(); ++i) {
        shipIndices[i] = i;
    }

    std::sort(shipIndices.begin(), shipIndices.end(),
        [&data](size_t a, size_t b) {
            return data.ships[a].length > data.ships[b].length;
        });

    std::vector<bool> shipUsed(shipManager.getShipCount(), false);

    for (size_t idx : shipIndices) {
        const ShipData& shipData = data.ships[idx];
        
        Ship* ship = nullptr;
        size_t shipIndex = 0;
        
        for (size_t j = 0; j < shipManager.getShipCount(); ++j) {
            if (!shipUsed[j] && shipManager.getShip(j)->getLength() == shipData.length) {
                ship = shipManager.getShip(j);
                shipIndex = j;
                break;
            }
        }

        if (!ship) {
            std::cerr << "Не найден подходящий корабль для восстановления\n";
            continue;
        }

        try {
            int x = data.shipPositions[idx].first;
            int y = data.shipPositions[idx].second;
            Orientation orientation = data.shipOrientations[idx];

            ship->setOrientation(orientation);

            field.placeShip(ship, x, y, orientation);
            shipUsed[shipIndex] = true;

            for (int j = 0; j < ship->getLength(); ++j) {
                if (j < static_cast<int>(shipData.segmentStatus.size())) {
                    SegmentStatus savedStatus = shipData.segmentStatus[j];
                    switch(savedStatus) {
                        case SegmentStatus::Damaged:
                            ship->applyDamage(j, 1);
                            break;
                        case SegmentStatus::Destroyed:
                            ship->applyDamage(j, 2);
                            break;
                        default:
                            break;
                    }
                }
            }

            shipManager.updateShip(ship);

        } catch (const std::exception& e) {
            std::cerr << "Ошибка восстановления корабля: " << e.what() << "\n";
        }
    }

    for (int y = 0; y < GameField::DEFAULT_HEIGHT; ++y) {
        for (int x = 0; x < GameField::DEFAULT_WIDTH; ++x) {
            if (field.getShipAt(x, y) == nullptr && 
                y < static_cast<int>(data.cells.size()) && 
                x < static_cast<int>(data.cells[y].size())) {
                field.setCellStatus(x, y, data.cells[y][x]);
            }
        }
    }
}

AbilitiesData GameState::serializeAbilities(const AbilityManager& manager) {
    AbilitiesData data;
    std::vector<int> counts = manager.getAbilityCounts();
    data.doubleDamageCount = counts[0];
    data.scannerCount = counts[1];
    data.barrageCount = counts[2];
    return data;
}

void GameState::deserializeAbilities(AbilityManager& manager, const AbilitiesData& data) {
    std::vector<int> counts = {
        data.doubleDamageCount,
        data.scannerCount,
        data.barrageCount
    };
    manager.setAbilitiesFromCounts(counts);
}

void GameState::writeFieldData(std::ostream& out, const FieldData& data) {
    for (const auto& row : data.cells) {
        for (const auto& cell : row) {
            out << static_cast<int>(cell) << ' ';
        }
        out << '\n';
    }
    
    out << data.ships.size() << '\n';
    
    for (size_t i = 0; i < data.ships.size(); ++i) {
        const auto& ship = data.ships[i];
        out << ship.length << ' '
            << static_cast<int>(ship.orientation) << ' '
            << data.shipPositions[i].first << ' '
            << data.shipPositions[i].second << ' ';
            
        out << ship.segmentStatus.size() << ' '; 
        for (const auto& status : ship.segmentStatus) {
            out << static_cast<int>(status) << ' ';
        }
        out << '\n';
    }
}

FieldData GameState::readFieldData(std::istream& in) {
    FieldData data;
    
    data.cells.resize(GameField::DEFAULT_HEIGHT, 
                     std::vector<CellStatus>(GameField::DEFAULT_WIDTH));
                     
    for (auto& row : data.cells) {
        for (auto& cell : row) {
            int status;
            if (!(in >> status)) {
                throw std::runtime_error("Error reading cell status");
            }
            cell = static_cast<CellStatus>(status);
        }
    }
    
    size_t shipCount;
    if (!(in >> shipCount)) {
        throw std::runtime_error("Error reading ship count");
    }
    
    for (size_t i = 0; i < shipCount; ++i) {
        ShipData shipData;
        int x, y, orientation, segmentCount;
        
        if (!(in >> shipData.length >> orientation >> x >> y >> segmentCount)) {
            throw std::runtime_error("Error reading ship data");
        }
        
        shipData.orientation = static_cast<Orientation>(orientation);
        data.shipPositions.emplace_back(x, y);
        data.shipOrientations.push_back(shipData.orientation);
        
        for (int j = 0; j < segmentCount; ++j) {
            int status;
            if (!(in >> status)) {
                throw std::runtime_error("Error reading ship segment status");
            }
            shipData.segmentStatus.push_back(static_cast<SegmentStatus>(status));
        }
        
        data.ships.push_back(shipData);
    }
    
    return data;
}

void GameState::writeAbilitiesData(std::ostream& out, const AbilitiesData& data) {
    out << data.doubleDamageCount << ' '
        << data.scannerCount << ' '
        << data.barrageCount << '\n';
}

AbilitiesData GameState::readAbilitiesData(std::istream& in) {
    AbilitiesData data;
    if (!(in >> data.doubleDamageCount 
           >> data.scannerCount 
           >> data.barrageCount)) {
        throw std::runtime_error("Error reading abilities data");
    }
    return data;
}

