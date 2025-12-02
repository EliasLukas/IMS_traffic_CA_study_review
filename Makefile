CXX := g++
CXXFLAGS := -std=c++20 -O2 -Wall -Wextra -Werror -pedantic
LDFLAGS :=

TARGET := ims_traffic

# Source files (exclude utils.cpp)
SRCS := \
  main.cpp \
  state.cpp \
  lane_change.cpp \
  velocity.cpp \
  simulation.cpp

# Object files
OBJS := $(SRCS:.cpp=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
