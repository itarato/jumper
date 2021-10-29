LIBS=-lraylib -lGL -lm -lpthread -ldl -lrt -lX11

CXXFLAGS=-std=c++2a -Wall -pedantic -Wformat

BIN=main

SRC=$(wildcard *.cpp)
SRC:=$(filter-out level_editor.cpp, $(SRC))

OBJ=$(addsuffix .o,$(basename $(SRC)))

all: executable

debug: CXXFLAGS += -DDEBUG -g
debug: executable

fs: CXXFLAGS += -DFULLSCREEN
fs: executable

executable: $(OBJ)
	$(CXX) -o $(BIN) $^ $(CXXFLAGS) $(LIBS)

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f ./*.o
	rm -f ./$(BIN)

# Level editor:

LE_BIN = level_editor
LE_SRC = level_editor.cpp
LE_OBJ=$(addsuffix .o,$(basename $(LE_SRC)))

le_executable: $(LE_OBJ)
	$(CXX) -o $(LE_BIN) $^ $(CXXFLAGS) $(LIBS)

le: le_executable
