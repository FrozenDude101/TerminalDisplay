#ifndef SPRITE_HEADER_LOADED
#define SPRITE_HEADER_LOADED


#include <string.h>

#include "display.h"
#include "pixelData.h"


typedef struct Sprite Sprite;
typedef void (*PixelModifier)(PixelData*);
typedef PixelData* (*PixelMapper)(PixelData*);


struct Sprite {

    size_t pixelCount;
    PixelData** pixels;

};


Sprite* newSprite(int x, int y, PixelData** pixels, size_t size);
void freeSprite(Sprite* sprite);

void drawSprite(Display* display, Sprite* sprite);

Sprite* rotateSprite(Sprite* sprite, int turns);
void __90Turn(PixelData* pixel);
void __180Turn(PixelData* pixel);
void __270Turn(PixelData* pixel);

void forEachPixel(Sprite* sprite, PixelModifier func);
PixelData** mapPixels(Sprite* sprite, PixelMapper func);


Sprite* newSprite(PixelData** pixels, size_t size) {

    Sprite* sprite = malloc(sizeof(Sprite));

    sprite -> pixelCount = size;

    sprite -> pixels = malloc(size * sizeof(PixelData*));
    memcpy(sprite -> pixels, pixels, size * sizeof(PixelData*));

    return sprite;

}
Sprite* cloneSprite(Sprite* sprite) {

    PixelData** pixelData = malloc(sprite -> pixelCount * sizeof(PixelData*));
    for (int i = 0; i < sprite -> pixelCount; i++) {
        PixelData* pixel = sprite -> pixels[i];
        pixelData[i] = newPixelData(pixel -> x, pixel -> y, pixel -> character, pixel -> colourPairId);
    }

    return newSprite(pixelData, sprite -> pixelCount);

}
void freeSprite(Sprite* sprite) {

    free(sprite -> pixels);
    free(sprite);

}

void drawSprite(Display* display, Sprite* sprite, int x, int y) {

    int scaledSpriteX = x;
    int scaledSpriteY = y;
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

Sprite* rotate(Sprite* sprite, int turns) {

    turns %= 4;
    turns += 4;
    turns %= 4;

    Sprite* clone = cloneSprite(sprite);

    switch (turns) {
        case 1: forEachPixel(clone, __90Turn); break;
        case 2: forEachPixel(clone, __180Turn); break;
        case 3: forEachPixel(clone, __270Turn); break;
    }

    return clone;

}
void __90Turn(PixelData* pixel) {

    int pixelX = pixel -> x;
    int pixelY = pixel -> y;

    pixel -> x = pixelY;
    pixel -> y = -pixelX - 1;

}
void __180Turn(PixelData* pixel) {

    int pixelX = pixel -> x;
    int pixelY = pixel -> y;

    pixel -> x = -pixelX - 1;
    pixel -> y = -pixelY - 1;

}
void __270Turn(PixelData* pixel) {

    int pixelX = pixel -> x;
    int pixelY = pixel -> y;

    pixel -> x = -pixelY - 1;
    pixel -> y = pixelX;

}

void forEachPixel(Sprite* sprite, PixelModifier func) {

    for (int i = 0; i < sprite -> pixelCount; i++) {
        func(sprite -> pixels[i]);
    }
    
}
PixelData** mapPixels(Sprite* sprite, PixelMapper func) {

    PixelData** newPixels = malloc(sprite -> pixelCount * sizeof(PixelData*));

    for (int i = 0; i < sprite -> pixelCount; i++) {
        newPixels[i] = func(sprite -> pixels[i]);
    }

    return newPixels;
    
}


#endif