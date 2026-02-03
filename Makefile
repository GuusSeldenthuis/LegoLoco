CXX = g++
CXXFLAGS = -std=c++17 -Wall
RAYLIB_DIR = raylib/src
RAYLIB_LIB = $(RAYLIB_DIR)/libraylib.a

INCLUDE = -I$(RAYLIB_DIR)
LDFLAGS = -L$(RAYLIB_DIR) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

TARGET = bin/lego_loco
SRC = src/game.cpp

all: $(TARGET)

$(TARGET): $(SRC) $(RAYLIB_LIB) | bin
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SRC) -o $(TARGET) $(LDFLAGS)

$(RAYLIB_LIB):
	$(MAKE) -C $(RAYLIB_DIR) PLATFORM=PLATFORM_DESKTOP

bin:
	mkdir -p bin

clean:
	rm -f $(TARGET)

clean-all: clean
	$(MAKE) -C $(RAYLIB_DIR) clean

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean clean-all run
