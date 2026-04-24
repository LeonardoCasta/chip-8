#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "raylib.h"
#include "cpu.h"

#define WIDTH 64
#define HEIGHT 32
#define CELLSIZE 20

typedef struct {
    int state;
    Rectangle rect;
    Color color; 
} Cell;

uint8_t keyboard[] = {0}; //values goes from 0x0 to 0xF

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
	mem.V[0xF] = 0;
	for(int i = 0; i < n; i++){
		uint8_t byte = mem.ram[mem.I+i];
		printf("%02X\n", byte);
		for(int j = 0; j < 8; j++){
			uint8_t number = (byte >> 7-j) & 1;
			int m = (x+j)%64;
			int z = (y+i)%32; 
			if(grid[m][z].state == 1 && number == 1)
				mem.V[0xF] = 1;

			grid[m][z].state = grid[m][z].state ^ number;
			printf("i:%d j:%d m:%d z:%d v:%d Vf:%d\n",i,j, m, z, number, mem.V[0xF]);
		}
	}
}

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
			if(!grid[i][j].state){
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

int main(int argc, char **argv){
	if(argv != 2)
		exit(-1);

	//load program in memory
	FILE *fd = fopen(argv[1], "rb");
	if(!fd) exit(-1);
	inizialize(fd);

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

			//emulation logic
			execute(grid);

			//drawing display
			BeginDrawing();
				ClearBackground(RAYWHITE);
				DrawGridR(grid);
			EndDrawing();
		}
    }
	fclose(fd);
    CloseWindow();
    return 0;	
}
