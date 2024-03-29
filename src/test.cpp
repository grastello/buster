#define TEST

#include "buster.cpp"
#include "bus.cpp"
#include "cpu.cpp"

#include <iostream>
#include <vector>
#include <string>

struct Test {
	std::string name;
	bool (* test)();
};

bool runTest(Test test) {
	bool result = (*test.test)();

	if (result) std::cout << test.name << ": PASS" << std::endl;
	return result;
}

void reportMismatch(std::string testName, std::string expected, uint64_t found) {
	std::cout << testName << ": FAILED" << std::endl;
	std::cout << "  expected " << expected << " but found 0x" 
	          << std::hex << unsigned(found) << std::endl;
}

// Test the LIT instruction.
bool testLIT() {
	Buster machine;
	Bus* bus = machine.getBusPtr();

	// Initialize some ram.
	bus->ram[0x0000] = 0x40; // LIT
	bus->ram[0x0001] = 0x03;
	bus->ram[0x0002] = 0x40; // LIT
	bus->ram[0x0003] = 0xFE;
	bus->ram[0x0004] = 0x00; // HALT

	// Run the machine.
	while(bus->cpu.running) bus->cpu.clock();

	// Check the results.
	uint16_t sp = bus->cpu.getSP();

	sp++;
	if (bus->read(sp) != 0xFE) {
		reportMismatch("LIT", "0xFE", bus->read(sp));
		return false;
	}

	sp++;
	if (bus->read(sp) != 0x03) {
		reportMismatch("LIT", "0x03", bus->read(sp));
		return false;
	}

	return true;
}

// Test the DROP instruction.
bool testDROP() {
	Buster machine;
	Bus* bus = machine.getBusPtr();

	// Initialize some ram.
	bus->ram[0x0000] = 0x40; // LIT
	bus->ram[0x0001] = 0x00;
	bus->ram[0x0002] = 0x20; // DROP
	bus->ram[0x0004] = 0x00; // HALT

	// Run the machine.
	while(bus->cpu.running) bus->cpu.clock();

	// Check the results.
	uint16_t sp = bus->cpu.getSP();

	if (sp != 0xFFFF) {
		reportMismatch("DROP", "0xFFFF", sp);
		return false;
	}

	return true;
}

// Test the STORE function.
bool testSTORE() {
	Buster machine;
	Bus* bus = machine.getBusPtr();

	//Initialize some ram.
	bus->ram[0x0000] = 0x40; // LIT
	bus->ram[0x0001] = 0x11;
	bus->ram[0x0002] = 0x40; // LIT
	bus->ram[0x0003] = 0x0E;
	bus->ram[0x0004] = 0x40; // LIT
	bus->ram[0x0005] = 0xA0;

	bus->ram[0x0006] = 0x30; // STORE
	bus->ram[0x0007] = 0x00; // HALT

	// Run the machine.
	while(bus->cpu.running) bus->cpu.clock();

	// Check results.
	if (bus->read(0xA00E) != 0x11) {
		reportMismatch("STORE", "0x11", bus->read(0xA00E));
		return false;
	}

	return true;
}

// Test the FETCH function.
bool testFETCH() {
	Buster machine;
	Bus *bus = machine.getBusPtr();

	// Populate memory.
	bus->ram[0x0000] = 0x40; // LIT
	bus->ram[0x0001] = 0xFF;
	bus->ram[0x0002] = 0x40; // LIT
	bus->ram[0x0003] = 0x01;
	bus->ram[0x0004] = 0x31; // FETCH
	bus->ram[0x0005] = 0x00; // HALT

	bus->ram[0x01FF] = 0x22;

	// Run the machine.
	while(bus->cpu.running) bus->cpu.clock();

	// Check results.
	uint16_t sp = bus->cpu.getSP();
	sp++;
	if (bus->read(sp) != 0x22) {
		reportMismatch("FETCH", "0x22", bus->read(sp));
		return false;
	}

	return true;
}

// Test the DUP isntruction.
bool testDUP() {
	Buster machine;
	Bus *bus = machine.getBusPtr();

	// Initialize the ram.
	bus->ram[0x0000] = 0x40; // LIT
	bus->ram[0x0001] = 0xA3;
	bus->ram[0x0002] = 0x21; // DUP
	bus->ram[0x0003] = 0x00;

	// Run the machine.
	while(bus->cpu.running) bus->cpu.clock();

	//Check results.
	uint16_t sp = bus->cpu.getSP();

	sp++;
	if (bus->read(sp) != 0xA3) {
		reportMismatch("DUP", "0xA3", bus->read(sp));
		return false;
	}

	sp++;
	if (bus->read(sp) != 0xA3) {
		reportMismatch("DUP", "0xA3", bus->read(sp));
		return false;
	}

	return true;
}

// Test the OVER instruction.
bool testOVER() {
	Buster machine;
	Bus *bus = machine.getBusPtr();

	// Initialize the ram.
	bus->ram[0x0000] = 0x40; // LIT
	bus->ram[0x0001] = 0xE1;
	bus->ram[0x0002] = 0x40; // LIT
	bus->ram[0x0003] = 0xE2;
	bus->ram[0x0004] = 0x22; // OVER
	bus->ram[0x0005] = 0x00; // HALT

	// Run the machine.
	while (bus->cpu.running) bus->cpu.clock();

	// Check results.
	uint16_t sp = bus->cpu.getSP();

	sp++;
	if (bus->read(sp) != 0xE1) {
		reportMismatch("OVER", "0xE1", bus->read(sp));
		return false;
	}

	sp++;
	if (bus->read(sp) != 0xE2) {
		reportMismatch("OVER", "0xE2", bus->read(sp));
		return false;
	}

	sp++;
	if (bus->read(sp) != 0xE1) {
		reportMismatch("OVER", "0xE1", bus->read(sp));
		return false;
	}

	return true;
}

// Test for the SWAP instruction.
bool testSWAP() {
	Buster machine;
	Bus *bus = machine.getBusPtr();

	// Initialize ram.
	bus->ram[0x0000] = 0x40; // LIT
	bus->ram[0x0001] = 0x0F;
	bus->ram[0x0002] = 0x40; // LIT
	bus->ram[0x0003] = 0xF0;
	bus->ram[0x0004] = 0x23; // SWAP
	bus->ram[0x0005] = 0x00; // HALT

	// Run the machine.
	while (bus->cpu.running) bus->cpu.clock();

	// Check the results.
	uint16_t sp = bus->cpu.getSP();

	sp++;
	if (bus->read(sp) != 0x0F) {
		reportMismatch("SWAP", "0x0F", bus->read(sp));
		return false;
	}

	sp++;
	if (bus->read(sp) != 0xF0) {
		reportMismatch("SWAP", "0xF0", bus->read(sp));
		return false;
	}

	return true;
}

// Test the IF instruction.
bool testIF_branch() {
	Buster machine;
	Bus *bus = machine.getBusPtr();

	// Initialize the ram.
	bus->ram[0x0000] = 0x40; // LIT
	bus->ram[0x0001] = 0x00;
	bus->ram[0x0002] = 0x50; // IF
	bus->ram[0x0003] = 0x11;
	bus->ram[0x0004] = 0x11;

	bus->ram[0x1111] = 0x00; // HALT

	// Run the machine.
	while (bus->cpu.running) bus->cpu.clock();

	// Check results.
	uint16_t pc = bus->cpu.getPC();
	if (pc != 0x1112) {
		reportMismatch("IF branch", "0x1112", pc);
		return false;
	}

	return true;
}

bool testIF_nobranch() {
	Buster machine;
	Bus *bus = machine.getBusPtr();

	// Initialize the ram.
	bus->ram[0x0000] = 0x40; // LIT
	bus->ram[0x0001] = 0xFF;
	bus->ram[0x0002] = 0x50; // IF
	bus->ram[0x0003] = 0x11;
	bus->ram[0x0004] = 0x11;
	bus->ram[0x0005] = 0x00; // HALT

	// Run the machine.
	while (bus->cpu.running) bus->cpu.clock();

	// Check results.
	uint16_t pc = bus->cpu.getPC();
	if (pc != 0x0006) {
		reportMismatch("IF branch", "0x0006", pc);
		return false;
	}

	return true;
}

// Test the CALL instruction.
bool testCALL() {
	Buster machine;
	Bus *bus = machine.getBusPtr();

	// Populate the RAM.
	bus->ram[0x0000] = 0x40; // LIT
	bus->ram[0x0001] = 0x00;
	bus->ram[0x0002] = 0x50; // IF
	bus->ram[0x0003] = 0xAA;
	bus->ram[0x0004] = 0xCC;

	bus->ram[0xAACC] = 0x60; // CALL
	bus->ram[0xAACD] = 0x11;
	bus->ram[0xAACE] = 0x22;
	bus->ram[0xAACF] = 0x00; // HALT (CALL return point)

	bus->ram[0x1122] = 0x00; // HALT

	// Run the machine.
	while (bus->cpu.running) bus->cpu.clock();

	// Check results.
	uint16_t sp = bus->cpu.getSP();

	sp++;
	if (bus->read(sp) != 0xAA) {
		reportMismatch("CALL", "0xAA", bus->read(sp));
		return false;
	}

	sp++;
	if (bus->read(sp) != 0xCF) {
		reportMismatch("CALL", "0xCF", bus->read(sp));
		return false;
	}

	uint16_t pc = bus->cpu.getPC();
	if (pc != 0x1123) {
		reportMismatch("CALL", "0x1123", pc);
		return false;
	}

	return true;
}

// Test the EXIT instruction.
bool testEXIT() {
	Buster machine;
	Bus *bus = machine.getBusPtr();

	// Initialize the RAM.
	bus->ram[0x0000] = 0x40; // LIT
	bus->ram[0x0001] = 0x12;
	bus->ram[0x0002] = 0x40; // LIT
	bus->ram[0x0003] = 0x23;
	bus->ram[0x0004] = 0x61; // EXIT

	bus->ram[0x2312] = 0x00; // HALT

	// Run the machine.
	while (bus->cpu.running) bus->cpu.clock();

	// Check the results.
	uint16_t pc = bus->cpu.getPC();
	if (pc != 0x2313) {
		reportMismatch("EXIT", "0x2313", pc);
		return false;
	}

	return true;
}

// Test a simple subroutine call.
bool testSubroutineCall1() {
	Buster machine;
	Bus *bus = machine.getBusPtr();

	// Initialize RAM.
	bus->ram[0x0000] = 0x40; // LIT
	bus->ram[0x0001] = 0xFE;
	bus->ram[0x0002] = 0x40; // LIT
	bus->ram[0x0003] = 0x00;
	bus->ram[0x0004] = 0x50; // IF
	bus->ram[0x0005] = 0xAA;
	bus->ram[0x0006] = 0xAA;

	bus->ram[0xAAAA] = 0x60; // CALL
	bus->ram[0xAAAB] = 0xBB;
	bus->ram[0xAAAC] = 0xBB;
	bus->ram[0xAAAD] = 0x00; // HALT

	bus->ram[0xBBBB] = 0x40; // LIT
	bus->ram[0xBBBC] = 0xFE;
	bus->ram[0xBBBD] = 0x20; // DROP
	bus->ram[0xBBBE] = 0x61; // EXIT

	// Run the machine.
	while (bus->cpu.running) bus->cpu.clock();

	// Check the results.
	uint16_t pc = bus->cpu.getPC();
	if (pc != 0xAAAE) {
		reportMismatch("Subroutine call", "0xAAAE", pc);
		return false;
	}

	uint16_t sp = bus->cpu.getSP();
	sp++;
	if (bus->read(sp) != 0xFE) {
		reportMismatch("Subroutine call", "0xFE", bus->read(sp));
		return false;
	}

	return true;
}

// Test a non-trivial (but stupid) subroutine call.
bool testSubroutineCall2() {
	Buster machine;
	Bus *bus = machine.getBusPtr();

	// Populate the RAM.
	bus->ram[0x0000] = 0x40; // LIT 0x0F
	bus->ram[0x0001] = 0x0F;
	bus->ram[0x0002] = 0x40; // LIT 0x03
	bus->ram[0x0003] = 0x03;
	bus->ram[0x0004] = 0x60; // CALL 0x1111
	bus->ram[0x0005] = 0x11;
	bus->ram[0x0006] = 0x11;
	bus->ram[0x0007] = 0x40; // LIT 0x22
	bus->ram[0x0008] = 0x22;
	bus->ram[0x0009] = 0x40; // LIT 0x22
	bus->ram[0x000A] = 0x22;
	bus->ram[0x000B] = 0x30; // STORE
	bus->ram[0x000C] = 0x00; // HALT

	bus->ram[0x1111] = 0x40; // LIT 0x00
	bus->ram[0x1112] = 0x00;
	bus->ram[0x1113] = 0x40; // LIT 0x00
	bus->ram[0x1114] = 0x00;
	bus->ram[0x1115] = 0x30; // STORE
	bus->ram[0x1116] = 0x40; // LIT 0x01
	bus->ram[0x1117] = 0x01;
	bus->ram[0x1118] = 0x40; // LIT 0x00
	bus->ram[0x1119] = 0x00;
	bus->ram[0x111A] = 0x30; // STORE
	bus->ram[0x111B] = 0x10; // ADD
	bus->ram[0x111C] = 0x40; // LIT 0x01
	bus->ram[0x111D] = 0x01;
	bus->ram[0x111E] = 0x40; // LIT 0x00
	bus->ram[0x111F] = 0x00;
	bus->ram[0x1120] = 0x31; // FETCH
	bus->ram[0x1121] = 0x40; // LIT 0x00
	bus->ram[0x1122] = 0x00;
	bus->ram[0x1123] = 0x40; // LIT 0x00
	bus->ram[0x1124] = 0x00;
	bus->ram[0x1125] = 0x31; // FETCH
	bus->ram[0x1126] = 0x61; // EXIT

	// Run the machine.
	while (bus->cpu.running) bus->cpu.clock();

	// Check the result;
	if (bus->read(0x2222) != 0x12) {
		reportMismatch("Subroutine call 2", "0x12", bus->read(0x2222));
		return false;
	}

	return true;
}

// Test ADD.
bool testADD() {
	Buster machine;
	Bus *bus = machine.getBusPtr();

	// Populate the RAM.
	bus->ram[0x0000] = 0x40; // LIT
	bus->ram[0x0001] = 0x0F;
	bus->ram[0x0002] = 0x40; // LIT
	bus->ram[0x0003] = 0x03;
	bus->ram[0x0004] = 0x10; // ADD
	bus->ram[0x0005] = 0x00; // HALT

	// Run the machine.
	while (bus->cpu.running) bus->cpu.clock();

	// Check the results.
	uint16_t sp = bus->cpu.getSP();
	sp++;
	if (bus->read(sp) != 0x12) {
		reportMismatch("ADD", "0x12", bus->read(sp));
		return false;
	}

	return true;
}

// Test the SUB instruction.
bool testSUB() {
	Buster machine;
	Bus *bus = machine.getBusPtr();

	// Initialize the RAM.
	bus->ram[0x0000] = 0x40; // LIT
	bus->ram[0x0001] = 0x0F;
	bus->ram[0x0002] = 0x40; // LIT
	bus->ram[0x0003] = 0x09;
	bus->ram[0x0004] = 0x11; // SUB
	bus->ram[0x0005] = 0x00; // HALT

	// Run the machine.
	while (bus->cpu.running) bus->cpu.clock();

	// Check the results.
	uint16_t sp = bus->cpu.getSP();
	sp++;
	if (bus->read(sp) != 0xFA) {
		reportMismatch("SUB", "0xFA", bus->read(sp));
		return false;
	}

	return true;
}

// Test the AND instruction.
bool testAND() {
	Buster machine;
	Bus *bus = machine.getBusPtr();

	// Initialize the RAM.
	bus->ram[0x0000] = 0x40; // LIT
	bus->ram[0x0001] = 0x0F;
	bus->ram[0x0002] = 0x40; // LIT
	bus->ram[0x0003] = 0x81;
	bus->ram[0x0004] = 0x12; // AND
	bus->ram[0x0005] = 0x00; // HALT

	// Run the machine.
	while (bus->cpu.running) bus->cpu.clock();

	// Check the results.
	uint16_t sp = bus->cpu.getSP();
	sp++;
	if (bus->read(sp) != 0x01) {
		reportMismatch("AND", "0x01", bus->read(sp));
		return false;
	}

	return true;
}

// Test the OR instruction.
bool testOR() {
	Buster machine;
	Bus *bus = machine.getBusPtr();

	// Initialize the RAM.
	bus->ram[0x0000] = 0x40; // LIT
	bus->ram[0x0001] = 0x0F;
	bus->ram[0x0002] = 0x40; // LIT
	bus->ram[0x0003] = 0x81;
	bus->ram[0x0004] = 0x13; // OR
	bus->ram[0x0005] = 0x00; // HALT

	// Run the machine.
	while (bus->cpu.running) bus->cpu.clock();

	// Check the results.
	uint16_t sp = bus->cpu.getSP();
	sp++;
	if (bus->read(sp) != 0x8F) {
		reportMismatch("OR", "0x8F", bus->read(sp));
		return false;
	}

	return true;
}

// Test the XOR instruction.
bool testXOR() {
	Buster machine;
	Bus *bus = machine.getBusPtr();

	// Initialize the RAM.
	bus->ram[0x0000] = 0x40; // LIT
	bus->ram[0x0001] = 0x0F;
	bus->ram[0x0002] = 0x40; // LIT
	bus->ram[0x0003] = 0x81;
	bus->ram[0x0004] = 0x14; // XOR
	bus->ram[0x0005] = 0x00; // HALT

	// Run the machine.
	while (bus->cpu.running) bus->cpu.clock();

	// Check the results.
	uint16_t sp = bus->cpu.getSP();
	sp++;
	if (bus->read(sp) != 0x8E) {
		reportMismatch("OR", "0x8E", bus->read(sp));
		return false;
	}

	return true;
}

int main() {
	bool allPass = true;
	std::vector<Test> instructionsTests = {
		{ "LIT",         &testLIT         }, { "DROP",      &testDROP      },
		{ "STORE",       &testSTORE       }, { "FETCH",     &testFETCH     },
		{ "DUP",         &testDUP         }, { "OVER",      &testOVER      },
		{ "SWAP",        &testSWAP        }, { "IF_branch", &testIF_branch },
		{ "IF_nobranch", &testIF_nobranch }, { "CALL",      &testCALL      },
		{ "EXIT",        &testEXIT        }, { "ADD",       &testADD       },
		{ "SUB",         &testSUB         }, { "AND",       &testAND       },
		{ "OR",          &testOR          }, { "XOR",       &testXOR       },
	};

	std::vector<Test> miscTests = {
		{ "Subroutine call 1", &testSubroutineCall1 },
		{ "Subroutine call 2", &testSubroutineCall2 },
	};

	// Execute the tests.
	for (auto &test: instructionsTests) allPass = runTest(test);
	for (auto &test: miscTests) allPass = runTest (test);

	if (allPass) return 0;
	return 1;
}
