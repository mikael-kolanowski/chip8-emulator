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
    unsigned char* data;
} Rom;

typedef struct {
    unsigned int pc;
    uint8_t regs[16];
    uint16_t I;
    unsigned int sp;
    Rom rom;
    unsigned char* display;
    unsigned char* memory;
} Chip8;

Chip8* chip8_init() {
    Chip8* cpu = malloc(sizeof(Chip8));
    cpu->memory = calloc(1, 1024);
    cpu->pc = 0x200;
    cpu->sp = 0xf00;  // TODO: find out the real sp position

    // TODO: find out where the display should actually be
    cpu->display = &cpu->memory[0xfaa];

    return cpu;
}

void chip8_dealloc(Chip8* cpu) {
    free(cpu->rom.data);
    free(cpu->memory);
    free(cpu);
}

void chip8_inc_pc(Chip8* cpu) {
    cpu->pc += 2;
}

void chip8_cycle(Chip8* cpu) {
    // TODO read instruction at pc
    uint16_t instruction = 0x00E0;
    printf("*pc = %x\n", cpu->rom.data[cpu->pc]);
    printf("*pc+1 = %x\n", cpu->rom.data[cpu->pc + 1]);
    uint8_t opcode = instruction >> 4;
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
        }
        // Skip next if not equal
        case 0x04: {
            if (cpu->regs[X(instruction) != NN(instruction)]) {
                chip8_inc_pc(cpu);
            }
            chip8_inc_pc(cpu);
        }
        // Skip next if registers equal
        case 0x05: {
            chip8_inc_pc(cpu);
        }
    }
}

Rom chip8_load_program(Chip8* cpu, FILE* file) {
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    unsigned char* data = calloc(1, file_size + 0x200);
    size_t read_bytes = fread(data + 0x200, 1, file_size, file);
    printf("Read %zu bytes\n", read_bytes);
    fclose(file);

    return (Rom){file_size, data};
}

int main() {
    FILE* file = fopen("2-ibm-logo.ch8", "rb");
    Chip8* cpu = chip8_init();
    chip8_load_program(cpu, file);
    chip8_cycle(cpu);
    chip8_dealloc(cpu);
    return EXIT_SUCCESS;
}
