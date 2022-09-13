#ifndef SPRITE_HEADER_LOADED
#define SPRITE_HEADER_LOADED


#include <string.h>

#include "display.h"
#include "pixelData.h"


typedef struct Sprite Sprite;
typedef void (*PixelModifier)(PixelData*);      //  Takes a PixelData*, returns nothing.
typedef PixelData* (*PixelMapper)(PixelData*);  //  Takes a PixelData*, returns a PixelData*.


struct Sprite {

    size_t pixelCount;
    PixelData** pixels;

};


//  newSprite   ->  Allocates and creates a new sprite.
//      PixelData** pixels  ->  An array of modified pixels from the background.
//      size_t size         ->  The length of the pixel array.
//      returns Sprite*     ->  A new sprite.
Sprite* newSprite(PixelData** pixels, size_t size);
//  cloneSprite ->  Clones a sprite, duplicating the pixel array to remove the reference.
//      Sprite* sprite  ->  The sprite to clone.
//      returns Sprite* ->  A new cloned sprite.
Sprite* cloneSprite(Sprite* sprite);
//  freeSprite  ->  Safely frees a sprite from memory.
//      Sprite* sprite  ->  The sprite to free.
void freeSprite(Sprite* sprite);

//  drawSprite  ->  Draws a sprite on the given display, at the given co-ordinates.
//      Display* display    ->  The display to draw the sprite to.
//      Sprite* sprite      ->  The sprite to draw.
//      int x               ->  The x position of the sprite.
//      int y               ->  The y position of the sprite.
void drawSprite(Display* display, Sprite* sprite, int x, int y);

//  rotateSprite    ->  Rotates a sprite around it's 0,0 top left corner <turns> times 90d clockwise.
//      Sprite* sprite  ->  The sprite to rotate.
//      int turns       ->  The number of times to rotate 90d clockwise.
//      returns Sprite* ->  A new rotated sprite.
Sprite* rotateSprite(Sprite* sprite, int turns);
void __90Turn(PixelData* pixel);
void __180Turn(PixelData* pixel);
void __270Turn(PixelData* pixel);

//  makeAllSpriteRotations  ->  Creates an array of all 4 sprite rotations.
//      Sprite* sprite  ->  A sprite to rotate.
//      return Sprite** ->  The array of rotated sprites.
Sprite** makeAllSpriteRotations(Sprite* sprite);

//  reflectSpriteX  ->  Reflects a sprite around y = 0, its left edge.
//      Sprite* sprite  ->  The sprite to reflect.
//      returns Sprite* ->  A new reflected sprite.
Sprite* reflectSpriteX(Sprite* sprite);
void __reflectX(PixelData* pixel);
//  reflectSpriteY  ->  Reflects a sprite around x = 0, its top edge.
//      Sprite* sprite  ->  The sprite to reflect.
//      returns Sprite* ->  A new reflected sprite.
Sprite* reflectSpriteY(Sprite* sprite);
void __reflectY(PixelData* pixel);

//  realignSprite   ->  Moves a sprite's pixels to align their top and left edge with x and y = 0 respectively.
//      Sprite* sprite  ->  The sprite to realign.
void realignSprite(Sprite* sprite);

//  forEachPixel    ->  Runs a given function on every PixelData* pixel of the given sprite.
//      Sprite* sprite      ->  The sprite to modify.  
//      PixelModifier func  ->  The function to call, takes a single PixelData* argument, returns void.
void forEachPixel(Sprite* sprite, PixelModifier func);
//  mapPixels   ->  Returns a new pixel array, the contents are the return of each pixel being passed to the mapper.
//      Sprite* sprite      ->  The sprite to get pixel data from.  
//      PixelMapper func    ->  The function to call, takes a single PixelData* argument, returns a PixelData*.
//      returns PixelData** ->  The new updated pixel array.
PixelData** mapPixels(Sprite* sprite, PixelMapper func);


Sprite* newSprite(PixelData** pixels, size_t size) {

    Sprite* sprite = malloc(sizeof(Sprite));

    sprite -> pixelCount = size;

    sprite -> pixels = malloc(size * sizeof(PixelData*));
    memcpy(sprite -> pixels, pixels, size * sizeof(PixelData*));    //  Copies only the pointers inside the array, not the PixelData structs themselves.

    return sprite;

}
Sprite* cloneSprite(Sprite* sprite) {

    PixelData** pixelData = malloc(sprite -> pixelCount * sizeof(PixelData*));
    for (int i = 0; i < sprite -> pixelCount; i++) {
        PixelData* pixel = sprite -> pixels[i];
        pixelData[i] = newPixelData(pixel -> x, pixel -> y, pixel -> character, pixel -> colourPairId); //  Duplicates the data to remove pointer references.
    }

    return newSprite(pixelData, sprite -> pixelCount);

}
void freeSprite(Sprite* sprite) {

    for (int i = 0; i < sprite -> pixelCount; i++) {
        freePixelData(sprite -> pixels[i]);
    }
    free(sprite -> pixels);
    free(sprite);

}

void drawSprite(Display* display, Sprite* sprite, int x, int y) {

    int scaledSpriteX = x;
    int scaledSpriteY = y;
    scaleXY(display, &scaledSpriteX, &scaledSpriteY);   //  Scales the coordinates of the sprite according to the display.

    for (int i = 0; i < sprite -> pixelCount; i++) {

        PixelData* pixel = sprite -> pixels[i];
        int scaledPixelX = pixel -> x;
        int scaledPixelY = pixel -> y;
        scaleXY(display, &scaledPixelX, &scaledPixelY);   //  Scales the coordinates of the pixel according to the display.

        wattron(display -> window, COLOR_PAIR(pixel -> colourPairId));  // Sets the colour pair.
        for (int j = 0; j < display -> scaleY; j++) {
            for (int k = 0; k < display -> scaleX; k++) {
                mvwaddch(display -> window, scaledSpriteY + scaledPixelY + j, scaledSpriteX + scaledPixelX + k, pixel -> character);    // Adds the character at the correct position(s).
            }
        }
        wattroff(display -> window, COLOR_PAIR(pixel -> colourPairId)); // Unsets the colour pair.

    }

}

Sprite* rotateSprite(Sprite* sprite, int turns) {

    turns %= 4;
    turns += 4;
    turns %= 4; //  Limits turns to between 0-3. Turning anticlockwise once is the same as turning clockwise 3 times.
    turns = 4 - turns;  //  Turns the anticlockwise rotation clockwise.

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
    pixel -> y = -pixelX - 1;    //  Aligns the new sprite correctly.

}
void __180Turn(PixelData* pixel) {

    int pixelX = pixel -> x;
    int pixelY = pixel -> y;

    pixel -> x = -pixelX - 1;    //  Aligns the new sprite correctly.
    pixel -> y = -pixelY - 1;    //  Aligns the new sprite correctly.

}
void __270Turn(PixelData* pixel) {

    int pixelX = pixel -> x;
    int pixelY = pixel -> y;

    pixel -> x = -pixelY - 1;    //  Aligns the new sprite correctly.
    pixel -> y = pixelX;

}

Sprite** makeAllSpriteRotations(Sprite* sprite) {

    Sprite** sprites = malloc(sizeof(Sprite*) * 4);

    sprites[0] = sprite;
    for (int i = 1; i < 4; i++) {
        sprites[i] = rotateSprite(sprite, i);
    }

    return sprites;

}

Sprite* reflectSpriteX(Sprite* sprite) {

    Sprite* clone = cloneSprite(sprite);
    forEachPixel(clone, __reflectX);
    return clone;

}
void __reflectX(PixelData* pixel) {

    pixel -> x *= -1;
    pixel -> x -= 1;    //  Aligns the new sprite correctly.

}
Sprite* reflectSpriteY(Sprite* sprite) {

    Sprite* clone = cloneSprite(sprite);
    forEachPixel(clone, __reflectY);
    return clone;

}
void __reflectY(PixelData* pixel) {

    pixel -> y *= -1;
    pixel -> y -= 1;    //  Aligns the new sprite correctly.

}

void realignSprite(Sprite* sprite) {

    int minX = sprite -> pixels[0] -> x;
    int minY = sprite -> pixels[1] -> y;
    for (int i = 1; i < sprite -> pixelCount; i++) {    //  Calculates the x and y distance from 0,0
        PixelData* pixel = sprite -> pixels[i];
        if (minX > pixel -> x) minX = pixel -> x;
        if (minY > pixel -> y) minY = pixel -> y;
    }

    for (int i = 0; i < sprite -> pixelCount; i++) {    //  Moves each of the pixels over.
        PixelData* pixel = sprite -> pixels[i];
        pixel -> x -= minX;
        pixel -> y -= minY;
    }

}

void forEachPixel(Sprite* sprite, PixelModifier func) {

    for (int i = 0; i < sprite -> pixelCount; i++) {
        func(sprite -> pixels[i]);  //  Applies the function to every pixel.
    }
    
}
PixelData** mapPixels(Sprite* sprite, PixelMapper func) {

    PixelData** newPixels = malloc(sprite -> pixelCount * sizeof(PixelData*));

    for (int i = 0; i < sprite -> pixelCount; i++) {
        newPixels[i] = func(sprite -> pixels[i]);   //  Applies the function to every pixel and stores the result in the correct index.
    }

    return newPixels;
    
}


#endif