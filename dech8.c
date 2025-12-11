#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "disassembly.h"

const int MEMORY_SIZE = 4096;

typedef struct {
	unsigned int pc;
	uint8_t* rom;
} Disassembler;

Disassembler* disassembler_create() {
	Disassembler* dis = malloc(sizeof(Disassembler));
	dis->rom = calloc(1, MEMORY_SIZE);
	dis->pc = 0;
	return dis;
}

void disassembler_destroy(Disassembler* dis) {
	free(dis->rom);
	free(dis);
}


bool load_rom(uint8_t* dest, const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        return false;
    }
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    size_t read_bytes = fread(dest, 1, file_size, file);
    printf("Read %zu bytes\n", read_bytes);
    fclose(file);
	return true;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Chip-8 disassembler\n");
		printf("Usage: dech8 <filename>\n");
		return EXIT_FAILURE;
	}

	const char* rom_file_path = argv[1];

	Disassembler* dis = disassembler_create();
	if (!load_rom(dis->rom, rom_file_path)) {
		printf("Unable to load ROM at %s\n", rom_file_path);
		return EXIT_FAILURE;
	}

	printf("%04x\n", NNN(0x1234));
	
	disassembler_destroy(dis);

	return EXIT_SUCCESS;
}
