#ifndef CPU
#define CPU

struct screen {
	uint8_t screen[32][64];
};
void initialize(FILE *fd);
void execute();
struct screen *getScreen();

#endif