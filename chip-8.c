#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "raylib.h"
#include "./cpu/cpu.h"

#define WIDTH 64
#define HEIGHT 32
#define CELLSIZE 20

void DrawGridR(){
	struct screen *scr = getScreen();
    for (int row = 0; row < WIDTH; row++){
        for (int col = 0; col < HEIGHT; col++){
            DrawRectangle(col*CELLSIZE,
				row*CELLSIZE,
				CELLSIZE,
				CELLSIZE,
				scr->screen[row][col] == 1 ? GREEN : BLACK);
        }
    }
}

int main(int argc, char **argv){
	if(argc != 2)
		exit(-1);

	//load program in memory
	FILE *fd = fopen(argv[1], "rb");
	if(!fd) exit(-1);
	initialize(fd);

	InitWindow(WIDTH*CELLSIZE, HEIGHT*CELLSIZE, "chip-8");
	SetTargetFPS(60);
	double executionTimer = 1;
	double threshold = 1/60.0;
	while (!WindowShouldClose())
    {
		executionTimer += GetFrameTime();
		if(executionTimer >= threshold){
			executionTimer = 0.0;

			//emulation logic
			execute();

			//drawing display
			BeginDrawing();
				ClearBackground(RAYWHITE);
				DrawGridR();
			EndDrawing();
		}
    }
	fclose(fd);
    CloseWindow();
    return 0;	
}
