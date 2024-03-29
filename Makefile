LIBS=-lraylib -lGL -lm -lpthread -ldl -lrt -lX11

CXXFLAGS=-std=c++2a -Wall -pedantic -Wformat -Werror

BIN=jumper

SRC=$(wildcard src/*.cpp) $(wildcard src/shared/*.cpp)
SRC:=$(filter-out src/level_editor.cpp, $(SRC))

OBJ=$(addsuffix .o,$(basename $(SRC)))

all: executable

debug: CXXFLAGS += -DDEBUG -g
debug: executable

executable: $(OBJ)
	$(CXX) -o $(BIN) $^ $(CXXFLAGS) $(LIBS)

# Level editor:

LE_BIN = level_editor
LE_SRC = src/level_editor.cpp $(wildcard src/shared/*.cpp)

LE_OBJ=$(addsuffix .o,$(basename $(LE_SRC)))

le_executable: $(LE_OBJ)
	$(CXX) -o $(LE_BIN) $^ $(CXXFLAGS) $(LIBS)

le: le_executable

clean:
	rm -f ./src/*.o
	rm -f ./src/*.out
	rm -f ./src/shared/*.o

clean_all:
	rm -f ./src/*.o
	rm -f ./src/*.out
	rm -f ./src/shared/*.o
	rm -f ./$(BIN)
	rm -f ./$(LE_BIN)
