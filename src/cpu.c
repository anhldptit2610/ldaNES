#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../inc/common.h"
#include "../inc/cpu.h"
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
        { " BVS", 0x70, 2, REL, BVS },   { " ADC", 0x71, 5, IDY, ADC },      { " JAM", 0x72, 0, JAM, jam },    { "*RRA", 0x73, 8, IDY, RRA },  { "*NOP", 0x74, 4, ZPX, NOP },    { " ADC", 0x75, 4, ZPX, ADC },   { " ROR", 0x76, 6, ZPX, ROR },  { "*RRA", 0x77, 6, ZPX, RRA },  { " SEI", 0x78, 2, IMP, SEI },  { " ADC", 0x79, 4, ABY, ADC },   { "*NOP", 0x7A, 2, IMP, NOP },     { "*RRA", 0x7B, 7, ABY, RRA },      { "*NOP", 0x7C, 4, ABX, NOP },         { " ADC", 0x7D, 4, ABX, ADC },         { " ROR", 0x7E, 7, ABX, ROR },         { "*RRA", 0x7F, 7, ABX, RRA },
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
void setFlag(struct bus *bus, uint8_t flag, uint8_t condition)
{
	if (!condition)
		bus->cpu->reg.p &= ~flag;
	else
		bus->cpu->reg.p |= flag;
}

void updateNandZFlags(struct bus *bus, uint8_t condition)
{
	setFlag(bus, N, condition & N);
	setFlag(bus, Z, condition == 0);
}

uint8_t getFlag(struct bus *bus, uint8_t flag)
{
	return !(bus->cpu->reg.p & flag) ? 0 : 1;
}

/* helper to debug */

char *getOpcodeName(uint8_t opcode)
{
	return lookupTable[opcode].name;
}

void exportLog(struct bus *bus, FILE *filp)
{
	uint16_t pc = bus->cpu->debug_RegPC;
	uint8_t a = bus->cpu->debug_RegA;
	uint8_t x = bus->cpu->debug_RegX;
	uint8_t y = bus->cpu->debug_RegY;
	uint8_t p = bus->cpu->debug_RegP;
	uint8_t sp = bus->cpu->debug_RegSP;
	uint16_t operand1 = (bus->cpu->getOperand >= 1) ? bus->cpu->opr1 : NOOPR;	
	uint16_t operand2 = (bus->cpu->getOperand == 2) ? bus->cpu->opr2 : NOOPR;	

	switch(bus->cpu->addressingMode) {
	case Immediate:
		fprintf(filp, "%04X  %02X %02X    %s #$%02X                        A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, bus->cpu->opcode, operand1, getOpcodeName(bus->cpu->opcode), bus->cpu->operand, a, x,
			y, p, sp);
		break;
	case Implied:
		fprintf(filp, "%04X  %02X       %s                             A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, bus->cpu->opcode, getOpcodeName(bus->cpu->opcode), a, x, y, p, sp);
		break;
	case ZeroPage:
		fprintf(filp, "%04X  %02X %02X    %s $%02X = %02X                    A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, bus->cpu->opcode, operand1, getOpcodeName(bus->cpu->opcode), bus->cpu->targetAddress, bus->cpu->operand,
			a, x, y, p, sp);
		break;
	case ZeroPageX:
		fprintf(filp, "%04X  %02X %02X    %s $%02X,X @ %02X = %02X             A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, bus->cpu->opcode, operand1, getOpcodeName(bus->cpu->opcode), operand1, bus->cpu->targetAddress,
			bus->cpu->operand, a, x, y, p, sp);
		break;
	case ZeroPageY:
		fprintf(filp, "%04X  %02X %02X    %s $%02X,Y @ %02X = %02X             A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, bus->cpu->opcode, operand1, getOpcodeName(bus->cpu->opcode), operand1, bus->cpu->targetAddress,
			bus->cpu->operand, a, x, y, p, sp);
		break;
	case Relative:
		fprintf(filp, "%04X  %02X %02X    %s $%04X                       A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, bus->cpu->opcode, operand1, getOpcodeName(bus->cpu->opcode), bus->cpu->targetAddress,
			a, x, y, p, sp);
		break;
	case Absolute:
		if (bus->cpu->opcode == 0x4C || bus->cpu->opcode == 0x20) { // JMP
			fprintf(filp, "%04X  %02X %02X %02X %s $%04X                       A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
				pc, bus->cpu->opcode, operand1, operand2, getOpcodeName(bus->cpu->opcode), bus->cpu->targetAddress,
				a, x, y, p, sp);
		} else {
			fprintf(filp, "%04X  %02X %02X %02X %s $%04X = %02X                  A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
				pc, bus->cpu->opcode, operand1, operand2, getOpcodeName(bus->cpu->opcode), bus->cpu->targetAddress, bus->cpu->operand,
				a, x, y, p, sp);
		}
		break;
	case AbsoluteX:
		fprintf(filp, "%04X  %02X %02X %02X %s $%04X,X @ %04X = %02X         A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, bus->cpu->opcode, operand1, operand2, getOpcodeName(bus->cpu->opcode), bus->cpu->providedAddress, bus->cpu->targetAddress,
			bus->cpu->operand, a, x, y, p, sp);
		break;
	case AbsoluteY:
		fprintf(filp, "%04X  %02X %02X %02X %s $%04X,Y @ %04X = %02X         A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, bus->cpu->opcode, operand1, operand2, getOpcodeName(bus->cpu->opcode), bus->cpu->providedAddress, bus->cpu->targetAddress,
			bus->cpu->operand, a, x, y, p, sp);
		break;
	case Indirect:
		fprintf(filp, "%04X  %02X %02X %02X %s ($%04X) = %04X              A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, bus->cpu->opcode, operand1, operand2, getOpcodeName(bus->cpu->opcode), bus->cpu->providedAddress,
			bus->cpu->targetAddress, a, x, y, p, sp);
		break;
	case IndirectX:
		fprintf(filp, "%04X  %02X %02X    %s ($%02X,X) @ %02X = %04X = %02X    A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, bus->cpu->opcode, operand1, getOpcodeName(bus->cpu->opcode), operand1, bus->cpu->debug_IDX,
			bus->cpu->targetAddress, bus->cpu->operand, a, x, y, p, sp);
		break;
	case IndirectY:
		fprintf(filp, "%04X  %02X %02X    %s ($%02X),Y = %04X @ %04X = %02X  A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
			pc, bus->cpu->opcode, operand1, getOpcodeName(bus->cpu->opcode), operand1, bus->cpu->debug_IDY,
			bus->cpu->targetAddress, bus->cpu->operand, a, x, y, p, sp);
		break;
	case Accumulator:
		fprintf(filp, "%04X  %02X       %s A                           A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
				pc, bus->cpu->opcode, getOpcodeName(bus->cpu->opcode), a, x, y, p, sp);
		break;
	default:
		break;
	}
}

/* addressing mode functions */
void IMP(struct bus *bus)
{
	bus->cpu->addressingMode = Implied;
	bus->cpu->getOperand = 0;
}

void ACC(struct bus *bus)
{
	bus->cpu->addressingMode = Accumulator;
	bus->cpu->getOperand = 0;
}

void IMM(struct bus *bus)
{
	bus->cpu->addressingMode = Immediate;
	bus->cpu->getOperand = 1;
	bus->cpu->operand = fetchByteFromPC(bus);
	bus->cpu->opr1 = bus->cpu->operand;
	bus->cpu->reg.pc++;
}

void ZP(struct bus *bus)
{
	uint16_t lo = (uint16_t)fetchByteFromPC(bus);
	
	bus->cpu->opr1 = lo;
	bus->cpu->addressingMode = ZeroPage;
	bus->cpu->getOperand = 1;
	bus->cpu->reg.pc++;
	bus->cpu->targetAddress = lo & 0x00FF;
	bus->cpu->operand = readUsingBus(bus, bus->cpu->targetAddress);
}

void ZPX(struct bus *bus)
{
	bus->cpu->addressingMode = Accumulator;
	uint16_t lo = (uint16_t)fetchByteFromPC(bus);

	bus->cpu->opr1 = lo;
	bus->cpu->addressingMode = ZeroPageX;
	bus->cpu->getOperand = 1;
	bus->cpu->reg.pc++;
	bus->cpu->targetAddress = (lo + bus->cpu->reg.x) & 0x00FF;
	bus->cpu->operand = readUsingBus(bus, bus->cpu->targetAddress);
}

void ZPY(struct bus *bus)
{
	uint16_t lo = (uint16_t)fetchByteFromPC(bus);

	bus->cpu->opr1 = lo;
	bus->cpu->addressingMode = ZeroPageY;
	bus->cpu->getOperand = 1;
	bus->cpu->reg.pc++;
	bus->cpu->targetAddress = (lo + bus->cpu->reg.y) & 0x00ff;
	bus->cpu->operand = readUsingBus(bus, bus->cpu->targetAddress);
}

void REL(struct bus *bus)
{
	uint8_t offset = fetchByteFromPC(bus);

	bus->cpu->addressingMode = Relative;
	bus->cpu->opr1 = offset;
	bus->cpu->getOperand = 1;
	bus->cpu->reg.pc++;
	if (offset & 0x80) {		/* if the offset is negative signed */
		offset ^= 0x00FF;
		offset += 1;
		bus->cpu->targetAddress = bus->cpu->reg.pc - offset;
	} else {
		bus->cpu->targetAddress = bus->cpu->reg.pc + offset;
	}
	if ((bus->cpu->reg.pc & 0xff00) != (bus->cpu->targetAddress & 0xff00))
		bus->cpu->clockCycles += 2;
	else
		bus->cpu->clockCycles += 1;
	bus->cpu->operand = readUsingBus(bus, bus->cpu->targetAddress);
}

void ABS(struct bus *bus)
{
	uint16_t lo, hi;

	bus->cpu->addressingMode = Absolute;
	bus->cpu->getOperand = 2;
	lo = fetchByteFromPC(bus);
	bus->cpu->opr1 = lo;
	bus->cpu->reg.pc++;
	hi = fetchByteFromPC(bus);
	bus->cpu->opr2 = hi;
	bus->cpu->reg.pc++;
	bus->cpu->targetAddress = lo | (hi << 8);
	if (!strcmp("*NOP", getOpcodeName(bus->cpu->opcode))) {
		bus->cpu->operand = lo;
		return;
	}
	bus->cpu->operand = readUsingBus(bus, bus->cpu->targetAddress);
}

void ABX(struct bus *bus)
{
	uint16_t lo, hi;

	bus->cpu->addressingMode = AbsoluteX;
	bus->cpu->getOperand = 2;
	lo = fetchByteFromPC(bus);
	bus->cpu->reg.pc++;
	bus->cpu->opr1 = lo;
	hi = fetchByteFromPC(bus);
	bus->cpu->reg.pc++;
	bus->cpu->opr2 = hi;
	bus->cpu->providedAddress = lo | (hi << 8);
	bus->cpu->targetAddress = (lo | (hi << 8)) + bus->cpu->reg.x;
	if ((bus->cpu->targetAddress & 0xff00) != (bus->cpu->providedAddress & 0xff00))
		bus->cpu->clockCycles++;
	if (bus->cpu->targetAddress == bus->cpu->providedAddress) {
		if (!strcmp("*NOP", getOpcodeName(bus->cpu->opcode))) {
			bus->cpu->operand = lo;
			return;
		}
	}
	bus->cpu->operand = readUsingBus(bus, bus->cpu->targetAddress);
}

void ABY(struct bus *bus)
{
	uint16_t lo, hi;

	bus->cpu->addressingMode = AbsoluteY;
	bus->cpu->getOperand = 2;
	lo = fetchByteFromPC(bus);
	bus->cpu->reg.pc++;
	bus->cpu->opr1 = lo;
	hi = fetchByteFromPC(bus);
	bus->cpu->reg.pc++;
	bus->cpu->opr2 = hi;
	bus->cpu->providedAddress = lo | (hi << 8);
	bus->cpu->targetAddress = (lo | (hi << 8)) + bus->cpu->reg.y;
	if ((bus->cpu->targetAddress & 0xff00) != (bus->cpu->providedAddress & 0xff00))
		bus->cpu->clockCycles++;
	bus->cpu->operand = readUsingBus(bus, bus->cpu->targetAddress);
}

void IND(struct bus *bus)
{
	uint16_t lo, hi, addr;
	struct cpu *cpu = bus->cpu;

	cpu->addressingMode = Indirect;
	cpu->getOperand = 2;
	lo = fetchByteFromPC(bus);
	cpu->reg.pc++;
	cpu->opr1 = lo;
	hi = fetchByteFromPC(bus);
	cpu->reg.pc++;
	cpu->opr2 = hi;
	addr = lo | (hi << 8);
	cpu->providedAddress = addr;
	if ((addr & 0x00ff) == 0x00ff)
		cpu->targetAddress = readUsingBus(bus, addr) | (readUsingBus(bus, addr & 0xff00) << 8);
	else
		cpu->targetAddress = readUsingBus(bus, addr) | (readUsingBus(bus, addr + 1) << 8);
	cpu->operand = readUsingBus(bus, cpu->targetAddress);
}

void IDX(struct bus *bus)
{
	uint16_t lo, addr;
	struct cpu *cpu = bus->cpu;

	cpu->addressingMode = IndirectX;
	cpu->getOperand = 1;
	lo = (uint16_t)fetchByteFromPC(bus);
	cpu->opr1 = lo;
	cpu->reg.pc++;
	addr = lo + cpu->reg.x;
	cpu->debug_IDX = addr & 0x00ff;
	cpu->targetAddress = readUsingBus(bus, addr & 0x00FF) | (readUsingBus(bus, (addr + 1) & 0x00FF) << 8);
	cpu->operand = readUsingBus(bus, cpu->targetAddress);
}

void IDY(struct bus *bus)
{
	uint16_t lo, addr;
	struct cpu *cpu = bus->cpu;

	cpu->addressingMode = IndirectY;
	cpu->getOperand = 1;
	lo = (uint16_t)fetchByteFromPC(bus);
	cpu->reg.pc++;
	cpu->opr1 = lo;
	addr = lo;
	cpu->targetAddress = readUsingBus(bus, addr & 0x00FF) | (readUsingBus(bus, (addr + 1) & 0x00FF) << 8);
	cpu->debug_IDY = cpu->targetAddress;
	cpu->targetAddress += cpu->reg.y;
	if ((cpu->targetAddress & 0xff00) != (cpu->debug_IDY & 0xff00))
		cpu->clockCycles++;
	cpu->operand = readUsingBus(bus, cpu->targetAddress);
}

void JAM(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->addressingMode = Accumulator;
	cpu->dummy = 0;
}

/********************************************************************/
/********************** INSTRUCTIONS ********************************/
/********************************************************************/

uint8_t fetchByteFromPC(struct bus *bus)
{
	return readUsingBus(bus, bus->cpu->reg.pc);
}

void fetchAndExecuteInstruction(struct bus *bus, int *cycles)
{
	uint8_t opcode;


	/* get information for debugging */
	bus->cpu->debug_RegPC = bus->cpu->reg.pc;
	bus->cpu->debug_RegA = bus->cpu->reg.a;
	bus->cpu->debug_RegX = bus->cpu->reg.x;
	bus->cpu->debug_RegY = bus->cpu->reg.y;
	bus->cpu->debug_RegP = bus->cpu->reg.p;
	bus->cpu->debug_RegSP = bus->cpu->reg.sp;

	opcode = fetchByteFromPC(bus);
	bus->cpu->opcode = opcode;
	bus->cpu->reg.pc++;
	bus->cpu->clockCycles = lookupTable[opcode].cycles;
	lookupTable[opcode].addrmode(bus);
	*cycles = bus->cpu->clockCycles;
	lookupTable[opcode].handler(bus);
}

void push(struct bus *bus, uint8_t data)
{
	writeUsingBus(bus, STACK_BEGIN + bus->cpu->reg.sp, data);
	bus->cpu->reg.sp--;
}

uint8_t pop(struct bus *bus)
{
	bus->cpu->reg.sp++;
	return readUsingBus(bus, STACK_BEGIN + bus->cpu->reg.sp);
}

void pushPCtoStack(struct bus *bus)
{
	push(bus, (bus->cpu->reg.pc >> 8) & 0x00FF);
	push(bus, bus->cpu->reg.pc & 0x00FF);
}

void pullPCfromStack(struct bus *bus)
{
	uint16_t pcl, pch;

	pcl = pop(bus);
	pch = pop(bus);
	bus->cpu->reg.pc = pcl | (pch << 8);
}

void LDA(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.a = cpu->operand;
	updateNandZFlags(bus, cpu->reg.a);
}

void LDX(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.x = cpu->operand;
	updateNandZFlags(bus, cpu->reg.x);
}

void LDY(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.y = cpu->operand;
	updateNandZFlags(bus, cpu->reg.y);
}

void STA(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	writeUsingBus(bus, cpu->targetAddress, cpu->reg.a);
}

void STX(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	writeUsingBus(bus, cpu->targetAddress, cpu->reg.x);
}

void STY(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	writeUsingBus(bus, cpu->targetAddress, cpu->reg.y);
}

void TAX(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.x = cpu->reg.a;
	updateNandZFlags(bus, cpu->reg.x);
}

void TXA(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.a = cpu->reg.x;
	updateNandZFlags(bus, cpu->reg.a);
}

void TAY(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.y = cpu->reg.a;
	updateNandZFlags(bus, cpu->reg.y);
}

void TYA(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.a = cpu->reg.y;
	updateNandZFlags(bus, cpu->reg.a);
}

/* stack operations */
void TSX(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.x = cpu->reg.sp;
	updateNandZFlags(bus, cpu->reg.x);
}

void TXS(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.sp = cpu->reg.x;
}

void PHA(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	push(bus, cpu->reg.a);
}

void PHP(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	push(bus, cpu->reg.p | B | NA);
}

void PLA(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.a = pop(bus);
	updateNandZFlags(bus, cpu->reg.a);
}

void PLP(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.p = pop(bus);
	setFlag(bus, NA, 1);
	setFlag(bus, B, 0);
}

/* logical */
void AND(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.a &= cpu->operand;
	updateNandZFlags(bus, cpu->reg.a);
}

void EOR(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.a ^= cpu->operand;
	updateNandZFlags(bus, cpu->reg.a);
}

void ORA(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.a |= cpu->operand;
	updateNandZFlags(bus, cpu->reg.a);
}

void BIT(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	setFlag(bus, V, cpu->operand & (1U << 6));
	setFlag(bus, N, cpu->operand & (1U << 7));
	setFlag(bus, Z, !(cpu->reg.a & cpu->operand));
}

/* arithmetic */

/* Note. The formula of ADC: a = a + b + c.
	 c is the carry which we can get from
	 the C flag of p register.
*/
void ADC(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	uint8_t c = getFlag(bus, C);
	uint8_t b = cpu->operand;
	uint8_t a = cpu->reg.a;
	uint16_t res;

	res = a + b + c;
	setFlag(bus, C, res > 0xFF);
	setFlag(bus, V, (a ^ res) & (b ^ res) & 0x80);
	setFlag(bus, N, res & N);
	cpu->reg.a = res & 0x00ff;
	setFlag(bus, Z, !cpu->reg.a);
}

/* Note. The formula of SBC: a = a + ~b + c.
	 The above formula can get from the 
	 original one: a = a - b - (1-c) using
	 one's complement and two's complement.
*/
void SBC(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	uint8_t c = getFlag(bus, C);
	uint8_t b = cpu->operand ^ 0x00FF;
	uint8_t a = cpu->reg.a;
	uint16_t res;

	res = a + b + c;
	setFlag(bus, C, res > 0xFF);
	setFlag(bus, V, (a ^ res) & (b ^ res) & 0x80);
	setFlag(bus, N, res & N);
	cpu->reg.a = res & 0x00ff;
	setFlag(bus, Z, !cpu->reg.a);
}

void CMP(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	setFlag(bus, C, cpu->reg.a >= cpu->operand);
	setFlag(bus, Z, cpu->reg.a == cpu->operand);
	setFlag(bus, N, (cpu->reg.a - cpu->operand) & 0x80);
}

void CPX(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	setFlag(bus, C, cpu->reg.x >= cpu->operand);
	setFlag(bus, Z, cpu->reg.x == cpu->operand);
	setFlag(bus, N, (cpu->reg.x - cpu->operand) & N);
}

void CPY(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	setFlag(bus, C, cpu->reg.y >= cpu->operand);
	setFlag(bus, Z, cpu->reg.y == cpu->operand);
	setFlag(bus, N, (cpu->reg.y - cpu->operand) & N);
}

/* increments & decrements */
void INC(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	uint8_t mem = readUsingBus(bus, cpu->targetAddress);

	mem++;
	writeUsingBus(bus, cpu->targetAddress, mem);
	updateNandZFlags(bus, mem);
}

void INX(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.x++;
	updateNandZFlags(bus, cpu->reg.x);
}

void INY(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.y++;
	updateNandZFlags(bus, cpu->reg.y);
}

void DEC(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	uint8_t mem = readUsingBus(bus, cpu->targetAddress);

	mem--;
	writeUsingBus(bus, cpu->targetAddress, mem);
	updateNandZFlags(bus, mem);
}

void DEX(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.x--;
	updateNandZFlags(bus, cpu->reg.x);
}

void DEY(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.y--;
	updateNandZFlags(bus, cpu->reg.y);
}

void ASL(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	uint8_t mem;

	switch (cpu->opcode) {
	case 0x0a:
	 	setFlag(bus, C, cpu->reg.a & (1U << 7));
		cpu->reg.a <<= 1;
		updateNandZFlags(bus, cpu->reg.a);
		break;
	default:
		mem = readUsingBus(bus, cpu->targetAddress);
		setFlag(bus, C, mem & (1U << 7));
		mem <<= 1;
		writeUsingBus(bus, cpu->targetAddress, mem);
		updateNandZFlags(bus, mem);
		break;
	}
}

void LSR(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	uint8_t mem;

	switch (cpu->opcode) {
	case 0x4a:
	 	setFlag(bus, C, cpu->reg.a & (1U << 0));
		cpu->reg.a >>= 1;
		updateNandZFlags(bus, cpu->reg.a);
		break;
	default:
		mem = readUsingBus(bus, cpu->targetAddress);
		setFlag(bus, C, mem & (1U << 0));
		mem >>= 1;
		writeUsingBus(bus, cpu->targetAddress, mem);
		updateNandZFlags(bus, mem);
		break;
	}
}

void ROL(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	uint8_t carry, mem;

	switch (cpu->opcode) {
	case 0x2a:
		carry = !(cpu->reg.a & (1U << 7)) ? 0 : 1;
		cpu->reg.a <<= 1;
		cpu->reg.a |= getFlag(bus, C);
		setFlag(bus, C, carry);
		updateNandZFlags(bus, cpu->reg.a);
		break;
	default:
		mem = readUsingBus(bus, cpu->targetAddress);
		carry = !(mem & (1U << 7)) ? 0 : 1;
		mem <<= 1;
		mem |= getFlag(bus, C);
		setFlag(bus, C, carry);
		writeUsingBus(bus, cpu->targetAddress, mem);
		updateNandZFlags(bus, mem);
		break;
	}
}

void ROR(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	uint8_t carry, mem;

	switch (cpu->opcode) {
	case 0x6a:
		carry = !(cpu->reg.a & (1U << 0)) ? 0 : 1;
		cpu->reg.a >>= 1;
		cpu->reg.a |= (getFlag(bus, C) << 7); 
		setFlag(bus, C, carry);
		updateNandZFlags(bus, cpu->reg.a);
		break;
	default:
		mem = readUsingBus(bus, cpu->targetAddress);
		carry = !(mem & (1U << 0)) ? 0 : 1;
		mem >>= 1;
		mem |= (getFlag(bus, C) << 7); 
		writeUsingBus(bus, cpu->targetAddress, mem);
		setFlag(bus, C, carry);
		updateNandZFlags(bus, mem);
		break;
	}
}

/* branches */
void BCC(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	if (!getFlag(bus, C))
		cpu->reg.pc = cpu->targetAddress;
}

void BCS(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	if (getFlag(bus, C))
		cpu->reg.pc = cpu->targetAddress;
}

void BEQ(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	if (getFlag(bus, Z))
		cpu->reg.pc = cpu->targetAddress;
}

void BMI(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	if (getFlag(bus, N))
		cpu->reg.pc = cpu->targetAddress;
}

void BNE(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	if (!getFlag(bus, Z))
		cpu->reg.pc = cpu->targetAddress;
}

void BPL(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	if (!getFlag(bus, N))
		cpu->reg.pc = cpu->targetAddress;
}

void BVC(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	if (!getFlag(bus, V))
		cpu->reg.pc = cpu->targetAddress;
}

void BVS(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	if (getFlag(bus, V))
		cpu->reg.pc = cpu->targetAddress;
}

/* status flag changes */
void CLC(struct bus *bus)
{
	setFlag(bus, C, 0);
}

void CLD(struct bus *bus)
{
	setFlag(bus, D, 0);
}

void CLI(struct bus *bus)
{
	setFlag(bus, I, 0);
}

void CLV(struct bus *bus)
{
	setFlag(bus, V, 0);
}

void SEC(struct bus *bus)
{
	setFlag(bus, C, 1);
}

void SED(struct bus *bus)
{
	setFlag(bus, D, 1);
}

void SEI(struct bus *bus)
{
	setFlag(bus, I, 1);
}

/* system functions */
void BRK(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.pc++;
	pushPCtoStack(bus);
	push(bus, bus->cpu->reg.p | B | NA);
	setFlag(bus, I, 1);
	cpu->reg.pc = readUsingBus(bus, 0xFFFE) | (readUsingBus(bus, 0xFFFF) << 8);
}

void NOP(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->dummy = 0;
}

void NMI(struct bus *bus, uint8_t *cycles)
{
	struct cpu *cpu = bus->cpu;
	push(bus, bus->cpu->reg.p | NA);
	pushPCtoStack(bus);
	setFlag(bus, I, 1);
	cpu->reg.pc = readUsingBus(bus, 0xfffa) | (readUsingBus(bus, 0xfffb) << 8);

	*cycles = 8;
}

void IRQ(struct bus *bus, uint8_t *cycles)
{
	struct cpu *cpu = bus->cpu;
	push(bus, cpu->reg.p | NA);
	pushPCtoStack(bus);
	setFlag(bus, I, 1);
	cpu->reg.pc = readUsingBus(bus, 0xfffe) | (readUsingBus(bus, 0xffff) << 8);

	*cycles = 7;
}

/* jumps and calls */
void JSR(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.pc--;
	pushPCtoStack(bus);
	cpu->reg.pc = cpu->targetAddress;
}

void RTI(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.p = pop(bus);
	setFlag(bus, B, 0);
	setFlag(bus, NA, 1);
	pullPCfromStack(bus);
}

void RTS(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	pullPCfromStack(bus);
	cpu->reg.pc++;
}

void JMP(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.pc = cpu->targetAddress;
}

/* illegal instructions */
void jam(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->dummy = 0;
}

void SLA(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->dummy = 0;
}

void RLA(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	uint8_t carry, mem;

	/* ROL part */
	mem = readUsingBus(bus, cpu->targetAddress);
	carry = !(mem & (1U << 7)) ? 0 : 1;
	mem <<= 1;
	mem |= getFlag(bus, C);
	setFlag(bus, C, carry);
	writeUsingBus(bus, cpu->targetAddress, mem);

	/* AND part */
	cpu->reg.a &= mem;
	updateNandZFlags(bus, cpu->reg.a);
}

void SRE(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	uint8_t mem;

	/* LSR part */
	mem = readUsingBus(bus, cpu->targetAddress);
	setFlag(bus, C, mem & (1U << 0));
	mem >>= 1;
	writeUsingBus(bus, cpu->targetAddress, mem);
	/* EOR part */
	cpu->reg.a ^= mem;
	updateNandZFlags(bus, cpu->reg.a);
}

void RRA(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	uint8_t carry, mem;

	/* ROR part */
	mem = readUsingBus(bus, cpu->targetAddress);
	carry = !(mem & (1U << 0)) ? 0 : 1;
	mem >>= 1;
	mem |= (getFlag(bus, C) << 7); 
	writeUsingBus(bus, cpu->targetAddress, mem);
	setFlag(bus, C, carry);

	/* ADC part */
	uint8_t c = getFlag(bus, C);
	uint8_t b = mem;
	uint8_t a = cpu->reg.a;
	uint16_t res;

	res = a + b + c;
	setFlag(bus, C, res > 0xFF);
	setFlag(bus, V, (a ^ res) & (b ^ res) & 0x80);
	setFlag(bus, N, res & N);
	cpu->reg.a = res & 0x00ff;
	setFlag(bus, Z, !cpu->reg.a);
}

void SAX(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	writeUsingBus(bus, cpu->targetAddress, cpu->reg.a & cpu->reg.x);
}

void SHA(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->dummy = 0;
}

void LAX(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.a = cpu->operand;
	cpu->reg.x = cpu->operand;
	updateNandZFlags(bus, cpu->reg.a);
}

void DCP(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	uint8_t mem = readUsingBus(bus, cpu->targetAddress);

	mem -= 1;
	writeUsingBus(bus, cpu->targetAddress, mem);
	setFlag(bus, C, cpu->reg.a >= mem);
	setFlag(bus, Z, cpu->reg.a == mem);
	setFlag(bus, N, (cpu->reg.a - mem) & 0x80);
}

void ISC(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	uint8_t mem = readUsingBus(bus, cpu->targetAddress);

	mem += 1;
	writeUsingBus(bus, cpu->targetAddress, mem);

	uint8_t c = getFlag(bus, C);
	uint8_t b = mem ^ 0x00FF;
	uint8_t a = cpu->reg.a;
	uint16_t res;

	res = a + b + c;
	setFlag(bus, C, res > 0xFF);
	setFlag(bus, V, (a ^ res) & (b ^ res) & 0x80);
	setFlag(bus, N, res & N);
	cpu->reg.a = res & 0x00ff;
	setFlag(bus, Z, !cpu->reg.a);

}

void SLO(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	uint8_t mem;

	/* ASL part */
	mem = readUsingBus(bus, cpu->targetAddress);
	setFlag(bus, C, mem & (1U << 7));
	mem <<= 1;
	writeUsingBus(bus, cpu->targetAddress, mem);

	/* ORA part */
	cpu->reg.a |= mem;
	updateNandZFlags(bus, cpu->reg.a);
}

void USBC(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	uint8_t c = getFlag(bus, C);
	uint8_t b = cpu->operand ^ 0x00FF;
	uint8_t a = cpu->reg.a;
	uint16_t res;

	res = a + b + c;
	setFlag(bus, C, res > 0xFF);
	setFlag(bus, V, (a ^ res) & (b ^ res) & 0x80);
	setFlag(bus, N, res & N);
	cpu->reg.a = res & 0x00ff;
	setFlag(bus, Z, !cpu->reg.a);
}

void ANC(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->dummy = 0;
}

void ALR(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->dummy = 0;
}

void ARR(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->dummy = 0;
}

void ANE(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->dummy = 0;
}

void LXA(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->dummy = 0;
}

void LAS(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->dummy = 0;
}

void SBX(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->dummy = 0;
}

void SHY(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->dummy = 0;
}

void SHX(struct bus *bus)
{
	struct cpu *cpu = bus->cpu;
	cpu->dummy = 0;
}

void resetCPU(struct bus *bus, int *cycles)
{
	struct cpu *cpu = bus->cpu;
	cpu->reg.a = cpu->reg.x = cpu->reg.y = 0;
	cpu->reg.pc = readUsingBus(bus, 0xFFFC) | (readUsingBus(bus, 0xFFFD) << 8);
	cpu->reg.sp = 0xFD;
	cpu->reg.p = 0 | NA | I;
	*cycles = 8;
}

