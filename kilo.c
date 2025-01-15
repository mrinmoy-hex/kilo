#include <unistd.h>
#include <termios.h>
#include <stdlib.h>


struct termios orig_termios;


void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}


void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);         // disableRawMode will get called when program exits


    struct termios raw = orig_termios;

    tcgetattr(STDIN_FILENO, &raw);      // reading terminal attributes

    // c_lflag -> local flags
    /*
    ECHO is a bitflag, defined as 00000000000000000000000000001000 in binary. 
    I use the bitwise-NOT operator (~) on this value to get 11111111111111111111111111110111. 
    I then bitwise-AND this value with the flags field, which forces the fourth bit in the flags field to become 0, and causes every other 
    bit to retain its current value.
    */
    raw.c_lflag &= ~(ECHO | ICANON);     // echo mode gets turned off
    // setting attributes to raw
    // TCSAFLUSH waits for all pending output to be written to the terminal, and also discards any input that hasn't been read.
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
    enableRawMode();

    char c;         // current char
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q');     // reading 1 byte from stdin
    return 0;
}