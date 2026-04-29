#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "raylib.h"

struct keyboard
{
    uint8_t kbd[16];
};
static struct keyboard keyboard;

void initializeKeyboard(){
    memset(keyboard.kbd, 0, 16);
}

uint8_t *getKeyboard(){
    return keyboard.kbd;
}

bool isKeyPressed(int key){
    return keyboard.kbd[key];
}

void getkeys() {
	keyboard.kbd[0x0] = IsKeyDown(KEY_ZERO);
	keyboard.kbd[0x1] = IsKeyDown(KEY_ONE);
	keyboard.kbd[0x2] = IsKeyDown(KEY_TWO);
	keyboard.kbd[0x3] = IsKeyDown(KEY_THREE);
	keyboard.kbd[0x4] = IsKeyDown(KEY_FOUR);
	keyboard.kbd[0x5] = IsKeyDown(KEY_FIVE);
	keyboard.kbd[0x6] = IsKeyDown(KEY_SIX);
	keyboard.kbd[0x7] = IsKeyDown(KEY_SEVEN);
	keyboard.kbd[0x8] = IsKeyDown(KEY_EIGHT);
	keyboard.kbd[0x9] = IsKeyDown(KEY_NINE);
	keyboard.kbd[0xA] = IsKeyDown(KEY_A);
	keyboard.kbd[0xB] = IsKeyDown(KEY_B);
	keyboard.kbd[0xC] = IsKeyDown(KEY_C);
	keyboard.kbd[0xD] = IsKeyDown(KEY_D);
	keyboard.kbd[0xE] = IsKeyDown(KEY_E);
	keyboard.kbd[0xF] = IsKeyDown(KEY_F);
}

static char *foo[16] = { "0", "1", "2", "3", "4", "5", "6", "7",
                         "8", "9", "A", "B", "C", "D", "E", "F"};
char *getKeyChar(int key){
    return foo[key];
}