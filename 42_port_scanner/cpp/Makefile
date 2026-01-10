# Compiler and flags
CXX := clang++
CXXFLAGS := -std=c++23 -fdiagnostics-color=always -Wall -Wextra -Werror -Iinclude -O2 -MMD -MP
LDFLAGS :=

# Output executable name
BIN := portscanner

# Source files
SRC := main.cpp scanner.cpp utils.cpp

# Object files
OBJS := $(SRC:.cpp=.o)

# Dependency files
DEPS := $(OBJS:.o=.d)

# Default rule
all: $(BIN)

# Link objects to binary
$(BIN): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

# Compile .cpp -> .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean build artifacts
.PHONY: clean
clean:
	$(RM) -f $(OBJS) $(DEPS) $(BIN)

# Include auto-generated dependency files (ignore if missing):
# First build: no `.d` files exist, `-include` is ignored
# Next build: make reads `.d` files, rebuild only objects that depend on it (no header change, no rebuild)
-include $(DEPS)
