# Makefile for Dungeon Run — GLSL raycaster (SDL2 + OpenGL 3.3 + GLEW)

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Iinclude

SDL2_CFLAGS := $(shell pkg-config --cflags sdl2)
SDL2_LIBS   := $(shell pkg-config --libs sdl2) -lGL

SRC := src/main.cpp src/renderer_gl.cpp src/map.cpp src/gl_core.cpp src/raycaster.cpp
OBJ := $(SRC:.cpp=.o)
TARGET := raycaster

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@ $(SDL2_LIBS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(SDL2_CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f src/*.o $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
