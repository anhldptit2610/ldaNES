#include <stdint.h>

enum PPUCTRL_BITS {
	N0 = (1U << 0),	
	N1 = (1U << 1),
	I  = (1U << 2),
	S  = (1U << 3),
	B  = (1U << 4),
	H  = (1U << 5),
	P  = (1U << 6),
	V  = (1U << 7),
};

enum PPUMASK_BITS {
	G = (1U << 0),
	m = (1U << 1),
	M = (1U << 2),
	b = (1U << 3),
	s = (1U << 4),
	R = (1U << 5),
	G = (1U << 6),
	B = (1U << 7),
};

enum PPUSTATUS_BITS {
	O  = (1U << 5),
	S  = (1U << 6),
	SV = (1U << 7),
};

struct PPU {
	uint8_t PPUCTRL;
	uint8_t PPUMASK;
	uint8_t PPUSTATUS;
	uint8_t OAMADDR;
	uint8_t OAMDATA;
	uint8_t PPUSCROLL;
	uint8_t PPUADDR;
	uint8_t PPUDATA;
	uint8_t OAMDMA;
};
