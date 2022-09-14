#include <ncurses.h>
#include <stdbool.h>
#include <time.h>

#include "TerminalDisplay/display.h"
#include "TerminalDisplay/sprite.h"
#include "TerminalDisplay/pixelData.h"
#include "queue.h"


#define UP 3
#define DOWN 1
#define RIGHT 0
#define LEFT 2


void gameLoop();

Sprite* makeSnakeHeadSprite(Display* display);
Sprite* makeSnakeBodySprite(Display* display);
Sprite* makeSnakeTailSprite(Display* display);
Sprite* makeAppleSprite(Display* display);

int directionBetween(int x1, int y1, int x2, int y2);

void drawSnake(Display* display, Queue* snake, Sprite** snakeHeads, Sprite** snakeBodies, Sprite** snakeTails);
void drawApple(Display* display, Sprite* appleSprite, int appleX, int appleY);

void moveSnake(Queue* snake, int direction);
void moveApple(Display* display, Queue* snake, int* appleX, int* appleY);

bool appleCollision(Queue* snake, int appleX, int appleY);
bool selfCollision(Queue* snake);
bool wallCollision(Display* display, Queue* snake);

int main(int argc, char** argv) {

    initscr();
    refresh();

    gameLoop();


    endwin();

    return EXIT_SUCCESS;

}

void gameLoop() {

    Display* display = newDisplay(64, 40, 1, 30);

    Sprite** snakeHeads = makeAllSpriteRotations(makeSnakeHeadSprite(display));
    Sprite** snakeBodies = makeAllSpriteRotations(makeSnakeBodySprite(display));
    Sprite** snakeTails = makeAllSpriteRotations(makeSnakeTailSprite(display));

    Sprite* appleSprite = makeAppleSprite(display);

    Queue* snake = newQueue();
    for (int i = 0; i < 5; i++) {
        enqueue(snake, i, 4);
    }

    int direction = RIGHT;

    int appleX, appleY;
    moveApple(display, snake, &appleX, &appleY);

    int quit;
    while (!quit) {

        if (display -> currentFrame % 10 == 0) {
            int tailX = snake -> head -> x;
            int tailY = snake -> head -> y;
            moveSnake(snake, direction);

            if (appleCollision(snake, appleX, appleY)) {
                moveApple(display, snake, &appleX, &appleY);
                push(snake, tailX, tailY);
            }

            if (selfCollision(snake) || wallCollision(display, snake)) {
                quit = 1;
            }
        }

        drawSnake(display, snake, snakeHeads, snakeBodies, snakeTails);
        drawApple(display, appleSprite, appleX, appleY);
            
        short c;
        while ((c = wgetch(display -> window)) != ERR) {
            switch (c) {
                case 'w':
                case 'W':
                case KEY_UP:
                    direction = UP; break;
                case 's':
                case 'S':
                case KEY_DOWN:
                    direction = DOWN; break;
                case 'a':
                case 'A':
                case KEY_LEFT:
                    direction = LEFT; break;
                case 'd':
                case 'D':
                case KEY_RIGHT:
                    direction = RIGHT; break;
            }
        }

        refreshDisplay(display);
        clearDisplay(display);

    }

}

Sprite* makeSnakeHeadSprite(Display* display) {

    short green = addColourPair(display, COLOR_GREEN, COLOR_GREEN);
    short yellow = addColourPair(display, COLOR_YELLOW, COLOR_YELLOW);

    short eyes = addColourPair(display, COLOR_BLACK, COLOR_YELLOW);
    short yellowMouth = addColourPair(display, COLOR_BLACK, COLOR_YELLOW);
    short mouth = addColourPair(display, COLOR_BLACK, COLOR_GREEN);

    PixelData** pixels = malloc(sizeof(PixelData*) * 14);

    pixels[0] = newPixelData(0, 0, ACS_CKBOARD, green);
    pixels[1] = newPixelData(1, 0, ACS_CKBOARD, green);
    pixels[2] = newPixelData(2, 0, ACS_CKBOARD, yellow);

    pixels[3] = newPixelData(0, 1, ACS_CKBOARD, green);
    pixels[4] = newPixelData(1, 1, ACS_CKBOARD, green);
    pixels[5] = newPixelData(2, 1, 'o', eyes);
    pixels[6] = newPixelData(3, 1, ACS_CKBOARD, green);

    pixels[7] = newPixelData(0, 2, ACS_CKBOARD, green);
    pixels[8] = newPixelData(1, 2, '_', yellowMouth);
    pixels[9] = newPixelData(2, 2, '_', mouth);
    pixels[10] = newPixelData(3, 2, ACS_CKBOARD, green);

    pixels[11] = newPixelData(0, 3, ACS_CKBOARD, green);
    pixels[12] = newPixelData(1, 3, ACS_CKBOARD, yellow);
    pixels[13] = newPixelData(2, 3, ACS_CKBOARD, green);

    return newSprite(pixels, 14);

}
Sprite* makeSnakeBodySprite(Display* display) {

    short green = addColourPair(display, COLOR_GREEN, COLOR_GREEN);
    short yellow = addColourPair(display, COLOR_YELLOW, COLOR_YELLOW);

    PixelData** pixels = malloc(sizeof(PixelData*) * 16);

    pixels[0] = newPixelData(0, 0, ACS_CKBOARD, green);
    pixels[1] = newPixelData(1, 0, ACS_CKBOARD, green);
    pixels[2] = newPixelData(2, 0, ACS_CKBOARD, yellow);
    pixels[3] = newPixelData(3, 0, ACS_CKBOARD, green);

    pixels[4] = newPixelData(0, 1, ACS_CKBOARD, green);
    pixels[5] = newPixelData(1, 1, ACS_CKBOARD, green);
    pixels[6] = newPixelData(2, 1, ACS_CKBOARD, yellow);
    pixels[7] = newPixelData(3, 1, ACS_CKBOARD, green);

    pixels[8] = newPixelData(0, 2, ACS_CKBOARD, green);
    pixels[9] = newPixelData(1, 2, ACS_CKBOARD, yellow);
    pixels[10] = newPixelData(2, 2, ACS_CKBOARD, green);
    pixels[11] = newPixelData(3, 2, ACS_CKBOARD, green);

    pixels[12] = newPixelData(0, 3, ACS_CKBOARD, green);
    pixels[13] = newPixelData(1, 3, ACS_CKBOARD, yellow);
    pixels[14] = newPixelData(2, 3, ACS_CKBOARD, green);
    pixels[15] = newPixelData(3, 3, ACS_CKBOARD, green);

    return newSprite(pixels, 16);
    
}
Sprite* makeSnakeTailSprite(Display* display) {

    short green = addColourPair(display, COLOR_GREEN, COLOR_GREEN);
    short yellow = addColourPair(display, COLOR_YELLOW, COLOR_YELLOW);

    PixelData** pixels = malloc(sizeof(PixelData*) * 12);

    pixels[0] = newPixelData(2, 0, ACS_CKBOARD, yellow);
    pixels[1] = newPixelData(3, 0, ACS_CKBOARD, green);

    pixels[2] = newPixelData(0, 1, ACS_CKBOARD, green);
    pixels[3] = newPixelData(1, 1, ACS_CKBOARD, green);
    pixels[4] = newPixelData(2, 1, ACS_CKBOARD, yellow);
    pixels[5] = newPixelData(3, 1, ACS_CKBOARD, green);

    pixels[6] = newPixelData(0, 2, ACS_CKBOARD, green);
    pixels[7] = newPixelData(1, 2, ACS_CKBOARD, yellow);
    pixels[8] = newPixelData(2, 2, ACS_CKBOARD, green);
    pixels[9] = newPixelData(3, 2, ACS_CKBOARD, green);

    pixels[10] = newPixelData(2, 3, ACS_CKBOARD, green);
    pixels[11] = newPixelData(3, 3, ACS_CKBOARD, green);

    return newSprite(pixels, 12);
    
}
Sprite* makeAppleSprite(Display* display) {

    short green = addColourPair(display, COLOR_GREEN, COLOR_GREEN);
    short red = addColourPair(display, COLOR_RED, COLOR_RED);

    PixelData** pixels = malloc(sizeof(PixelData*) * 13);

    pixels[0] = newPixelData(3, 0, ACS_CKBOARD, green);

    pixels[1] = newPixelData(0, 1, ACS_CKBOARD, red);
    pixels[2] = newPixelData(1, 1, ACS_CKBOARD, red);
    pixels[3] = newPixelData(2, 1, ACS_CKBOARD, green);
    pixels[4] = newPixelData(3, 1, ACS_CKBOARD, red);

    pixels[5] = newPixelData(0, 2, ACS_CKBOARD, red);
    pixels[6] = newPixelData(1, 2, ACS_CKBOARD, red);
    pixels[7] = newPixelData(2, 2, ACS_CKBOARD, red);
    pixels[8] = newPixelData(3, 2, ACS_CKBOARD, red);

    pixels[9] = newPixelData(0, 3, ACS_CKBOARD, red);
    pixels[10] = newPixelData(1, 3, ACS_CKBOARD, red);
    pixels[11] = newPixelData(2, 3, ACS_CKBOARD, red);
    pixels[12] = newPixelData(3, 3, ACS_CKBOARD, red);

    return newSprite(pixels, 13);
    
}

int directionBetween(int x1, int y1, int x2, int y2) {

    int deltaX = x2 - x1;
    int deltaY = y2 - y1;

    if (deltaX == 1) return RIGHT;
    if (deltaX == -1) return LEFT;
    if (deltaY == 1) return DOWN;
    if (deltaY == -1) return UP;

    return -1;

}

void drawSnake(Display* display, Queue* snake, Sprite** snakeHeads, Sprite** snakeBodies, Sprite** snakeTails) {

    int* snakeData = getQueueData(snake);
    int snakeLength = queueLength(snake) - 1;

    int direction = directionBetween(snakeData[0], snakeData[1], snakeData[2], snakeData[3]);
    drawSprite(display, snakeTails[direction], snakeData[0] * 4, snakeData[1] * 4);

    for (int i = 1; i < snakeLength; i++) {
        direction = directionBetween(snakeData[i*2], snakeData[i*2 + 1], snakeData[i*2 + 2], snakeData[i*2 + 3]);
        drawSprite(display, snakeBodies[direction], snakeData[i*2] * 4, snakeData[i*2 + 1] * 4);
    }

    drawSprite(display, snakeHeads[direction], snakeData[snakeLength*2] * 4, snakeData[snakeLength*2 + 1] * 4);

    freeQueueData(snakeData);

}
void drawApple(Display* display, Sprite* appleSprite, int appleX, int appleY) {

    drawSprite(display, appleSprite, appleX*4, appleY*4);

}

void moveSnake(Queue* snake, int direction) {

    int headX = snake -> tail -> x;
    int headY = snake -> tail -> y;

    switch (direction) {
        case UP:    headY--; break;
        case DOWN:  headY++; break;
        case LEFT:  headX--; break;
        case RIGHT: headX++; break;
    }

    dequeue(snake);
    enqueue(snake, headX, headY);

}
void moveApple(Display* display, Queue* snake, int* appleX, int* appleY) {

    do {
        *appleX = random() % (display -> width / 4);
        *appleY = random() % (display -> height / 4);
    } while (appleCollision(snake, *appleX, *appleY));

}

bool appleCollision(Queue* snake, int appleX, int appleY) {

    int* snakeData = getQueueData(snake);
    
    for (int i = 0; i < queueLength(snake); i++) {
        if (snakeData[2 * i] == appleX && snakeData[2 * i + 1] == appleY) {
            free(snakeData);
            return true;
        }
    }

    free(snakeData);
    return false;

}
bool selfCollision(Queue* snake) {

    int headX = snake -> tail -> x;
    int headY = snake -> tail -> y;

    int* snakeData = getQueueData(snake);
    
    for (int i = 0; i < queueLength(snake) - 1; i++) {
        if (snakeData[2 * i] == headX && snakeData[2 * i + 1] == headY) {
            free(snakeData);
            return true;
        }
    }

    free(snakeData);
    return false;

}
bool wallCollision(Display* display, Queue* snake) {

    int headX = snake -> tail -> x;
    int headY = snake -> tail -> y;

    return headX > display -> width / 4 - 1 || headX < 0 || headY > display -> height / 4 - 1 || headY < 0;

}