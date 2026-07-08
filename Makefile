CXX ?= g++

CXXFLAGS ?= -std=c++20 -Wall -Wextra -Wpedantic -O2
CPPFLAGS ?= -Iinclude
SERVER_CPPFLAGS ?= -Iboost
SERVER_LDFLAGS ?= -lws2_32 -lmswsock

BUILD_DIR := build
CLI_TARGET := $(BUILD_DIR)/kvcache_cli.exe
SERVER_TARGET := $(BUILD_DIR)/kvcache_server.exe

CLI_SRCS := \
	src/main.cpp \
	src/command/command_executor.cpp \
	src/command/command_table.cpp \
	src/command/line_parser.cpp \
	src/storage/thread_safe_kv_store.cpp

SERVER_SRCS := \
	src/server_main.cpp \
	src/command/command_executor.cpp \
	src/command/command_table.cpp \
	src/command/line_parser.cpp \
	src/concurrency/thread_pool.cpp \
	src/networking/tcp_server.cpp \
	src/networking/tcp_session.cpp \
	src/server/request_dispatcher.cpp \
	src/storage/thread_safe_kv_store.cpp

CLI_OBJS := $(CLI_SRCS:src/%.cpp=$(BUILD_DIR)/%.o)
SERVER_OBJS := $(SERVER_SRCS:src/%.cpp=$(BUILD_DIR)/%.o)

.PHONY: all build build-all build-cli build-server run run-cli run-server clean

all: build

build: build-cli

build-all: build-cli build-server

build-cli: $(CLI_TARGET)

build-server: $(SERVER_TARGET)

$(CLI_TARGET): $(CLI_OBJS)
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(CLI_OBJS) -o $(CLI_TARGET)

$(SERVER_TARGET): $(SERVER_OBJS)
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(SERVER_CPPFLAGS) $(SERVER_OBJS) -o $(SERVER_TARGET) $(SERVER_LDFLAGS)

$(BUILD_DIR)/%.o: src/%.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(SERVER_CPPFLAGS) -c $< -o $@

run: run-cli

run-cli: $(CLI_TARGET)
	$(CLI_TARGET)

run-server: $(SERVER_TARGET)
	$(SERVER_TARGET)

clean:
	@if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"
