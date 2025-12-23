CC=g++
CFLAGS= -std=c++20 -g -Wall
BIN=portscanner

.PHONY: clean

clean:
	$(RM) $(BIN)

build: main.cpp
	$(CC) $(CFLAGS) main.cpp -o $(BIN)
