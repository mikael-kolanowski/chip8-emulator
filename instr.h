#ifndef INSTR_H
#define INSTR_H

#define X(instr) (((instr)&0x0f00) >> 8)
#define Y(instr) (((instr)&0x00f0) >> 4)
#define Z(instr) (((instr)&0x000f))
#define NN(instr) (((instr)&0x00ff))
#define NNN(instr)(((instr)&0x0fff))

#endif
