#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>


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

    /* 
    ** Local Flags **

    ICANON => used to disable canonical mode

    ISIG => to turn off Ctrl-C, it sends SIGINT signal to terminate current process and Ctrl-Z sends SIGTSTP signal to the current process which suspends it

    IEXTEN => to turn off Ctrl-V, it also fixed Ctrl-O in macOS, whose terminal driver is otherwise set to discard that control char
    
    */

   // turn off some miscellaneous flags

    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);     // echo mode gets turned off

    /*
    Ctrl-S stops data from being transmitted to the terminal until you press Ctrl-Q
    */
    raw.c_lflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);                     // disable Ctrl-S and Ctrl-Q
    raw.c_lflag &= ~(OPOST);    // to disable carriage return   

    /*
    CS8 is not a flag, it is a bit mask with multiple bits, which we set using the bitwise-OR (|) operator unlike all the flags we are turning off. It sets the character size (CS) to 8 bits per byte. 
    */
    raw.c_lflag |= ~(CS8);        

    // setting a timeout for read()
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    // setting attributes to raw
    // TCSAFLUSH waits for all pending output to be written to the terminal, and also discards any input that hasn't been read.
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
    enableRawMode();

    // reading 1 byte from stdin
    while (1) {
        char c = '\0';         // current char
        read(STDIN_FILENO, &c, 1);

        // iscntrl() -> ctype.h, these are control character, and i don't want to print them, I only need their ASCII code

        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }

        if (c == 'q') break;
    }


    return 0;
}