#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../inc/ldaNES_6502.h"
#include "../inc/bus.h"

struct instruction lookupTable[] =
{
        { " BRK", 0x00, 7, IMP, BRK },   { " ORA", 0x01, 6, IDX, ORA },      { " JAM", 0x02, 0, JAM, jam },    { "*SLO", 0x03, 8, IDX, SLO },  { "*NOP", 0x04, 3, ZP, NOP },     { " ORA", 0x05, 3, ZP, ORA },    { " ASL", 0x06, 5, ZP, ASL },   { "*SLO", 0x07, 5, ZP, SLO},    { " PHP", 0x08, 3, IMP, PHP },  { " ORA", 0x09, 2, IMM, ORA },   { " ASL", 0x0A, 2, ACC, ASL },     { " ANC", 0x0B, 2, IMM, ANC },      { "*NOP", 0x0C, 4, ABS, NOP },        { " ORA", 0x0D, 4, ABS, ORA },         { " ASL", 0x0E, 6, ABS, ASL },         { "*SLO", 0x0F, 6, ABS, SLO },
        { " BPL", 0x10, 2, REL, BPL },   { " ORA", 0x11, 5, IDY, ORA },      { " JAM", 0x12, 0, JAM, jam },    { "*SLO", 0x13, 8, IDY, SLO },  { "*NOP", 0x14, 4, ZPX, NOP },    { " ORA", 0x15, 4, ZPX, ORA },   { " ASL", 0x16, 6, ZPX, ASL },  { "*SLO", 0x17, 6, ZPX, SLO },  { " CLC", 0x18, 2, IMP, CLC },  { " ORA", 0x19, 4, ABY, ORA },   { "*NOP", 0x1A, 2, IMP, NOP },     { "*SLO", 0x1B, 7, ABY, SLO },      { "*NOP", 0x1C, 4, ABX, NOP },        { " ORA", 0x1D, 4, ABX, ORA },         { " ASL", 0x1E, 7, ABX, ASL },         { "*SLO", 0x1F, 7, ABX, SLO },
        { " JSR", 0x20, 6, ABS, JSR },   { " AND", 0x21, 6, IDX, AND },      { " JAM", 0x22, 0, JAM, jam },    { "*RLA", 0x23, 8, IDX, RLA },  { " BIT", 0x24, 3, ZP, BIT },     { " AND", 0x25, 3, ZP, AND },    { " ROL", 0x26, 5, ZP, ROL },   { "*RLA", 0x27, 5, ZP, RLA },   { " PLP", 0x28, 4, IMP, PLP },  { " AND", 0x29, 2, IMM, AND },   { " ROL", 0x2A, 2, ACC, ROL },     { " ANC", 0x2B, 2, IMM, ANC },      { " BIT", 0x2C, 4, ABS, BIT },        { " AND", 0x2D, 4, ABS, AND },         { " ROL", 0x2E, 6, ABS, ROL },         { "*RLA", 0x2F, 6, ABS, RLA },
        { " BMI", 0x30, 2, REL, BMI },   { " AND", 0x31, 5, IDY, AND },      { " JAM", 0x32, 0, JAM, jam },    { "*RLA", 0x33, 8, IDY, RLA },  { "*NOP", 0x34, 4, ZPX, NOP },    { " AND", 0x35, 4, ZPX, AND },   { " ROL", 0x36, 6, ZPX, ROL },  { "*RLA", 0x37, 6, ZPX, RLA },  { " SEC", 0x38, 2, IMP, SEC },  { " AND", 0x39, 4, ABY, AND },   { "*NOP", 0x3A, 2, IMP, NOP },     { "*RLA", 0x3B, 7, ABY, RLA },      { "*NOP", 0x3C, 4, ABX, NOP },        { " AND", 0x3D, 4, ABX, AND },         { " ROL", 0x3E, 7, ABX, ROL },         { "*RLA", 0x3F, 7, ABX, RLA },
        { " RTI", 0x40, 6, IMP, RTI },   { " EOR", 0x41, 6, IDX, EOR },      { " JAM", 0x42, 0, JAM, jam },    { "*SRE", 0x43, 8, IDX, SRE },  { "*NOP", 0x44, 3, ZP,NOP },      { " EOR", 0x45, 3, ZP, EOR },    { " LSR", 0x46, 5, ZP, LSR },   { "*SRE", 0x47, 5, ZP, SRE },   { " PHA", 0x48, 3, IMP, PHA },  { " EOR", 0x49, 2, IMM, EOR },   { " LSR", 0x4A, 2, ACC, LSR },     { " ALR", 0x4B, 2, IMM, ALR },      { " JMP", 0x4C, 3, ABS, JMP },        { " EOR", 0x4D, 4, ABS, EOR },         { " LSR", 0x4E, 6, ABS, LSR },         { "*SRE", 0x4F, 6, ABS, SRE },
        { " BVC", 0x50, 2, REL, BVC },   { " EOR", 0x51, 5, IDY, EOR },      { " JAM", 0x52, 0, JAM, jam },    { "*SRE", 0x53, 8, IDY, SRE},   { "*NOP", 0x54, 4, ZPX, NOP },    { " EOR", 0x55, 4, ZPX, EOR },   { " LSR", 0x56, 6, ZPX, LSR },  { "*SRE", 0x57, 6, ZPX, SRE },  { " CLI", 0x58, 2, IMP, CLI },  { " EOR", 0x59, 4, ABY, EOR },   { "*NOP", 0x5A, 2, IMP, NOP },     { "*SRE", 0x5B, 7, ABY, SRE },      { "*NOP", 0x5C, 4, ABX, NOP },        { " EOR", 0x5D, 4, ABX, EOR },         { " LSR", 0x5E, 7, ABX, LSR },         { "*SRE", 0x5F, 7, ABX, SRE },
        { " RTS", 0x60, 6, IMP, RTS },   { " ADC", 0x61, 6, IDX, ADC },      { " JAM", 0x62, 0, JAM, jam },    { "*RRA", 0x63, 8, IDX, RRA },  { "*NOP", 0x64, 3, ZP, NOP },     { " ADC", 0x65, 3, ZP, ADC },    { " ROR", 0x66, 5, ZP, ROR },   { "*RRA", 0x67, 5, ZP, RRA },   { " PLA", 0x68, 4, IMP, PLA },  { " ADC", 0x69, 2, IMM, ADC },   { " ROR", 0x6A, 2, ACC, ROR },     { " ARR", 0x6B, 2, IMM, ARR },      { " JMP", 0x6C, 5, IND, JMP },        { " ADC", 0x6D, 4, ABS, ADC },         { " ROR", 0x6E, 6, ABS, ROR },         { "*RRA", 0x6F, 6, ABS, RRA },
        { " BVS", 0x70, 2, REL, BVS },   { " ADC", 0x71, 5, IDY, ADC },      { " JAM", 0x72, 0, JAM, jam },    { "*RRA", 0x73, 8, IDY, RRA },  { "*NOP", 0x74, 4, ZPX, NOP },    { " ADC", 0x75, 4, ZPX, ADC },   { " ROR", 0x76, 6, ZPX, ROR },  { "*RRA", 0x77, 6, ZPX, RRA },  { " SEI", 0x78, 2, IMP, SEI },  { " ADC", 0x79, 4, ABY, ADC },   { "*NOP", 0x7A, 2, IMP, NOP },     { "*RRA", 0x7B, 7, ABY, RRA },      { "*NOP", 0x7C, 4,ABX, NOP },         { " ADC", 0x7D, 4, ABX, ADC },         { " ROR", 0x7E, 7, ABX, ROR },         { "*RRA", 0x7F, 7, ABX, RRA },
        { "*NOP", 0x80, 2, IMM, NOP },   { " STA", 0x81, 6, IDX, STA },      { "*NOP", 0x82, 2, IMM, NOP },    { "*SAX", 0x83, 6, IDX, SAX },  { " STY", 0x84, 3, ZP, STY },     { " STA", 0x85, 3, ZP, STA },    { " STX", 0x86, 3, ZP, STX },   { "*SAX", 0x87, 3, ZP, SAX },   { " DEY", 0x88, 2, IMP, DEY },  { " NOP", 0x89, 2, IMM, NOP},    { " TXA", 0x8A, 2, IMP, TXA },     { " ANE", 0x8B, 2, IMM, ANE },      { " STY", 0x8C, 4, ABS, STY },        { " STA", 0x8D, 4, ABS, STA },         { " STX", 0x8E, 4, ABS, STX },         { "*SAX", 0x8F, 4, ABS, SAX },
        { " BCC", 0x90, 2, REL, BCC },   { " STA", 0x91, 6, IDY, STA },      { " JAM", 0x92, 0, JAM, jam },    { " SHA", 0x93, 6, IDX, SHA },  { " STY", 0x94, 4, ZPX, STY },    { " STA", 0x95, 4, ZPX, STA },   { " STX", 0x96, 4, ZPY, STX },  { "*SAX", 0x97, 4, ZPY, SAX },  { " TYA", 0x98, 2, IMP, TYA },  { " STA", 0x99, 5, ABY, STA },   { " TXS", 0x9A, 2, IMP, TXS },     { " TAX", 0x9B, 2, ABY, TAX },      { " SHY", 0x9C, 5, ABX, SHY },        { " STA", 0x9D, 5, ABX, STA },         { " SHX", 0x9E, 5, ABY, SHX },         { " SHA", 0x9F, 5, ABY, SHA },
        { " LDY", 0xA0, 2, IMM, LDY },   { " LDA", 0xA1, 6, IDX, LDA },      { " LDX", 0xA2, 2, IMM, LDX },    { "*LAX", 0xA3, 6, IDX, LAX },  { " LDY", 0xA4, 3, ZP, LDY },     { " LDA", 0xA5, 3, ZP, LDA },    { " LDX", 0xA6, 3, ZP, LDX },   { "*LAX", 0xA7, 3, ZP, LAX },   { " TAY", 0xA8, 2, IMP, TAY },  { " LDA", 0xA9, 2, IMM, LDA },   { " TAX", 0xAA, 2, IMP, TAX },     { " LXA", 0xAB, 2, IMM, LXA },      { " LDY", 0xAC, 4, ABS, LDY },        { " LDA", 0xAD, 4, ABS, LDA },         { " LDX", 0xAE, 4, ABS, LDX },         { "*LAX", 0xAF, 4, ABS, LAX },
        { " BCS", 0xB0, 2, REL, BCS },   { " LDA", 0xB1, 5, IDY, LDA },      { " JAM", 0xB2, 0, JAM, jam },    { "*LAX", 0xB3, 5, IDY, LAX },  { " LDY", 0xB4, 4, ZPX, LDY },    { " LDA", 0xB5, 4, ZPX, LDA },   { " LDX", 0xB6, 4, ZPY, LDX },  { "*LAX", 0xB7, 4, ZPY, LAX },  { " CLV", 0xB8, 2, IMP, CLV },  { " LDA", 0xB9, 4, ABY, LDA },   { " TSX", 0xBA, 2, IMP, TSX },     { " LAS", 0xBB, 4, ABY, LAS },      { " LDY", 0xBC, 4, ABX, LDY },        { " LDA", 0xBD, 4, ABX, LDA },         { " LDX", 0xBE, 4, ABY, LDX },         { "*LAX", 0xBF, 4, ABY, LAX },
        { " CPY", 0xC0, 2, IMM, CPY },   { " CMP", 0xC1, 6, IDX, CMP },      { "*NOP", 0xC2, 2, IMM, NOP },    { "*DCP", 0xC3, 8, IDX, DCP },  { " CPY", 0xC4, 3, ZP, CPY },     { " CMP", 0xC5, 3, ZP, CMP },    { " DEC", 0xC6, 5, ZP, DEC },   { "*DCP", 0xC7, 5, ZP, DCP },   { " INY", 0xC8, 2, IMP, INY },  { " CMP", 0xC9, 2, IMM, CMP },   { " DEX", 0xCA, 2, IMP, DEX },     { " SBX", 0xCB, 2, IMM, SBX },      { " CPY", 0xCC, 4, ABS, CPY },        { " CMP", 0xCD, 4, ABS, CMP },         { " DEC", 0xCE, 6, ABS, DEC },         { "*DCP", 0xCF, 6, ABS, DCP },
        { " BNE", 0xD0, 2, REL, BNE },   { " CMP", 0xD1, 5, IDY, CMP },      { " JAM", 0xD2, 0, JAM, jam },    { "*DCP", 0xD3, 8, IDY, DCP },  { "*NOP", 0xD4, 4, ZPX, NOP },    { " CMP", 0xD5, 4, ZPX,CMP },    { " DEC", 0xD6, 6, ZPX, DEC },  { "*DCP", 0xD7, 6, ZPX, DCP },  { " CLD", 0xD8, 2, IMP, CLD },  { " CMP", 0xD9, 4, ABY, CMP },   { "*NOP", 0xDA, 2, IMP, NOP },     { "*DCP", 0xDB, 7, ABY, DCP },      { "*NOP", 0xDC, 4, ABX, NOP },        { " CMP", 0xDD, 4, ABX, CMP },         { " DEC", 0xDE, 7, ABX, DEC },         { "*DCP", 0xDF, 7, ABX, DCP },
        { " CPX", 0xE0, 2, IMM, CPX },   { " SBC" , 0xE1, 6, IDX, SBC },     { "*NOP", 0xE2, 2, IMM, NOP },    { "*ISB", 0xE3, 8, IDX, ISC },  { " CPX", 0xE4, 3, ZP, CPX },     { " SBC", 0xE5, 3, ZP, SBC },    { " INC", 0xE6, 5, ZP, INC },   { "*ISB", 0xE7, 5, ZP, ISC },   { " INX", 0xE8, 2, IMP, INX },  { " SBC", 0xE9, 2, IMM, SBC },   { " NOP", 0xEA, 2, IMP, NOP },     { "*SBC", 0xEB, 2, IMM, USBC },     { " CPX", 0xEC, 4, ABS, CPX },        { " SBC", 0xED, 4, ABS, SBC },         { " INC", 0xEE, 6, ABS, INC },         { "*ISB", 0xEF, 6, ABS, ISC },
        { " BEQ", 0xF0, 2, REL, BEQ },   { " SBC", 0xF1, 5, IDY, SBC },      { " JAM", 0xF2, 0, JAM, jam },    { "*ISB", 0xF3, 8, IDY, ISC },  { "*NOP", 0xF4, 4, ZPX, NOP },    { " SBC", 0xF5, 4, ZPX, SBC },   { " INC", 0xF6, 6, ZPX, INC },  { "*ISB", 0xF7, 6, ZPX, ISC },  { " SED", 0xF8, 2, IMP, SED },  { " SBC", 0xF9, 4, ABY, SBC },   { "*NOP", 0xFA, 2, IMP, NOP },     { "*ISB", 0xFB, 7, ABY, ISC },      { "*NOP", 0xFC, 4, ABX, NOP },        { " SBC", 0xFD, 4, ABX, SBC },         { " INC", 0xFE, 7, ABX, INC },         { "*ISB", 0xFF, 7, ABX, ISC },
        {}
};

/* helper to set/unset flags of the P register */
void setFlag(struct cpu *cpu, uint8_t flag, uint8_t condition)
{
	if (!condition)
		cpu->reg.p &= ~flag;
	else
		cpu->reg.p |= flag;
}

void updateNandZFlags(struct cpu *cpu, uint8_t condition)
{
	setFlag(cpu, N, condition & N);
	setFlag(cpu, Z, condition == 0);
}

uint8_t getFlag(struct cpu *cpu, uint8_t flag)
{
	return !(cpu->reg.p & flag) ? 0 : 1;
}

/* helper to debug */

char *getOpcodeName(uint8_t opcode)
{
	return lookupTable[opcode].name;
}

void exportLog(struct cpu *cpu, FILE *filp)
{
	uint16_t pc = cpu->debug_RegPC;
	uint8_t a = cpu->debug_RegA;
	uint8_t x = cpu->debug_RegX;
	uint8_t y = cpu->debug_RegY;
	uint8_t p = cpu->debug_RegP;
	uint8_t sp = cpu->debug_RegSP;
	uint16_t operand1 = (cpu->getOperand >= 1) ? cpu->opr1 : NOOPR;	
	uint16_t operand2 = (cpu->getOperand == 2) ? cpu->opr2 : NOOPR;	

	switch(cpu->addressingMode) {
	case Immediate:
		fprintf(filp, "%04X  %02X %02X    %s #$%02X                        A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, cpu->opcode, operand1, getOpcodeName(cpu->opcode), cpu->operand, a, x,
			y, p, sp);
		break;
	case Implied:
		fprintf(filp, "%04X  %02X       %s                             A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, cpu->opcode, getOpcodeName(cpu->opcode), a, x, y, p, sp);
		break;
	case ZeroPage:
		fprintf(filp, "%04X  %02X %02X    %s $%02X = %02X                    A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, cpu->opcode, operand1, getOpcodeName(cpu->opcode), cpu->targetAddress, cpu->operand,
			a, x, y, p, sp);
		break;
	case ZeroPageX:
		fprintf(filp, "%04X  %02X %02X    %s $%02X,X @ %02X = %02X             A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, cpu->opcode, operand1, getOpcodeName(cpu->opcode), operand1, cpu->targetAddress,
			cpu->operand, a, x, y, p, sp);
		break;
	case ZeroPageY:
		fprintf(filp, "%04X  %02X %02X    %s $%02X,Y @ %02X = %02X             A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, cpu->opcode, operand1, getOpcodeName(cpu->opcode), operand1, cpu->targetAddress,
			cpu->operand, a, x, y, p, sp);
		break;
	case Relative:
		fprintf(filp, "%04X  %02X %02X    %s $%04X                       A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, cpu->opcode, operand1, getOpcodeName(cpu->opcode), cpu->targetAddress,
			a, x, y, p, sp);
		break;
	case Absolute:
		if (cpu->opcode == 0x4C || cpu->opcode == 0x20) { // JMP
			fprintf(filp, "%04X  %02X %02X %02X %s $%04X                       A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
				pc, cpu->opcode, operand1, operand2, getOpcodeName(cpu->opcode), cpu->targetAddress,
				a, x, y, p, sp);
		} else {
			fprintf(filp, "%04X  %02X %02X %02X %s $%04X = %02X                  A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
				pc, cpu->opcode, operand1, operand2, getOpcodeName(cpu->opcode), cpu->targetAddress, cpu->operand,
				a, x, y, p, sp);
		}
		break;
	case AbsoluteX:
		fprintf(filp, "%04X  %02X %02X %02X %s $%04X,X @ %04X = %02X         A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, cpu->opcode, operand1, operand2, getOpcodeName(cpu->opcode), cpu->providedAddress, cpu->targetAddress,
			cpu->operand, a, x, y, p, sp);
		break;
	case AbsoluteY:
		fprintf(filp, "%04X  %02X %02X %02X %s $%04X,Y @ %04X = %02X         A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, cpu->opcode, operand1, operand2, getOpcodeName(cpu->opcode), cpu->providedAddress, cpu->targetAddress,
			cpu->operand, a, x, y, p, sp);
		break;
	case Indirect:
		fprintf(filp, "%04X  %02X %02X %02X %s ($%04X) = %04X              A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, cpu->opcode, operand1, operand2, getOpcodeName(cpu->opcode), cpu->providedAddress,
			cpu->targetAddress, a, x, y, p, sp);
		break;
	case IndirectX:
		fprintf(filp, "%04X  %02X %02X    %s ($%02X,X) @ %02X = %04X = %02X    A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, cpu->opcode, operand1, getOpcodeName(cpu->opcode), operand1, cpu->debug_IDX,
			cpu->targetAddress, cpu->operand, a, x, y, p, sp);
		break;
	case IndirectY:
		fprintf(filp, "%04X  %02X %02X    %s ($%02X),Y = %04X @ %04X = %02X  A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, cpu->opcode, operand1, getOpcodeName(cpu->opcode), operand1, cpu->debug_IDY,
			cpu->targetAddress, cpu->operand, a, x, y, p, sp);
		break;
	case Accumulator:
		fprintf(filp, "%04X  %02X       %s A                           A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
				pc, cpu->opcode, getOpcodeName(cpu->opcode), a, x, y, p, sp);
		break;
	default:
		break;
	}
}

/* addressing mode functions */
void IMP(struct cpu *cpu)
{
	cpu->addressingMode = Implied;
	cpu->getOperand = 0;
}

void ACC(struct cpu *cpu)
{
	cpu->addressingMode = Accumulator;
	cpu->getOperand = 0;
}

void IMM(struct cpu *cpu)
{
	cpu->addressingMode = Immediate;
	cpu->getOperand = 1;
	cpu->operand = fetchByteFromPC(cpu);
	cpu->opr1 = cpu->operand;
	cpu->reg.pc++;
}

void ZP(struct cpu *cpu)
{
	uint16_t lo = (uint16_t)fetchByteFromPC(cpu);
	
	cpu->opr1 = lo;
	cpu->addressingMode = ZeroPage;
	cpu->getOperand = 1;
	cpu->reg.pc++;
	cpu->targetAddress = lo & 0x00FF;
	cpu->operand = readUsingBus(cpu, cpu->targetAddress);
}

void ZPX(struct cpu *cpu)
{
	cpu->addressingMode = Accumulator;
	uint16_t lo = (uint16_t)fetchByteFromPC(cpu);

	cpu->opr1 = lo;
	cpu->addressingMode = ZeroPageX;
	cpu->getOperand = 1;
	cpu->reg.pc++;
	cpu->targetAddress = (lo + cpu->reg.x) & 0x00FF;
	cpu->operand = readUsingBus(cpu, cpu->targetAddress);
}

void ZPY(struct cpu *cpu)
{
	uint16_t lo = (uint16_t)fetchByteFromPC(cpu);

	cpu->opr1 = lo;
	cpu->addressingMode = ZeroPageY;
	cpu->getOperand = 1;
	cpu->reg.pc++;
	cpu->targetAddress = (lo + cpu->reg.y) & 0x00ff;
	cpu->operand = readUsingBus(cpu, cpu->targetAddress);
}

void REL(struct cpu *cpu)
{
	uint8_t offset = fetchByteFromPC(cpu);

	cpu->addressingMode = Relative;
	cpu->opr1 = offset;
	cpu->getOperand = 1;
	cpu->reg.pc++;
	if (offset & 0x80) {		/* if the offset is negative signed */
		offset ^= 0x00FF;
		offset += 1;
		cpu->targetAddress = cpu->reg.pc - offset;
	} else {
		cpu->targetAddress = cpu->reg.pc + offset;
	}
	cpu->operand = readUsingBus(cpu, cpu->targetAddress);
}

void ABS(struct cpu *cpu)
{
	uint16_t lo, hi;

	cpu->addressingMode = Absolute;
	cpu->getOperand = 2;
	lo = fetchByteFromPC(cpu);
	cpu->opr1 = lo;
	cpu->reg.pc++;
	hi = fetchByteFromPC(cpu);
	cpu->opr2 = hi;
	cpu->reg.pc++;
	cpu->targetAddress = lo | (hi << 8);
	cpu->operand = readUsingBus(cpu, cpu->targetAddress);
}

void ABX(struct cpu *cpu)
{
	uint16_t lo, hi;

	cpu->addressingMode = AbsoluteX;
	cpu->getOperand = 2;
	lo = fetchByteFromPC(cpu);
	cpu->reg.pc++;
	cpu->opr1 = lo;
	hi = fetchByteFromPC(cpu);
	cpu->reg.pc++;
	cpu->opr2 = hi;
	cpu->providedAddress = lo | (hi << 8);
	cpu->targetAddress = (lo | (hi << 8)) + cpu->reg.x;
	cpu->operand = readUsingBus(cpu, cpu->targetAddress);
}

void ABY(struct cpu *cpu)
{
	uint16_t lo, hi;

	cpu->addressingMode = AbsoluteY;
	cpu->getOperand = 2;
	lo = fetchByteFromPC(cpu);
	cpu->reg.pc++;
	cpu->opr1 = lo;
	hi = fetchByteFromPC(cpu);
	cpu->reg.pc++;
	cpu->opr2 = hi;
	cpu->providedAddress = lo | (hi << 8);
	cpu->targetAddress = (lo | (hi << 8)) + cpu->reg.y;
	cpu->operand = readUsingBus(cpu, cpu->targetAddress);
}

void IND(struct cpu *cpu)
{
	uint16_t lo, hi, addr;

	cpu->addressingMode = Indirect;
	cpu->getOperand = 2;
	lo = fetchByteFromPC(cpu);
	cpu->reg.pc++;
	cpu->opr1 = lo;
	hi = fetchByteFromPC(cpu);
	cpu->reg.pc++;
	cpu->opr2 = hi;
	addr = lo | (hi << 8);
	cpu->providedAddress = addr;
	if ((addr & 0x00ff) == 0x00ff)
		cpu->targetAddress = readUsingBus(cpu, addr) | (readUsingBus(cpu, addr & 0xff00) << 8);
	else
		cpu->targetAddress = readUsingBus(cpu, addr) | (readUsingBus(cpu, addr + 1) << 8);
	cpu->operand = readUsingBus(cpu, cpu->targetAddress);
}

void IDX(struct cpu *cpu)
{
	uint16_t lo, addr;

	cpu->addressingMode = IndirectX;
	cpu->getOperand = 1;
	lo = (uint16_t)fetchByteFromPC(cpu);
	cpu->opr1 = lo;
	cpu->reg.pc++;
	addr = lo + cpu->reg.x;
	cpu->debug_IDX = addr & 0x00ff;
	cpu->targetAddress = readUsingBus(cpu, addr & 0x00FF) | (readUsingBus(cpu, (addr + 1) & 0x00FF) << 8);
	cpu->operand = readUsingBus(cpu, cpu->targetAddress);
}

void IDY(struct cpu *cpu)
{
	uint16_t lo, addr;

	cpu->addressingMode = IndirectY;
	cpu->getOperand = 1;
	lo = (uint16_t)fetchByteFromPC(cpu);
	cpu->reg.pc++;
	cpu->opr1 = lo;
	addr = lo;
	cpu->targetAddress = readUsingBus(cpu, addr & 0x00FF) | (readUsingBus(cpu, (addr + 1) & 0x00FF) << 8);
	cpu->debug_IDY = cpu->targetAddress;
	cpu->targetAddress += cpu->reg.y;
	cpu->operand = readUsingBus(cpu, cpu->targetAddress);
}

void JAM(struct cpu *cpu)
{
	cpu->addressingMode = Accumulator;
	cpu->dummy = 0;
}

/********************************************************************/
/********************** INSTRUCTIONS ********************************/
/********************************************************************/

uint8_t fetchByteFromPC(struct cpu *cpu)
{
	return readUsingBus(cpu, cpu->reg.pc);
}

void fetchAndExecuteInstruction(struct cpu *cpu, uint8_t *cycles)
{
	uint8_t opcode;


	/* get information for debugging */
	cpu->debug_RegPC = cpu->reg.pc;
	cpu->debug_RegA = cpu->reg.a;
	cpu->debug_RegX = cpu->reg.x;
	cpu->debug_RegY = cpu->reg.y;
	cpu->debug_RegP = cpu->reg.p;
	cpu->debug_RegSP = cpu->reg.sp;

	opcode = fetchByteFromPC(cpu);
	cpu->opcode = opcode;
	cpu->reg.pc++;
	cpu->clockCycles = lookupTable[opcode].cycles;
	*cycles = cpu->clockCycles;
	lookupTable[opcode].addrmode(cpu);
	lookupTable[opcode].handler(cpu);
}

void push(struct cpu *cpu, uint8_t data)
{
	writeUsingBus(cpu, STACK_BEGIN + cpu->reg.sp, data);
	cpu->reg.sp--;
}

uint8_t pop(struct cpu *cpu)
{
	cpu->reg.sp++;
	return readUsingBus(cpu, STACK_BEGIN + cpu->reg.sp);
}

void pushPCtoStack(struct cpu *cpu)
{
	push(cpu, (cpu->reg.pc >> 8) & 0x00FF);
	push(cpu, cpu->reg.pc & 0x00FF);
}

void pullPCfromStack(struct cpu *cpu)
{
	uint16_t pcl, pch;

	pcl = pop(cpu);
	pch = pop(cpu);
	cpu->reg.pc = pcl | (pch << 8);
}

void LDA(struct cpu *cpu)
{
	cpu->reg.a = cpu->operand;
	updateNandZFlags(cpu, cpu->reg.a);
}

void LDX(struct cpu *cpu)
{
	cpu->reg.x = cpu->operand;
	updateNandZFlags(cpu, cpu->reg.x);
}

void LDY(struct cpu *cpu)
{
	cpu->reg.y = cpu->operand;
	updateNandZFlags(cpu, cpu->reg.y);
}

void STA(struct cpu *cpu)
{
	writeUsingBus(cpu, cpu->targetAddress, cpu->reg.a);
}

void STX(struct cpu *cpu)
{
	writeUsingBus(cpu, cpu->targetAddress, cpu->reg.x);
}

void STY(struct cpu *cpu)
{
	writeUsingBus(cpu, cpu->targetAddress, cpu->reg.y);
}

void TAX(struct cpu *cpu)
{
	cpu->reg.x = cpu->reg.a;
	updateNandZFlags(cpu, cpu->reg.x);
}

void TXA(struct cpu *cpu)
{
	cpu->reg.a = cpu->reg.x;
	updateNandZFlags(cpu, cpu->reg.a);
}

void TAY(struct cpu *cpu)
{
	cpu->reg.y = cpu->reg.a;
	updateNandZFlags(cpu, cpu->reg.y);
}

void TYA(struct cpu *cpu)
{
	cpu->reg.a = cpu->reg.y;
	updateNandZFlags(cpu, cpu->reg.a);
}

/* stack operations */
void TSX(struct cpu *cpu)
{
	cpu->reg.x = cpu->reg.sp;
	updateNandZFlags(cpu, cpu->reg.x);
}

void TXS(struct cpu *cpu)
{
	cpu->reg.sp = cpu->reg.x;
}

void PHA(struct cpu *cpu)
{
	push(cpu, cpu->reg.a);
}

void PHP(struct cpu *cpu)
{
	push(cpu, cpu->reg.p | B | NA);
}

void PLA(struct cpu *cpu)
{
	cpu->reg.a = pop(cpu);
	updateNandZFlags(cpu, cpu->reg.a);
}

void PLP(struct cpu *cpu)
{
	cpu->reg.p = pop(cpu);
	setFlag(cpu, NA, 1);
	setFlag(cpu, B, 0);
}

/* logical */
void AND(struct cpu *cpu)
{
	cpu->reg.a &= cpu->operand;
	updateNandZFlags(cpu, cpu->reg.a);
}

void EOR(struct cpu *cpu)
{
	cpu->reg.a ^= cpu->operand;
	updateNandZFlags(cpu, cpu->reg.a);
}

void ORA(struct cpu *cpu)
{
	cpu->reg.a |= cpu->operand;
	updateNandZFlags(cpu, cpu->reg.a);
}

void BIT(struct cpu *cpu)
{
	setFlag(cpu, V, cpu->operand & (1U << 6));
	setFlag(cpu, N, cpu->operand & (1U << 7));
	setFlag(cpu, Z, !(cpu->reg.a & cpu->operand));
}

/* arithmetic */

/* Note. The formula of ADC: a = a + b + c.
	 c is the carry which we can get from
	 the C flag of p register.
*/
void ADC(struct cpu *cpu)
{
	uint8_t c = getFlag(cpu, C);
	uint8_t b = cpu->operand;
	uint8_t a = cpu->reg.a;
	uint16_t res;

	res = a + b + c;
	setFlag(cpu, C, res > 0xFF);
	setFlag(cpu, V, (a ^ res) & (b ^ res) & 0x80);
	setFlag(cpu, N, res & N);
	cpu->reg.a = res & 0x00ff;
	setFlag(cpu, Z, !cpu->reg.a);
}

/* Note. The formula of SBC: a = a + ~b + c.
	 The above formula can get from the 
	 original one: a = a - b - (1-c) using
	 one's complement and two's complement.
*/
void SBC(struct cpu *cpu)
{
	uint8_t c = getFlag(cpu, C);
	uint8_t b = cpu->operand ^ 0x00FF;
	uint8_t a = cpu->reg.a;
	uint16_t res;

	res = a + b + c;
	setFlag(cpu, C, res > 0xFF);
	setFlag(cpu, V, (a ^ res) & (b ^ res) & 0x80);
	setFlag(cpu, N, res & N);
	cpu->reg.a = res & 0x00ff;
	setFlag(cpu, Z, !cpu->reg.a);
}

void CMP(struct cpu *cpu)
{
	setFlag(cpu, C, cpu->reg.a >= cpu->operand);
	setFlag(cpu, Z, cpu->reg.a == cpu->operand);
	setFlag(cpu, N, (cpu->reg.a - cpu->operand) & 0x80);
}

void CPX(struct cpu *cpu)
{
	setFlag(cpu, C, cpu->reg.x >= cpu->operand);
	setFlag(cpu, Z, cpu->reg.x == cpu->operand);
	setFlag(cpu, N, (cpu->reg.x - cpu->operand) & N);
}

void CPY(struct cpu *cpu)
{
	setFlag(cpu, C, cpu->reg.y >= cpu->operand);
	setFlag(cpu, Z, cpu->reg.y == cpu->operand);
	setFlag(cpu, N, (cpu->reg.y - cpu->operand) & N);
}

/* increments & decrements */
void INC(struct cpu *cpu)
{
	uint8_t mem = readUsingBus(cpu, cpu->targetAddress);

	mem++;
	writeUsingBus(cpu, cpu->targetAddress, mem);
	updateNandZFlags(cpu, mem);
}

void INX(struct cpu *cpu)
{
	cpu->reg.x++;
	updateNandZFlags(cpu, cpu->reg.x);
}

void INY(struct cpu *cpu)
{
	cpu->reg.y++;
	updateNandZFlags(cpu, cpu->reg.y);
}

void DEC(struct cpu *cpu)
{
	uint8_t mem = readUsingBus(cpu, cpu->targetAddress);

	mem--;
	writeUsingBus(cpu, cpu->targetAddress, mem);
	updateNandZFlags(cpu, mem);
}

void DEX(struct cpu *cpu)
{
	cpu->reg.x--;
	updateNandZFlags(cpu, cpu->reg.x);
}

void DEY(struct cpu *cpu)
{
	cpu->reg.y--;
	updateNandZFlags(cpu, cpu->reg.y);
}

void ASL(struct cpu *cpu)
{
	uint8_t mem;

	switch (cpu->opcode) {
	case 0x0a:
	 	setFlag(cpu, C, cpu->reg.a & (1U << 7));
		cpu->reg.a <<= 1;
		updateNandZFlags(cpu, cpu->reg.a);
		break;
	default:
		mem = readUsingBus(cpu, cpu->targetAddress);
		setFlag(cpu, C, mem & (1U << 7));
		mem <<= 1;
		writeUsingBus(cpu, cpu->targetAddress, mem);
		updateNandZFlags(cpu, mem);
		break;
	}
}

void LSR(struct cpu *cpu)
{
	uint8_t mem;

	switch (cpu->opcode) {
	case 0x4a:
	 	setFlag(cpu, C, cpu->reg.a & (1U << 0));
		cpu->reg.a >>= 1;
		updateNandZFlags(cpu, cpu->reg.a);
		break;
	default:
		mem = readUsingBus(cpu, cpu->targetAddress);
		setFlag(cpu, C, mem & (1U << 0));
		mem >>= 1;
		writeUsingBus(cpu, cpu->targetAddress, mem);
		updateNandZFlags(cpu, mem);
		break;
	}
}

void ROL(struct cpu *cpu)
{
	uint8_t carry, mem;

	switch (cpu->opcode) {
	case 0x2a:
		carry = !(cpu->reg.a & (1U << 7)) ? 0 : 1;
		cpu->reg.a <<= 1;
		cpu->reg.a |= getFlag(cpu, C);
		setFlag(cpu, C, carry);
		updateNandZFlags(cpu, cpu->reg.a);
		break;
	
	default:
		mem = readUsingBus(cpu, cpu->targetAddress);
		carry = !(mem & (1U << 7)) ? 0 : 1;
		mem <<= 1;
		mem |= getFlag(cpu, C);
		setFlag(cpu, C, carry);
		writeUsingBus(cpu, cpu->targetAddress, mem);
		updateNandZFlags(cpu, mem);
		break;
	}
}

void ROR(struct cpu *cpu)
{
	uint8_t carry, mem;

	switch (cpu->opcode) {
	case 0x6a:
		carry = !(cpu->reg.a & (1U << 0)) ? 0 : 1;
		cpu->reg.a >>= 1;
		cpu->reg.a |= (getFlag(cpu, C) << 7); 
		setFlag(cpu, C, carry);
		updateNandZFlags(cpu, cpu->reg.a);
		break;
	
	default:
		mem = readUsingBus(cpu, cpu->targetAddress);
		carry = !(mem & (1U << 0)) ? 0 : 1;
		mem >>= 1;
		mem |= (getFlag(cpu, C) << 7); 
		writeUsingBus(cpu, cpu->targetAddress, mem);
		setFlag(cpu, C, carry);
		updateNandZFlags(cpu, mem);
		break;
	}
}

/* branches */
void BCC(struct cpu *cpu)
{
	if (!getFlag(cpu, C))
		cpu->reg.pc = cpu->targetAddress;
}

void BCS(struct cpu *cpu)
{
	if (getFlag(cpu, C))
		cpu->reg.pc = cpu->targetAddress;
}

void BEQ(struct cpu *cpu)
{
	if (getFlag(cpu, Z))
		cpu->reg.pc = cpu->targetAddress;
}

void BMI(struct cpu *cpu)
{
	if (getFlag(cpu, N))
		cpu->reg.pc = cpu->targetAddress;
}

void BNE(struct cpu *cpu)
{
	if (!getFlag(cpu, Z))
		cpu->reg.pc = cpu->targetAddress;
}

void BPL(struct cpu *cpu)
{
	if (!getFlag(cpu, N))
		cpu->reg.pc = cpu->targetAddress;
}

void BVC(struct cpu *cpu)
{
	if (!getFlag(cpu, V))
		cpu->reg.pc = cpu->targetAddress;
}

void BVS(struct cpu *cpu)
{
	if (getFlag(cpu, V))
		cpu->reg.pc = cpu->targetAddress;
}

/* status flag changes */
void CLC(struct cpu *cpu)
{
	setFlag(cpu, C, 0);
}

void CLD(struct cpu *cpu)
{
	setFlag(cpu, D, 0);
}

void CLI(struct cpu *cpu)
{
	setFlag(cpu, I, 0);
}

void CLV(struct cpu *cpu)
{
	setFlag(cpu, V, 0);
}

void SEC(struct cpu *cpu)
{
	setFlag(cpu, C, 1);
}

void SED(struct cpu *cpu)
{
	setFlag(cpu, D, 1);
}

void SEI(struct cpu *cpu)
{
	setFlag(cpu, I, 1);
}

/* system functions */
void BRK(struct cpu *cpu)
{
	cpu->reg.pc++;
	pushPCtoStack(cpu);
	push(cpu, cpu->reg.p | B | NA);
	setFlag(cpu, I, 1);
	cpu->reg.pc = readUsingBus(cpu, 0xFFFE) | (readUsingBus(cpu, 0xFFFF) << 8);
}

void NOP(struct cpu *cpu)
{
	cpu->dummy = 0;
}

void NMI(struct cpu *cpu, uint8_t *cycles)
{
	push(cpu, cpu->reg.p | NA);
	pushPCtoStack(cpu);
	setFlag(cpu, I, 1);
	cpu->reg.pc = readUsingBus(cpu, 0xfffa) | (readUsingBus(cpu, 0xfffb) << 8);

	*cycles = 8;
}

void IRQ(struct cpu *cpu, uint8_t *cycles)
{
	push(cpu, cpu->reg.p | NA);
	pushPCtoStack(cpu);
	setFlag(cpu, I, 1);
	cpu->reg.pc = readUsingBus(cpu, 0xfffe) | (readUsingBus(cpu, 0xffff) << 8);

	*cycles = 7;
}

/* jumps and calls */
void JSR(struct cpu *cpu)
{
	cpu->reg.pc--;
	pushPCtoStack(cpu);
	cpu->reg.pc = cpu->targetAddress;
}

void RTI(struct cpu *cpu)
{
	cpu->reg.p = pop(cpu);
	setFlag(cpu, B, 0);
	setFlag(cpu, NA, 1);
	pullPCfromStack(cpu);
}

void RTS(struct cpu *cpu)
{
	pullPCfromStack(cpu);
	cpu->reg.pc++;
}

void JMP(struct cpu *cpu)
{
	cpu->reg.pc = cpu->targetAddress;
}

/* illegal instructions */
void jam(struct cpu *cpu)
{
	cpu->dummy = 0;
}

void SLA(struct cpu *cpu)
{
	cpu->dummy = 0;
}

void RLA(struct cpu *cpu)
{
	uint8_t carry, mem;

	/* ROL part */
	mem = readUsingBus(cpu, cpu->targetAddress);
	carry = !(mem & (1U << 7)) ? 0 : 1;
	mem <<= 1;
	mem |= getFlag(cpu, C);
	setFlag(cpu, C, carry);
	writeUsingBus(cpu, cpu->targetAddress, mem);

	/* AND part */
	cpu->reg.a &= mem;
	updateNandZFlags(cpu, cpu->reg.a);
}

void SRE(struct cpu *cpu)
{
	uint8_t mem;

	/* LSR part */
	mem = readUsingBus(cpu, cpu->targetAddress);
	setFlag(cpu, C, mem & (1U << 0));
	mem >>= 1;
	writeUsingBus(cpu, cpu->targetAddress, mem);
	/* EOR part */
	cpu->reg.a ^= mem;
	updateNandZFlags(cpu, cpu->reg.a);
}

void RRA(struct cpu *cpu)
{
	uint8_t carry, mem;

	/* ROR part */
	mem = readUsingBus(cpu, cpu->targetAddress);
	carry = !(mem & (1U << 0)) ? 0 : 1;
	mem >>= 1;
	mem |= (getFlag(cpu, C) << 7); 
	writeUsingBus(cpu, cpu->targetAddress, mem);
	setFlag(cpu, C, carry);

	/* ADC part */
	uint8_t c = getFlag(cpu, C);
	uint8_t b = mem;
	uint8_t a = cpu->reg.a;
	uint16_t res;

	res = a + b + c;
	setFlag(cpu, C, res > 0xFF);
	setFlag(cpu, V, (a ^ res) & (b ^ res) & 0x80);
	setFlag(cpu, N, res & N);
	cpu->reg.a = res & 0x00ff;
	setFlag(cpu, Z, !cpu->reg.a);
}

void SAX(struct cpu *cpu)
{
	writeUsingBus(cpu, cpu->targetAddress, cpu->reg.a & cpu->reg.x);
}

void SHA(struct cpu *cpu)
{
	cpu->dummy = 0;
}

void LAX(struct cpu *cpu)
{
	cpu->reg.a = cpu->operand;
	cpu->reg.x = cpu->operand;
	updateNandZFlags(cpu, cpu->reg.a);
}

void DCP(struct cpu *cpu)
{
	uint8_t mem = readUsingBus(cpu, cpu->targetAddress);

	mem -= 1;
	writeUsingBus(cpu, cpu->targetAddress, mem);
	setFlag(cpu, C, cpu->reg.a >= mem);
	setFlag(cpu, Z, cpu->reg.a == mem);
	setFlag(cpu, N, (cpu->reg.a - mem) & 0x80);
}

void ISC(struct cpu *cpu)
{
	uint8_t mem = readUsingBus(cpu, cpu->targetAddress);

	mem += 1;
	writeUsingBus(cpu, cpu->targetAddress, mem);

	uint8_t c = getFlag(cpu, C);
	uint8_t b = mem ^ 0x00FF;
	uint8_t a = cpu->reg.a;
	uint16_t res;

	res = a + b + c;
	setFlag(cpu, C, res > 0xFF);
	setFlag(cpu, V, (a ^ res) & (b ^ res) & 0x80);
	setFlag(cpu, N, res & N);
	cpu->reg.a = res & 0x00ff;
	setFlag(cpu, Z, !cpu->reg.a);

}

void SLO(struct cpu *cpu)
{
	uint8_t mem;

	/* ASL part */
	mem = readUsingBus(cpu, cpu->targetAddress);
	setFlag(cpu, C, mem & (1U << 7));
	mem <<= 1;
	writeUsingBus(cpu, cpu->targetAddress, mem);

	/* ORA part */
	cpu->reg.a |= mem;
	updateNandZFlags(cpu, cpu->reg.a);
}

void USBC(struct cpu *cpu)
{
	uint8_t c = getFlag(cpu, C);
	uint8_t b = cpu->operand ^ 0x00FF;
	uint8_t a = cpu->reg.a;
	uint16_t res;

	res = a + b + c;
	setFlag(cpu, C, res > 0xFF);
	setFlag(cpu, V, (a ^ res) & (b ^ res) & 0x80);
	setFlag(cpu, N, res & N);
	cpu->reg.a = res & 0x00ff;
	setFlag(cpu, Z, !cpu->reg.a);
}

void ANC(struct cpu *cpu)
{
	cpu->dummy = 0;
}

void ALR(struct cpu *cpu)
{
	cpu->dummy = 0;
}

void ARR(struct cpu *cpu)
{
	cpu->dummy = 0;
}

void ANE(struct cpu *cpu)
{
	cpu->dummy = 0;
}

void LXA(struct cpu *cpu)
{
	cpu->dummy = 0;
}

void LAS(struct cpu *cpu)
{
	cpu->dummy = 0;
}

void SBX(struct cpu *cpu)
{
	cpu->dummy = 0;
}

void SHY(struct cpu *cpu)
{
	cpu->dummy = 0;
}

void SHX(struct cpu *cpu)
{
	cpu->dummy = 0;
}

void resetCPU(struct cpu *cpu, uint8_t *cycles)
{
	cpu->reg.a = cpu->reg.x = cpu->reg.y = 0;
	cpu->reg.pc = readUsingBus(cpu, 0xFFFC) | (readUsingBus(cpu, 0xFFFD) << 8);
	cpu->reg.sp = 0xFD;
	cpu->reg.p = 0 | NA | I;
	*cycles = 8;
}

