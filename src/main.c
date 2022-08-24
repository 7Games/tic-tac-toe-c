#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

const char initalBoard[227] = "╔═══╦═══╦═══╗\n║   ║   ║   ║\n╠═══╬═══╬═══╣\n║   ║   ║   ║\n╠═══╬═══╬═══╣\n║   ║   ║   ║\n╚═══╩═══╩═══╝\n";
const int validBoardPositions[3][3] = { { 44, 50, 56 }, { 106, 112, 118 }, { 168, 174, 180 } };
int tokenPositions[3][3] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } }; // 0 = no token; 1 = cross ; 2 = circle

const char cursorChar =  '#';
const char crossToken =  'x';
const char circleToken = 'o';

int8_t cursorPosX = 1;
int8_t cursorPosY = 1;

_Bool turn = 0;

_Bool isRunning = 1;
_Bool isPlaying = 0;

void termInit() {
    // Make termios struct
    struct termios term;
    // 
    if ((tcgetattr(1, &term)) != -1) {
        // Canonical input
        term.c_lflag &= ~ICANON;
        // Turns off echo
        term.c_lflag &= ~ECHO;
        if ((tcsetattr(1, TCSANOW, &term)) == -1) {
            printf("%s", "\x1b[31m[Error] \x1b[0m tcsetattr() EBADF|EINTR|EINVAL|ENOTTY||EIO");
            _exit(-1);
        }
    } else {
        printf("%s", "\x1b[31m[Error] \x1b[0m tcgetattr() EBADF|ENOTTY");
        _exit(-1);
    }
}

int getInput() {
    char ch = 0;
    while ((ch = getchar()) != EOF) {
        return ch;
    }
    return 0;
}

int main() {
    // Starts termios
    termInit();
    while (isRunning == 1) {
        // Clears the screen
        system("clear");
        // Moves the cursor to the top left.
        printf("\033[99A\033[99D");
        printf("Tic-Tac-Toe\n\n[p] Play game.\n[q] Quit game.\n");
        int key = getInput();
        switch (key) {
            case 112:                               // 'p' (start the game).
                isPlaying = 1;
                system("clear");
                break;
            case 113:                               // 'q' (quit to menu).
                isRunning = 0;
                break;
        }
        
        while (isPlaying == 1) {        
            printf("\033[99A\033[99D");
            // Makes buffer char array with size of 1024 (1KiB).
            char* buffer = (char*) malloc(1024);
            // Copys the initalBoard into the buffer.
            strcpy(buffer, initalBoard);

            for (int i = 0; i <= 2; i ++) {
                for (int j = 0; j <= 2; j++) {
                    if (tokenPositions[i][j] == 1)
                        buffer[validBoardPositions[i][j]] = crossToken;
                    if (tokenPositions[i][j] == 2)
                        buffer[validBoardPositions[i][j]] = circleToken;
                }
            }

            buffer[validBoardPositions[cursorPosY][cursorPosX]] = cursorChar;
            
            // strncat(buffer, '', 5);

            // Concatenates the help test into the buffer.
            strcat(buffer, "Use [w|a|s|d] to move the cursor\nand press [SPACE] to place your token.\nPress [q] to exit.\n");

            // Prints the buffer to the screen.
            printf("%s", buffer);
            // Frees up the memory made by the buffer (malloc).
            free(buffer);

            // Gets input as decimal ascii code (https://www.asciitable.com/).
            int key = getInput();
            switch (key) {
            case 100:                               // 'd' (move cursor right).
                if (cursorPosX < 2)
                    cursorPosX += 1;
                break;
            case 97:                                // 'a' (move cursor left).
                if (cursorPosX > 0)
                    cursorPosX -= 1;
                break;
            case 115:                               // 's' (move cursor down).
                if (cursorPosY < 2)
                    cursorPosY += 1;
                break;
            case 119:                               // 'w' (move cursor up).
                if (cursorPosY > 0)
                    cursorPosY -= 1;
                break;
            case 32:
                tokenPositions[cursorPosY][cursorPosX] = (turn + 1);
                if (turn == 0)
                    turn = 1;
                else
                    turn = 0;
                break;
            case 113:                               // 'q' (quit to menu).
                isPlaying = 0;
                break;
            case 81:                                // 'Q' (quit game).
                isPlaying = 0;
                isRunning = 0;
                break;
            }
        }
    }
    
    // Return 0 error code (success)
    return 0;
}
