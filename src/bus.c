#include <stdio.h>
#include <stdint.h>
#include "../inc/common.h"
#include "../inc/ppu.h"
#include "../inc/bus.h"

void writeUsingBus(struct bus *bus, uint16_t addr, uint8_t data)
{
	if (addr <= RAM_MIRRORING_RANGE)
		addr &= 0b0000011111111111;
	else if (addr > RAM_MIRRORING_RANGE && addr <= PPU_MIRRORING_RANGE) {
		addr &= 0b0010000000000111;
		if (addr == PPUSTATUS) {
			fprintf(stderr, "writeUsingBus: you can't write the address %04x.\n", addr);
			return;
		}
		writePPU(bus, addr, data);
		return;
	}
	bus->cpu->mem[addr] = data;
}
uint8_t readUsingBus(struct bus *bus, uint16_t addr)
{
	if (addr <= RAM_MIRRORING_RANGE)
		addr &= 0b0000011111111111;
	else if (addr > RAM_MIRRORING_RANGE && addr <= PPU_MIRRORING_RANGE) {
		addr &= 0b0010000000000111;
		if (addr == PPUCTRL || addr == PPUMASK || addr == OAMADDR || addr == PPUSCROLL
			|| addr == PPUADDR || addr == OAMDMA) {
			fprintf(stderr, "readUsingBus: you can't read the address %04x.\n", addr);
			return 0;
		}
		return readPPU(bus, addr);
	}
	return bus->cpu->mem[addr];
}