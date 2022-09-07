#ifndef DISPLAY_HEADER_LOADED
#define DISPLAY_HEADER_LOADED


#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>


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


Display* initDisplay(int width, int height, int scale, int fps);
void refreshDisplay(Display* display);
void clearDisplay(Display* display);

void scaleXY(Display* display, int* x, int* y);
void scaleX(Display* display, int* x);
void scaleY(Display* display, int* y);

int addColourPair(Display* display, short foreground, short background);

void colourPixel(Display* display, short colourIndex, int x, int y);


Display* newDisplay(int width, int height, int scale, int fps) {

    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
    start_color();

    Display* display = malloc(sizeof(Display));

    display -> width = width;
    display -> height = height;

    display -> scaleX = scale * 2;
    display -> scaleY = scale;

    display -> currentFrame = 0;
    display -> msPerFrame = 1000/fps;

    display -> maxColourId = 0;

    int scaledWidth = width * scale * 2;
    int scaledHeight = height * scale;

    int centerX = (COLS - scaledWidth) / 2;
    int centerY = (LINES - scaledHeight) / 2;

    WINDOW* borderWin = newwin(scaledHeight + 2, scaledWidth + 4, centerY - 1, centerX - 2);

    wborder(borderWin, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD);

    wmove(borderWin, 0, 1);
    wvline(borderWin, ACS_CKBOARD, scaledHeight + 2);
    wmove(borderWin, 0, scaledWidth + 2);
    wvline(borderWin, ACS_CKBOARD, scaledHeight + 2);

    wrefresh(borderWin);

    WINDOW* win = newwin(scaledHeight, scaledWidth, centerY, centerX);
    
    curs_set(0);        // Hides the cursor.
    keypad(win, true);  // Enables special characters like F keys and arrow keys.
    nodelay(win, true); // Stops getch from blocking.
    noecho();           // Stops getch from echoing input characters.
    wbkgd(win, ' ');
    wbkgd(win, ' ');

    display -> window = win;

    addColourPair(display, COLOR_BLACK, COLOR_BLACK);

    clearDisplay(display);
    refreshDisplay(display);

    return display;

}
void refreshDisplay(Display* display) {

    wrefresh(display -> window);
    refresh();

    usleep(display -> msPerFrame * 1000);
    display -> currentFrame += 1;

}
void clearDisplay(Display* display) {

    for (int i = 0; i < display -> width; i++) {
        for (int j = 0; j < display -> height; j++) {
            colourPixel(display, 1, i, j);
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

int addColourPair(Display* display, short foreground, short background) {

    display -> maxColourId ++;
    init_pair(display -> maxColourId, foreground, background);
    return display -> maxColourId;

}

void colourPixel(Display* display, short colourIndex, int x, int y) {

    scaleXY(display, &x, &y);

    wattron(display -> window, COLOR_PAIR(colourIndex));
    for (int i = 0; i < display -> scaleY; i++) {
        for (int j = 0; j < display -> scaleX; j++) {
            mvwaddch(display -> window, y + i, x + j, ACS_CKBOARD);
        }
    }
    wattroff(display -> window, COLOR_PAIR(colourIndex));

}

#endif