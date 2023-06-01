#include <stdint.h>

#define CPU_MEMSIZE 	65536
#define PPU_MEMSIZE	0x4000
#define OAM_MEMSIZE	256

struct cpuRegister {
	uint16_t pc;		/* program counter */
	uint8_t a;
	uint8_t x;
	uint8_t y;
	uint8_t sp;		/* stack pointer */
	uint8_t p;		/* status register */
};

struct cpu {
	struct cpuRegister reg;
	uint8_t mem[CPU_MEMSIZE];

	uint16_t targetAddress;
	uint8_t opcode;
	uint8_t operand;
	uint8_t clockCycles;

	/* for handling some 1 byte instructions */
	uint8_t dummy;		

	/* for debugging purpose */
	uint16_t debug_RegPC;
	uint8_t debug_IDX;
	uint8_t debug_NOP;
	uint16_t debug_IDY;
	uint8_t debug_RegA;
	uint8_t debug_RegX;
	uint8_t debug_RegY;
	uint8_t debug_RegP;
	uint8_t debug_RegSP;
	uint16_t providedAddress;
	uint8_t opr1;
	uint8_t opr2;
	int getOperand;
	uint8_t addressingMode;
};

struct ppu {
	uint8_t mem[PPU_MEMSIZE];
	uint8_t oam[OAM_MEMSIZE];
};

struct bus {
	struct cpu *cpu;
	struct ppu *ppu;
};