#include <ncurses.h>
#include <stdbool.h>
#include <time.h>

#include "../source/display.h"
#include "queue.h"

#define SNAKE_HEAD 1
#define SNAKE_BODY 2
#define APPLE      3
#define TEXT       4

#define WIDTH 17
#define HEIGHT 9
#define SCALE 4
#define FPS 30

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3


int main(int argc, char** argv);
void gameLoop(Display* display);

bool wallCollision(Display* display, Queue* queue);

bool selfCollision(Queue* snake);
void moveSnake(Queue* snake, int direction);
void drawSnake(Display* display, Queue* snake);

bool appleCollision(Queue* snake, int appleX, int appleY);
void positionApple(Display* display, Queue* snake, int* appleX, int* appleY);
void drawApple(Display* display, int appleX, int appleY);


int main(int argc, char** argv) {

    initscr();
    refresh();

    srandom(time(NULL));

    Display* display = initDisplay(WIDTH, HEIGHT, SCALE, FPS);

    init_pair(SNAKE_HEAD, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(SNAKE_BODY, COLOR_GREEN, COLOR_GREEN);
    init_pair(APPLE, COLOR_RED, COLOR_RED);
    init_pair(TEXT, COLOR_WHITE, COLOR_WHITE);

    gameLoop(display);

    endwin();

}
void gameLoop(Display* display) {

    Queue* snake = newQueue();
    for (int i = 1; i < 5; i++) {
        enqueue(snake, i, 4);
    }
    int direction = RIGHT;
    int nextDirection = -1;

    int appleX, appleY;
    positionApple(display, snake, &appleX, &appleY);

    int startFrame = -1;
    int frameDelay = 10;

    int score;

    int state = 0;
    bool quit = false;
    bool paused = false;
    while (!quit) {

        if (state == 0) {

            if (display -> frame % 10 == 0) {

                int tailX = snake -> head -> x;
                int tailY = snake -> head -> y;

                if (nextDirection != -1) direction = nextDirection;
                moveSnake(snake, direction);

                if (appleCollision(snake, appleX, appleY)) {
                    for (int i = 0; i < 1; i++) {
                        push(snake, tailX, tailY);
                    }
                    score += 1;
                    positionApple(display, snake, &appleX, &appleY);
                }

                if (wallCollision(display, snake) || selfCollision(snake)) {

                    startFrame = display -> frame;
                    state = 1;

                }

            }

            drawSnake(display, snake);
            drawApple(display, appleX, appleY);
            
            short c;
            while ((c = wgetch(display -> window)) != ERR || paused) {
                switch (c) {
                    case 'q':
                    case 'Q':
                        paused = false; quit = true; break;
                    case 'p':
                    case 'P':
                        paused = !paused; break;
                    case 'w':
                    case 'W':
                    case KEY_UP:
                        nextDirection = direction == DOWN ? -1 : UP; break;
                    case 's':
                    case 'S':
                    case KEY_DOWN:
                        nextDirection = direction == UP ? -1 : DOWN; break;
                    case 'a':
                    case 'A':
                    case KEY_LEFT:
                        nextDirection = direction == RIGHT ? -1 : LEFT; break;
                    case 'd':
                    case 'D':
                    case KEY_RIGHT:
                        nextDirection = direction == LEFT ? -1 : RIGHT; break;
                }
            }

        } else if (state == 1) {

            if (display -> frame - startFrame == frameDelay) {
                dequeue(snake);
                startFrame = display -> frame;
                if (frameDelay != 1) frameDelay -= 1;

                if (queueLength(snake) == 0) {
                    state = 2;
                    continue;
                }
            }

            drawSnake(display, snake);
            drawApple(display, appleX, appleY);
            
            short c;
            while ((c = wgetch(display -> window)) != ERR || paused) {
                switch (c) {
                    case 'q':
                    case 'Q':
                        paused = false; quit = true; break;
                    case 'p':
                    case 'P':
                        paused = !paused; break;
                }
            }

        } else if (state == 2) {

            colourPixel(display, TEXT, 1, 2);
            colourPixel(display, TEXT, 1, 3);
            colourPixel(display, TEXT, 1, 4);
            
            colourPixel(display, TEXT, 2, 1);
            colourPixel(display, TEXT, 2, 5);
            
            colourPixel(display, TEXT, 3, 1);
            colourPixel(display, TEXT, 3, 5);
            
            colourPixel(display, TEXT, 4, 1);
            colourPixel(display, TEXT, 4, 4);

            colourPixel(display, TEXT, 5, 2);
            colourPixel(display, TEXT, 5, 3);
            colourPixel(display, TEXT, 5, 5);

            colourPixel(display, TEXT, 7, 3);
            colourPixel(display, TEXT, 7, 4);

            colourPixel(display, TEXT, 8, 5);

            colourPixel(display, TEXT, 9, 3);
            colourPixel(display, TEXT, 9, 4);

            colourPixel(display, TEXT, 11, 2);
            colourPixel(display, TEXT, 11, 4);
            colourPixel(display, TEXT, 11, 5);

            colourPixel(display, TEXT, 13, 2);

            colourPixel(display, TEXT, 14, 1);
            colourPixel(display, TEXT, 14, 2);
            colourPixel(display, TEXT, 14, 3);
            colourPixel(display, TEXT, 14, 4);
            colourPixel(display, TEXT, 14, 5);

            colourPixel(display, TEXT, 15, 2);

            for (int i = 1; i < 6; i++) {
                colourPixel(display, TEXT, i, 7);
            }

            short c;
            while ((c = wgetch(display -> window)) != ERR || paused) {
                switch (c) {
                    case 'q':
                    case 'Q':
                        paused = false; quit = true; break;
                }
            }

        }

        refreshDisplay(display);
        clearDisplay(display);

    }

}

bool wallCollision(Display* display, Queue* snake) {

    int headX = snake -> tail -> x;
    int headY = snake -> tail -> y;

    return headX > display -> width - 1 || headX < 0 || headY > display -> height - 1 || headY < 0;

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
void drawSnake(Display* display, Queue* snake) {

    int* snakeData = getQueueData(snake);
    int snakeLength = queueLength(snake) - 1;

    colourPixel(display, SNAKE_HEAD, snakeData[snakeLength * 2], snakeData[snakeLength * 2 + 1]);
    for (int i = 0; i < snakeLength; i++) {
        colourPixel(display, SNAKE_BODY, snakeData[2 * i], snakeData[2 * i + 1]);
    }

    free(snakeData);

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
void positionApple(Display* display, Queue* snake, int* appleX, int* appleY) {

    do {
        *appleX = random() % display -> width;
        *appleY = random() % display -> height;
    } while (appleCollision(snake, *appleX, *appleY));

}
void drawApple(Display* display, int appleX, int appleY) {

    colourPixel(display, APPLE, appleX, appleY);

}
