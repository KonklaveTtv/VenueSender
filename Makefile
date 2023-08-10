# Makefile for venuesender project

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -Isrc/include -Isrc -L/usr/lib
INCLUDES = -Isrc/include -Isrc

# Directories
SRCDIR = src
TESTDIR = src/test
OBJDIR = obj
BINDIR = bin

# Targets
TARGET = $(BINDIR)/venuesender
TEST_TARGET = $(BINDIR)/test_runner

# Source files
SRCS = $(wildcard $(SRCDIR)/*.cpp)
TEST_SRCS = $(wildcard $(TESTDIR)/*.cpp)

# Object files
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))
TEST_OBJS = $(patsubst $(TESTDIR)/%.cpp, $(OBJDIR)/%.o, $(TEST_SRCS))

# Libraries
LIBS = -lcurl -ljsoncpp -lsodium

# Catch2 library
CATCH2_LIB = catch2

# Compile rules
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/%.o: $(TESTDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

# Link the main executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

# Build the test objects
test: directories $(TEST_OBJS)

# Link the test runner
$(TEST_TARGET): $(TEST_OBJS) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(TEST_OBJS) -l$(CATCH2_LIB) $(LIBS)

# Run tests
run_tests: test $(TEST_TARGET)
	$(TEST_TARGET)

# Clean up
clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all directories clean test run_tests
