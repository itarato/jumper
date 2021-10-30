LIBS=-lraylib -lGL -lm -lpthread -ldl -lrt -lX11

CXXFLAGS=-std=c++2a -Wall -pedantic -Wformat

BIN=jumper

SRC=$(wildcard src/*.cpp)
SRC:=$(filter-out src/level_editor.cpp, $(SRC))

OBJ=$(addsuffix .o,$(basename $(SRC)))

all: executable

debug: CXXFLAGS += -DDEBUG -g
debug: executable

fs: CXXFLAGS += -DFULLSCREEN
fs: executable

executable: $(OBJ)
	$(CXX) -o $(BIN) $^ $(CXXFLAGS) $(LIBS)

# Level editor:

LE_BIN = level_editor
LE_SRC = level_editor.cpp
LE_OBJ=$(addsuffix .o,$(basename $(LE_SRC)))

le_executable: $(LE_OBJ)
	$(CXX) -o $(LE_BIN) $^ $(CXXFLAGS) $(LIBS)

le: le_executable

clean:
	rm -f ./src/*.o
	rm -f ./src/*.out
	rm -f ./$(BIN)
	rm -f ./$(LE_BIN)
