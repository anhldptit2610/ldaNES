#include <stdint.h>


enum PPU_MMR {
	PPUCTRL = 0x2000,
	PPUMASK = 0x2001,
	PPUSTATUS = 0x2002,
	OAMADDR = 0x2003,
	OAMDATA = 0x2004,
	PPUSCROLL = 0x2005,
	PPUADDR = 0x2006,
	PPUDATA = 0x2007,
	OAMDMA = 0x4014,
};

void resetPPU(void);
void writePPU(struct bus *bus, uint16_t addr, uint8_t data);
uint8_t readPPU(struct bus *bus, uint16_t addr);
