#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "../inc/common.h"
#include "../inc/cpu.h"
#include "../inc/bus.h"
#include "../inc/cartridge.h"
#include "../inc/ppu.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREEN_WIDTH		320
#define SCREEN_HEIGHT		320


int main(void)
{
	struct cpu cpu;
	struct ppu ppu;
	struct bus bus = {
		.cpu = &cpu,
		.ppu = &ppu,
	};
	int clockCycle = 0;
	FILE *filp = NULL;
	struct cartridge nestest;

	resetCPU(&bus, &clockCycle);
	writeUsingBus(&bus, 0xfffc, 0x00);
	writeUsingBus(&bus, 0xfffd, 0xC0);
	cpu.reg.pc = 0xC000;
	parseCartridge(&nestest, &bus, "../src/test/nestest.nes");

	filp = fopen("../src/test/log.txt", "w");
	if (!filp) {
		fprintf(stderr, "ldaNES: can't open log file!\n");
		exit(1);
	}
	writeUsingBus(&bus, 0x0002, 0xff);
	int i = 1;
	while (1) {
		if (!clockCycle) {
			if (i == 8992)
				return 0;
			fetchAndExecuteInstruction(&bus, &clockCycle);
			exportLog(&bus, filp);
			i++;
		} else {
			clockCycle--;
		}
	}
	
	fclose(filp);
	return 0;
}