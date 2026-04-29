#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "raylib.h"
#include "raymath.h"
#include "./cpu/cpu.h"
#include "./keyboard/keyboard.h"

#define WIDTH 64
#define HEIGHT 32
#define CELLSIZE 20
#define SCREEN_WIDTH WIDTH*CELLSIZE
#define SCREEN_HEIGHT HEIGHT*CELLSIZE
#define BUTTON_SIZE SCREEN_WIDTH/8
#define FONT_SIZE 50

void drawScreen(){
	struct screen *scr = getScreen();
    for (int row = 0; row < HEIGHT; row++){
        for (int col = 0; col < WIDTH; col++){
            DrawRectangle(col*CELLSIZE,
				row*CELLSIZE,
				CELLSIZE,
				CELLSIZE,
				scr->screen[row][col] == 1 ? GREEN : BLACK);
        }
    }
}

void drawKeys(){
	Font font = GetFontDefault();
	uint8_t *kbd = getKeyboard();
	uint8_t keyIndex = 0;
	for(int i=0; i<2; i++){
		for(int j=0; j<8; j++){
			int posX = (j*BUTTON_SIZE);
			int posY = (i*BUTTON_SIZE)+SCREEN_HEIGHT;
			char *text = getKeyChar(keyIndex);
			DrawRectangle(
				posX,
				posY,
				BUTTON_SIZE,
				BUTTON_SIZE,
				kbd[keyIndex] == 1 ? DARKGREEN : WHITE);

			// Get the size of the text to draw
			Vector2 textSize = MeasureTextEx(
				font, 
				text, 
				FONT_SIZE, 
				FONT_SIZE*.1f);

			// Calculate the top-left text position based on the rectangle and alignment
			Vector2 textPos = (Vector2){
				posX + Lerp(0.0f, BUTTON_SIZE - textSize.x, 0.5),
				posY + Lerp(0.0f, BUTTON_SIZE - textSize.y, 0.5)
			};

			// Draw the text
			DrawTextEx(
				font, 
				text, 
				textPos, 
				FONT_SIZE, 
				FONT_SIZE*.1f, 
				BLACK);

			keyIndex++;
		}
	}
}

int main(int argc, char **argv){
	if(argc != 2)
		exit(-1);

	//load program in memory
	FILE *fd = fopen(argv[1], "rb");
	if(!fd) exit(-1);
	initializeCpu(fd);
	initializeKeyboard();

	InitWindow(WIDTH*CELLSIZE, HEIGHT*CELLSIZE+(BUTTON_SIZE*2), "chip-8");
	SetTargetFPS(60);
	double executionTimer = 1;
	double threshold = 1/60.0;
	while (!WindowShouldClose())
    {
		executionTimer += GetFrameTime();
		if(executionTimer >= threshold){
			executionTimer = 0.0;
			//save key state
			getkeys();

			//emulation logic
			execute();

			//drawing display
			BeginDrawing();
				ClearBackground(RAYWHITE);
				drawScreen();
				drawKeys();
			EndDrawing();
		}
    }
	fclose(fd);
    CloseWindow();
    return 0;	
}
