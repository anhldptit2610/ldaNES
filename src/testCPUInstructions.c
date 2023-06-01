#include <stdio.h>
#include <stdint.h>
#include "../inc/common.h"
#include "../inc/cpu.h"
#include "../inc/bus.h"
#include <setjmp.h>
#include <unity/unity.h>
#include <unity/unity_internals.h>

struct asmInstruction {
	uint8_t opcode;
	uint8_t operand1;
	uint8_t operand2;
};

void setUp(void)
{
}

void tearDown(void)
{
}

void testInstruction(struct cpu *cpu, struct asmInstruction cmd)
{
	struct bus bus;

	bus.cpu = cpu;

	cpu->mem[cpu->reg.pc] = cmd.opcode;
	cpu->mem[cpu->reg.pc + 1] = cmd.operand1;
	cpu->mem[cpu->reg.pc + 2] = cmd.operand2;
	int cycles = 0;
	fetchAndExecuteInstruction(&bus, &cycles);
}

/* Note. I use testLDA to test all the addressing modes 
         also.
*/

void testLDA(void)
{
	struct asmInstruction LDA;
	struct cpu cpu;

	/* test immediate */
	LDA.opcode = 0xA9;
	LDA.operand1 = 0x44;
	testInstruction(&cpu, LDA);	
	TEST_ASSERT_EQUAL(0x44, cpu.reg.a);
	LDA.operand1 = 0x80;
	testInstruction(&cpu, LDA);
	TEST_ASSERT_EQUAL(0x80, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);

	/* test zeropage */
	LDA.opcode = 0xA5;
	LDA.operand1 = 0x44;
	cpu.mem[0x0044] = 0x00;
	testInstruction(&cpu, LDA);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(Z, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);

	/* test zeropage,x */
	LDA.opcode = 0xB5;
	LDA.operand1 = 0x44;
	cpu.reg.x = 1;
	cpu.mem[0x0045] = 0x80;
	testInstruction(&cpu, LDA);
	TEST_ASSERT_EQUAL(0x80, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	LDA.operand1 = 0x80;
	cpu.reg.x = 0xFF;
	cpu.mem[0x007F] = 0x80;
	testInstruction(&cpu, LDA);
	TEST_ASSERT_EQUAL(0x80, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);

	/* test absolute */
	LDA.opcode = 0xAD;
	LDA.operand1 = 0x00;
	LDA.operand2 = 0x44;
	cpu.mem[0x4400] = 0x00;
	testInstruction(&cpu, LDA);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(Z, cpu.reg.p);

	/* test absolute,x */
	LDA.opcode = 0xBD;
	LDA.operand1 = 0x00;
	LDA.operand2 = 0x44;
	cpu.reg.x = 0xFF;
	cpu.mem[0x44FF] = 0x00;
	testInstruction(&cpu, LDA);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(Z, cpu.reg.p);

	/* test absolute,y */
	LDA.opcode = 0xB9;
	LDA.operand1 = 0x00;
	LDA.operand2 = 0x44;
	cpu.reg.y = 0xFF;
	cpu.mem[0x44FF] = 0x80;
	testInstruction(&cpu, LDA);
	TEST_ASSERT_EQUAL(0x80, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);

	/* test indirect,x */
	LDA.opcode = 0xA1;
	LDA.operand1 = 0x80;
	cpu.reg.x = 0xFF;
	cpu.mem[0x007F] = 0x34;
	cpu.mem[0x0080] = 0x12;
	cpu.mem[0x1234] = 0x80;
	testInstruction(&cpu, LDA);
	TEST_ASSERT_EQUAL(0x80, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);

	/* test indirect,y */
	LDA.opcode = 0xB1;
	LDA.operand1 = 0x80;
	cpu.reg.y = 0xFF;
	cpu.mem[0x0080] = 0x80;
	cpu.mem[0x0081] = 0x00;
	cpu.mem[0x017F] = 0x00;
	testInstruction(&cpu, LDA);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(Z, cpu.reg.p);

}

void testLDX(void)
{	
	struct asmInstruction LDX;
	struct cpu cpu;

	/* test immediate */
	LDX.opcode = 0xA2;
	LDX.operand1 = 0x44;
	testInstruction(&cpu, LDX);	
	TEST_ASSERT_EQUAL(0x44, cpu.reg.x);
	LDX.operand1 = 0x80;
	testInstruction(&cpu, LDX);
	TEST_ASSERT_EQUAL(0x80, cpu.reg.x);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);

	/* test absolute */
	LDX.opcode = 0xAE;
	LDX.operand1 = 0x00;
	LDX.operand2 = 0x44;
	cpu.mem[0x4400] = 0x00;
	testInstruction(&cpu, LDX);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.x);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(Z, cpu.reg.p);

	/* test absolute,y */
	LDX.opcode = 0xBE;
	LDX.operand1 = 0x00;
	LDX.operand2 = 0x44;
	cpu.reg.y = 0xFF;
	cpu.mem[0x44FF] = 0x80;
	testInstruction(&cpu, LDX);
	TEST_ASSERT_EQUAL(0x80, cpu.reg.x);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);

	/* test zeropage */
	LDX.opcode = 0xA6;
	LDX.operand1 = 0x44;
	cpu.mem[0x0044] = 0x00;
	testInstruction(&cpu, LDX);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.x);
	TEST_ASSERT_BITS_HIGH(Z, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);

	/* test zeropage,y */
	LDX.opcode = 0xB6;
	LDX.operand1 = 0x44;
	cpu.reg.y = 1;
	cpu.mem[0x0045] = 0x80;
	testInstruction(&cpu, LDX);
	TEST_ASSERT_EQUAL(0x80, cpu.reg.x);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	LDX.operand1 = 0x80;
	cpu.reg.y = 0xFF;
	cpu.mem[0x007F] = 0x80;
	testInstruction(&cpu, LDX);
	TEST_ASSERT_EQUAL(0x80, cpu.reg.x);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);

}

void testLDY(void)
{
	struct asmInstruction LDY;
	struct cpu cpu;
	int cycles; 
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);

	/* test immediate */
	LDY.opcode = 0xA0;
	LDY.operand1 = 0x44;
	testInstruction(&cpu, LDY);	
	TEST_ASSERT_EQUAL(0x44, cpu.reg.y);
	LDY.operand1 = 0x80;
	testInstruction(&cpu, LDY);
	TEST_ASSERT_EQUAL(0x80, cpu.reg.y);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);

	/* test absolute */
	LDY.opcode = 0xAC;
	LDY.operand1 = 0x00;
	LDY.operand2 = 0x44;
	cpu.mem[0x4400] = 0x00;
	testInstruction(&cpu, LDY);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.y);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(Z, cpu.reg.p);

	/* test absolute,x */
	LDY.opcode = 0xBC;
	LDY.operand1 = 0x00;
	LDY.operand2 = 0x44;
	cpu.reg.x = 0xFF;
	cpu.mem[0x44FF] = 0x80;
	testInstruction(&cpu, LDY);
	TEST_ASSERT_EQUAL(0x80, cpu.reg.y);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);

	/* test zeropage */
	LDY.opcode = 0xA4;
	LDY.operand1 = 0x44;
	cpu.mem[0x0044] = 0x00;
	testInstruction(&cpu, LDY);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.y);
	TEST_ASSERT_BITS_HIGH(Z, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);

	/* test zeropage,x */
	LDY.opcode = 0xB4;
	LDY.operand1 = 0x44;
	cpu.reg.x = 1;
	cpu.mem[0x0045] = 0x80;
	testInstruction(&cpu, LDY);
	TEST_ASSERT_EQUAL(0x80, cpu.reg.y);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	LDY.operand1 = 0x80;
	cpu.reg.x = 0xFF;
	cpu.mem[0x007F] = 0x80;
	testInstruction(&cpu, LDY);
	TEST_ASSERT_EQUAL(0x80, cpu.reg.y);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
}

void testSTA(void)
{
	struct cpu cpu;
	struct asmInstruction STA;

	/* test zeropage */
	STA.opcode = 0x85;
	STA.operand1 = 0x80;
	cpu.reg.a = 0x20;
	testInstruction(&cpu, STA);
	TEST_ASSERT_EQUAL(0x20, cpu.mem[0x0080]);
	
	/* test zeropage,x */
	STA.opcode = 0x95;
	STA.operand1 = 0x80;
	cpu.reg.x = 0xFF;
	cpu.reg.a = 0x20;
	testInstruction(&cpu, STA);
	TEST_ASSERT_EQUAL(0x20, cpu.mem[0x007f]);

	/* test absolute */
	STA.opcode = 0x8D;
	STA.operand1 = 0x80;
	STA.operand2 = 0x23;
	cpu.reg.a = 0x20;
	testInstruction(&cpu, STA);
	TEST_ASSERT_EQUAL(0x20, cpu.mem[0x2380]);
	
	/* test absolute,x */
	STA.opcode = 0x9D;
	STA.operand1 = 0x80;
	STA.operand2 = 0x23;
	cpu.reg.x = 2;
	cpu.reg.a = 0x20;
	testInstruction(&cpu, STA);
	TEST_ASSERT_EQUAL(0x20, cpu.mem[0x2380 + cpu.reg.x]);

	/* test absolute,y */
	STA.opcode = 0x99;
	STA.operand1 = 0x80;
	STA.operand2 = 0x23;
	cpu.reg.y = 2;
	cpu.reg.a = 0x20;
	testInstruction(&cpu, STA);
	TEST_ASSERT_EQUAL(0x20, cpu.mem[0x2380 + cpu.reg.y]);

	/* test (indirect,x) */
	STA.opcode = 0x81;
	STA.operand1 = 0x80;
	cpu.reg.x = 0xFF;
	cpu.reg.a = 0x20;
	cpu.mem[0x007F] = 0x34;
	cpu.mem[0x0080] = 0x12;
	testInstruction(&cpu, STA);
	TEST_ASSERT_EQUAL(0x20, cpu.mem[0x1234]);

	/* test (indirect),y */
	STA.opcode = 0x91;
	STA.operand1 = 0x80;
	cpu.reg.y = 0xFF;
	cpu.reg.a = 0x20;
	cpu.mem[0x0080] = 0x34;
	cpu.mem[0x0081] = 0x12;
	testInstruction(&cpu, STA);
	TEST_ASSERT_EQUAL(0x20, cpu.mem[0x1234 + cpu.reg.y]);

}

void testSTX(void)
{
	struct cpu cpu;
	struct asmInstruction STX;

	/* test zeropage */
	STX.opcode = 0x86;
	STX.operand1 = 0x80;
	cpu.reg.x = 0x20;
	testInstruction(&cpu, STX);
	TEST_ASSERT_EQUAL(0x20, cpu.mem[0x0080]);

	/* test zeropage,y */
	STX.opcode = 0x96;
	STX.operand1 = 0x80;
	cpu.reg.x = 0x20;
	cpu.reg.y = 0xFF;
	testInstruction(&cpu, STX);
	TEST_ASSERT_EQUAL(0x20, cpu.mem[0x007F]);

	/* test absolute */
	STX.opcode = 0x8e;
	STX.operand1 = 0x80;
	STX.operand2 = 0x48;
	cpu.reg.x = 0x20;
	testInstruction(&cpu, STX);
	TEST_ASSERT_EQUAL(0x20, cpu.mem[0x4880]);
}

void testSTY(void)
{
	struct cpu cpu;
	struct asmInstruction STY;

	/* test zeropage */
	STY.opcode = 0x84;
	STY.operand1 = 0x80;
	cpu.reg.y = 0x20;
	testInstruction(&cpu, STY);
	TEST_ASSERT_EQUAL(0x20, cpu.mem[0x0080]);

	/* test zeropage,x */
	STY.opcode = 0x96;
	STY.operand1 = 0x80;
	cpu.reg.y = 0x20;
	cpu.reg.x = 0xFF;
	testInstruction(&cpu, STY);
	TEST_ASSERT_EQUAL(0x20, cpu.mem[0x007F]);

	/* test absolute */
	STY.opcode = 0x8c;
	STY.operand1 = 0x80;
	STY.operand2 = 0x48;
	cpu.reg.y = 0x20;
	testInstruction(&cpu, STY);
	TEST_ASSERT_EQUAL(0x20, cpu.mem[0x4880]);
}

void testTAX(void)
{
	struct cpu cpu;
	struct asmInstruction TAX;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	TAX.opcode = 0xAA;
	cpu.reg.a = 0x80;
	testInstruction(&cpu, TAX);
	TEST_ASSERT_EQUAL(0x80, cpu.reg.x);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	cpu.reg.a = 0x00;
	testInstruction(&cpu, TAX);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.x);
	TEST_ASSERT_BITS_HIGH(Z, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);
}

void testTAY(void)
{
	struct cpu cpu;
	struct asmInstruction TAY;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	TAY.opcode = 0xA8;
	cpu.reg.a = 0x80;
	testInstruction(&cpu, TAY);
	TEST_ASSERT_EQUAL(0x80, cpu.reg.y);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	cpu.reg.a = 0x00;
	testInstruction(&cpu, TAY);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.y);
	TEST_ASSERT_BITS_HIGH(Z, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);
}

void testTXA(void)
{
	struct cpu cpu;
	struct asmInstruction TXA;

	TXA.opcode = 0x8a;
	cpu.reg.x = 0x80;
	testInstruction(&cpu, TXA);
	TEST_ASSERT_EQUAL(0x80, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	cpu.reg.x = 0x00;
	testInstruction(&cpu, TXA);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(Z, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);
}

void testTYA(void)
{
	struct cpu cpu;
	struct asmInstruction TYA;

	TYA.opcode = 0x98;
	cpu.reg.y = 0x80;
	testInstruction(&cpu, TYA);
	TEST_ASSERT_EQUAL(0x80, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	cpu.reg.y = 0x00;
	testInstruction(&cpu, TYA);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(Z, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);
}

void testTSX(void)
{
	struct cpu cpu;
	struct asmInstruction TSX;

	TSX.opcode = 0xba;
	cpu.reg.sp = 0x80;
	testInstruction(&cpu, TSX);
	TEST_ASSERT_EQUAL(0x80, cpu.reg.x);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	cpu.reg.sp = 0x00;
	testInstruction(&cpu, TSX);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.x);
	TEST_ASSERT_BITS_HIGH(Z, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);
}

void testTXS(void)
{
	struct cpu cpu;
	struct asmInstruction TXS;

	TXS.opcode = 0x9a;
	cpu.reg.x = 0x80;
	testInstruction(&cpu, TXS);
	TEST_ASSERT_EQUAL(0x80, cpu.reg.sp);
	cpu.reg.x = 0x00;
	testInstruction(&cpu, TXS);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.sp);
}

void testPHA(void)
{
	struct cpu cpu;
	struct asmInstruction PHA;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	PHA.opcode = 0x48;
	cpu.reg.a = 0x30;
	testInstruction(&cpu, PHA);
	TEST_ASSERT_EQUAL(0x30, cpu.mem[STACK_BEGIN + cpu.reg.sp + 1]);
}

void testPHP(void)
{
	struct cpu cpu;
	struct asmInstruction PHP;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	PHP.opcode = 0x08;
	cpu.reg.p = 0x30;
	testInstruction(&cpu, PHP);
	TEST_ASSERT_EQUAL(0x30 | B | NA, cpu.mem[STACK_BEGIN + cpu.reg.sp + 1]);
}

void testPLA(void)
{
	struct cpu cpu;
	struct asmInstruction PLA;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	cpu.mem[STACK_BEGIN + cpu.reg.sp + 1] = 0x80;
	PLA.opcode = 0x68;
	testInstruction(&cpu, PLA);
	TEST_ASSERT_EQUAL(0x80, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);	
	cpu.mem[STACK_BEGIN + cpu.reg.sp + 1] = 0x00;
	testInstruction(&cpu, PLA);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(Z, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);	
}

void testPLP(void)
{
	struct cpu cpu;
	struct asmInstruction PLP;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	cpu.reg.p = 0x13;
	cpu.mem[STACK_BEGIN + cpu.reg.sp + 1] = 0x3A;
	PLP.opcode = 0x28;
	testInstruction(&cpu, PLP);
	TEST_ASSERT_EQUAL(0x0A, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(Z, cpu.reg.p);

}

void testAND(void)
{
	struct cpu cpu;
	struct asmInstruction AND;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	cpu.reg.a = 0xB4;

	/* test immediate */
	AND.opcode = 0x29;
	AND.operand1 = 0xCE;
	testInstruction(&cpu, AND);
	TEST_ASSERT_EQUAL(0x84, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);

	/* test zeropage */
	cpu.reg.a = 0xB4;
	AND.opcode = 0x25;
	AND.operand1 = 0x80;
	cpu.mem[0x0080] = 0x00;
	testInstruction(&cpu, AND);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(Z, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);

	/* test zeropage,x */
	cpu.reg.a = 0xB4;
	AND.opcode = 0x35;
	AND.operand1 = 0x80;
	cpu.reg.x = 0xFF;
	cpu.mem[0x007F] = 0xCE;
	testInstruction(&cpu, AND);
	TEST_ASSERT_EQUAL(0x84, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);


	/* test absolute */
	cpu.reg.a = 0xB4;
	AND.opcode = 0x2d;
	AND.operand1 = 0x3F;
	AND.operand2 = 0x45;
	cpu.mem[0x453F] = 0xCE;
	testInstruction(&cpu, AND);
	TEST_ASSERT_EQUAL(0x84, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);

	/* test absolute,x */
	cpu.reg.a = 0xB4;
	AND.opcode = 0x3d;
	AND.operand1 = 0x3F;
	AND.operand2 = 0x45;
	cpu.reg.x = 2;
	cpu.mem[0x4541] = 0xCE;
	testInstruction(&cpu, AND);
	TEST_ASSERT_EQUAL(0x84, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);


	/* test absolute,y */
	cpu.reg.a = 0xB4;
	AND.opcode = 0x39;
	AND.operand1 = 0x3F;
	AND.operand2 = 0x45;
	cpu.reg.y = 3;
	cpu.mem[0x4542] = 0xCE;
	testInstruction(&cpu, AND);
	TEST_ASSERT_EQUAL(0x84, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);

	/* test (indirect,x) */
	cpu.reg.a = 0xB4;
	AND.opcode = 0x21;
	AND.operand1 = 0x80;
	cpu.reg.x = 0xFF;
	cpu.mem[0x007F] = 0x34;
	cpu.mem[0x0080] = 0x12;
	cpu.mem[0x1234] = 0xCE;
	testInstruction(&cpu, AND);
	TEST_ASSERT_EQUAL(0x84, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);

	/* test (indirect),y */
	cpu.reg.a = 0xB4;
	AND.opcode = 0x31;
	AND.operand1 = 0x80;
	cpu.mem[0x0080] = 0x34;
	cpu.mem[0x0081] = 0x12;
	cpu.reg.y = 0xFF;
	cpu.mem[0x1333] = 0xCE;
	testInstruction(&cpu, AND);
	TEST_ASSERT_EQUAL(0x84, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
}

void testEOR(void)
{
	struct cpu cpu;
	struct asmInstruction EOR;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	cpu.reg.a = 0xB4;

	/* test immediate */
	EOR.opcode = 0x49;
	EOR.operand1 = 0xCE;
	testInstruction(&cpu, EOR);
	TEST_ASSERT_EQUAL(0x7a, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(N | Z, cpu.reg.p);

	/* test zeropage */
	cpu.reg.a = 0xB4;
	EOR.opcode = 0x45;
	EOR.operand1 = 0x80;
	cpu.mem[0x0080] = 0xB4;
	testInstruction(&cpu, EOR);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(Z, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);

	/* test zeropage,x */
	cpu.reg.a = 0xB4;
	EOR.opcode = 0x55;
	EOR.operand1 = 0x80;
	cpu.reg.x = 0xFF;
	cpu.mem[0x007F] = 0xCE;
	testInstruction(&cpu, EOR);
	TEST_ASSERT_EQUAL(0x7a, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(N | Z, cpu.reg.p);


	/* test absolute */
	cpu.reg.a = 0xB4;
	EOR.opcode = 0x4d;
	EOR.operand1 = 0x3F;
	EOR.operand2 = 0x45;
	cpu.mem[0x453F] = 0xCE;
	testInstruction(&cpu, EOR);
	TEST_ASSERT_EQUAL(0x7a, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(Z | N, cpu.reg.p);

	/* test absolute,x */
	cpu.reg.a = 0xB4;
	EOR.opcode = 0x5d;
	EOR.operand1 = 0x3F;
	EOR.operand2 = 0x45;
	cpu.reg.x = 2;
	cpu.mem[0x4541] = 0xCE;
	testInstruction(&cpu, EOR);
	TEST_ASSERT_EQUAL(0x7a, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(Z | N, cpu.reg.p);


	/* test absolute,y */
	cpu.reg.a = 0xB4;
	EOR.opcode = 0x59;
	EOR.operand1 = 0x3F;
	EOR.operand2 = 0x45;
	cpu.reg.y = 3;
	cpu.mem[0x4542] = 0xCE;
	testInstruction(&cpu, EOR);
	TEST_ASSERT_EQUAL(0x7a, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(Z | N, cpu.reg.p);

	/* test (indirect,x) */
	cpu.reg.a = 0xB4;
	EOR.opcode = 0x41;
	EOR.operand1 = 0x80;
	cpu.reg.x = 0xFF;
	cpu.mem[0x007F] = 0x34;
	cpu.mem[0x0080] = 0x12;
	cpu.mem[0x1234] = 0xCE;
	testInstruction(&cpu, EOR);
	TEST_ASSERT_EQUAL(0x7a, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(Z | N, cpu.reg.p);

	/* test (indirect),y */
	cpu.reg.a = 0xB4;
	EOR.opcode = 0x51;
	EOR.operand1 = 0x80;
	cpu.mem[0x0080] = 0x34;
	cpu.mem[0x0081] = 0x12;
	cpu.reg.y = 0xFF;
	cpu.mem[0x1333] = 0xCE;
	testInstruction(&cpu, EOR);
	TEST_ASSERT_EQUAL(0x7a, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(Z | N, cpu.reg.p);
}

void testORA(void)
{
	struct cpu cpu;
	struct asmInstruction ORA;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	cpu.reg.a = 0xB4;

	/* test immediate */
	ORA.opcode = 0x09;
	ORA.operand1 = 0xCE;
	testInstruction(&cpu, ORA);
	TEST_ASSERT_EQUAL(0xfe, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);

	/* test zeropage */
	cpu.reg.a = 0xB4;
	ORA.opcode = 0x05;
	ORA.operand1 = 0x80;
	cpu.mem[0x0080] = 0xCE;
	testInstruction(&cpu, ORA);
	TEST_ASSERT_EQUAL(0xfe, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);

	/* test zeropage,x */
	cpu.reg.a = 0xB4;
	ORA.opcode = 0x15;
	ORA.operand1 = 0x80;
	cpu.reg.x = 0xFF;
	cpu.mem[0x007F] = 0xCE;
	testInstruction(&cpu, ORA);
	TEST_ASSERT_EQUAL(0xfe, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);


	/* test absolute */
	cpu.reg.a = 0xB4;
	ORA.opcode = 0x0d;
	ORA.operand1 = 0x3F;
	ORA.operand2 = 0x45;
	cpu.mem[0x453F] = 0xCE;
	testInstruction(&cpu, ORA);
	TEST_ASSERT_EQUAL(0xfe, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);

	/* test absolute,x */
	cpu.reg.a = 0xB4;
	ORA.opcode = 0x1d;
	ORA.operand1 = 0x3F;
	ORA.operand2 = 0x45;
	cpu.reg.x = 2;
	cpu.mem[0x4541] = 0xCE;
	testInstruction(&cpu, ORA);
	TEST_ASSERT_EQUAL(0xfe, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);


	/* test absolute,y */
	cpu.reg.a = 0xB4;
	ORA.opcode = 0x19;
	ORA.operand1 = 0x3F;
	ORA.operand2 = 0x45;
	cpu.reg.y = 3;
	cpu.mem[0x4542] = 0xCE;
	testInstruction(&cpu, ORA);
	TEST_ASSERT_EQUAL(0xfe, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);

	/* test (indirect,x) */
	cpu.reg.a = 0xB4;
	ORA.opcode = 0x01;
	ORA.operand1 = 0x80;
	cpu.reg.x = 0xFF;
	cpu.mem[0x007F] = 0x34;
	cpu.mem[0x0080] = 0x12;
	cpu.mem[0x1234] = 0xCE;
	testInstruction(&cpu, ORA);
	TEST_ASSERT_EQUAL(0xfe, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);

	/* test (indirect),y */
	cpu.reg.a = 0xB4;
	ORA.opcode = 0x11;
	ORA.operand1 = 0x80;
	cpu.mem[0x0080] = 0x34;
	cpu.mem[0x0081] = 0x12;
	cpu.reg.y = 0xFF;
	cpu.mem[0x1333] = 0xCE;
	testInstruction(&cpu, ORA);
	TEST_ASSERT_EQUAL(0xfe, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
}

void testBIT(void)
{
	struct cpu cpu;
	struct asmInstruction BIT;
	struct bus bus = {.cpu = &cpu};

	cpu.reg.a = 0x34;

	/* test zeropage */
	BIT.opcode = 0x24;
	BIT.operand1 = 0x8C;
	cpu.mem[0x008C] = 0x8C;
	testInstruction(&cpu, BIT);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(V, cpu.reg.p);

	/* test absolute */
	BIT.opcode = 0x2C;
	BIT.operand1 = 0x34;
	BIT.operand2 = 0x12;
	cpu.mem[0x1234] = 0xc0;
	testInstruction(&cpu, BIT);
	TEST_ASSERT_BITS_HIGH(N | V | Z, cpu.reg.p);

	uint8_t c = getFlag(&bus, N);
	TEST_ASSERT_EQUAL(1, c);
}


/* Note. The test cases for ADC and SBC
         were taken from: 
	 http://www.righto.com/2012/12/the-6502-overflow-flag-explained.html
*/
void testADC(void)
{
	struct cpu cpu;
	struct asmInstruction ADC;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	setFlag(&bus, C, 0);

	/* test immediate */
	setFlag(&bus, C, 0);
	ADC.opcode = 0x69;
	ADC.operand1 = 0x10;
	cpu.reg.a = 0x50;
	testInstruction(&cpu, ADC);
	TEST_ASSERT_EQUAL(0x60, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(C | V, cpu.reg.p);

	/* test zeropage */
	setFlag(&bus, C, 0);
	ADC.opcode = 0x65;
	ADC.operand1 = 0x80;
	cpu.reg.a = 0x50;
	cpu.mem[0x0080] = 0x50;
	testInstruction(&cpu, ADC);
	TEST_ASSERT_EQUAL(0xa0, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(C, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(V, cpu.reg.p);

	/* test zeropage,x */
	setFlag(&bus, C, 0);
	ADC.opcode = 0x75;
	ADC.operand1 = 0x80;
	cpu.reg.x = 0xFF;
	cpu.reg.a = 0x50;
	cpu.mem[0x007F] = 0x90;
	testInstruction(&cpu, ADC);
	TEST_ASSERT_EQUAL(0xe0, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(C | V, cpu.reg.p);

	/* test absolute */
	setFlag(&bus, C, 0);
	ADC.opcode = 0x6d;
	ADC.operand1 = 0x34;
	ADC.operand2 = 0x12;
	cpu.reg.a = 0x50;
	cpu.mem[0x1234] = 0xd0;
	testInstruction(&cpu, ADC);
	TEST_ASSERT_EQUAL(0x20, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(C, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(V, cpu.reg.p);

	/* test absolute,x */
	setFlag(&bus, C, 0);
	ADC.opcode = 0x7d;
	ADC.operand1 = 0x34;
	ADC.operand2 = 0x12;
	cpu.reg.a = 0xd0;
	cpu.reg.x = 0xff;
	cpu.mem[0x1333] = 0x10;
	testInstruction(&cpu, ADC);
	TEST_ASSERT_EQUAL(0xe0, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(C | V, cpu.reg.p);

	/* test absolute,y */
	setFlag(&bus, C, 0);
	ADC.opcode = 0x79;
	ADC.operand1 = 0x34;
	ADC.operand2 = 0x12;
	cpu.reg.a = 0xd0;
	cpu.reg.y = 0xfe;
	cpu.mem[0x1332] = 0x50;
	testInstruction(&cpu, ADC);
	TEST_ASSERT_EQUAL(0x20, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(C, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(V, cpu.reg.p);

	/* test (indirect,x) */
	setFlag(&bus, C, 0);
	ADC.opcode = 0x61;
	ADC.operand1 = 0x80;
	cpu.reg.x = 0xFF;
	cpu.mem[0x007f] = 0x34;
	cpu.mem[0x0080] = 0x12;
	cpu.reg.a = 0xd0;
	cpu.mem[0x1234] = 0x90;
	testInstruction(&cpu, ADC);
	TEST_ASSERT_EQUAL(0x60, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(C | V, cpu.reg.p);

	/* test (indirect),y */
	setFlag(&bus, C, 0);
	ADC.opcode = 0x71;
	ADC.operand1 = 0x80;
	cpu.mem[0x0080] = 0x34;
	cpu.mem[0x0081] = 0x12;
	cpu.reg.y = 0xff;
	cpu.reg.a = 0xd0;
	cpu.mem[0x1333] = 0xd0;
	testInstruction(&cpu, ADC);
	TEST_ASSERT_EQUAL(0xa0, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(V, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(C, cpu.reg.p);
}

void testSBC(void)
{
	struct cpu cpu;
	struct asmInstruction SBC;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	setFlag(&bus, C, 0);

	/* test immediate */
	setFlag(&bus, C, 1);
	SBC.opcode = 0xe9;
	SBC.operand1 = 0xf0;
	cpu.reg.a = 0x50;
	testInstruction(&cpu, SBC);
	TEST_ASSERT_EQUAL(0x60, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(V | C, cpu.reg.p);

	/* test zeropage */
	setFlag(&bus, C, 1);
	SBC.opcode = 0xe5;
	SBC.operand1 = 0x80;
	cpu.reg.a = 0x50;
	cpu.mem[0x0080] = 0xb0;
	testInstruction(&cpu, SBC);
	TEST_ASSERT_EQUAL(0xa0, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(V, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(C, cpu.reg.p);

	/* test zeropage,x */
	setFlag(&bus, C, 1);
	SBC.opcode = 0xf5;
	SBC.operand1 = 0x80;
	cpu.reg.x = 0xFF;
	cpu.reg.a = 0x50;
	cpu.mem[0x007F] = 0x70;
	testInstruction(&cpu, SBC);
	TEST_ASSERT_EQUAL(0xe0, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(V | C, cpu.reg.p);

	/* test absolute */
	setFlag(&bus, C, 1);
	SBC.opcode = 0xed;
	SBC.operand1 = 0x34;
	SBC.operand2 = 0x12;
	cpu.reg.a = 0x50;
	cpu.mem[0x1234] = 0x30;
	testInstruction(&cpu, SBC);
	TEST_ASSERT_EQUAL(0x20, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(V, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(C, cpu.reg.p);

	/* test absolute,x */
	setFlag(&bus, C, 1);
	SBC.opcode = 0xfd;
	SBC.operand1 = 0x34;
	SBC.operand2 = 0x12;
	cpu.reg.a = 0xd0;
	cpu.reg.x = 0xff;
	cpu.mem[0x1333] = 0xf0;
	testInstruction(&cpu, SBC);
	TEST_ASSERT_EQUAL(0xe0, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(V | C, cpu.reg.p);

	/* test absolute,y */
	setFlag(&bus, C, 1);
	SBC.opcode = 0xf9;
	SBC.operand1 = 0x34;
	SBC.operand2 = 0x12;
	cpu.reg.a = 0xd0;
	cpu.reg.y = 0xfe;
	cpu.mem[0x1332] = 0xb0;
	testInstruction(&cpu, SBC);
	TEST_ASSERT_EQUAL(0x20, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(V, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(C, cpu.reg.p);

	/* test (indirect,x) */
	setFlag(&bus, C, 1);
	SBC.opcode = 0xe1;
	SBC.operand1 = 0x80;
	cpu.reg.x = 0xFF;
	cpu.mem[0x007f] = 0x34;
	cpu.mem[0x0080] = 0x12;
	cpu.reg.a = 0xd0;
	cpu.mem[0x1234] = 0x70;
	testInstruction(&cpu, SBC);
	TEST_ASSERT_EQUAL(0x60, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(C | V, cpu.reg.p);

	/* test (indirect),y */
	setFlag(&bus, C, 1);
	SBC.opcode = 0xf1;
	SBC.operand1 = 0x80;
	cpu.mem[0x0080] = 0x34;
	cpu.mem[0x0081] = 0x12;
	cpu.reg.y = 0xff;
	cpu.reg.a = 0xd0;
	cpu.mem[0x1333] = 0x30;
	testInstruction(&cpu, SBC);
	TEST_ASSERT_EQUAL(0xa0, cpu.reg.a);
	TEST_ASSERT_BITS_LOW(V, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(C, cpu.reg.p);
}

void testCMP(void)
{
	struct cpu cpu;
	struct asmInstruction CMP;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	cpu.reg.a = 0x30;

	/* test immediate */
	CMP.opcode = 0xc9;
	CMP.operand1 = 0x30;
	testInstruction(&cpu, CMP);
	TEST_ASSERT_BITS_HIGH(C | Z, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);

	/* test zeropage */
	CMP.opcode = 0xc5;
	CMP.operand1 = 0x80;
	cpu.mem[0x0080] = 0x80;
	testInstruction(&cpu, CMP);
	TEST_ASSERT_BITS_LOW(C | Z, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);

	/* test zeropage,x */
	CMP.opcode = 0xd5;
	CMP.operand1 = 0x80;
	cpu.reg.x = 0xff;
	cpu.mem[0x007f] = 0x32;
	testInstruction(&cpu, CMP);
	TEST_ASSERT_BITS_LOW(C | Z, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);

	/* test aboslute */
	CMP.opcode = 0xcd;
	CMP.operand1 = 0x39;
	CMP.operand2 = 0xff;
	cpu.mem[0xff39] = 0x29;
	testInstruction(&cpu, CMP);
	TEST_ASSERT_BITS_HIGH(C, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z | N, cpu.reg.p);

	/* test absolute,x */
	CMP.opcode = 0xdd;
	CMP.operand1 = 0x39;
	CMP.operand2 = 0xff;
	cpu.reg.x = 0x02;
	cpu.mem[0xff42] = 0x29;
	testInstruction(&cpu, CMP);
	TEST_ASSERT_BITS_HIGH(C, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z | N, cpu.reg.p);

	/* test absolute,y */
	CMP.opcode = 0xd9;
	CMP.operand1 = 0x39;
	CMP.operand2 = 0xff;
	cpu.reg.y = 0x05;
	cpu.mem[0xff44] = 0x29;
	testInstruction(&cpu, CMP);
	TEST_ASSERT_BITS_HIGH(C, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z | N, cpu.reg.p);

	/* test (indirect,x) */
	CMP.opcode = 0xc1;
	CMP.operand1 = 0x80;
	cpu.reg.x = 0xff;
	cpu.mem[0x7f] = 0x34;
	cpu.mem[0x80] = 0x12;
	cpu.mem[0x1234] = 0x30;
	testInstruction(&cpu, CMP);
	TEST_ASSERT_BITS_HIGH(Z | C, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);

	/* test (indirect),y */
	CMP.opcode = 0xd1;
	CMP.operand1 = 0x80;
	cpu.mem[0x0080] = 0x34;
	cpu.mem[0x0081] = 0x12;
	cpu.reg.y = 0xff;
	cpu.mem[0x1333] = 0x80;
	testInstruction(&cpu, CMP);
	TEST_ASSERT_BITS_LOW(C | Z, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
}

void testCPX(void)
{
	struct cpu cpu;
	struct asmInstruction CPX;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	cpu.reg.x = 0x40;

	/* test immediate */
	CPX.opcode = 0xe0;
	CPX.operand1 = 0x40;
	testInstruction(&cpu, CPX);
	TEST_ASSERT_BITS_HIGH(C | Z, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);

	/* test zeropage */
	CPX.opcode = 0xe4;
	CPX.operand1 = 0x90;
	cpu.mem[0x0090] = 0x50;
	testInstruction(&cpu, CPX);
	TEST_ASSERT_BITS_LOW(C | Z, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);

	/* test absolute */
	CPX.opcode = 0xec;
	CPX.operand1 = 0x80;
	CPX.operand2 = 0xcb;
	cpu.mem[0xcb80] = 0x50;
	testInstruction(&cpu, CPX);
	TEST_ASSERT_BITS_LOW(C | Z, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
}

void testCPY(void)
{
	struct cpu cpu;
	struct asmInstruction CPY;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	cpu.reg.y = 0x40;

	/* test immediate */
	CPY.opcode = 0xc0;
	CPY.operand1 = 0x40;
	testInstruction(&cpu, CPY);
	TEST_ASSERT_BITS_HIGH(C | Z, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);

	/* test zeropage */
	CPY.opcode = 0xc4;
	CPY.operand1 = 0x90;
	cpu.mem[0x0090] = 0x50;
	testInstruction(&cpu, CPY);
	TEST_ASSERT_BITS_LOW(C | Z, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);

	/* test absolute */
	CPY.opcode = 0xcc;
	CPY.operand1 = 0x80;
	CPY.operand2 = 0xcb;
	cpu.mem[0xcb80] = 0x50;
	testInstruction(&cpu, CPY);
	TEST_ASSERT_BITS_LOW(C | Z, cpu.reg.p);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
}

void testINC(void)
{
	struct cpu cpu;
	struct asmInstruction INC;

	/* test zeropage */
	INC.opcode = 0xe6;
	INC.operand1 = 0x80;
	cpu.mem[0x0080] = 0x9f;
	testInstruction(&cpu, INC);
	TEST_ASSERT_EQUAL(0xa0, cpu.mem[0x0080]);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);

	/* test zeropage,x */
	INC.opcode = 0xf6;
	INC.operand1 = 0x80;
	cpu.reg.x = 0xff;
	cpu.mem[0x007f] = 0xff;
	testInstruction(&cpu, INC);
	TEST_ASSERT_EQUAL(0x00, cpu.mem[0x007f]);
	TEST_ASSERT_BITS_HIGH(Z, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);

	/* test absolute */
	INC.opcode = 0xee;
	INC.operand1 = 0x34;
	INC.operand2 = 0x12;
	cpu.mem[0x1234] = 0xce;
	testInstruction(&cpu, INC);
	TEST_ASSERT_EQUAL(0xcf, cpu.mem[0x1234]);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);

	/* test absolute,x */
	INC.opcode = 0xfe;
	INC.operand1 = 0x34;
	INC.operand2 = 0x12;
	cpu.reg.x = 0xff;
	cpu.mem[0x1333] = 0x9f;
	testInstruction(&cpu, INC);
	TEST_ASSERT_EQUAL(0xa0, cpu.mem[0x1333]);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
}

void testINX(void)
{
	struct cpu cpu;
	struct asmInstruction INX;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	INX.opcode = 0xe8;
	cpu.reg.x = 0x9f;
	testInstruction(&cpu, INX);
	TEST_ASSERT_EQUAL(0xa0, cpu.reg.x);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
}

void testINY(void)
{
	struct cpu cpu;
	struct asmInstruction INY;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	INY.opcode = 0xc8;
	cpu.reg.y = 0x9f;
	testInstruction(&cpu, INY);
	TEST_ASSERT_EQUAL(0xa0, cpu.reg.y);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
}

void testDEC(void)
{
	struct cpu cpu;
	struct asmInstruction DEC;

	/* test zeropage */
	DEC.opcode = 0xc6;
	DEC.operand1 = 0x80;
	cpu.mem[0x0080] = 0xa1;
	testInstruction(&cpu, DEC);
	TEST_ASSERT_EQUAL(0xa0, cpu.mem[0x0080]);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);

	/* test zeropage,x */
	DEC.opcode = 0xd6;
	DEC.operand1 = 0x80;
	cpu.reg.x = 0xff;
	cpu.mem[0x007f] = 0x04;
	testInstruction(&cpu, DEC);
	TEST_ASSERT_EQUAL(0x03, cpu.mem[0x007f]);
	TEST_ASSERT_BITS_LOW(N | Z, cpu.reg.p);

	/* test absolute */
	DEC.opcode = 0xce;
	DEC.operand1 = 0x34;
	DEC.operand2 = 0x12;
	cpu.mem[0x1234] = 0x01;
	testInstruction(&cpu, DEC);
	TEST_ASSERT_EQUAL(0x00, cpu.mem[0x1234]);
	TEST_ASSERT_BITS_HIGH(Z, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);

	/* test absolute,x */
	DEC.opcode = 0xde;
	DEC.operand1 = 0x34;
	DEC.operand2 = 0x12;
	cpu.reg.x = 0xff;
	cpu.mem[0x1333] = 0x00;
	testInstruction(&cpu, DEC);
	TEST_ASSERT_EQUAL(0xff, cpu.mem[0x1333]);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);
}

void testDEX(void)
{
	struct cpu cpu;
	struct asmInstruction DEX;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	DEX.opcode = 0xca;
	cpu.reg.x = 0x01;
	testInstruction(&cpu, DEX);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.x);
	TEST_ASSERT_BITS_HIGH(Z, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);
}

void testASL(void)
{
	struct cpu cpu;
	struct asmInstruction ASL;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);

	/* test accumulator */
	ASL.opcode = 0x0a;
	cpu.reg.a = 0xc2;
	setFlag(&bus, C, 0);
	testInstruction(&cpu, ASL);
	TEST_ASSERT_EQUAL(0x84, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(C | N, cpu.reg.p);

	/* test zeropage */
	ASL.opcode = 0x06;
	ASL.operand1 = 0x80;
	cpu.mem[0x0080] = 0x80;
	setFlag(&bus, C, 0);
	testInstruction(&cpu, ASL);
	TEST_ASSERT_EQUAL(0x00, cpu.mem[0x0080]);
	TEST_ASSERT_BITS_HIGH(C | Z, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);

	/* test zeropage,x */
	ASL.opcode = 0x16;
	ASL.operand1 = 0x80;
	cpu.reg.x = 0xff;
	cpu.mem[0x007f] = 0xc2;
	setFlag(&bus, C, 0);
	testInstruction(&cpu, ASL);
	TEST_ASSERT_EQUAL(0x84, cpu.mem[0x007f]);
	TEST_ASSERT_BITS_HIGH(C | N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);

	/* test absolute */
	ASL.opcode = 0x0e;
	ASL.operand1 = 0x34;
	ASL.operand2 = 0x12;
	cpu.mem[0x1234] = 0x22;
	testInstruction(&cpu, ASL);
	TEST_ASSERT_EQUAL(0x44, cpu.mem[0x1234]);
	TEST_ASSERT_BITS_LOW(N | C | Z, cpu.reg.p);

	/* test absolute,x */
	ASL.opcode = 0x1e;
	ASL.operand1 = 0x34;
	ASL.operand2 = 0x12;
	cpu.mem[0x1333] = 0x22;
	testInstruction(&cpu, ASL);
	TEST_ASSERT_EQUAL(0x44, cpu.mem[0x1333]);
	TEST_ASSERT_BITS_LOW(N | C | Z, cpu.reg.p);
}

void testLSR(void)
{
	struct cpu cpu;
	struct asmInstruction LSR;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);

	/* test accumulator */
	LSR.opcode = 0x4a;
	cpu.reg.a = 0x01;
	setFlag(&bus, C, 0);
	testInstruction(&cpu, LSR);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(C | Z, cpu.reg.p);

	/* test zeropage */
	LSR.opcode = 0x46;
	LSR.operand1 = 0x80;
	cpu.mem[0x0080] = 0x3e;
	testInstruction(&cpu, LSR);
	TEST_ASSERT_EQUAL(0x1F, cpu.mem[0x0080]);
	TEST_ASSERT_BITS_LOW(C, cpu.reg.p);

	/* test zeropage,x */
	LSR.opcode = 0x56;
	LSR.operand1 = 0x80;
	cpu.reg.x = 0xff;
	cpu.mem[0x007f] = 0x3e;
	testInstruction(&cpu, LSR);
	TEST_ASSERT_EQUAL(0x1F, cpu.mem[0x0080]);
	TEST_ASSERT_BITS_LOW(C, cpu.reg.p);

	/* test absolute */
	LSR.opcode = 0x4e;
	LSR.operand1 = 0x3a;
	LSR.operand2 = 0xbb;
	cpu.mem[0xbb3a] = 0x01;
	testInstruction(&cpu, LSR);
	TEST_ASSERT_EQUAL(0x00, cpu.mem[0xbb3a]);
	TEST_ASSERT_BITS_HIGH(C | Z, cpu.reg.p);

	/* test absolute,x */
	LSR.opcode = 0x5e;
	LSR.operand1 = 0x3a;
	LSR.operand2 = 0xbb;
	cpu.reg.x = 0x05;
	cpu.mem[0xbb3f] = 0x01;
	testInstruction(&cpu, LSR);
	TEST_ASSERT_EQUAL(0x00, cpu.mem[0xbb3f]);
	TEST_ASSERT_BITS_HIGH(C | Z, cpu.reg.p);
}

void testROL(void)
{
	struct cpu cpu;
	struct asmInstruction ROL;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);

	/* test accumulator */
	ROL.opcode = 0x2a;
	setFlag(&bus, C, 0);
	cpu.reg.a = 0x80;
	testInstruction(&cpu, ROL);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(Z | C, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);

	/* test zeropage */
	ROL.opcode = 0x26;
	ROL.operand1 = 0x34;
	setFlag(&bus, C, 1);
	cpu.mem[0x0034] = 0x80;
	testInstruction(&cpu, ROL);
	TEST_ASSERT_EQUAL(0x01, cpu.mem[0x0034]);
	TEST_ASSERT_BITS_HIGH(C, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z | N, cpu.reg.p);

	/* test zeropage,x */
	ROL.opcode = 0x36;
	ROL.operand1 = 0x80;
	cpu.reg.x = 0xff;
	setFlag(&bus, C, 1);
	cpu.mem[0x007f] = 0xc0;
	testInstruction(&cpu, ROL);
	TEST_ASSERT_EQUAL(0x81, cpu.mem[0x007f]);
	TEST_ASSERT_BITS_HIGH(C, cpu.reg.p);

	/* test absolute */
	ROL.opcode = 0x2e;
	ROL.operand1 = 0x34;
	ROL.operand2 = 0x12;
	cpu.mem[0x1234] = 0x9c;
	setFlag(&bus, C, 0);
	testInstruction(&cpu, ROL);
	TEST_ASSERT_EQUAL(0x38, cpu.mem[0x1234]);
	TEST_ASSERT_BITS_HIGH(C, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z | N, cpu.reg.p);

	/* test absolute,x */
	ROL.opcode = 0x3e;
	ROL.operand1 = 0x34;
	ROL.operand2 = 0x12;
	cpu.reg.x = 0xff;
	cpu.mem[0x1333] = 0x9c;
	setFlag(&bus, C, 0);
	testInstruction(&cpu, ROL);
	TEST_ASSERT_EQUAL(0x38, cpu.mem[0x1234]);
	TEST_ASSERT_BITS_HIGH(C, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z | N, cpu.reg.p);
}

void testROR(void)
{
	struct cpu cpu;
	struct asmInstruction ROR;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);

	/* test accumulator */
	ROR.opcode = 0x6a;
	setFlag(&bus, C, 0);
	cpu.reg.a = 0x01;
	testInstruction(&cpu, ROR);
	TEST_ASSERT_EQUAL(0x00, cpu.reg.a);
	TEST_ASSERT_BITS_HIGH(Z | C, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(N, cpu.reg.p);

	/* test zeropage */
	ROR.opcode = 0x66;
	ROR.operand1 = 0x34;
	setFlag(&bus, C, 1);
	cpu.mem[0x0034] = 0x01;
	testInstruction(&cpu, ROR);
	TEST_ASSERT_EQUAL(0x80, cpu.mem[0x0034]);
	TEST_ASSERT_BITS_HIGH(C | N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(Z, cpu.reg.p);

	/* test zeropage,x */
	ROR.opcode = 0x76;
	ROR.operand1 = 0x80;
	cpu.reg.x = 0xff;
	setFlag(&bus, C, 1);
	cpu.mem[0x007f] = 0xc0;
	testInstruction(&cpu, ROR);
	TEST_ASSERT_EQUAL(0xe0, cpu.mem[0x007f]);
	TEST_ASSERT_BITS_HIGH(N, cpu.reg.p);
	TEST_ASSERT_BITS_LOW(C | Z, cpu.reg.p);

	/* test absolute */
	ROR.opcode = 0x6e;
	ROR.operand1 = 0x34;
	ROR.operand2 = 0x12;
	cpu.mem[0x1234] = 0x9c;
	setFlag(&bus, C, 0);
	testInstruction(&cpu, ROR);
	TEST_ASSERT_EQUAL(0x4e, cpu.mem[0x1234]);
	TEST_ASSERT_BITS_LOW(C | N | Z, cpu.reg.p);

	/* test absolute,x */
	ROR.opcode = 0x7e;
	ROR.operand1 = 0x34;
	ROR.operand2 = 0x12;
	cpu.reg.x = 0xff;
	cpu.mem[0x1333] = 0x9c;
	setFlag(&bus, C, 0);
	testInstruction(&cpu, ROR);
	TEST_ASSERT_EQUAL(0x4e, cpu.mem[0x1234]);
	TEST_ASSERT_BITS_LOW(C | N | Z, cpu.reg.p);
}

void testBCC(void)
{
	struct cpu cpu;
	struct asmInstruction BCC;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	cpu.reg.x = 0x04;
	cpu.reg.pc = 0x8014;
	setFlag(&bus, C, 0);
	cpu.mem[0x8010] = 0xe8; 

	BCC.opcode = 0x90;
	BCC.operand1 = 0xFA;
	testInstruction(&cpu, BCC);
	fetchAndExecuteInstruction(&bus, &cycles);
	TEST_ASSERT_EQUAL(0x05, cpu.reg.x);
	TEST_ASSERT_BITS_LOW(N | Z, cpu.reg.p);
}

void testBCS(void)
{
	struct cpu cpu;
	struct asmInstruction BCS;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	cpu.reg.x = 0x04;
	cpu.reg.pc = 0x8014;
	setFlag(&bus, C, 1);
	cpu.mem[0x801c] = 0xe8; 

	BCS.opcode = 0xb0;
	BCS.operand1 = 0x06;
	testInstruction(&cpu, BCS);
	fetchAndExecuteInstruction(&bus, &cycles);
	TEST_ASSERT_EQUAL(0x05, cpu.reg.x);
	TEST_ASSERT_BITS_LOW(N | Z, cpu.reg.p);
}

void testBEQ(void)
{
	struct cpu cpu;
	struct asmInstruction BEQ;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	cpu.reg.x = 0x04;
	cpu.reg.a = 0x20;
	cpu.reg.pc = 0x8014;
	setFlag(&bus, Z, 1);
	cpu.mem[0x8010] = 0xe8; 

	BEQ.opcode = 0xf0;
	BEQ.operand1 = 0xfa;
	testInstruction(&cpu, BEQ);
	fetchAndExecuteInstruction(&bus, &cycles);
	TEST_ASSERT_EQUAL(0x05, cpu.reg.x);
	TEST_ASSERT_BITS_LOW(N | Z, cpu.reg.p);
}

void testBMI(void)
{
	struct cpu cpu;
	struct asmInstruction BMI;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	cpu.reg.x = 0x04;
	cpu.reg.a = 0x20;
	cpu.reg.pc = 0x8014;
	setFlag(&bus, N, 1);
	cpu.mem[0x8024] = 0xe8; 

	BMI.opcode = 0x30;
	BMI.operand1 = 0x0e;
	testInstruction(&cpu, BMI);
	fetchAndExecuteInstruction(&bus, &cycles);
	TEST_ASSERT_EQUAL(0x05, cpu.reg.x);
	TEST_ASSERT_BITS_LOW(N | Z, cpu.reg.p);
}

void testBNE(void)
{
	struct cpu cpu;
	struct asmInstruction BNE;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	cpu.reg.x = 0x04;
	cpu.reg.a = 0x20;
	cpu.reg.pc = 0x8014;
	setFlag(&bus, Z, 0);
	cpu.mem[0x8024] = 0xe8; 

	BNE.opcode = 0xd0;
	BNE.operand1 = 0x0e;
	testInstruction(&cpu, BNE);
	fetchAndExecuteInstruction(&bus, &cycles);
	TEST_ASSERT_EQUAL(0x05, cpu.reg.x);
	TEST_ASSERT_BITS_LOW(N | Z, cpu.reg.p);
}

void testBPL(void)
{
	struct cpu cpu;
	struct asmInstruction BPL;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	cpu.reg.x = 0x04;
	cpu.reg.a = 0x20;
	cpu.reg.pc = 0x8014;
	setFlag(&bus, Z, 0);
	cpu.mem[0x8024] = 0xe8; 

	BPL.opcode = 0x10;
	BPL.operand1 = 0x0e;
	testInstruction(&cpu, BPL);
	fetchAndExecuteInstruction(&bus, &cycles);
	TEST_ASSERT_EQUAL(0x05, cpu.reg.x);
	TEST_ASSERT_BITS_LOW(N | Z, cpu.reg.p);
}

void testBVC(void)
{
	struct cpu cpu;
	struct asmInstruction BVC;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	cpu.reg.x = 0x04;
	cpu.reg.a = 0x20;
	cpu.reg.pc = 0x8014;
	setFlag(&bus, V, 0);
	cpu.mem[0x8024] = 0xe8; 

	BVC.opcode = 0x50;
	BVC.operand1 = 0x0e;
	testInstruction(&cpu, BVC);
	fetchAndExecuteInstruction(&bus, &cycles);
	TEST_ASSERT_EQUAL(0x05, cpu.reg.x);
	TEST_ASSERT_BITS_LOW(N | Z, cpu.reg.p);
}

void testBVS(void)
{
	struct cpu cpu;
	struct asmInstruction BVS;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	cpu.reg.x = 0x04;
	cpu.reg.a = 0x20;
	cpu.reg.pc = 0x8014;
	setFlag(&bus, V, 1);
	cpu.mem[0x8024] = 0xe8; 

	BVS.opcode = 0x70;
	BVS.operand1 = 0x0e;
	testInstruction(&cpu, BVS);
	fetchAndExecuteInstruction(&bus, &cycles);
	TEST_ASSERT_EQUAL(0x05, cpu.reg.x);
	TEST_ASSERT_BITS_LOW(N | Z, cpu.reg.p);
}

void testDEY(void)
{
	struct cpu cpu;
	struct asmInstruction DEY;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	DEY.opcode = 0x88;
	cpu.reg.y = 0x4e;
	testInstruction(&cpu, DEY);
	TEST_ASSERT_EQUAL(0x4d, cpu.reg.y);
	TEST_ASSERT_BITS_LOW(N | Z, cpu.reg.p);
}

void testCLC(void)
{
	struct cpu cpu;
	struct asmInstruction CLC;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	setFlag(&bus, C, 1);
	CLC.opcode = 0x18;
	testInstruction(&cpu, CLC);
	TEST_ASSERT_BITS_LOW(C, cpu.reg.p);
}

void testCLI(void)
{
	struct cpu cpu;
	struct asmInstruction CLI;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	setFlag(&bus, I, 1);
	CLI.opcode = 0x58;
	testInstruction(&cpu, CLI);
	TEST_ASSERT_BITS_LOW(I, cpu.reg.p);
}

void testCLD(void)
{
	struct cpu cpu;
	int cycles = 0;
	struct asmInstruction CLD;
	struct bus bus = {.cpu = &cpu};
	resetCPU(&bus, &cycles);
	setFlag(&bus, D, 1);
	CLD.opcode = 0xd8;
	testInstruction(&cpu, CLD);
	TEST_ASSERT_BITS_LOW(D, cpu.reg.p);
}

void testCLV(void)
{
	struct cpu cpu;
	int cycles = 0;
	struct asmInstruction CLV;
	struct bus bus = {.cpu = &cpu};
	resetCPU(&bus, &cycles);
	setFlag(&bus, V, 1);
	CLV.opcode = 0xb8;
	testInstruction(&cpu, CLV);
	TEST_ASSERT_BITS_LOW(V, cpu.reg.p);
}

void testSEC(void)
{
	struct cpu cpu;
	int cycles = 0;
	struct asmInstruction SEC;
	struct bus bus = {.cpu = &cpu};
	resetCPU(&bus, &cycles);
	setFlag(&bus, C, 0);
	SEC.opcode = 0x38;
	testInstruction(&cpu, SEC);
	TEST_ASSERT_BITS_HIGH(C, cpu.reg.p);
}

void testSED(void)
{
	struct cpu cpu;
	int cycles = 0;
	struct asmInstruction SED;
	struct bus bus = {.cpu = &cpu};
	resetCPU(&bus, &cycles);
	setFlag(&bus, D, 1);
	SED.opcode = 0xf8;
	testInstruction(&cpu, SED);
	TEST_ASSERT_BITS_HIGH(D, cpu.reg.p);
}

void testSEI(void)
{
	struct cpu cpu;
	int cycles = 0;
	struct asmInstruction SEI;
	struct bus bus = {.cpu = &cpu};
	resetCPU(&bus, &cycles);
	setFlag(&bus, C, 1);
	SEI.opcode = 0x78;
	testInstruction(&cpu, SEI);
	TEST_ASSERT_BITS_HIGH(I, cpu.reg.p);
}

void testJSR(void)
{
	struct cpu cpu;
	struct asmInstruction JSR;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	cpu.reg.pc = 0x8001;
	JSR.opcode = 0x20;
	JSR.operand1 = 0x45;
	JSR.operand2 = 0x90;
	testInstruction(&cpu, JSR);
	TEST_ASSERT_EQUAL(0x9045, cpu.reg.pc);
	TEST_ASSERT_EQUAL(0x03, cpu.mem[STACK_BEGIN + cpu.reg.sp + 1]);
	TEST_ASSERT_EQUAL(0x80, cpu.mem[STACK_BEGIN + cpu.reg.sp + 2]);

}
void testRTI(void)
{
	struct cpu cpu;
	struct asmInstruction RTI;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	cpu.reg.sp = 0xf0;
	cpu.mem[STACK_BEGIN + cpu.reg.sp + 1] = 0xb3;
	cpu.mem[STACK_BEGIN + cpu.reg.sp + 2] = 0x03;
	cpu.mem[STACK_BEGIN + cpu.reg.sp + 3] = 0x80;
	RTI.opcode = 0x40;
	testInstruction(&cpu, RTI);
	TEST_ASSERT_EQUAL(0x8003, cpu.reg.pc);
	TEST_ASSERT_EQUAL(0x83, cpu.reg.p);

}
void testRTS(void)
{
	struct cpu cpu;
	struct asmInstruction RTS;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	cpu.reg.pc = 0x9000;
	pushPCtoStack(&bus);
	RTS.opcode = 0x60;
	testInstruction(&cpu, RTS);
	TEST_ASSERT_EQUAL(0x9001, cpu.reg.pc);
}
void testJMP(void)
{
	struct cpu cpu;
	struct asmInstruction JMP;
	int cycles = 0;
	struct bus bus = {.cpu = &cpu};

	resetCPU(&bus, &cycles);
	
	/* test absolute */
	JMP.opcode = 0x4c;
	JMP.operand1 = 0x34;
	JMP.operand2 = 0x12;
	testInstruction(&cpu, JMP);
	TEST_ASSERT_EQUAL(0x1234, cpu.reg.pc);
	/* test indirect */
	JMP.opcode = 0x6c;
	JMP.operand1 = 0x20;
	JMP.operand2 = 0x01;
	cpu.mem[0x0120] = 0x93;
	cpu.mem[0x0121] = 0x80;
	testInstruction(&cpu, JMP);
	TEST_ASSERT_EQUAL(0x8093, cpu.reg.pc);
}

int main(void)
{
	UnityBegin("testCPUInstructions.c");
	RUN_TEST(testLDA);
	RUN_TEST(testLDX);
	RUN_TEST(testLDY);
	RUN_TEST(testINX);
	RUN_TEST(testSTA);
	RUN_TEST(testSTX);
	RUN_TEST(testSTY);
	RUN_TEST(testTAX);
	RUN_TEST(testTAY);
	RUN_TEST(testTXA);
	RUN_TEST(testTYA);
	RUN_TEST(testTSX);
	RUN_TEST(testTXS);
	RUN_TEST(testPHA);
	RUN_TEST(testPHP);
	RUN_TEST(testPLA);
	RUN_TEST(testPLP);
	RUN_TEST(testAND);
	RUN_TEST(testEOR);
	RUN_TEST(testBIT);
	RUN_TEST(testADC);
	RUN_TEST(testSBC);
	RUN_TEST(testCMP);
	RUN_TEST(testCPX);
	RUN_TEST(testCPY);
	RUN_TEST(testINC);
	RUN_TEST(testINX);
	RUN_TEST(testINY);
	RUN_TEST(testDEC);
	RUN_TEST(testDEX);
	RUN_TEST(testDEY);
	RUN_TEST(testASL);
	RUN_TEST(testLSR);
	RUN_TEST(testROL);
	RUN_TEST(testROR);
	RUN_TEST(testBCC);
	RUN_TEST(testBCS);
	RUN_TEST(testBEQ);
	RUN_TEST(testBMI);
	RUN_TEST(testBNE);
	RUN_TEST(testBPL);
	RUN_TEST(testBVC);
	RUN_TEST(testBVS);
	RUN_TEST(testCLC);
	RUN_TEST(testCLD);
	RUN_TEST(testCLI);
	RUN_TEST(testCLV);
	RUN_TEST(testSEC);
	RUN_TEST(testSED);
	RUN_TEST(testSEI);
	RUN_TEST(testJSR);
	RUN_TEST(testRTI);
	RUN_TEST(testRTS);
	RUN_TEST(testJMP);
	return (UnityEnd());
}
