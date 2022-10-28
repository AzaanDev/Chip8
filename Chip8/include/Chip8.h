#pragma once

#include "SDL.h"
#include <string>
#include <stack>
#include <chrono>
#include "thread"
#include <iostream>

typedef unsigned char u8;
typedef unsigned short int u16;

struct u4
{
	u8 x: 4;
	u8 y: 4;
};


class Chip8
{
public:
	Chip8();
	~Chip8() = default;
	void LoadFile(const std::string File);
	void Run();
	u16 FetchOpcode();
	void Execute();
	void Timers();
	void Render();
	void Events();
	void SetKeys(bool down, uint32_t key_code);

private:
	uint8_t Buffer[64 * 32] = {0x0};
	u8 Memory[0xFFF] = {0x0};
	u8 Registers[16];
	u8 Keypad[16];
	u8 DelayTimer;
	u8 SoundTimer;
	bool RenderFlag = false;

	u16 I;
	u16 PC;
	u16 Opcode;
	std::stack<u16> Stack;
;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
public:
	/*OPCODES*/
	void OPCODE_0NNN(); void OPCODE_00E0(); void OPCODE_00EE();
	void OPCODE_1NNN();
	void OPCODE_2NNN();
	void OPCODE_3XNN();
	void OPCODE_4XNN();
	void OPCODE_5XY0();
	void OPCODE_6XNN();
	void OPCODE_7XNN();
	void OPCODE_8XY0(); void OPCODE_8XY1(); void OPCODE_8XY2(); 
	void OPCODE_8XY3(); void OPCODE_8XY4(); void OPCODE_8XY5(); 
	void OPCODE_8XY6(); void OPCODE_8XY7(); void OPCODE_8XYE();
	void OPCODE_9XY0();
	void OPCODE_ANNN();
	void OPCODE_BNNN();
	void OPCODE_CXNN();
	void OPCODE_DXYN();
	void OPCODE_EX9E(); void OPCODE_EXA1();
	void OPCODE_FX07(); void OPCODE_FX0A(); void OPCODE_FX15(); 
	void OPCODE_FX18(); void OPCODE_FX1E(); void OPCODE_FX29(); 
	void OPCODE_FX33(); void OPCODE_FX55(); void OPCODE_FX65();
};

const u8 Fonts[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};