#ifndef KEYBOARD
#define KEYBOARD

void getkeys();
bool isKeyPressed(int key);
void initializeKeyboard();
uint8_t *getKeyboard();
char *getKeyChar(int key);

#endif