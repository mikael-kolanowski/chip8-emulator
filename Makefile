OUT=chip8

chip8: chip8.c
	cc chip8.c $$(pkg-config --libs --cflags raylib) -o $(OUT)
