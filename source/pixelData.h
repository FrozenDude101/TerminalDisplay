#ifndef PIXELDATA_HEADER_LOADED
#define PIXELDATA_HEADER_LOADED


#include <ncurses.h>

#include "display.h"


struct Display;
typedef struct PixelData PixelData;


struct PixelData {

    int x;
    int y;

    chtype character;

    int colourPairId;

};


PixelData* newPixelData(int x, int y, chtype character, int colourPairId) {

    PixelData* pixelData = malloc(sizeof(PixelData));

    pixelData -> x = x;
    pixelData -> y = y;
    pixelData -> character = character;
    pixelData -> colourPairId = colourPairId;

    return pixelData;

}
void freePixelData(PixelData* pixelData) {

    free(pixelData);

}


#endif