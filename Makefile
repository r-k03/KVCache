CXX ?= g++

CXXFLAGS ?= -std=c++20 -Wall -Wextra -Wpedantic -O2
CPPFLAGS ?= -Iinclude

BUILD_DIR := build
TARGET := $(BUILD_DIR)/kvcache_cli.exe

CLI_SRCS := \
	src/main.cpp \
	src/command/command_executor.cpp \
	src/command/command_table.cpp \
	src/command/line_parser.cpp

CLI_OBJS := $(CLI_SRCS:src/%.cpp=$(BUILD_DIR)/%.o)

.PHONY: all build run clean

all: build

build: $(TARGET)

$(TARGET): $(CLI_OBJS)
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(CLI_OBJS) -o $(TARGET)

$(BUILD_DIR)/%.o: src/%.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

run: $(TARGET)
	$(TARGET)

clean:
	@if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"
