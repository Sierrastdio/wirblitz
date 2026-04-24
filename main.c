#include "terminal.h"

int main() {
    term_init();
    int x = 1, y = 1;
    int key;

    while (1) {
        term_clear();
        term_move(y, x);
        printf("@"); // 커서 위치 표시
        
        term_move(24, 1);
        term_set_color_reverse();
        printf(" [Sierra Editor] Arrow keys to move, ESC to exit. ");
        term_reset();

        fflush(stdout);

        key = term_getch();
        if (key == KEY_ESC) break;

        switch (key) {
            case KEY_UP:    if (y > 1) y--; break;
            case KEY_DOWN:  y++; break;
            case KEY_LEFT:  if (x > 1) x--; break;
            case KEY_RIGHT: x++; break;
        }
    }

    term_close();
    return 0;
}