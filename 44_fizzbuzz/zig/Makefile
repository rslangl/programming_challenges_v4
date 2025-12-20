BIN=fizzbuzz

.PHONY: clean

clean:
	rm -f $(BIN) $(BIN).o

build: main.zig
	zig build-exe main.zig --name $(BIN)
