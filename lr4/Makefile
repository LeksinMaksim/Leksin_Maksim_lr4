CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
LDFLAGS =

# Directories
SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Target executable
TARGET = $(BIN_DIR)/battleship

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)

# Link the executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Clean and rebuild
rebuild: clean all

# Prevent make from treating clean and rebuild as file names
.PHONY: all clean rebuild directories

# Dependencies
$(OBJ_DIR)/ability_manager.o: ability_manager.cpp ability_manager.h ability.h barrage_ability.h double_damage_ability.h scanner_ability.h exceptions.h
$(OBJ_DIR)/barrage_ability.o: barrage_ability.cpp barrage_ability.h ability.h game_field.h
$(OBJ_DIR)/double_damage_ability.o: double_damage_ability.cpp double_damage_ability.h ability.h game_field.h
$(OBJ_DIR)/scanner_ability.o: scanner_ability.cpp scanner_ability.h ability.h game_field.h exceptions.h
$(OBJ_DIR)/game_field.o: game_field.cpp game_field.h ship.h ship_manager.h ability_manager.h exceptions.h
$(OBJ_DIR)/ship.o: ship.cpp ship.h
$(OBJ_DIR)/ship_manager.o: ship_manager.cpp ship_manager.h ship.h
$(OBJ_DIR)/main.o: main.cpp ability_manager.h game_field.h ship.h ship_manager.h

# Debug target
debug: CXXFLAGS += -g -DDEBUG
debug: all
