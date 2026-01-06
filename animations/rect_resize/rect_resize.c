#include <curses.h>

#define STAR '*'
#define SPACE ' '

enum { init_dimension = 3,
       key_escape = 27,
       key_space = 32 };

static void draw_rect(int x, int y, int length, int height, char c)
{
    int i, j;
    for(i = 0; i < height; i++)
        for(j = 0; j < length; j++) {
            move(y+i, x+j);
            addch(c);
        }
}

static void update_coordinate(int *coord, int max_coord, int step)
{
    *coord += step;
    if(*coord > max_coord) 
        *coord = max_coord;
    else if(*coord < 0)
        *coord = 0;
}

static void update_dimension(int *dimension, int *coord, int max_value, int *swap, int step)
{
    if(*swap)
        step *= -1;
    *dimension += step;
    if(*dimension > max_value)
        *dimension = max_value;
    else if(*dimension < 1) {
        *swap = !*swap;
        *dimension = 2;
        step *= -1;
    }
    if(*swap)
        update_coordinate(coord, *coord + max_value, -step);
}


int main()
{
    int row, col, length, height, x, y, key;

    initscr();
    timeout(100);
    cbreak();
    noecho();
    keypad(stdscr, 1);
    curs_set(0);

    getmaxyx(stdscr, row, col);

    const int max_length = col / 2 - 1;
    const int max_height = row / 2;

    int swap_x, swap_y;
    swap_x = swap_y = 0;

    length = height = init_dimension;
    x = (col - length) / 2;
    y = (row - height) / 2;

    while((key = getch()) != key_escape) {
        draw_rect(x, y, length, height, SPACE);
        switch(key) {
        case KEY_RIGHT:
            update_dimension(&length, &x, max_length, &swap_x, 1);
            break;
        case KEY_LEFT:
            update_dimension(&length, &x, max_length, &swap_x, -1);
            break;
        case KEY_UP:
            update_dimension(&height, &y, max_height, &swap_y, -1);
            break;
        case KEY_DOWN:
            update_dimension(&height, &y, max_height, &swap_y, 1);
            break;
        }
        draw_rect(x, y, length, height, STAR);
        refresh();
    }

    endwin();
    return 0;
}
