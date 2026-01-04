#define _DEFAULT_SOURCE
#include <curses.h>
#include <unistd.h>

#define HASHTAG '#'
#define STAR '*'

enum { square_size = 10, delay_duration = 100000,
       min_screen_size = 12};

static void draw_square(int x, int y)
{
    int i, j;
    for(i = 0; i < square_size; i++)
        for(j = 0; j < square_size; j++) {
            move(y + i, x + j);
            addch(STAR);
        }
}

static void draw_at(int x, int y, char c)
{
    move(y, x);
    addch(c);
}

static void update_square_position(int *x, int *y, int *dx, int *dy, int min_x, int max_x, int min_y, int max_y) {
    // Move the coordinates first
    *x += *dx;
    *y += *dy;

    // Phase 1: Moving Right -> Hit Right Wall -> Turn Down
    if (*dx == 1 && *x >= max_x) {
        *x = max_x; // Snap to edge
        *dx = 0;    // Stop horizontal
        *dy = 1;    // Start vertical (down)
    }
    // Phase 2: Moving Down -> Hit Bottom Wall -> Turn Left
    else if (*dy == 1 && *y >= max_y) {
        *y = max_y;
        *dy = 0;
        *dx = -1;   // Start horizontal (left)
    }
    // Phase 3: Moving Left -> Hit Left Wall -> Turn Up
    else if (*dx == -1 && *x <= min_x) {
        *x = min_x;
        *dx = 0;
        *dy = -1;   // Start vertical (up)
    }
    // Phase 4: Moving Up -> Hit Top Wall -> Turn Right
    else if (*dy == -1 && *y <= min_y) {
        *y = min_y;
        *dy = 0;
        *dx = 1;    // Start horizontal (right)
    }
}


int main()
{
    int row, col, x, y, max_x, max_y, min_x, min_y, dx, dy;

    initscr();
    timeout(0);
    cbreak();
    noecho();
    keypad(stdscr, 1);
    curs_set(0);

    getmaxyx(stdscr, row, col);
    if(row < min_screen_size || col < min_screen_size) {
        endwin();
        fprintf(stderr, "Error: Terminal too small.\n");
        return 1;
    }

    min_x = (col - square_size) / 2;
    min_y = (row - square_size) / 2;
    max_x = min_x + square_size - 1;
    max_y = min_y + square_size - 1;

    x = min_x;
    y = min_y;
    dx = 1;
    dy = 0;

    draw_square(x, y);

    while(getch() == ERR) {
        draw_at(x, y, STAR);
        update_square_position(&x, &y, &dx, &dy, min_x, max_x, min_y, max_y);
        draw_at(x, y, HASHTAG);
        refresh(); 
        usleep(delay_duration);
    }    
    endwin();
    return 0;
}
