#include <ncurses.h>
#include <time.h>

#include "display.h"
#include "sprite.h"
#include "pixelData.h"

void foo(PixelData* pixel);

int main() {

    initscr();
    refresh();

    srandom(time(NULL));

    Display* display = newDisplay(16, 10, 2, 30);
    int a = addColourPair(display, COLOR_GREEN, COLOR_GREEN);
    int b = addColourPair(display, COLOR_YELLOW, COLOR_YELLOW);
    int c = addColourPair(display, COLOR_MAGENTA, COLOR_MAGENTA);
    int d = addColourPair(display, COLOR_CYAN, COLOR_CYAN);

    PixelData** pixels = malloc(sizeof(PixelData*) * 7);
    pixels[0] = newPixelData(display, 0, 0, ACS_CKBOARD, 0);
    pixels[1] = newPixelData(display, 1, 0, ACS_CKBOARD, 0);
    pixels[2] = newPixelData(display, 2, 0, ACS_CKBOARD, 0);
    pixels[3] = newPixelData(display, 0, 1, ACS_CKBOARD, 0);
    pixels[4] = newPixelData(display, 0, 2, ACS_CKBOARD, 0);
    pixels[5] = newPixelData(display, 1, 2, ACS_CKBOARD, 0);
    pixels[6] = newPixelData(display, 2, 2, ACS_CKBOARD, 0);

    Sprite* sprite = newSprite(1, 1, pixels, 7);

    drawSprite(display, sprite);
    refreshDisplay(display);
    getch();
    clearDisplay(display);

    forEachPixel(sprite, *foo);
    drawSprite(display, sprite);
    refreshDisplay(display);
    getch();
    clearDisplay(display);

    forEachPixel(sprite, *foo);
    drawSprite(display, sprite);
    refreshDisplay(display);
    getch();
    clearDisplay(display);

    forEachPixel(sprite, *foo);
    drawSprite(display, sprite);
    refreshDisplay(display);
    getch();
    clearDisplay(display);

    forEachPixel(sprite, *foo);
    drawSprite(display, sprite);
    refreshDisplay(display);
    getch();
    clearDisplay(display);

    forEachPixel(sprite, *foo);
    drawSprite(display, sprite);
    refreshDisplay(display);
    getch();
    clearDisplay(display);

    endwin();

}

void foo(PixelData* pixel) {

    pixel -> colourPairId ++;

}