#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "../inc/common.h"
#include "../inc/bus.h"
#include "../inc/ppu.h"
#include "../inc/cartridge.h"

void parseCartridge(struct cartridge *cart, struct bus *bus, char *rom_path)
{
	FILE *filp = NULL;

	filp = fopen(rom_path, "rb");
	if (!filp) {
		fprintf(stderr, "Can't open the rom file\n");
		return;
	}

	/* check if the ROM file is iNES file or not. */
	fread(cart->header, 1, 16, filp);
	if (cart->header[0] != 0x4e || cart->header[1] != 0x45 || cart->header[2] != 0x53 || cart->header[3] != 0x1a) {
		fprintf(stderr, "%s is not a iNES file!\n", rom_path);
		return;
	} else {
		printf("ok! We read an iNES file xD\n");
	}

	cart->prgromSize = cart->header[4];
	cart->chrromSize = cart->header[5];
	cart->mirroringMode = (cart->header[6] & (1U << 3)) ? FOUR_SCREEN : cart->header[6] & (1U << 0);
	cart->mapperID = ((cart->header[6] & 0xF0) >> 4) | (cart->header[7] & 0xF0);
	cart->skipTrainer = (cart->header[6] & (1U << 2)) ? false : true;
	cart->cartridgeVersion = (((cart->header[6] >> 2) & 0x11) == 2) ? NESv2 : iNES;

	fread(&bus->cpu->mem[0xc000], 1, 16384 * cart->prgromSize, filp);


	fclose(filp);
}


