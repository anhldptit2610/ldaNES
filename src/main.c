#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "../inc/ldaNES_6502.h"
#include "../inc/bus.h"
#include "../inc/cartridge.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREEN_WIDTH		320
#define SCREEN_HEIGHT		320


int main(void)
{
	struct cpu cpu;
	struct cartridge cart;
	int clockCycle = 0;
	FILE *filp = NULL;
	struct cartridge nestest;

	resetCPU(&cpu, &clockCycle);
	writeUsingBus(&cpu, 0xfffc, 0x00);
	writeUsingBus(&cpu, 0xfffd, 0xC0);
	cpu.reg.pc = 0xC000;
	parseCartridge(&nestest, &cpu, "../src/nestest.nes");

	filp = fopen("../src/log.txt", "w");
	if (!filp) {
		fprintf(stderr, "ldaNES: can't open log file!\n");
		exit(1);
	}
	writeUsingBus(&cpu, 0x0002, 0xff);
	int i = 1;
	while (1) {
		if (i == 8992)
			return 0;
		fetchAndExecuteInstruction(&cpu, &clockCycle);
		exportLog(&cpu, filp);
		i++;
	}
	
	fclose(filp);
	return 0;
}