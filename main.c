#include <stdio.h>
#include <stdlib.h>

#include "gba.h"
#include "main.h"
#include "images/start.h"
#include "images/apple.h"
#include "images/basket.h"
#include "images/background.h"

int highScore = 0;
int currentScore = 0;

enum gba_state state;
u32 previousButtons;
u32 currentButtons;

PlayerBasket player;
FallingApple falling;

int prevAppleRow, prevAppleCol;
int prevPlayerRow, prevPlayerCol;

int highScoreDrawn = 0;
int startScreenDrawn = 0;

void initGame(void) {
    drawImageDMA(0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, background);

    player.row = 140;
    player.col = 110;

    falling.row = 25;
    falling.col = rand() % (240 - APPLE_WIDTH);
    falling.width = APPLE_WIDTH;
    falling.height = APPLE_HEIGHT;
    falling.speed = 2;

    prevAppleRow = falling.row;
    prevAppleCol = falling.col;
    prevPlayerRow = player.row;
    prevPlayerCol = player.col;

    currentScore = 0;
}

void resetGame(void) {
    highScore = 0;
    currentScore = 0;

    player.row = 140;
    player.col = 110;

    falling.row = 25;
    falling.col = rand() % (240 - APPLE_WIDTH);
    falling.width = APPLE_WIDTH;
    falling.height = APPLE_HEIGHT;
    falling.speed = 2;

    prevAppleRow = falling.row;
    prevAppleCol = falling.col;
    prevPlayerRow = player.row;
    prevPlayerCol = player.col;

    highScoreDrawn = 0;
    startScreenDrawn = 0;
    state = START;
}

void updateGame(void) {
    if (KEY_DOWN(BUTTON_LEFT, currentButtons) && player.col > 0) player.col -= 2;
    if (KEY_DOWN(BUTTON_RIGHT, currentButtons) && player.col < 210) player.col += 2;
    if (KEY_DOWN(BUTTON_UP, currentButtons) && player.row > 0) player.row -= 2;
    if (KEY_DOWN(BUTTON_DOWN, currentButtons) && player.row < 150) player.row += 2;

    falling.row += falling.speed;

    if (falling.row >= 160) {
        if (currentScore > highScore) highScore = currentScore;
        state = START;
        highScoreDrawn = 0;
        startScreenDrawn = 0;
    } else if (
        falling.row + falling.height >= player.row &&
        falling.col + falling.width >= player.col &&
        falling.col <= player.col + BASKET_WIDTH) {
        currentScore++;
        falling.row = 25;
        falling.col = rand() % (240 - APPLE_WIDTH);
    }
}

void undrawImage(int row, int col, int width, int height) {
    for (int r = 0; r < height; r++) {
        const u16* bgSrc = &background[(row + r) * BACKGROUND_WIDTH + col];
        volatile u16* screenDst = &videoBuffer[(row + r) * BACKGROUND_WIDTH + col];
        DMA[3].src = bgSrc;
        DMA[3].dst = (u16*)screenDst;
        DMA[3].cnt = width | DMA_ON;
    }
}

void drawScore(void) {
    drawRectDMA(5, 145, 80, 10, BLACK);
    char text[20];
    snprintf(text, sizeof(text), "Score: %d", currentScore);
    drawString(5, 145, text, WHITE);
}

void drawHighScore(void) {
    drawRectDMA(15, 145, 90, 10, BLACK);
    char text[30];
    snprintf(text, sizeof(text), "High Score: %d", highScore);
    drawString(15, 145, text, WHITE);
}

void drawGame(void) {
    undrawImage(prevAppleRow, prevAppleCol, APPLE_WIDTH, APPLE_HEIGHT);
    undrawImage(prevPlayerRow, prevPlayerCol, BASKET_WIDTH, BASKET_HEIGHT);

    drawImageDMA(falling.row, falling.col, APPLE_WIDTH, APPLE_HEIGHT, apple);
    drawImageDMA(player.row, player.col, BASKET_WIDTH, BASKET_HEIGHT, basket);

    prevAppleRow = falling.row;
    prevAppleCol = falling.col;
    prevPlayerRow = player.row;
    prevPlayerCol = player.col;

    waitForVBlank();
    drawScore();
}

int main(void) {
    REG_DISPCNT = MODE3 | BG2_ENABLE;

    previousButtons = BUTTONS;
    currentButtons = BUTTONS;
    state = START;

    while (1) {
        currentButtons = BUTTONS;
        waitForVBlank();

        if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
            resetGame();
        }

        switch (state) {
            case START:
                if (!startScreenDrawn) {
                    drawImageDMA(0, 0, START_WIDTH, START_HEIGHT, start);
                    startScreenDrawn = 1;
                }

                if (!highScoreDrawn) {
                    drawHighScore();
                    highScoreDrawn = 1;
                }

                if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
                    initGame();
                    highScoreDrawn = 0;
                    startScreenDrawn = 0;
                    state = PLAY;
                }
                break;

            case PLAY:
                updateGame();
                drawGame();
                break;
        }

        previousButtons = currentButtons;
    }
    return 0;
}