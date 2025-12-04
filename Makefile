CXX := g++
CXXFLAGS := -std=c++20 -O2 -Wall -Wextra -Werror -pedantic
LDFLAGS :=

TARGET := ims_traffic
SRC_DIR := src
OBJ_DIR := obj

# Source files (exclude utils.cpp)
SRCS := \
	$(SRC_DIR)/main.cpp \
	$(SRC_DIR)/state.cpp \
	$(SRC_DIR)/lane_change.cpp \
	$(SRC_DIR)/velocity.cpp \
	$(SRC_DIR)/simulation.cpp \
	$(SRC_DIR)/data_gathering.cpp
# Object files in obj/
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: all clean run dirs

all: dirs $(TARGET)

dirs:
	mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files into obj/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)