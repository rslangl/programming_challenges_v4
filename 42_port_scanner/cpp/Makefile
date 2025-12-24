CC=clang++
CFLAGS= -std=c++23 -fdiagnostics-color=always -g -Wall
BIN=portscanner

.PHONY: clean

clean:
	$(RM) $(BIN)

build: main.cpp
	$(CC) $(CFLAGS) main.cpp -o $(BIN)
