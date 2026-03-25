# Compiler and flags
CC      := clang
CFLAGS  := -Wall -Wextra -Werror
# Use pkg-config for Raylib plus explicitly add math
LDFLAGS := $(shell pkg-config --cflags --libs raylib) -lm 

TARGET  := first_try.out
SRC     := first_try.c

# Default target
all: $(TARGET)

# Build rule
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	rm -f $(TARGET)

# Force full rebuild
rebuild: clean all

# These are not real files
.PHONY: all run clean rebuild