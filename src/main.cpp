#include "engine.hpp"
#include <cstdio>

int main(int argc, char** argv) {
	try {
		return Engine().run();
	} catch (std::exception& e) {
		fprintf(stderr, "%s\n", e.what());
		return -1;
	} catch (const char* str) {
		fprintf(stderr, "%s\n", str);
		return -1;
	}
}
