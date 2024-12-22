#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "game_field.h"
#include "ship_manager.h"
#include "ability_manager.h"

struct ShipData {
    int length;
    Orientation orientation;
    std::vector<SegmentStatus> segmentStatus;
};

struct FieldData {
    std::vector<std::vector<CellStatus>> cells;
    std::vector<ShipData> ships;
    std::vector<std::pair<int, int>> shipPositions;
    std::vector<Orientation> shipOrientations;
};

struct AbilitiesData {
    int doubleDamageCount;
    int scannerCount;
    int barrageCount;
};

class GameState {
public:
    friend std::ostream& operator<<(std::ostream& os, const GameState& state);
    friend std::istream& operator>>(std::istream& is, GameState& state);

    static void saveToFile(const std::string& filename, 
                          const GameField& userField, 
                          const GameField& computerField,

                          const AbilityManager& abilities);

    static void loadFromFile(const std::string& filename,
                           GameField& userField,
                           GameField& computerField,
                           ShipManager& userShips,
                           ShipManager& computerShips,
                           AbilityManager& abilities);

private:
    FieldData userFieldData;
    FieldData computerFieldData;
    AbilitiesData abilitiesData;

    static FieldData serializeField(const GameField& field);
    static void deserializeField(GameField& field, const FieldData& data, ShipManager& shipManager);
    static AbilitiesData serializeAbilities(const AbilityManager& manager);
    static void deserializeAbilities(AbilityManager& manager, const AbilitiesData& data);
    
    static void writeFieldData(std::ostream& out, const FieldData& data);
    static FieldData readFieldData(std::istream& in);
    static void writeAbilitiesData(std::ostream& out, const AbilitiesData& data);
    static AbilitiesData readAbilitiesData(std::istream& in);
};

#endif