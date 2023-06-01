#include <stdio.h>
#include <stdint.h>

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

struct instruction {
	char name[5];
	uint8_t opcode;
	uint8_t cycles;
	void (*addrmode)(struct bus *bus);
	void (*handler)(struct bus *bus);
};

/* helper to access memory */
uint8_t fetchByteFromPC(struct bus *bus);
void push(struct bus *bus, uint8_t data);
uint8_t pop(struct bus *bus);
void pushPCtoStack(struct bus *bus);
void pullPCfromStack(struct bus *bus);

/* helper to debug */
char *getOpcodeName(uint8_t opcode);
void exportLog(struct bus *bus, FILE *filp);

/* helper to set/unset flags of the p register */
void setFlag(struct bus *bus, uint8_t flag, uint8_t condition);
void updateNandZFlags(struct bus *bus, uint8_t condition);
uint8_t getFlag(struct bus *bus, uint8_t flag);


void fetchAndExecuteInstruction(struct bus *bus, int *cycles);

/* addressing mode functions */
void IMP(struct bus *bus);
void ACC(struct bus *bus);
void IMM(struct bus *bus);
void ZP(struct bus *bus);
void ZPX(struct bus *bus);
void ZPY(struct bus *bus);
void REL(struct bus *bus);
void ABS(struct bus *bus);
void ABX(struct bus *bus);
void ABY(struct bus *bus);
void IND(struct bus *bus);
void IDX(struct bus *bus);
void IDY(struct bus *bus);
void JAM(struct bus *bus);

/* load/store operations */
void LDA(struct bus *bus);
void LDX(struct bus *bus);
void LDY(struct bus *bus);
void STA(struct bus *bus);
void STX(struct bus *bus);
void STY(struct bus *bus);

/* register transfers */
void TAX(struct bus *bus);
void TAY(struct bus *bus);
void TXA(struct bus *bus);
void TYA(struct bus *bus);

/* stack operations */
void TSX(struct bus *bus);
void TXS(struct bus *bus);
void PHA(struct bus *bus);
void PHP(struct bus *bus);
void PLA(struct bus *bus);
void PLP(struct bus *bus);

/* logical */
void AND(struct bus *bus);
void EOR(struct bus *bus);
void ORA(struct bus *bus);
void BIT(struct bus *bus);

/* arithmetic */
void ADC(struct bus *bus);
void SBC(struct bus *bus);
void CMP(struct bus *bus);
void CPX(struct bus *bus);
void CPY(struct bus *bus);

/* increments & decrements */
void INC(struct bus *bus);
void INX(struct bus *bus);
void INY(struct bus *bus);
void DEC(struct bus *bus);
void DEX(struct bus *bus);
void DEY(struct bus *bus);

/* shifts */
void ASL(struct bus *bus);
void LSR(struct bus *bus);
void ROL(struct bus *bus);
void ROR(struct bus *bus);

/* branches */
void BCC(struct bus *bus);
void BCS(struct bus *bus);
void BEQ(struct bus *bus);
void BMI(struct bus *bus);
void BNE(struct bus *bus);
void BPL(struct bus *bus);
void BVC(struct bus *bus);
void BVS(struct bus *bus);

/* status flag changes */
void CLC(struct bus *bus);
void CLD(struct bus *bus);
void CLI(struct bus *bus);
void CLV(struct bus *bus);
void SEC(struct bus *bus);
void SED(struct bus *bus);
void SEI(struct bus *bus);

/* system functions */
void BRK(struct bus *bus);
void NOP(struct bus *bus);
void RTI(struct bus *bus);

/* jumps and calls */
void JSR(struct bus *bus);
void RTS(struct bus *bus);
void JMP(struct bus *bus);

/* illegal instructions */
void jam(struct bus *bus);
void SLA(struct bus *bus);
void RLA(struct bus *bus);
void SRE(struct bus *bus);
void RRA(struct bus *bus);
void SAX(struct bus *bus);
void SHA(struct bus *bus);
void LAX(struct bus *bus);
void DCP(struct bus *bus);
void ISC(struct bus *bus);
void SLO(struct bus *bus);
void USBC(struct bus *bus);
void ANC(struct bus *bus);
void ALR(struct bus *bus);
void ARR(struct bus *bus);
void ANE(struct bus *bus);
void LXA(struct bus *bus);
void LAS(struct bus *bus);
void SBX(struct bus *bus);
void SHY(struct bus *bus);
void SHX(struct bus *bus);

void resetCPU(struct bus *bus, int *cycles);
void NMI(struct bus *bus, uint8_t *cycles);
void IRQ(struct bus *bus, uint8_t *cycles);