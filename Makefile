# Makefile pour le projet de calibration Heston
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2
INCLUDE = -I/usr/include
INCLUDE += -Isrc
LIBS = -lnlopt -lm

# Vérifier si nlopt est installé
NLOPT_CHECK := $(shell pkg-config --exists nlopt && echo "yes" || echo "no")

ifeq ($(NLOPT_CHECK),yes)
    # Utiliser pkg-config si disponible
    CXXFLAGS += $(shell pkg-config --cflags nlopt)
    LIBS = $(shell pkg-config --libs nlopt) -lm
else
    # Fallback vers les chemins par défaut
    LIBS = -lnlopt -lm
endif

CURL_CHECK := $(shell pkg-config --exists libcurl && echo "yes" || echo "no")
JSONCPP_CHECK := $(shell pkg-config --exists jsoncpp && echo "yes" || echo "no")

ifeq ($(CURL_CHECK),yes)
  CXXFLAGS += $(shell pkg-config --cflags libcurl)
  LIBS += $(shell pkg-config --libs libcurl)
else
  LIBS += -lcurl
endif

ifeq ($(JSONCPP_CHECK),yes)
  CXXFLAGS += $(shell pkg-config --cflags jsoncpp)
  LIBS += $(shell pkg-config --libs jsoncpp)
else
  INCLUDE += -I/usr/include/jsoncpp
  LIBS += -ljsoncpp
endif

# Ajout des flags MongoDB
CXXFLAGS += -I/usr/local/include/mongocxx/v_noabi -I/usr/local/include/bsoncxx/v_noabi
LIBS += -L/usr/local/lib -lmongocxx -lbsoncxx

SRC = src/calibrator.cpp src/main.cpp src/fetchData.cpp
TARGET = heston_calibrator

all: $(TARGET)

$(TARGET): $(SRC)
	@echo "Compilation avec les flags: $(CXXFLAGS)"
	@echo "Libraries: $(LIBS)"
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SRC) -o $@ $(LIBS) -Wl,-rpath,/usr/local/lib

# Règle pour installer nlopt si nécessaire
install-nlopt:
	@echo "Installation de nlopt..."
	@if command -v apt-get >/dev/null 2>&1; then \
		sudo apt-get update && sudo apt-get install -y libnlopt-dev; \
	elif command -v yum >/dev/null 2>&1; then \
		sudo yum install -y nlopt-devel; \
	elif command -v brew >/dev/null 2>&1; then \
		brew install nlopt; \
	else \
		echo "Gestionnaire de paquets non reconnu. Veuillez installer nlopt manuellement."; \
	fi

# Règle pour vérifier les dépendances
check-deps:
	@echo "Vérification des dépendances..."
	@if pkg-config --exists nlopt; then \
		echo "✓ nlopt trouvé via pkg-config"; \
		echo "  Version: $(shell pkg-config --modversion nlopt)"; \
		echo "  Cflags: $(shell pkg-config --cflags nlopt)"; \
		echo "  Libs: $(shell pkg-config --libs nlopt)"; \
	else \
		echo "✗ nlopt non trouvé via pkg-config"; \
		echo "  Tentative de compilation avec les chemins par défaut..."; \
	fi

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

test-compile: $(TARGET)
	@echo "✓ Compilation réussie!"
	@echo "Exécutable créé: $(TARGET)"

test-mongo: $(TARGET)
	@echo "✓ Test MongoDB compilé"
	./$(TARGET)

.PHONY: all install-nlopt check-deps clean run test-compile
