#include "buster.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

Buster::Settings parseFlags(std::vector<std::string> &arguments) {
	Buster::Settings settings;
	std::vector<std::string>::iterator it;

	// `--debug` flag.
	while (true) {
		it = std::find(arguments.begin(), arguments.end(), "--debug");
		if (it != arguments.end()) {
			settings.debugMode = true;
			arguments.erase(it);
		} else {
			break;
		}
	}

	// `--watch` flag.
	while (true) {
		it = std::find(arguments.begin(), arguments.end(), "--watch");
		if (it < arguments.end() - 1) {
			try {
				uint16_t cell = std::stoi(*(it + 1), nullptr, 16);
				settings.watchedCells.push_back(cell);
				arguments.erase(it, it + 2);
			} catch(...) {
				std::cerr << "argument to --watch option is not a number" 
				          << std::endl;
				std::exit(1);
			}
		} else {
			break;
		}
	}

	return settings;
}

int main(int argn, char *argv[]) {
	bool debugMode = false;
	std::vector<uint16_t> watchedCells;

	// Store relevant arguments in a vector.
	std::vector<std::string> arguments;
	uint64_t i;
	for (i = 1; i < argn; i++) arguments.push_back(argv[i]);

	// Handle flags.
	Buster::Settings settings = parseFlags(arguments);
	
	// Check the remaining number of arguments.
	if (arguments.size() != 1) {
		std::cerr << "usage: buster [--debug] [--watch cell] FILE" << std::endl;
		std::exit(1);
	}

	// Make an instance of the stack machine.
	Buster machine;
	machine.settings = settings;

	try { machine.loadProgram(arguments[0]); }
	catch (...) {
		std::exit(1);
	}

	machine.run();
	
	return 0;
}
