#ifndef CPU
#define CPU

struct screen {
	uint8_t screen[32][64];
};
void initializeCpu(FILE *fd);
void execute();
struct screen *getScreen();

#endif