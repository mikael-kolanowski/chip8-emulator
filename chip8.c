#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define X(instr) (((instr)&0x0f00) >> 8)
#define Y(instr) (((instr)&0x00f0) >> 4)
#define Z(instr) (((instr)&0x000f) >> 4)
#define NN(instr) (((instr)&0x00ff))
#define NNN(instr) (((instr)&0x0fff))

typedef struct Rom {
    size_t size;
    uint8_t* data;
} Rom;

typedef struct {
    Rom* rom;
    unsigned int pc;
    uint8_t regs[16];
    uint16_t I;
    unsigned int sp;
    uint8_t* display;
    uint8_t* memory;
} Chip8;

Chip8* chip8_init() {
    Chip8* cpu = malloc(sizeof(Chip8));
    cpu->memory = calloc(1, 1024);
    cpu->pc = 0x200;
    cpu->sp = 0xFA0;

    cpu->display = &cpu->memory[0xF00];

    return cpu;
}

void chip8_dealloc(Chip8* cpu) {
    free(cpu->rom->data);
    free(cpu->rom);
    free(cpu->memory);
    free(cpu);
}

void chip8_inc_pc(Chip8* cpu) {
    cpu->pc += 2;
}

void chip8_cycle(Chip8* cpu) {
    uint8_t* instr_ptr = &cpu->rom->data[cpu->pc];
    uint8_t upper = instr_ptr[0];
    uint8_t lower = instr_ptr[1];
    uint8_t opcode = upper >> 4;
    uint16_t instruction = upper << 8 | lower;
    printf("instr=%04X\n", instruction);
    switch (opcode) {
        case 0x0: {
            // TODO implement all special cases
            chip8_inc_pc(cpu);
            break;
        }
        /* Unconditional jump */
        case 0x1: {
            cpu->pc = NNN(instruction);
            break;
        }
        // Call subroutine
        case 0x2: {
            // 1. push current pc onto the stack
            // 2. set pc to NNN of the current instruction
            break;
        }
        // Skip next if equal
        case 0x03: {
            if (cpu->regs[X(instruction) == NN(instruction)]) {
                chip8_inc_pc(cpu);
            }
            chip8_inc_pc(cpu);
            break;
        }
        // Skip next if not equal
        case 0x04: {
            if (cpu->regs[X(instruction) != NN(instruction)]) {
                chip8_inc_pc(cpu);
            }
            chip8_inc_pc(cpu);
            break;
        }
        // Skip next if registers equal
        case 0x05: {
            chip8_inc_pc(cpu);
            break;
        }
        case 0x6: {
            uint8_t r = X(instruction);
            uint16_t value = NN(instruction);
            cpu->regs[r] = value;
            chip8_inc_pc(cpu);
            break;
        }
        case 0xA: {
            uint16_t target = NNN(instruction);
            cpu->I = target;
            chip8_inc_pc(cpu);
            break;
        }
        default:
            chip8_inc_pc(cpu);
    }
}

void chip8_load_program(Chip8* cpu, FILE* file) {
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    unsigned char* data = calloc(1, file_size + 0x200);
    size_t read_bytes = fread(data + 0x200, 1, file_size, file);
    printf("Read %zu bytes\n", read_bytes);
    fclose(file);

    Rom* rom = malloc(sizeof(Rom));
    rom->size = file_size;
    rom->data = data;
    cpu->rom = rom;
}

int main() {
    FILE* file = fopen("2-ibm-logo.ch8", "rb");
    Chip8* cpu = chip8_init();
    chip8_load_program(cpu, file);
    while (cpu->pc - 0x200 < cpu->rom->size) {
        chip8_cycle(cpu);
    }
    chip8_dealloc(cpu);
    return EXIT_SUCCESS;
}
