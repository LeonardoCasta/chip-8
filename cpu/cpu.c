#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

uint8_t font_set[80] = {0xF0, 0x90, 0x90, 0x90, 0xF0,
						0x20, 0x60, 0x20, 0x20, 0x60, 						
						0xF0, 0x10, 0xF0, 0xF0,	0x80,						
						0xF0, 0x10, 0xF0, 0x10, 0xF0,
						0x90, 0x90, 0xF0, 0x10, 0x10,	
						0xF0, 0x80, 0xF0, 0x10, 0xF0,
						0xF0, 0x80, 0xF0, 0x90, 0xF0,
						0xF0, 0x10, 0x20, 0x40, 0x40,
						0xF0, 0x90, 0xF0, 0x90, 0xF0,
						0xF0, 0x90, 0xF0, 0x10, 0xF0,
						0xF0, 0x90, 0xF0, 0x90, 0x90,
						0xE0, 0x90, 0xE0, 0x90, 0xE0,
						0xF0, 0x80, 0x80, 0x80, 0xF0,
						0xE0, 0x90, 0x90, 0x90, 0xE0,
						0xF0, 0x80, 0xF0, 0x80, 0xF0,
						0xF0, 0x80, 0xF0, 0x80, 0x80};

struct memory {
	//ram is 0xFFF so 0 to 4095 bytes
	//ram for emulatro 0x000 to 0x1FF
	//ram for program 0x200 to 0xFFF
	uint8_t ram[4096];
	
	uint8_t V[16];     //16 general purpose 8 bit registers	
	uint16_t I;        //reg for memory addresses, only lowest 12 bits used
	
	uint8_t DT;
	uint8_t ST;
	
	uint16_t opcode;    //currently executed operation
	uint16_t PC;        //currently executing address
	uint8_t SP;         //stack pointer
	uint16_t stack[16]; //stack

    uint8_t keyboard[32][64];
};
struct memory mem;
bool isInitialized = false;

void initialize(FILE *fd){
    mem.opcode = 0;
    mem.I = 0;
    mem.PC = 0x200;
    mem.SP = 0;
    mem.DT = 0;
    mem.ST = 0;

    memset(mem.ram, 0, 4096);
	memset(mem.V, 0, sizeof(uint8_t)*16);

    //set sprites in memory, al shifted by 0x50
	for(int i = 0; i < 80; i++){
		mem.ram[i] = font_set[i];
	}

    //load program to memory
	int read = fread((mem.ram)+0x200, sizeof(uint8_t), 1024, fd);
	if(!read){
		exit(-1);
	}

	srand(time(NULL));
    isInitialized = true;
}

void execute(){
    if(!isInitialized) exit(-1);

	//shift the byte one byte and in the second part i put the second byte
	// 1111 -> 11110000 | 1010 -> 11111010
	mem.opcode = mem.ram[mem.PC] << 8 | mem.ram[mem.PC+1];
	mem.PC += 2; //increment PC to next instruction
	//printf("%04X\n", mem.opcode);
	//take only the first byte 
	switch(mem.opcode & 0xF000){
		case 0x0000:
			if((mem.opcode & 0x00FF) == 0x00E0){
				clearDisplay = 1;
			}else if((mem.opcode & 0x00FF) == 0x00EE){
				mem.SP--;
				mem.PC = mem.stack[mem.SP];
			}
			break;
		case 0x1000:
			mem.PC = mem.opcode & 0x0FFF;
			break;
		case 0x2000:
			mem.stack[mem.SP] = mem.PC;
			mem.SP++;
			mem.PC = mem.opcode & 0x0FFF;
			break;
		case 0x3000: {
			uint8_t x = (mem.opcode & 0x0F00) >> 8;
			uint8_t kk = mem.opcode & 0x00FF;
			if(mem.V[x] == kk){
				mem.PC += 2;
			} 
			break;
		}
		case 0x4000:{
			uint8_t x = (mem.opcode & 0x0F00) >> 8;
			uint8_t kk = mem.opcode & 0x00FF;
			if(mem.V[x] != kk){
				mem.PC += 2;
			} 
			break;
		}
		case 0x5000: {
			uint8_t x = (mem.opcode & 0x0F00) >> 8;
			uint8_t y = (mem.opcode & 0x00F0) >> 4;
			if(mem.V[x] == mem.V[y]){
				mem.PC += 2;
			} 
			break;
		}
		case 0x6000: {
			uint8_t x = (mem.opcode & 0x0F00) >> 8;
			uint8_t kk = mem.opcode & 0x00FF;
			mem.V[x] = kk;
			//printf("x:%X kk:%X Vx:%X\n", x, kk, mem.V[x]);
			break;
		}
		case 0x7000: {
			uint8_t x = (mem.opcode & 0x0F00) >> 8;
			uint8_t kk = mem.opcode & 0x00FF;
			mem.V[x] += kk;
			break;
		}
		case 0x8000: {
			uint8_t x = (mem.opcode & 0x0F00) >> 8;
			uint8_t y = (mem.opcode & 0x00F0) >> 4;
			switch (mem.opcode & 0x000F)
			{
				case 0x0:
					mem.V[x] = mem.V[y];
					break;
				case 0x1:
					mem.V[x] = mem.V[x] | mem.V[y];
					break;
				case 0x2:
					mem.V[x] = mem.V[x] & mem.V[y];
					break;
				case 0x3:
					mem.V[x] = mem.V[x] ^ mem.V[y];
					break;
				case 0x4: {
					//takes the byte of sume and check
					uint16_t i = mem.V[x] + mem.V[y];
					mem.V[x] = i & 0xFF;
					mem.V[0xF] = (i > 255);
					break;
				}
				case 0x5: 
					mem.V[0xF] = mem.V[x] > mem.V[y];
					mem.V[x] = mem.V[x] - mem.V[y];
					break;
				case 0x6:
					mem.V[0xF] = mem.V[x] & 0x1;
					mem.V[x] >>= 1; //shift by one bit is like dividing
					break;
				case 0x7:
					mem.V[0xF] = mem.V[y] > mem.V[x];
					mem.V[x] = mem.V[y] - mem.V[x];
					break;
				case 0xE:
					mem.V[0xF] = (mem.V[x] & 0x80) >> 7;
					mem.V[x] *= 2;
					break;
				break;
			}
			break;
		}
		case 0x9000: {
			uint8_t x = (mem.opcode & 0x0F00) >> 8;
			uint8_t y = (mem.opcode & 0x00F0) >> 4;
			if(mem.V[x] != mem.V[y])
				mem.PC += 2;
			break;
		}
		case 0xA000:
			mem.I = mem.opcode & 0x0FFF;
			//printf("I:%X\n", mem.I);
			break;
		case 0xB000:
			mem.PC = (mem.opcode & 0x0FFF) + mem.V[0];
			break;
		case 0xC000: {
			uint8_t x = (mem.opcode & 0x0F00) >> 8;
			uint8_t kk = (mem.opcode & 0x00FF);
			int r = rand() & 0xFF;
			mem.V[x] = r & kk;
			break;
		}
		case 0xD000: {
			uint8_t x = (mem.opcode & 0x0F00) >> 8;
			uint8_t y = (mem.opcode & 0x00F0) >> 4;	
			uint8_t n = mem.opcode & 0x000F;
			printf("%04X\n", mem.opcode);
			drawSprite(mem.V[x], mem.V[y], n, grid);
			//for(int i = 0; i < n; i++){
			//	drawSprite(mem.V[x], mem.V[y+i], mem.ram[mem.I+i], grid);
			//}
			break;
		}
		case 0xE000:{
			uint8_t x = (mem.opcode & 0x0F00) >> 8;
			bool isKeyPressed = keyboard[mem.V[x]]; 
			if((mem.opcode & 0x00FF) == 0x009E){
				if(isKeyPressed) mem.PC += 2;	
			}else if((mem.opcode & 0x00FF) == 0x00A1){
				if(!isKeyPressed) mem.PC += 2;	
			}
			break;
		}
		case 0xF000: {
			uint8_t x = (mem.opcode & 0x0F00) >> 8;
			switch (mem.opcode & 0x00FF)
			{
				case 0x07:
					mem.V[x] = mem.DT;
					break;
				case 0x0A: {
					//waits for a key to be pressed if not it wastes the cycle
					bool isKeyPressed = 0;
					for(int i=0; i<16; i++){
						if(keyboard[i]){
							mem.V[x] = i;
							isKeyPressed = 1;
							break;
						}
					}
					if(!isKeyPressed) mem.PC -= 2;
					break;
				}
				case 0x15:
					mem.DT = mem.V[x];
					break;
				case 0x18:
					mem.ST = mem.V[x];
					break;	
				case 0x1E:
					mem.I += mem.V[x];
					break;
				case 0x29:
					mem.I = (mem.V[x] * 5);//sprite mem location
					break;
				case 0x33: {
					int val = (int)mem.V[x];
					uint8_t h = (val / 100) % 10;
					uint8_t d = (val / 10) % 10;	
					uint8_t u = val % 10;
					mem.ram[mem.I] = h;
					mem.ram[mem.I+1] = d;
					mem.ram[mem.I+2] = u;
					break;
				}
				case 0x55:
					memcpy((mem.ram+mem.I), mem.V, x+1);
					break;
				case 0x65: {
					uint8_t i = 0;	
					while(i <= x){
						mem.V[i] = mem.ram[mem.I + i];
						i++;
					}
					break;
				}
			}
			break;
		}
	}
    if(mem.DT > 0){
		mem.DT--;
	}
}