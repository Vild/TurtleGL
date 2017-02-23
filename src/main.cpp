// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "engine.hpp"
#include <cstdio>

int main(int argc, char** argv) {
	try {
		return Engine::getInstance().run();
	} catch (std::exception& e) {
		fprintf(stderr, "%s\n", e.what());
		return -1;
	} catch (const char* str) {
		fprintf(stderr, "%s\n", str);
		return -1;
	}
}
