#include "engine.hpp"
#include <cstdio>

int main(int argc, char ** argv) {
	try {
		return Engine().run();
	} catch (char * str) {
		fprintf(stderr, str);
		return -1;
	}
}
