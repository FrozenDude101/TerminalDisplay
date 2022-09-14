#ifndef DISPLAY_HEADER_LOADED
#define DISPLAY_HEADER_LOADED


#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>


#define FILLER_CHAR ' '
//  ' ' and ACS_CKBOARD both work.
//  ' ' would set depending on the background colour.
//  ACS_CKBOARD would set depending on the foreground colour.


typedef struct Display Display;


struct Display {

    WINDOW* window;

    int height;
    int width;
    int scaleX;
    int scaleY;

    int currentFrame;
    int msPerFrame;

    short maxColourId;

};


//  newDisplay  ->  Allocates and creates a new display.
//      int width           ->  The unscaled width of the display.
//      int height          ->  The unscaled height of the display.
//      int scale           ->  The scale to use, multiples all pixels to zoom in.
//      int fps             ->  Determines how long a pause exists when calling refreshDisplay.
//      returns Display*    ->  A new display.
Display* newDisplay(int width, int height, int scale, int fps);

//  refreshDisplay  ->  Refreshes a display to draw the next frame.
//      Display* display    ->  The display to refresh.
void refreshDisplay(Display* display);
//  clearDisplay  ->  Clears a display to reset it.
//      Display* display    ->  The display to clear.
void clearDisplay(Display* display);

//  scaleXY ->  Scales a value in the x direction, and a value in the y direction.
//      Display* display    ->  The display to scale on.
//      int* x              ->  A pointer to the integer to scale in the X direction.
//      int* y              ->  A pointer to the integer to scale in the Y direction.
void scaleXY(Display* display, int* x, int* y);
//  scaleX ->  Scales a value in the x direction.
//      Display* display    ->  The display to scale on.
//      int* x              ->  A pointer to the integer to scale in the X direction.
void scaleX(Display* display, int* x);
//  scaleY ->  Scales a value in the y direction.
//      Display* display    ->  The display to scale on.
//      int* y              ->  A pointer to the integer to scale in the Y direction.
void scaleY(Display* display, int* y);

//  addColourPair   ->  Registers a colour pair with the window and returns the id.
//      Display* display    ->  The display to register to. (Prevents ID clashes.)
//      short foreground    ->  The colour to display in the foreground text.
//      short background    ->  The colour to display in the background.
//      returns short       ->  The colour pair's ID.
short addColourPair(Display* display, short foreground, short background);

//  colourPixel ->  Colours a specific pixel on the display.
//      Display* display    ->  The display to colour.
//      short colourIndex   ->  The ID of the colour pair to use.
//      int x               ->  The x co-ordinate of the pixel to colour.
//      int y               ->  The y co-ordinate of the pixel to colour.
void colourPixel(Display* display, short colourIndex, int x, int y);


Display* newDisplay(int width, int height, int scale, int fps) {

    if (has_colors() == FALSE) {    //  Checks if the terminal can support colour, exits if it can't.
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
    start_color();

    Display* display = malloc(sizeof(Display));

    display -> width = width;
    display -> height = height;

    display -> scaleX = scale * 2;  //  The X scale is twice as big to make the pixels look square.
    display -> scaleY = scale;

    display -> currentFrame = 0;
    display -> msPerFrame = 1000/fps;   //  Calculates the ms between frames.

    display -> maxColourId = 0; //  Colour id 0 cannot be used.

    int scaledWidth = width * display -> scaleX;
    int scaledHeight = height * display -> scaleY;

    int centerX = (COLS - scaledWidth) / 2;
    int centerY = (LINES - scaledHeight) / 2;

    WINDOW* borderWin = newwin(scaledHeight + 2, scaledWidth + 4, centerY - 1, centerX - 2);    //  The separate window keeps the border in place forever.

    wborder(borderWin, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD);

    wmove(borderWin, 0, 1);
    wvline(borderWin, ACS_CKBOARD, scaledHeight + 2);   //  Makes the border the same thickness throughout.
    wmove(borderWin, 0, scaledWidth + 2);
    wvline(borderWin, ACS_CKBOARD, scaledHeight + 2);   //  Makes the border the same thickness throughout.

    wrefresh(borderWin);    //  Displays the border.

    WINDOW* win = newwin(scaledHeight, scaledWidth, centerY, centerX);
    
    curs_set(0);        // Hides the cursor.
    keypad(win, true);  // Enables special characters like F keys and arrow keys.
    nodelay(win, true); // Stops getch from blocking.
    noecho();           // Stops getch from echoing input characters.

    display -> window = win;

    addColourPair(display, COLOR_BLACK, COLOR_BLACK);   //  The background colour for 'empty' pixels.

    clearDisplay(display);      //  Fills the background with 'empty'.
    refreshDisplay(display);    //  Displays the empty window.

    return display;

}

void refreshDisplay(Display* display) {

    wrefresh(display -> window);
    refresh();  //  The parent window has to be refreshed after to show any changes.

    usleep(display -> msPerFrame * 1000);   //  TODO:   Take into account time taken during the frame.
    display -> currentFrame += 1;

}
void clearDisplay(Display* display) {

    for (int i = 0; i < display -> width; i++) {
        for (int j = 0; j < display -> height; j++) {
            colourPixel(display, 1, i, j);  //  Colours every pixel 'empty'.
        }
    }
    
}

void scaleXY(Display* display, int* x, int* y) {

    scaleX(display, x);
    scaleY(display, y);

}
void scaleX(Display* display, int* x) {

    *x *= display -> scaleX;

}
void scaleY(Display* display, int* y) {

    *y *= display -> scaleY;

}

short addColourPair(Display* display, short foreground, short background) {

    display -> maxColourId ++;  //  Ensures the colour ID has not been used, as long as this function is used.
    init_pair(display -> maxColourId, foreground, background);
    return display -> maxColourId;

}

void colourPixel(Display* display, short colourIndex, int x, int y) {

    scaleXY(display, &x, &y);   //  Scales the pixel co-ordinates.

    wattron(display -> window, COLOR_PAIR(colourIndex));    //  Sets the colour pair.
    for (int i = 0; i < display -> scaleY; i++) {
        for (int j = 0; j < display -> scaleX; j++) {
            mvwaddch(display -> window, y + i, x + j, FILLER_CHAR); //  Fills the pixel.
        }
    }
    wattroff(display -> window, COLOR_PAIR(colourIndex));   //  Unsets the colour pair.

}

#endif