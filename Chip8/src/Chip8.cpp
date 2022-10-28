#include "Chip8.h"
#include <cstdlib>
#include <ctime>

Chip8::Chip8()
{
	PC = 0x200;
	I = 0;
	for (int i = 0; i < 16; i++)
	{
		Registers[i] = 0;
		Keypad[i] = 0;
	}

	for (int i = 0; i < 80; i++)
		Memory[i] = Fonts[i];

	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 680, 500, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, 0);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
	srand(time(0));
}

void Chip8::LoadFile(const std::string File)
{
	FILE* fp;
	fp = fopen(File.c_str(), "rb");
	if (fp == NULL)
	{
		std::cout << "ERROR: FAILED TO OPEN FILE" << std::endl;
		exit(-1);
	}
	std::cout << "LOADED FILE: " << File << std::endl;
	fread(&Memory[0x200], 0xFFF, 1, fp);
	fclose(fp);
}

void Chip8::Run()
{

	while (true)
	{
		Events();
		Execute();
		Timers();
		if (RenderFlag)
		{
			Render();
			RenderFlag = false;
		}
		std::this_thread::sleep_for(std::chrono::microseconds(1200));

	}
}

u16 Chip8::FetchOpcode()
{
	Opcode = Memory[PC] << 8 | Memory[PC + 1];
	PC+=2;

	return Opcode;
}

void Chip8::Execute()
{
	Opcode = FetchOpcode();
	//35 opcodes - two bytes long
	switch (Opcode & 0xF000)
	{
	case 0x0000:
		switch (Opcode & 0x00FF)
		{
		case 0x00E0:
			OPCODE_00E0();
			break;
		case 0x00EE:
			OPCODE_00EE();
			break;
		}
		break;
	case 0x1000:
		OPCODE_1NNN();
		break;
	case 0x2000:
		OPCODE_2NNN();
		break;
	case 0x3000:
		OPCODE_3XNN();
		break;
	case 0x4000:
		OPCODE_4XNN();
		break;
	case 0x5000:
		OPCODE_5XY0();
		break;
	case 0x6000:
		OPCODE_6XNN();
		break;
	case 0x7000:
		OPCODE_7XNN();
		break;
	case 0x8000:
		switch (Opcode & 0x000F)
		{
		case 0x0000:
			OPCODE_8XY0();
			break;
		case 0x0001:
			OPCODE_8XY1();
			break;
		case 0x0002:
			OPCODE_8XY2();
			break;
		case 0x0003:
			OPCODE_8XY3();
			break;
		case 0x0004:
			OPCODE_8XY4();
			break;
		case 0x0005:
			OPCODE_8XY5();
			break;
		case 0x0006:
			OPCODE_8XY6();
			break;
		case 0x0007:
			OPCODE_8XY7();
			break;
		case 0x000E:
			OPCODE_8XYE();
			break;
		}
		break;
	case 0x9000:
		OPCODE_9XY0();
		break;
	case 0xA000:
		OPCODE_ANNN();
		break;
	case 0xB000:
		OPCODE_BNNN();
		break;
	case 0xC000:
		OPCODE_CXNN();
		break;
	case 0xD000:
		OPCODE_DXYN();
		break;
	case 0xE000:
		switch (Opcode & 0x00FF)
		{
		case 0x009E:
			OPCODE_EX9E();
			break;
		case 0x00A1:
			OPCODE_EXA1();
			break;
		}
		break;
	case 0xF000:
		switch (Opcode & 0x00FF)
		{
		case 0x0007:
			OPCODE_FX07();
			break;
		case 0x000A:
			OPCODE_FX0A();
			break;
		case 0x0015:
			OPCODE_FX15();
			break;
		case 0x0018:
			OPCODE_FX18();
			break;
		case 0x001E:
			OPCODE_FX1E();
			break;
		case 0x0029:
			OPCODE_FX29();
			break;
		case 0x0033:
			OPCODE_FX33();
			break;
		case 0x0055:
			OPCODE_FX55();
			break;
		case 0x0065:
			OPCODE_FX65();
			break;
		}
		break;
	}
}

void Chip8::Timers()
{
	if (DelayTimer > 0)
		DelayTimer--;
	if (SoundTimer > 0)
		SoundTimer--;
	//Play Sound if Sound Timer == 0
}

void Chip8::Render()
{
	uint32_t pixels[2048];
	for (int i = 0; i < 2048; i++) 
	{
		uint8_t pixel = Buffer[i];
		pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
	}
	//Translate 8 bit pixel to 32 bit pixel for SDL2
	SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(uint32_t));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void Chip8::Events()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_KEYDOWN)
		{
			SetKeys(true, e.key.keysym.sym);
		}
		if (e.type == SDL_KEYUP)
		{
			SetKeys(false, e.key.keysym.sym);
		}
		if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
			exit(1);
		}
	}
}

void Chip8::SetKeys(bool down, uint32_t key_code)
{
	switch (key_code)
	{
	case SDLK_ESCAPE:
		exit(1);
		break;
	case SDLK_x:
		Keypad[0] = down;
		break;
	case SDLK_1:
		Keypad[1] = down; 
		break;
	case SDLK_2:
		Keypad[2] = down;
		break;
	case SDLK_3:
		Keypad[3] = down;
		break;
	case SDLK_q:
		Keypad[4] = down;
		break;
	case SDLK_w:
		Keypad[5] = down;
		break;
	case SDLK_e:
		Keypad[6] = down;
		break;
	case SDLK_a:
		Keypad[7] = down;
		break;
	case SDLK_s:
		Keypad[8] = down;
		break;
	case SDLK_d:
		Keypad[9] = down;
		break;
	case SDLK_z:
		Keypad[0xA] = down;
		break;
	case SDLK_c:
		Keypad[0xB] = down;
		break;
	case SDLK_4:
		Keypad[0xC] = down;
		break;
	case SDLK_r:
		Keypad[0xD] = down;
		break;
	case SDLK_f:
		Keypad[0xE] = down;
		break;
	case SDLK_v:
		Keypad[0xF] = down;
		break;
	}
}

void Chip8::OPCODE_0NNN()
{
}

void Chip8::OPCODE_00E0()
{
	for (int i = 0; i < (64 * 32); i++)
	{
		Buffer[i] = 0;
	}
	RenderFlag = true;
}

void Chip8::OPCODE_00EE()
{
	PC = Stack.top();
	Stack.pop();
}

void Chip8::OPCODE_1NNN()
{
	PC = Opcode & 0x0FFF;
}

void Chip8::OPCODE_2NNN()
{
	Stack.push(PC);
	PC = Opcode & 0x0FFF;
}

void Chip8::OPCODE_3XNN()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	if (Registers[reg.x] == (Opcode & 0x00FF))
		PC += 2;
}

void Chip8::OPCODE_4XNN()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	if (Registers[reg.x] != (Opcode & 0x00FF))
		PC += 2;
}

void Chip8::OPCODE_5XY0()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	reg.y = (Opcode & 0x00F0) >> 4;
	if (Registers[reg.y] == Registers[reg.x])
		PC += 2;
}

void Chip8::OPCODE_6XNN()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	Registers[reg.x] = Opcode & 0x00FF;
}

void Chip8::OPCODE_7XNN()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	Registers[reg.x] += Opcode & 0x00FF;
}

void Chip8::OPCODE_8XY0()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	reg.y = (Opcode & 0x00F0) >> 4;
	Registers[reg.x] = Registers[reg.y];
}

void Chip8::OPCODE_8XY1()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	reg.y = (Opcode & 0x00F0) >> 4;
	Registers[reg.x] |= Registers[reg.y];
}

void Chip8::OPCODE_8XY2()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	reg.y = (Opcode & 0x00F0) >> 4;
	Registers[reg.x] &= Registers[reg.y];
}

void Chip8::OPCODE_8XY3()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	reg.y = (Opcode & 0x00F0) >> 4;
	Registers[reg.x] ^= Registers[reg.y];
}

void Chip8::OPCODE_8XY4()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	reg.y = (Opcode & 0x00F0) >> 4;
	if (uint32_t(Registers[reg.x] + Registers[reg.y]) > 255)
		Registers[0xF] = 1;
	else
		Registers[0xF] = 0;
	Registers[reg.x] += Registers[reg.y];
}

void Chip8::OPCODE_8XY5()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	reg.y = (Opcode & 0x00F0) >> 4;
	if (Registers[reg.y] > Registers[reg.x])
		Registers[0xF] = 0;
	else
		Registers[0xF] = 1;
	Registers[reg.x] -= Registers[reg.y];
}

void Chip8::OPCODE_8XY6()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	reg.y = (Opcode & 0x00F0) >> 4;
	Registers[0xF] = Registers[reg.x] & 0x1;
	Registers[reg.x] = Registers[reg.x] >> 1;
}

void Chip8::OPCODE_8XY7()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	reg.y = (Opcode & 0x00F0) >> 4;
	if (Registers[reg.x] > Registers[reg.y])
		Registers[0xF] = 0;
	else
		Registers[0xF] = 1;
	Registers[reg.x] = Registers[reg.y] - Registers[reg.x];
}

void Chip8::OPCODE_8XYE()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	reg.y = (Opcode & 0x00F0) >> 4;
	Registers[0xF] = Registers[reg.x] >> 7;
	Registers[reg.x] = Registers[reg.x] << 1;
}

void Chip8::OPCODE_9XY0()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	reg.y = (Opcode & 0x00F0) >> 4;
	if (Registers[reg.x] != Registers[reg.y])
		PC += 2;
}

void Chip8::OPCODE_ANNN()
{
	I = Opcode & 0x0FFF;
}

void Chip8::OPCODE_BNNN()
{
	PC = Registers[0] + (Opcode & 0x0FFF);
}

void Chip8::OPCODE_CXNN()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	Registers[reg.x] = rand() & (Opcode & 0x00FF);
}

void Chip8::OPCODE_DXYN()
{
	//(VX, VY) Width = 8;  Height = N;
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	reg.y = (Opcode & 0x00F0) >> 4;
	uint8_t height = Opcode & 0x000F;
	uint8_t pixel;

	Registers[0xF] = 0;
	for (int y = 0; y < height; y++)
	{
		uint8_t pixel = Memory[I + y];
		for (int x = 0; x < 8; x++)
		{
			if ((pixel & (0x80 >> x)) != 0)
			{
				if (Buffer[(Registers[reg.x] + x + ((Registers[reg.y] + y) * 64))] == 1)
					Registers[0xF] = 1;
				Buffer[Registers[reg.x] + x + ((Registers[reg.y] + y) * 64)] ^= 1;
			}
		}
	}
	RenderFlag = true;
}

void Chip8::OPCODE_EX9E()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	if (Keypad[Registers[reg.x]] != 0)
		PC += 2;
		
}

void Chip8::OPCODE_EXA1()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	if (Keypad[Registers[reg.x]] == 0)
		PC += 2;
}

void Chip8::OPCODE_FX07()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	Registers[reg.x] = DelayTimer;
}

void Chip8::OPCODE_FX0A()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	bool KeyIsPressed = false;

	for (int i = 0; i < 16; ++i)
	{
		if (Keypad[i] != 0)
		{
			Registers[reg.x] = i;
			KeyIsPressed = true;
		}
	}
	if (!KeyIsPressed)
		PC -= 2;
}


void Chip8::OPCODE_FX15()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	DelayTimer = Registers[reg.x];
}

void Chip8::OPCODE_FX18()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	SoundTimer = Registers[reg.x];
}

void Chip8::OPCODE_FX1E()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	I += Registers[reg.x];
}

void Chip8::OPCODE_FX29()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	I = Registers[reg.x] * 5;
}

void Chip8::OPCODE_FX33()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	uint16_t val = Registers[reg.x];
	int reverse = 2;
	while (val != 0)
	{
		Memory[I + reverse] = val % 10;
		val = val / 10;
		reverse--;
	}
}

void Chip8::OPCODE_FX55()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	for (uint8_t i = 0; i <= reg.x; i++)
	{
		Memory[I + i] = Registers[i];
	}
}

void Chip8::OPCODE_FX65()
{
	u4 reg;
	reg.x = (Opcode & 0x0F00) >> 8;
	for (uint8_t i = 0; i <= reg.x; i++)
	{
		Registers[i] = Memory[I + i];
	}
}



