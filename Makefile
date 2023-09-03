# Makefile for venuesender project

# Detect the OS type
UNAME_S := $(shell uname -s)

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -Wunused-parameter -Isrc/include -Isrc -I/path/to/boost/include -L/usr/lib -L/usr/local/lib
INCLUDES = -Isrc/include -Isrc

# Directories
SRCDIR = src
TESTDIR = src/test
OBJDIR = obj
DEBUGOBJDIR = obj/debug
BINDIR = bin
DEBUGBINDIR = bin/debug

# Targets
TARGET = $(BINDIR)/venuesender
DEBUG_TARGET = $(DEBUGBINDIR)/venuesender_debug
TEST_TARGET = $(BINDIR)/venuesender_test

# Source files
SRCS = $(wildcard $(SRCDIR)/*.cpp)
TEST_SRCS = $(wildcard $(TESTDIR)/*.cpp)

# Object files
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))
DEBUG_OBJS = $(patsubst $(SRCDIR)/%.cpp, $(DEBUGOBJDIR)/%.o, $(SRCS))
TEST_OBJS = $(patsubst $(TESTDIR)/%.cpp, $(OBJDIR)/%.o, $(TEST_SRCS))
TEST_MAIN_OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/test_%.o, $(SRCS))

# Libraries common to all platforms
LIBS = -lcurl -ljsoncpp -lssl -lcrypto -lsqlite3 -lboost_system -lboost_filesystem -lboost_iostreams -lboost_regex

# Add X11 library if on Linux
ifeq ($(UNAME_S),Linux)
    LIBS += -lX11
endif

# Add platform-specific libraries for macOS
ifeq ($(UNAME_S),Darwin)
    LIBS += -framework Cocoa
endif

# Add platform-specific libraries for Windows
ifeq ($(UNAME_S),Windows_NT)
    LIBS += -luser32 -lgdi32
endif

# Compile rules
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(DEBUGOBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/test_%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -DUNIT_TESTING -g $(INCLUDES) -c $< -o $@

$(OBJDIR)/%.o: $(TESTDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -DUNIT_TESTING -g $(INCLUDES) -c $< -o $@

# Default target
all: directories $(TARGET)

# Debug target
debug: CXXFLAGS += -g
debug: directories $(DEBUG_TARGET)

# Test target
test: directories $(TEST_TARGET)

# Create necessary directories
directories:
	mkdir -p $(OBJDIR)
	mkdir -p $(DEBUGOBJDIR)
	mkdir -p $(BINDIR)
	mkdir -p $(DEBUGBINDIR)

# Link the main executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

# Link the debug executable
$(DEBUG_TARGET): $(DEBUG_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(DEBUG_OBJS) $(LIBS)

# Link the venuesender_test
$(TEST_TARGET): $(TEST_OBJS) $(TEST_MAIN_OBJS)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(TEST_OBJS) $(filter-out $(OBJDIR)/test_main.o, $(TEST_MAIN_OBJS)) $(LIBS)

# Run tests
run_tests: test $(TEST_TARGET)
	$(TEST_TARGET)

# Clean up
clean:
	rm -rf $(OBJDIR) $(DEBUGOBJDIR) $(BINDIR) $(DEBUGBINDIR) test_output.txt

.PHONY: all debug directories clean test run_tests
