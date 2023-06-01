#include <stdbool.h>
#include <stdio.h>
#include "../inc/common.h"
#include "../inc/ppu.h"
#include "../inc/bus.h"

/*

7  bit  0
---- ----
VPHB SINN
|||| ||||
|||| ||++- Base nametable address
|||| ||    (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
|||| |+--- VRAM address increment per CPU read/write of PPUDATA
|||| |     (0: add 1, going across; 1: add 32, going down)
|||| +---- Sprite pattern table address for 8x8 sprites
||||       (0: $0000; 1: $1000; ignored in 8x16 mode)
|||+------ Background pattern table address (0: $0000; 1: $1000)
||+------- Sprite size (0: 8x8 pixels; 1: 8x16 pixels – see PPU OAM#Byte 1)
|+-------- PPU master/slave select
|          (0: read backdrop from EXT pins; 1: output color on EXT pins)
+--------- Generate an NMI at the start of the
           vertical blanking interval (0: off; 1: on)

*/

enum PPUCTRL_BITS {
	NAMETABLE0 = (1U << 0),	
	NAMETABLE1 = (1U << 1),
	I  = (1U << 2),			/* VRAM address increment per CPU r/w of PPU data */
	S  = (1U << 3),			/* Sprite pattern table address for 8x8 sprites */
	B  = (1U << 4),			/* Background pattern table address */
	H  = (1U << 5),			/* Sprite size */
	P  = (1U << 6),			/* PPU master/slave select */
	V  = (1U << 7),			/* Generate an NMI at the start of the vertical blanking
					   interval */
};

/*

7  bit  0
---- ----
BGRs bMmG
|||| ||||
|||| |||+- Greyscale (0: normal color, 1: produce a greyscale display)
|||| ||+-- 1: Show background in leftmost 8 pixels of screen, 0: Hide
|||| |+--- 1: Show sprites in leftmost 8 pixels of screen, 0: Hide
|||| +---- 1: Show background
|||+------ 1: Show sprites
||+------- Emphasize red (green on PAL/Dendy)
|+-------- Emphasize green (red on PAL/Dendy)
+--------- Emphasize blue

*/

enum PPUMASK_BITS {
	GREYSCALE = (1U << 0),			/* Greyscale */
	m = (1U << 1),			/* 1: show background in leftmost 8 pixels of screen, 0: hide */
	M = (1U << 2),			/* 1: show sprites in leftmost 8 pixels of scren, 0: hide */
	b = (1U << 3),			/* 1: show background */
	s = (1U << 4),			/* 1: show sprites */
	RED = (1U << 5),			/* emphasize red (green on PAL/Dendy) */
	GREEN = (1U << 6),			/* emphasize green (red on PAL/Dendy) */
	BLUE = (1U << 7),			/* emphasize blue */
};

/*

7  bit  0
---- ----
VSO. ....
|||| ||||
|||+-++++- PPU open bus. Returns stale PPU bus contents.
||+------- Sprite overflow. The intent was for this flag to be set
||         whenever more than eight sprites appear on a scanline, but a
||         hardware bug causes the actual behavior to be more complicated
||         and generate false positives as well as false negatives; see
||         PPU sprite evaluation. This flag is set during sprite
||         evaluation and cleared at dot 1 (the second dot) of the
||         pre-render line.
|+-------- Sprite 0 Hit.  Set when a nonzero pixel of sprite 0 overlaps
|          a nonzero background pixel; cleared at dot 1 of the pre-render
|          line.  Used for raster timing.
+--------- Vertical blank has started (0: not in vblank; 1: in vblank).
           Set at dot 1 of line 241 (the line *after* the post-render
           line); cleared after reading $2002 and at dot 1 of the
           pre-render line.

*/
enum PPUSTATUS_BITS {
	O  = (1U << 5),
	SPRITE0  = (1U << 6),
	VBLANK = (1U << 7),
};


static uint8_t latch = 0;
static uint8_t accessTimes = 0;

enum SPRITE_SIZE {
	x8 = 0,
	x16 = 1,
};

enum GENERATE_NMI {
	OFF = 0,
	ON = 1,
};

/* ppuctrl variables */
uint16_t baseNametableAddress = 0;
uint8_t vramAddressIncrement = 0;
uint16_t spritePatternTableAddress = 0;
uint16_t backgroundPatternTabbleAddress = 0;
bool spriteSize = 0;
bool selectppuMaster = 0;
bool generateNMI = 0;


/* ppumask variables */
bool greyScale = 0;
bool showBackgroundInLeftmost8PixelsOfScreen = 0;
bool showSpriteInLeftmost8PixelsOfScreen = 0;
bool showBackground = 0;
bool showSprites = 0;
bool emphasizeRed = 0;
bool emphasizeGreen = 0;
bool emphasizeBlue = 0;

/* ppustatus variables */
uint8_t ppustatus = 0;


/* ppuaddr variables */
uint8_t vramAddress = 0;

/* ppudata variables */
uint8_t effectiveValue = 0;
uint8_t ppudataAccess = 0;

/* oamaddr variables */
uint16_t oamAddress = 0;

void resetPPU(void)
{
}

void writePPU(struct bus *bus, uint16_t addr, uint8_t data)
{
	uint8_t nameTableAddress, hi;
	uint16_t start;

	switch (addr) {
	case PPUCTRL:
		nameTableAddress = data & 0x03;
		baseNametableAddress = 0x2000 + (2 * nameTableAddress * 0x200);
		vramAddressIncrement = (data & (1U << 2)) ? 32 : 1;		
		spritePatternTableAddress = (data & 0x04) ? 0x1000 : 0x0000;
		backgroundPatternTabbleAddress = (data & 0x10) ? 0x1000 : 0x0000;
		spriteSize = (data & 0x20) ? x16 : x8;
		selectppuMaster = (data & 0x40) ? 1 : 0;
		generateNMI = (data & 0x80) ? ON : OFF;
		break;
	case PPUMASK:
		greyScale = (data & (1U << 0)) ? 1 : 0;
		showBackgroundInLeftmost8PixelsOfScreen = (data & (1U << 1)) ? 1 : 0;
		showSpriteInLeftmost8PixelsOfScreen = (data & (1U << 2)) ? 1 : 0;
		showBackground = (data & (1U << 3)) ? 1 : 0;
		showSprites = (data & (1U << 4)) ? 1 : 0;
		emphasizeRed = (data & (1U << 5)) ? 1 : 0;
		emphasizeGreen = (data & (1U << 6)) ? 1 : 0;
		emphasizeBlue = (data & (1U << 7)) ? 1 : 0;
		break;
		case OAMADDR:
		oamAddress = data;
		break;
	case OAMDATA:
	 	bus->ppu->oam[oamAddress] = data;
		oamAddress++;
		break;
	case PPUSCROLL:
		break;
	case PPUADDR:
		if (accessTimes == 1) {
			vramAddress = data | (latch << 8);
			accessTimes = 0;
		} else {
			accessTimes++;
		}
		latch = data;
		break;
	case PPUDATA:
		bus->ppu->mem[vramAddress] = data;
		vramAddress += vramAddressIncrement;
		break;
	case OAMDMA:
		hi = data;
		start = 0x0000 | (hi << 8);
		for (uint16_t i = 0; i < 256; i++)
			bus->ppu->oam[i] = bus->cpu->mem[start + i];
		break;
	default:
		break;
	}
	latch = data;
}

uint8_t readPPU(struct bus *bus, uint16_t addr)
{
	uint8_t tmp = 0;

	switch (addr) {
	case PPUSTATUS:
		tmp = ppustatus;
		/* clear bit 7 and the address latch */
		ppustatus &= ~(1U << 7);
		latch = 0;
		return ppustatus;
	case PPUDATA:
		if (ppudataAccess == 1) {
			effectiveValue = bus->ppu->mem[vramAddress];
			ppudataAccess = 0;
		} else {
			ppudataAccess++;
		}
		vramAddress += vramAddressIncrement;
		return effectiveValue;
	case OAMDATA:
		return bus->ppu->oam[addr];
	default:
		fprintf(stderr, "readPPU: can't read data from %04x.\n", addr);
		return latch;
	}
	return 0;
}