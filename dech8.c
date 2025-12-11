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

int load_rom(uint8_t* dest, const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        return -1;
    }
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    size_t read_bytes = fread(dest, 1, file_size, file);
    printf("Read %zu bytes\n", read_bytes);
    fclose(file);
	return read_bytes;
}

void disassemble_next(Disassembler* dis) {
    uint8_t* instr_ptr = &dis->rom[dis->pc];
    uint8_t upper = instr_ptr[0];
    uint8_t lower = instr_ptr[1];
    uint8_t opcode = upper >> 4;
    uint16_t instruction = upper << 8 | lower;
	char buffer[50];
	chip8_disassemble(instruction, buffer);
	// TODO: Detect sprites and display them.
	printf("%04x | %04x\t%s\n", dis->pc + 0x200, instruction, buffer);
}

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Chip-8 disassembler\n");
		printf("Usage: dech8 <filename>\n");
		return EXIT_FAILURE;
	}

	const char* rom_file_path = argv[1];

	Disassembler* dis = disassembler_create();
	int read_bytes = load_rom(dis->rom, rom_file_path);
	if (!read_bytes) {
		printf("Unable to load ROM at %s\n", rom_file_path);
		return EXIT_FAILURE;
	}

	for (int i = 0; i < read_bytes / 2; i++) {
		disassemble_next(dis);
		dis->pc += 2;
	}
	
	disassembler_destroy(dis);

	return EXIT_SUCCESS;
}
