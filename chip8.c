#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define X(instr) (((instr)&0x0f00) >> 8)
#define Y(instr) (((instr)&0x00f0) >> 4)
#define Z(instr) (((instr)&0x000f))
#define NN(instr) (((instr)&0x00ff))
#define NNN(instr) (((instr)&0x0fff))

#define CHIP8_DISPLAY_ROWS 31
#define CHIP8_DISPLAY_COLS 64

#define CHIP8_STACK_DEPTH 12
#define CHIP8_MEMORY_SIZE 4096

typedef struct {
    unsigned int pc;
    uint8_t regs[16];
    uint16_t I;
    unsigned int sp;
    uint16_t stack[CHIP8_STACK_DEPTH];
    uint8_t display[CHIP8_DISPLAY_COLS * CHIP8_DISPLAY_ROWS];
    bool keys[16];
    uint8_t* memory;
} Chip8;

Chip8* chip8_init() {
    Chip8* cpu = malloc(sizeof(Chip8));
    cpu->memory = calloc(1, CHIP8_MEMORY_SIZE);
    cpu->pc = 0x200;
    cpu->sp = 0;

    memset(cpu->display, 0, CHIP8_DISPLAY_ROWS * CHIP8_DISPLAY_COLS);
    memset(cpu->stack, 0, CHIP8_STACK_DEPTH);

    return cpu;
}

void chip8_dealloc(Chip8* cpu) {
    free(cpu->memory);
    free(cpu);
}

void chip8_inc_pc(Chip8* cpu) {
    cpu->pc += 2;
}

void chip8_cycle(Chip8* cpu) {
    uint8_t* instr_ptr = &cpu->memory[cpu->pc];
    uint8_t upper = instr_ptr[0];
    uint8_t lower = instr_ptr[1];
    uint8_t opcode = upper >> 4;
    uint16_t instruction = upper << 8 | lower;
    // printf("instr=%04X\n", instruction);
    switch (opcode) {
        case 0x0: {
            uint8_t op = NN(instruction);
            switch (op) {
                case 0xE0:
                    // Clear screen
                    break;
                case 0xEE:
                    // Return from procedure
                    --cpu->sp;
                    cpu->pc = cpu->memory[cpu->sp];
                    break;
                default:
                    // Unimplemented
                    break;
            }
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
            cpu->memory[cpu->sp] = cpu->pc;
            ++cpu->sp;
            uint16_t target = NNN(instruction);
            cpu->pc = target;
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
            uint8_t r = X(instruction);
            uint16_t value = NN(instruction);
            if (cpu->regs[r] != value) {
                chip8_inc_pc(cpu);
            }
            chip8_inc_pc(cpu);
            break;
        }
        // Skip next if registers equal
        case 0x05: {
            uint8_t r = X(instruction);
            uint8_t s = Y(instruction);
            if (cpu->regs[r] == cpu->regs[s]) {
                chip8_inc_pc(cpu);
            }
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
        case 0x7: {
            uint8_t r = X(instruction);
            uint16_t value = NN(instruction);
            cpu->regs[r] += value;
            chip8_inc_pc(cpu);
            break;
        }
        case 0x8: {
            uint8_t r = X(instruction);
            uint8_t s = Y(instruction);
            uint8_t op = Z(instruction);
            switch (op) {
                case 0:
                    cpu->regs[r] = cpu->regs[s];
                    break;
                case 1:
                    cpu->regs[r] |= cpu->regs[s];
                    break;
                case 2:
                    cpu->regs[r] &= cpu->regs[s];
                    break;
                case 3:
                    cpu->regs[r] ^= cpu->regs[s];
                    break;
                case 4: {
                    uint16_t sum = cpu->regs[r] + cpu->regs[s];
                    cpu->regs[0xF] = sum > 0xFF;
                    cpu->regs[r] = sum & 0xFF;
                    break;
                }
                case 5:
                    cpu->regs[0xF] = cpu->regs[r] > cpu->regs[s];
                    cpu->regs[r] -= cpu->regs[s];
                    break;
                case 6: {
                    uint8_t lsb = cpu->regs[r] % 2 == 0;
                    cpu->regs[r] >>= 1;
                    cpu->regs[0xF] = lsb;
                    break;
                }
                case 7:
                    cpu->regs[r] = cpu->regs[s] - cpu->regs[r];
                    break;
                case 0xE: {
                    uint8_t msb = cpu->regs[r] >> 7;
                    cpu->regs[r] <<= 1;
                    cpu->regs[0xF] = msb;
                    break;
                }
            }
            chip8_inc_pc(cpu);
            break;
        }
        case 0x9: {
            uint8_t r = X(instruction);
            uint8_t s = Y(instruction);
            if (cpu->regs[r] != cpu->regs[s]) {
                chip8_inc_pc(cpu);
            }
            chip8_inc_pc(cpu);
            break;
        }
        case 0xA: {
            uint16_t target = NNN(instruction);
            cpu->I = target;
            chip8_inc_pc(cpu);
            break;
        }
        case 0xB: {
            uint16_t target = NNN(instruction);
            cpu->pc = target + cpu->regs[0];
            break;
        }
        case 0xC: {
            uint8_t r = X(instruction);
            uint8_t mask = NN(instruction);
            uint8_t rnd = (rand() % 0xFF) & mask;
            cpu->regs[r] = rnd;
            chip8_inc_pc(cpu);
            break;
        }
        case 0xD: {
            uint8_t r = X(instruction);
            uint8_t s = Y(instruction);
            uint8_t height = Z(instruction);

            uint8_t sx = cpu->regs[r] % CHIP8_DISPLAY_COLS;
            uint8_t sy = cpu->regs[s] % CHIP8_DISPLAY_COLS;

            for (uint8_t irow = 0; irow < height; ++irow) {
                uint8_t row = cpu->memory[cpu->I + irow];
                printf("%c%c%c%c%c%c%c%c\n", row & 0x80 ? '#' : ' ',
                       row & 0x40 ? '#' : ' ', row & 0x20 ? '#' : ' ',
                       row & 0x10 ? '#' : ' ', row & 0x08 ? '#' : ' ',
                       row & 0x04 ? '#' : ' ', row & 0x02 ? '#' : ' ',
                       row & 0x01 ? '#' : ' ');
                for (uint8_t bit = 0; bit < 8; ++bit) {
                    uint8_t index = sx + bit + (sy + irow) * CHIP8_DISPLAY_COLS;
                    uint8_t pixel = cpu->display[index];
                    uint8_t sprite_pixel = (row & (1 << (7 - bit))) != 0;
                    if (pixel & sprite_pixel) {
                        cpu->regs[0xF] = 1;
                    }
                    if (!pixel && sprite_pixel) {
                        cpu->regs[0xF] = 0;
                    }
                    cpu->display[index] ^= sprite_pixel;
                }
            }
            printf("\n");

            chip8_inc_pc(cpu);
            break;
        }
        case 0xE: {
            uint8_t r = X(instruction);
            uint8_t op = NN(instruction);
            switch (op) {
                case 0x9E:
                    if (cpu->keys[cpu->regs[r]]) {
                        chip8_inc_pc(cpu);
                    }
                    break;
                case 0xA1:
                    if (!cpu->keys[cpu->regs[r]]) {
                        chip8_inc_pc(cpu);
                    }
                    break;
            }
            chip8_inc_pc(cpu);
            break;
        }
        case 0xF: {
            uint8_t r = X(instruction);
            uint16_t op = NN(instruction);
            switch (op) {
                case 0x07:
                    // Set register to delay timer value
                    cpu->regs[r] = 0;
                    break;
                case 0x0A:
                    // Await key press and store in register
                    break;
                case 0x15:
                    // Set delay timer value
                    break;
                case 0x18:
                    break;
                case 0x1E:
                    cpu->I += cpu->regs[r];
                    break;
                case 0x29:
                    // Set I to sprite addr of char in register
                    break;
                case 0x33:
                    // BCD
                    break;
                case 0x55:  // Register dump
                    for (uint8_t i = 0; i <= cpu->regs[r]; ++i) {
                        cpu->memory[cpu->I + i] = cpu->regs[i];
                    }
                    break;
                case 0x65:  // Register load
                    for (uint8_t i = 0; i <= cpu->regs[r]; ++i) {
                        cpu->regs[i] = cpu->memory[cpu->I + i];
                    }
                    break;
            }
            chip8_inc_pc(cpu);
            break;
        }
        default:
            chip8_inc_pc(cpu);
    }
}

bool chip8_load_program(Chip8* cpu, const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        return false;
    }
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    size_t read_bytes = fread(cpu->memory + 0x200, 1, file_size, file);
    printf("Read %zu bytes\n", read_bytes);
    fclose(file);

    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: chip8 <filename>\n");
        return EXIT_FAILURE;
    }

    const char* rom_file_path = argv[1];
    Chip8* cpu = chip8_init();
    if (!chip8_load_program(cpu, rom_file_path)) {
        printf("Unable to load ROM file %s\n", rom_file_path);
        return EXIT_FAILURE;
    }

    const int pixel_size = 8;
    InitWindow(CHIP8_DISPLAY_COLS * pixel_size, CHIP8_DISPLAY_ROWS * pixel_size,
               "Chip8");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        chip8_cycle(cpu);

        BeginDrawing();
        ClearBackground(BLACK);
        for (size_t r = 0; r < CHIP8_DISPLAY_ROWS; ++r) {
            for (size_t c = 0; c < CHIP8_DISPLAY_COLS; ++c) {
                uint8_t pixel = cpu->display[c + r * CHIP8_DISPLAY_COLS];
                if (pixel) {
                    DrawRectangle(c * pixel_size, r * pixel_size, pixel_size,
                                  pixel_size, RAYWHITE);
                }
            }
        }
        EndDrawing();
    }
    chip8_dealloc(cpu);
    return EXIT_SUCCESS;
}
