#ifndef MAIN_H
#define MAIN_H

#include "gba.h"

enum gba_state {
    START,
    PLAY,
};

typedef struct {
    int row, col;
} PlayerBasket;

typedef struct {
    int row, col;
    int width, height;
    int speed;
} FallingApple;

extern int highScore;
extern int currentScore;

extern enum gba_state state;
extern u32 previousButtons;
extern u32 currentButtons;

extern PlayerBasket player;
extern FallingApple falling;

void initGame(void);
void resetGame(void);
void updateGame(void);
void drawGame(void);
void drawScore(void);
void drawHighScore(void);
void undrawImage(int row, int col, int width, int height);

#endif