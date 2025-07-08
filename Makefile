# Makefile pour projets C++
# Variables
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
DEBUG_FLAGS = -g -O0 -DDEBUG
RELEASE_FLAGS = -O2 -DNDEBUG

# Dossiers
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Fichiers sources
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Nom de l'exécutable
TARGET = $(BIN_DIR)/main

# Règles par défaut
all: debug

# Compilation en mode debug
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

# Compilation en mode release
release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(TARGET)

# Création des dossiers
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compilation de l'exécutable
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Compilation des fichiers objets
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Formatage du code
format:
	clang-format -i $(SRC_DIR)/*.cpp $(SRC_DIR)/*.h

# Vérification du formatage
check-format:
	clang-format --dry-run --Werror $(SRC_DIR)/*.cpp $(SRC_DIR)/*.h

# Exécution
run: $(TARGET)
	./$(TARGET)

# Installation des dépendances (Ubuntu/Debian)
install-deps:
	sudo apt update
	sudo apt install -y build-essential gdb valgrind

# Installation des dépendances (Fedora)
install-deps-fedora:
	sudo dnf install -y gcc-c++ gdb valgrind

# Installation des dépendances (Arch)
install-deps-arch:
	sudo pacman -S gcc gdb valgrind

.PHONY: all debug release clean run install-deps install-deps-fedora install-deps-arch 