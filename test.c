#include <stdio.h>
#include <stdlib.h>


#ifdef _WIN32   //만약 Windows(32bit or 64bit) 환경이라면 아래 코드 실행.
    #include <conio.h>
#else           // 그외(Linux, macOS 등) 환경이라면 아래 코드 실행.
    #include <termios.h>
    #include <unistd.h>
    // 리눅스용 _getch 구현
    int _getch() {
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
#endif

// ANSI 제어 매크로
#define CLEAR_SCREEN() printf("\x1b[2J")
#define MOVE_CURSOR(y, x) printf("\x1b[%d;%dH", (y), (x))
#define RESET_COLOR() printf("\x1b[0m")
#define STATUS_BAR_COLOR() printf("\x1b[7m")

void draw_interface(int cy, int cx) {
    CLEAR_SCREEN();
    MOVE_CURSOR(1, 1);
    printf("--- Sierra Cross-Platform Editor (Esc to Exit) ---");
    
    MOVE_CURSOR(24, 1);
    STATUS_BAR_COLOR();
    printf(" Mode: ANSI | Pos: %d, %d                               ", cy, cx);
    RESET_COLOR();

    MOVE_CURSOR(cy, cx);
    fflush(stdout); // 버퍼 강제 비우기 (중요)
}

int main() {
    int x = 1, y = 2;
    int ch;

    while (1) {
        draw_interface(y, x);
        ch = _getch();

        if (ch == 27) break; // Esc

        // 방향키 처리 (간소화된 로직)
        if (ch == 224 || ch == 0) { // Windows 방향키
            ch = _getch();
            if (ch == 72) y--; else if (ch == 80) y++;
            else if (ch == 75) x--; else if (ch == 77) x++;
        } 
        #ifndef _WIN32
        else if (ch == '[') { // Linux 방향키 (Esc [ A/B/C/D)
            ch = _getch();
            if (ch == 'A') y--; else if (ch == 'B') y++;
            else if (ch == 'D') x--; else if (ch == 'C') x++;
        }
        #endif
        else if (ch >= 32 && ch <= 126) {
            x++; // 실제 에디터라면 여기서 배열에 글자 저장
        }
    }

    CLEAR_SCREEN();
    MOVE_CURSOR(1, 1);
    printf("Editor Exit.\n");
    return 0;
}