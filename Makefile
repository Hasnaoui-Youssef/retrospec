# Name of the final executable
TARGET = dbms_re

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -O3 $(shell mysql_config --cflags)

# Directories
SRC_DIR = src
BUILD_DIR = build
QUERIES_DIR = queries
BUILD_QUERIES_DIR = $(BUILD_DIR)/queries

# Source and object files
SRCS = $(shell find $(SRC_DIR) -name "*.cpp")
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Linker flags
LDFLAGS = $(shell mysql_config --libs)

# Default target
all: $(TARGET) copy_queries

copy_queries:
	@mkdir -p $(BUILD_DIR)
	cp -r $(QUERIES_DIR) $(BUILD_DIR)

# Link the executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean copy_queries

