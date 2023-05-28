#define RAM_MIRRORING_RANGE 0x1fff
#define PPU_MIRRORING_RANGE 0x3fff

void writeUsingBus(struct cpu *cpu, uint16_t addr, uint8_t data);
uint8_t readUsingBus(struct cpu *cpu, uint16_t addr);