#include "Chip8.h"

int main(int argc, char* argv[])
{
	if (argc <= 1) {
		std::cout << "./Chip8.exe PATH_TO_ROM_FILE" << std::endl;
		exit(1);
	}

	Chip8 emu;
	emu.LoadFile(argv[1]);
	emu.Run();

	return 0;
}