OUT_DIR=bin
OUT=$(OUT_DIR)/chip8

STD=c99

all: $(OUT)

$(OUT): chip8.c dech8.c | $(OUT_DIR)
	cc -std=$(STD) chip8.c $$(pkg-config --libs --cflags raylib) -o $(OUT)
	cc -std=$(STD) dech8.c disassembly.c -o $(OUT_DIR)/dech8

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

clean:
	rm -rf $(OUT_DIR)

.PHONY: all clean
