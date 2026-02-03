CXX = g++
CXXFLAGS = -std=c++17 -Wall
RAYLIB_DIR = raylib/src
RAYLIB_LIB = $(RAYLIB_DIR)/libraylib.a

SRC_DIR = src
INCLUDE = -I$(RAYLIB_DIR) -I$(SRC_DIR)
LDFLAGS = -L$(RAYLIB_DIR) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

TARGET = bin/lego_loco
SRCS = $(SRC_DIR)/game.cpp $(SRC_DIR)/Tile.cpp $(SRC_DIR)/World.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS) $(RAYLIB_LIB) | bin
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(RAYLIB_LIB):
	$(MAKE) -C $(RAYLIB_DIR) PLATFORM=PLATFORM_DESKTOP

bin:
	mkdir -p bin

clean:
	rm -f $(TARGET) $(OBJS)

clean-all: clean
	$(MAKE) -C $(RAYLIB_DIR) clean

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean clean-all run
