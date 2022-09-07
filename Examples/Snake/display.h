#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>


typedef struct Display Display;


struct Display {

    WINDOW* window;

    int width;
    int height;
    int scale;

    int frame;
    int msPerFrame;

};


Display* initDisplay(int width, int height, int scale, int fps);
void refreshDisplay(Display* display);
void clearDisplay(Display* display);

void colourPixel(Display* display, short colourIndex, int x, int y);

void scaleXY(Display* display, int* x, int* y);
int scaleX(Display* display, int x);
int scaleY(Display* display, int y);

Display* initDisplay(int width, int height, int scale, int fps) {

    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
    start_color();

    Display* display = (Display*) malloc(sizeof(Display));

    display -> width = width;
    display -> height = height;
    display -> scale = scale;

    display -> frame = 0;
    display -> msPerFrame = 1000/fps;

    width *= scale * 2;
    height *= scale;

    int x = (COLS - width) / 2;
    int y = (LINES - height) / 2;

    WINDOW* borderWin = newwin(height + 2, width + 4, y - 1, x - 2);

    wborder(borderWin, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD);

    wmove(borderWin, 0, 1);
    wvline(borderWin, ACS_CKBOARD, height + 2);
    wmove(borderWin, 0, width + 2);
    wvline(borderWin, ACS_CKBOARD, height + 2);   // Makes the border appear to be the same thickness throughout.

    wrefresh(borderWin);

    WINDOW* win = newwin(height, width, y, x);  // This separate win has the cursor aligned at 0, 0 being the top left.

    curs_set(0);        // Hides the cursor.
    keypad(win, true);  // Enables special characters like F keys and arrow keys.
    nodelay(win, true); // Stops getch from blocking.
    noecho();           // Stops getch from echoing input characters.
    wbkgd(win, ' ');
    wbkgd(win, ' ');

    display -> window = win;

    clearDisplay(display);
    refreshDisplay(display);
    
    return display;

}
void refreshDisplay(Display* display) {

    wrefresh(display -> window);
    refresh();

    usleep(display -> msPerFrame * 1000);
    display -> frame += 1;

}
void clearDisplay(Display* display) {

    for (int i = 0; i < display -> width; i++) {
        for (int j = 0; j < display -> height; j++) {
            colourPixel(display, -1, i, j);
        }
    }
    
}

void colourPixel(Display* display, short colourIndex, int x, int y) {

    scaleXY(display, &x, &y);

    wattron(display -> window, COLOR_PAIR(colourIndex));
    for (int i = 0; i < scaleY(display, 1); i++) {
        for (int j = 0; j < scaleX(display, 1); j++) {
            mvwaddch(display -> window, y + i, x + j, ACS_CKBOARD);
        }
    }
    wattroff(display -> window, COLOR_PAIR(colourIndex));

}

void scaleXY(Display* display, int* x, int* y) {

    *x = scaleX(display, *x);
    *y = scaleY(display, *y);

}
int scaleX(Display* display, int x) {

    return x * display -> scale * 2;

}
int scaleY(Display* display, int y) {

    return y * display -> scale;

}