#include <stdio.h>

#include "disassembly.h"

void chip8_disassemble(uint16_t instr, char* buffer) {
	uint8_t opcode = instr >> 12;
	switch (opcode) {
		case 0x0: {
			uint8_t op = NN(instr);
			switch (op) {
				case 0xE0: sprintf(buffer, "CLEAR"); break;
				case 0xEE: sprintf(buffer, "RETURN"); break;
				default:   sprintf(buffer, "UNIMPLEMENTED"); break;
			}
			break;
		}
		case 0x1: {
			sprintf(buffer, "JUMP %04x", NNN(instr));
			break;
		}
		case 0x2: {
			sprintf(buffer, "CALL %04x", NNN(instr));
			break;
		}
		case 0x3: {
			sprintf(buffer, "SKIP IF V%x == %d", X(instr), NN(instr));
			break;
		}
		case 0x4: {
			sprintf(buffer, "SKIP IF V%x != %d", X(instr), NN(instr));
			break;
		}
		case 0x5: {
			sprintf(buffer, "SKIP IF V%x == V%x", X(instr), Y(instr));
			break;
		}
		case 0x6: {
			sprintf(buffer, "V%x = %d", X(instr), NN(instr));
			break;
		}
		case 0x7: {
			sprintf(buffer, "V%x += %d", X(instr), NN(instr));
			break;
		}
		case 0x8: {
			// TODO: Properly disassemble this one
			sprintf(buffer, "ARITHMETIC");
			break;
		}
		case 0x9: {
			sprintf(buffer, "SKIP IF V%x != V%x", X(instr), Y(instr));
			break;
		}
		case 0xA: {
			sprintf(buffer, "I = %04x", NNN(instr));
			break;
		}
		case 0xB: {
			sprintf(buffer, "I = %04x + V0", NNN(instr));
			break;
		}
		case 0xC: {
			sprintf(buffer, "V%x = RAND %% %d", X(instr), NN(instr));
			break;
		}
		case 0xD: {
			sprintf(buffer, "DRAW V%x, V%x, %d", X(instr), Y(instr), Z(instr));
			break;
		}
		case 0xE: {
			switch (NN(instr)) {
				case 0x9E: sprintf(buffer, "SKIP IF KEY == V%x", X(instr)); break;
				case 0xA1: sprintf(buffer, "SKIP IF KEY != V%x", X(instr)); break;
				default  : sprintf(buffer, "INVALID"); break;
			}
			break;
		}
		case 0xF: {
			sprintf(buffer, "TIMEROP");
			switch (NN(instr)) {
				case 0x07: sprintf(buffer, "V%x = DELAY", X(instr)); break;
				case 0x0A: sprintf(buffer, "V%x = KEY", X(instr)); break;
				case 0x15: sprintf(buffer, "DELAY = V%x", X(instr)); break;
				case 0x18: sprintf(buffer, "SOUND = V%x", X(instr)); break;
				case 0x1E: sprintf(buffer, "I += V%x", X(instr)); break;
				case 0x29: sprintf(buffer, "I = CHAR V%x", X(instr)); break;
				case 0x33: sprintf(buffer, "BCD V%x", X(instr)); break;
				case 0x55: sprintf(buffer, "REG DUMP V%x", X(instr)); break;
				case 0x65: sprintf(buffer, "REG LOAD V%x", X(instr)); break;
			}
			break;
		}
		default: sprintf(buffer, "TBD");
	}
}
