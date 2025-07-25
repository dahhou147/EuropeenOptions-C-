# Makefile pour projet QuantLib C++ simple
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
INCLUDE = -I$(CONDA_PREFIX)/include
LIBS = -L$(CONDA_PREFIX)/lib -lQuantLib
SRC = src/pricer.cpp
TARGET = bin/main

all: $(TARGET)

$(TARGET): $(SRC) | bin
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -o $@ $(LIBS)

bin:
	mkdir -p bin

clean:
	rm -rf bin

run: $(TARGET)
	LD_LIBRARY_PATH=$(CONDA_PREFIX)/lib ./$(TARGET)

.PHONY: all clean run
