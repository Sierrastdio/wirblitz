#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdio.h>
#include <stdlib.h>

/* --- 운영체제별 헤더 및 환경 설정 --- */
#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
#else
    #include <termios.h>
    #include <unistd.h>
    #include <sys/ioctl.h>
#endif

/* --- ANSI 제어 시퀀스 정의 --- */
#define ANSI_CLEAR         "\x1b[2J"
#define ANSI_HOME          "\x1b[H"
#define ANSI_RESET         "\x1b[0m"
#define ANSI_REVERSE       "\x1b[7m"
#define ANSI_HIDE_CURSOR   "\x1b[?25l"
#define ANSI_SHOW_CURSOR   "\x1b[?25h"

/* --- 키 입력 상수 (운영체제 통합용) --- */
enum KeyCode {
    KEY_UP = 1001,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_ESC = 27,
    KEY_ENTER = 13,
    KEY_BACKSPACE = 8
};

/* --- 터미널 초기화 및 종료 --- */
static inline void term_init() {
#ifdef _WIN32
    // 윈도우 파워쉘에서 ANSI 이스케이프 코드를 활성화
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
    printf(ANSI_HOME);
    fflush(stdout);
}

static inline void term_close() {
    printf(ANSI_SHOW_CURSOR ANSI_RESET ANSI_CLEAR ANSI_HOME);
    fflush(stdout);
}

/* --- 화면 제어 함수 --- */
static inline void term_clear() { printf(ANSI_CLEAR); }
static inline void term_move(int y, int x) { printf("\x1b[%d;%dH", y, x); }
static inline void term_set_color_reverse() { printf(ANSI_REVERSE); }
static inline void term_reset() { printf(ANSI_RESET); }

/* --- 통합 키 입력 함수 (핵심) --- */
static inline int term_getch() {
#ifdef _WIN32
    int ch = _getch();
    if (ch == 0 || ch == 224) { // 윈도우 특수키(방향키)
        ch = _getch();
        switch (ch) {
            case 72: return KEY_UP;
            case 80: return KEY_DOWN;
            case 75: return KEY_LEFT;
            case 77: return KEY_RIGHT;
        }
    }
    return ch;
#else
    // 리눅스용 Raw Mode 구현
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    ch = getchar();
    if (ch == 27) { // 리눅스 이스케이프 시퀀스 처리
        struct termios temp_t;
        tcgetattr(STDIN_FILENO, &temp_t);
        temp_t.c_cc[VMIN] = 0; // Non-blocking read
        temp_t.c_cc[VTIME] = 1;
        tcsetattr(STDIN_FILENO, TCSANOW, &temp_t);
        
        if (getchar() == '[') {
            switch (getchar()) {
                case 'A': ch = KEY_UP; break;
                case 'B': ch = KEY_DOWN; break;
                case 'D': ch = KEY_LEFT; break;
                case 'C': ch = KEY_RIGHT; break;
            }
        }
        tcsetattr(STDIN_FILENO, TCSANOW, &newt); // 복구
    }
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
}

#endif // TERMINAL_H