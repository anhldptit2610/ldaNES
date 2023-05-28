#include <stdio.h>
#include <stdint.h>

#define MEM_SIZE 65536
#define STACK_BEGIN 	0x0100
#define STACK_END	0x01FF
#define NOOPR		0x101

enum cpuAddressingMode {
	Immediate,
	Accumulator,
	Implied,
	ZeroPage,
	ZeroPageX,
	ZeroPageY,
	Relative,
	Absolute,
	AbsoluteX,
	AbsoluteY,
	Indirect,
	IndirectX,
	IndirectY,
};


enum statusRegisterFlag {
	C = 1U << 0,
	Z = 1U << 1,
	I = 1U << 2,
	D = 1U << 3,
	B = 1U << 4,		/* for BRK command */
	NA = 1U << 5,		/* unused */
	V = 1U << 6,
	N = 1U << 7
};

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
	uint8_t mem[MEM_SIZE];

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

struct instruction {
	char name[5];
	uint8_t opcode;
	uint8_t cycles;
	void (*addrmode)(struct cpu *cpu);
	void (*handler)(struct cpu *cpu);
};

/* helper to access memory */
uint8_t memRead(struct cpu *cpu, uint16_t addr);
void memWrite(struct cpu *cpu, uint16_t addr, uint8_t data); 
uint8_t fetchByteFromPC(struct cpu *cpu);
void push(struct cpu *cpu, uint8_t data);
uint8_t pop(struct cpu *cpu);
void pushPCtoStack(struct cpu *cpu);
void pullPCfromStack(struct cpu *cpu);

/* helper to debug */
char *getOpcodeName(uint8_t opcode);
void exportLog(struct cpu *cpu, FILE *filp);

/* helper to set/unset flags of the p register */
void setFlag(struct cpu *cpu, uint8_t flag, uint8_t condition);
void updateNandZFlags(struct cpu *cpu, uint8_t condition);
uint8_t getFlag(struct cpu *cpu, uint8_t flag);


void fetchAndExecuteInstruction(struct cpu *cpu, uint8_t *cycles);
void run(struct cpu *cpu);

/* addressing mode functions */
void IMP(struct cpu *cpu);
void ACC(struct cpu *cpu);
void IMM(struct cpu *cpu);
void ZP(struct cpu *cpu);
void ZPX(struct cpu *cpu);
void ZPY(struct cpu *cpu);
void REL(struct cpu *cpu);
void ABS(struct cpu *cpu);
void ABX(struct cpu *cpu);
void ABY(struct cpu *cpu);
void IND(struct cpu *cpu);
void IDX(struct cpu *cpu);
void IDY(struct cpu *cpu);
void JAM(struct cpu *cpu);

/* load/store operations */
void LDA(struct cpu *cpu);
void LDX(struct cpu *cpu);
void LDY(struct cpu *cpu);
void STA(struct cpu *cpu);
void STX(struct cpu *cpu);
void STY(struct cpu *cpu);

/* register transfers */
void TAX(struct cpu *cpu);
void TAY(struct cpu *cpu);
void TXA(struct cpu *cpu);
void TYA(struct cpu *cpu);

/* stack operations */
void TSX(struct cpu *cpu);
void TXS(struct cpu *cpu);
void PHA(struct cpu *cpu);
void PHP(struct cpu *cpu);
void PLA(struct cpu *cpu);
void PLP(struct cpu *cpu);

/* logical */
void AND(struct cpu *cpu);
void EOR(struct cpu *cpu);
void ORA(struct cpu *cpu);
void BIT(struct cpu *cpu);

/* arithmetic */
void ADC(struct cpu *cpu);
void SBC(struct cpu *cpu);
void CMP(struct cpu *cpu);
void CPX(struct cpu *cpu);
void CPY(struct cpu *cpu);

/* increments & decrements */
void INC(struct cpu *cpu);
void INX(struct cpu *cpu);
void INY(struct cpu *cpu);
void DEC(struct cpu *cpu);
void DEX(struct cpu *cpu);
void DEY(struct cpu *cpu);

/* shifts */
void ASL(struct cpu *cpu);
void LSR(struct cpu *cpu);
void ROL(struct cpu *cpu);
void ROR(struct cpu *cpu);

/* branches */
void BCC(struct cpu *cpu);
void BCS(struct cpu *cpu);
void BEQ(struct cpu *cpu);
void BMI(struct cpu *cpu);
void BNE(struct cpu *cpu);
void BPL(struct cpu *cpu);
void BVC(struct cpu *cpu);
void BVS(struct cpu *cpu);

/* status flag changes */
void CLC(struct cpu *cpu);
void CLD(struct cpu *cpu);
void CLI(struct cpu *cpu);
void CLV(struct cpu *cpu);
void SEC(struct cpu *cpu);
void SED(struct cpu *cpu);
void SEI(struct cpu *cpu);

/* system functions */
void BRK(struct cpu *cpu);
void NOP(struct cpu *cpu);
void RTI(struct cpu *cpu);

/* jumps and calls */
void JSR(struct cpu *cpu);
void RTI(struct cpu *cpu);
void RTS(struct cpu *cpu);
void JMP(struct cpu *cpu);

/* illegal instructions */
void jam(struct cpu *cpu);
void SLA(struct cpu *cpu);
void RLA(struct cpu *cpu);
void SRE(struct cpu *cpu);
void RRA(struct cpu *cpu);
void SAX(struct cpu *cpu);
void SHA(struct cpu *cpu);
void LAX(struct cpu *cpu);
void DCP(struct cpu *cpu);
void ISC(struct cpu *cpu);
void SLO(struct cpu *cpu);
void USBC(struct cpu *cpu);
void ANC(struct cpu *cpu);
void ALR(struct cpu *cpu);
void ARR(struct cpu *cpu);
void ANE(struct cpu *cpu);
void LXA(struct cpu *cpu);
void LAS(struct cpu *cpu);
void SBX(struct cpu *cpu);
void SHY(struct cpu *cpu);
void SHX(struct cpu *cpu);

void resetCPU(struct cpu *cpu, uint8_t *cycles);
void NMI(struct cpu *cpu, uint8_t *cycles);
void IRQ(struct cpu *cpu, uint8_t *cycles);