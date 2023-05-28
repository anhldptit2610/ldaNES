#include <stdint.h>
#include "../inc/ldaNES_6502.h"
#include "../inc/bus.h"

void writeUsingBus(struct cpu *cpu, uint16_t addr, uint8_t data)
{
	//if (addr <= RAM_MIRRORING_RANGE)
	//	addr &= 0b0000011111111111;
	//else if (addr > RAM_MIRRORING_RANGE && addr <= PPU_MIRRORING_RANGE)
	//	addr &= 0b0010000000000111;
	cpu->mem[addr] = data;
}
uint8_t readUsingBus(struct cpu *cpu, uint16_t addr)
{
	//if (addr <= RAM_MIRRORING_RANGE)
	//	addr &= 0b0000011111111111;
	//else if (addr > RAM_MIRRORING_RANGE && addr <= PPU_MIRRORING_RANGE)
	//	addr &= 0b0010000000000111;
	return cpu->mem[addr];
}