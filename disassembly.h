#ifndef DISASSEMBLY_H
#define DISASSEMBLY_H

#include <stdint.h>

#include "instr.h"

void chip8_disassemble(uint16_t instr, char* buffer);

#endif /* DISASSEMBLY_H */
