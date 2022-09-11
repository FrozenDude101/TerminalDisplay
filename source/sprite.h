#ifndef SPRITE_HEADER_LOADED
#define SPRITE_HEADER_LOADED


#include <string.h>

#include "display.h"
#include "pixelData.h"


typedef struct Sprite Sprite;
typedef void (*PixelModifier)(PixelData*);


struct Sprite {

    int x;
    int y;

    size_t pixelCount;
    PixelData** pixels;

};


Sprite* newSprite(int x, int y, PixelData** pixels, size_t size);
void freeSprite(Sprite* sprite);

void drawSprite(Display* display, Sprite* sprite);


Sprite* newSprite(int x, int y, PixelData** pixels, size_t size) {

    Sprite* sprite = malloc(sizeof(Sprite));

    sprite -> x = x;
    sprite -> y = y;

    sprite -> pixels = malloc(size * sizeof(PixelData*));
    memcpy(sprite -> pixels, pixels, size * sizeof(PixelData*));

    sprite -> pixelCount = size;

    return sprite;

}
void freeSprite(Sprite* sprite) {

    free(sprite -> pixels);
    free(sprite);

}

void drawSprite(Display* display, Sprite* sprite) {

    int scaledSpriteX = sprite -> x;
    int scaledSpriteY = sprite -> y;
    scaleXY(display, &scaledSpriteX, &scaledSpriteY);

    for (int i = 0; i < sprite -> pixelCount; i++) {

        PixelData* pixel = sprite -> pixels[i];
        int scaledPixelX = pixel -> x;
        int scaledPixelY = pixel -> y;
        scaleXY(display, &scaledPixelX, &scaledPixelY);

        wattron(display -> window, COLOR_PAIR(pixel -> colourPairId));
        for (int j = 0; j < display -> scaleY; j++) {
            for (int k = 0; k < display -> scaleX; k++) {
                mvwaddch(display -> window, scaledSpriteY + scaledPixelY + j, scaledSpriteX + scaledPixelX + k, pixel -> character);
            }
        }
        wattroff(display -> window, COLOR_PAIR(pixel -> colourPairId));

    }

}

void forEachPixel(Sprite* sprite, PixelModifier func) {

    for (int i = 0; i < sprite -> pixelCount; i++) {
        func(sprite -> pixels[i]);
    }
    
}


#endif