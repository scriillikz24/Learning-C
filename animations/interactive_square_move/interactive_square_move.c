#include <curses.h>

#define HASHTAG '#'
#define STAR '*'

// Delay here is measured in ms
enum { square_size = 10, init_delay_duration = 100,
       min_screen_size = 12, key_escape = 27,
       key_space = 32, max_delay = 1000, 
       delay_step = 10};

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

static void update_square_position(int *x, int *y, int *dx, int *dy, int min_x, int max_x, int min_y, int max_y, int clockwise) {
    *x += *dx;
    *y += *dy;

    if (*dx == 1 && *x >= max_x) {
        *x = max_x;
        *dx = 0;
        *dy = (clockwise ? 1 : -1);
    }
    else if (*dy == 1 && *y >= max_y) {
        *y = max_y;
        *dy = 0;
        *dx = (clockwise ? -1 : 1);
    }
    else if (*dx == -1 && *x <= min_x) {
        *x = min_x;
        *dx = 0;
        *dy = (clockwise ? -1 : 1);
    }
    else if (*dy == -1 && *y <= min_y) {
        *y = min_y;
        *dy = 0;
        *dx = (clockwise ? 1 : -1);
    }
}

static void update_speed(int *delay, int step)
{
    *delay += step;
    if(*delay > max_delay)
        *delay = max_delay;
    else if(*delay < 0)
        *delay = 0;
    move(0, 0);
    printw("DELAY DURATION: %.2fs         ", (double)*delay / max_delay);
}

int main()
{
    int key, row, col, x, y, max_x, max_y, min_x, min_y, dx, dy;
    int delay_duration = init_delay_duration;
    int clockwise = 1;

    initscr();
    timeout(delay_duration);
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

    while((key = getch()) != key_escape) {
        switch(key) {
        case key_space:
            clockwise = !clockwise;
            dx *= -1;
            dy *= -1;
            break;
        case KEY_LEFT:
            update_speed(&delay_duration, delay_step);
            timeout(delay_duration);
            break;
        case KEY_RIGHT:
            update_speed(&delay_duration, -delay_step);
            timeout(delay_duration);
            break;
        }
        draw_at(x, y, STAR);
        update_square_position(&x, &y, &dx, &dy, min_x, max_x, min_y, max_y, clockwise);
        draw_at(x, y, HASHTAG);
        refresh(); 
    }    
    endwin();
    return 0;
}
