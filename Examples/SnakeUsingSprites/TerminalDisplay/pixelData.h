#ifndef PIXELDATA_HEADER_LOADED
#define PIXELDATA_HEADER_LOADED


#include <ncurses.h>


typedef struct PixelData PixelData;


struct PixelData {

    int x;
    int y;  //  The offset from the sprite's 0,0.

    chtype character;

    int colourPairId;

};


//  newPixelData   ->  Allocates and creates a new pixelData.
//      int x               ->  The x offset of the pixel.
//      int y               ->  The y offset of the pixel.
//      chtype character    ->  The foreground character to display.    
//      int colourPairdId   ->  The colour pair to use when displaying.
//      returns PixelData*  ->  A new pixelData.
PixelData* newPixelData(int x, int y, chtype character, int colourPairId);
//  freePixelData  ->  Safely frees a pixelData from memory.
//      PixelData* pixelData  ->  The pixelData to free.
void freePixelData(PixelData* pixelData);

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