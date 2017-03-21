// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "engine.hpp"
#include <cstdio>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#undef main
int main(int argc, char** argv) {
	srand(time(NULL));
	try {
		return Engine::getInstance().run();
	} catch (std::exception& e) {
		fprintf(stderr, "%s\n", e.what());
		std::string in;
		std::cin >> in;
		return -1;
	} catch (const char* str) {
		fprintf(stderr, "%s\n", str);
		std::string in;
		std::cin >> in;
		return -1;
	}
}
