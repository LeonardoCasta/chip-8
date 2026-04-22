#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "raylib.h"

#define WIDTH 64
#define HEIGHT 32
#define CELLSIZE 20

typedef struct {
    int state;
    Rectangle rect;
    Color color; 
} Cell;

struct memory {
	//ram is 0xFFF so 0 to 4095 bytes
	//ram for emulatro 0x000 to 0x1FF
	//ram for program 0x200 to 0xFFF
	uint8_t ram[4096];
	
	uint8_t V[16];     //16 general purpose 8 bit registers	
	uint16_t I;        //reg for memory addresses, only lowest 12 bits used
	
	int DT;
	uint8_t sound;
	
	uint16_t opcode;    //currently executed operation
	uint16_t PC;        //currently executing address
	uint8_t SP;         //stack pointer
	uint16_t stack[16]; //stack
};
struct memory mem = {
	.opcode = 0,
	.I = 0,
	.PC = 0x200,
	.SP = 0,
	.DT = 0,
	.sound = 0 };

uint8_t keyboard[] = {0}; //values goes from 0x0 to 0xF
int clearDisplay = 0;
float timer = 0;

//Cell (*grid)[WIDTH];

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

int getRayKey() {
	keyboard[0x0] = IsKeyDown(KEY_ZERO);
	keyboard[0x1] = IsKeyDown(KEY_ONE);
	keyboard[0x2] = IsKeyDown(KEY_TWO);
	keyboard[0x3] = IsKeyDown(KEY_THREE);
	keyboard[0x4] = IsKeyDown(KEY_FOUR);
	keyboard[0x5] = IsKeyDown(KEY_FIVE);
	keyboard[0x6] = IsKeyDown(KEY_SIX);
	keyboard[0x7] = IsKeyDown(KEY_SEVEN);
	keyboard[0x8] = IsKeyDown(KEY_EIGHT);
	keyboard[0x9] = IsKeyDown(KEY_NINE);
	keyboard[0xA] = IsKeyDown(KEY_A);
	keyboard[0xB] = IsKeyDown(KEY_B);
	keyboard[0xC] = IsKeyDown(KEY_C);
	keyboard[0xD] = IsKeyDown(KEY_E);
	keyboard[0xE] = IsKeyDown(KEY_F);
	keyboard[0xF] = IsKeyDown(KEY_G);
}

void drawSprite(uint8_t x, uint8_t y, uint8_t n, Cell (*grid)[WIDTH]){
	printf("%d\n", n);
	for(int i = 0; i < n; i++){
		uint8_t byte = mem.ram[mem.I+i];
		printf("%02X\n", byte);
		for(int j = 0; j < 8; j++){
			uint8_t number = (byte >> 7-j) & 1;
			int m = (x+j)%64;
			int z = (y+i)%32; 
			if(grid[m][z].state & number)
				mem.V[0xF] = 1;
			else
				mem.V[0xF] = 0;

			grid[m][z].state = grid[m][z].state ^ number;
			printf("i:%d j:%d m:%d z:%d v:%d Vf:%d\n",i,j, m, z, number, mem.V[0xF]);
		}
	}
}

void execute(Cell (*grid)[WIDTH]){
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
				mem.PC = mem.stack[mem.SP];
				mem.SP--;
			}
			break;
		case 0x1000:
			mem.PC = mem.opcode & 0x0FFF;
			break;
		case 0x2000:
			mem.SP++;
			mem.stack[mem.SP] = mem.PC;
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
					uint16_t i = mem.V[x] + mem.V[y];
					mem.V[x] = i & 0x0F;
					if(((i & 0xF0) >> 8) != 0)
						mem.V[0xF] = 1;	
					else
						mem.V[0xF] = 0;
					break;
				}
				case 0x5: 
					if(mem.V[x] > mem.V[y])
						mem.V[0xF] = 1;	
					else
						mem.V[0xF] = 0;
					mem.V[x] = mem.V[x] - mem.V[y];
					break;
				case 0x6:
					if(mem.V[x] % 2 != 0)
						mem.V[0xF] = 1;	
					else
						mem.V[0xF] = 0;
					mem.V[x] /= 2;
					break;
				case 0x7:
					if(mem.V[y] > mem.V[x])
						mem.V[0xF] = 1;	
					else
						mem.V[0xF] = 0;
					mem.V[x] = mem.V[y] - mem.V[x];
					break;
				case 0xE:
					if((mem.V[x] & 0x80) == 0)	
						mem.V[0xF] = 0;
					else
						mem.V[0xF] = 1;
					break;
			break;
			}
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
			srand(time(NULL));
			int r = rand() % 255;
			mem.V[x] = r & kk;
			break;
		}
		case 0xD000: {
			uint8_t x = (mem.opcode & 0x0F00) >> 8;
			uint8_t y = (mem.opcode & 0x00F0) >> 4;	
			uint8_t n = mem.opcode & 0x000F;
			printf("%04X\n", mem.opcode);
			drawSprite(x, y, n, grid);
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
					char key;
					scanf("%c", &key);
					uint8_t a = key - '0';
					mem.V[x] = key;
					break;
				}
				case 0x15:
					mem.DT = mem.V[x];
					break;
				case 0x18:
					mem.sound = mem.V[x];
					break;	
				case 0x1E:
					mem.I += mem.V[x];
					break;
				case 0x29:
					mem.I = 0x50 + (mem.V[x] * 5);//sprite mem location
					break;
				case 0x33: {
					int x = (int)mem.V[x];
					uint8_t h = (x / 100) % 10;
					uint8_t d = (x / 10) % 10;	
					uint8_t u = x % 10;
					mem.ram[mem.I] = h;
					mem.ram[mem.I+1] = h;
					mem.ram[mem.I+2] = u;
					break;
				}
				case 0x55:
					memcpy((mem.ram+mem.I), mem.V, sizeof(uint8_t)*16);
					break;
				case 0x65: {
					uint8_t i = 0;	
					while(i < 15){
						mem.V[i] = mem.ram[mem.I + i];
						i++;
					}
					break;
				}
			}
			break;
		}
	}
}

struct display {

};

void CreateGrid(Cell (*grid)[WIDTH]){
    for (int j = 0; j < WIDTH; j++){
        for (int i = 0; i < HEIGHT; i++){
            struct Rectangle rect = {.x = j*CELLSIZE, .y = i*CELLSIZE, .height = CELLSIZE, .width = CELLSIZE}; 
            grid[i][j] = (Cell){.state = 0, .rect = rect, .color = GREEN};
        }
    }
}

void DrawGridR(Cell (*grid)[WIDTH]){
    for (int j = 0; j < WIDTH; j++){
        for (int i = 0; i < HEIGHT; i++){
            //set color
			if(clearDisplay || !grid[i][j].state){
				grid[i][j].color = BLACK;
			}else{
                grid[i][j].color = GREEN;
            }

            DrawRectangleRounded(
                grid[i][j].rect,
			 	0.0f,
				0.0f,
                grid[i][j].color            
            );
        }
    }
}

int main(int argv, char **argc){
	if(argv != 2)
		exit(-1);
		
	memset(mem.ram, 0, 4096);
	memset(mem.V, 0, sizeof(uint8_t)*16);
	
	//set sprites in memory, al shifted by 0x50
	for(int i = 0; i < 80; i++){
		mem.ram[i+0x50] = font_set[i];
	}

	//load program in memory
	FILE *fd = fopen(argc[1], "r");
	if(!fd) exit(-1);
	int a = fread((mem.ram)+0x200, sizeof(uint8_t), 1024, fd);
	if(!a){
		exit(-1);
		//printf("%d\n", a);
		//printf("%02X\n", mem.ram[0x1FF]);
		//printf("%02X\n", mem.ram[0x200]);
	}

	//using brackets cos i want a pointer to a array of cells on an array or pointers 

    Cell (*grid)[WIDTH] = malloc(sizeof(Cell[WIDTH][HEIGHT]));
    CreateGrid(grid);

	InitWindow(WIDTH*CELLSIZE, HEIGHT*CELLSIZE, "chip-8");
	SetTargetFPS(60);
	double executionTimer = 1;
	double threshold = 1/60.0;
	while (!WindowShouldClose())
    {
		executionTimer += GetFrameTime();
		if(executionTimer >= threshold){
			executionTimer = 0.0;
			//save key state
			getRayKey();

			if(mem.DT > 0){
				mem.DT--;
			}

			//emulation logic
			execute(grid);

			//drawing display
			BeginDrawing();
				ClearBackground(RAYWHITE);
				DrawGridR(grid);
				if(clearDisplay) clearDisplay = 0; //after cleared i put 0
			EndDrawing();
		}
    }

    CloseWindow();
    return 0;	
}
