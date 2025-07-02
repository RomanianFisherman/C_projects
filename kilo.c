#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>

/****defines****/

#define CTRL_KEY(k) ((k) & 0x1f)

/*data*/
struct termios orig_termios;

/*terminal*/
void die(const char* mes) {
    perror(mes);
    exit(1);
}

void disableRawMode() {
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios)==-1) {
        die("tcsetattr");
    }
}
//disable ECHO
void enableRawMode() {

    if (tcgetattr(STDIN_FILENO, &orig_termios)) { die("tcgetattr");}
    atexit(disableRawMode); // register a function to be called  at normal process termination

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON );
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= ~(CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); //lflag stands for local flag while I for input flag
    //disable CTRL-V 
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

char editorReadKey() {
    int nread;
    char c;
    while ((nread == read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) die ("read");
    }
    return c;
}

void editorRefreshScreen() {
    write(STDOUT_FILENO, "\x1b[2J",4);  //escape sequence

}

void editorProcessKeypress() {
    char c = editorReadKey();

    switch(c) {
        case CTRL_KEY('q'):
            exit(0);
            break;
    }
}

/*init*/
int main() {
    enableRawMode();
    while (1 ) {
        editorRefreshScreen();
        editorProcessKeypress();
    }
    return 0;
}