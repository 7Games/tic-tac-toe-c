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

_Bool turn = 0;         // 0 = x; 1 = o

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

_Bool checkWinning(int token) {
    token += 1;
    for (int i = 0; i <= 2; i++) {
        for (int j = 0; j <= 2; j++) {
            if (tokenPositions[i][j] == token) {
                // Horizontal
                if (j == 0) {
                    if(tokenPositions[i][j+1] == token) {
                        if(tokenPositions[i][j+2] == token) {
                            return 1;
                        }
                    }
                }
                // Vertical
                if (i == 0) {
                    if(tokenPositions[i+1][j] == token) {
                        if(tokenPositions[i+2][j] == token) {
                            return 1;
                        }
                    }
                }
                // Diagonals
                if (i == 0 && j == 0) {
                    if(tokenPositions[i+1][j+1] == token) {
                        if(tokenPositions[i+2][j+2] == token) {
                            return 1;
                        }
                    }
                }
                if (i == 0 && j == 2) {
                    if(tokenPositions[i+1][j-1] == token) {
                        if(tokenPositions[i+2][j-2] == token) {
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

int main() {
    // Starts termios
    termInit();
    while (isRunning == 1) {
        // Resets all values to default
        turn = 0;
        cursorPosX = 1; cursorPosY = 1;
        for (int i = 0; i <= 2; i ++) {
            for (int j = 0; j <= 2; j++) {
                tokenPositions[i][j] = 0;
            }
        }

        // Clears the screen
        system("clear");
        printf("Tic-Tac-Toe\n\n[p] Play game.\n[q] Quit game.\n");
        int key = getInput();
        switch (key) {
            case 112:                               // 'p' (start the game).
                isPlaying = 1;
                system("clear");
                break;
            case 113:                               // 'q' (quit to menu).
                isRunning = 0;
                isPlaying = 0;
                break;
        }
        
        while (isPlaying == 1) {
            // Checks which player won.
            if (checkWinning(0)) {
                printf("Cross wins!\n");
                getInput();
                isPlaying = 0;
                break;
            } else if (checkWinning(1)) {
                printf("Circle wins!\n");
                getInput();
                isPlaying = 0;
                break;
            }

            // Moves the cursor to the top left.
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

            // Concatenates the help test into the buffer.
            strcat(buffer, "Use [w|a|s|d] to move the cursor\nand press [SPACE] to place your token.\nPress [q] to exit.\n");

            // Concatenates text to tell the players which turn it is
            if (turn == 0)
                strcat(buffer, "Croses Turn.\n");
            else
                strcat(buffer, "Circles Turn.\n");
            
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
            case 32:                                // [SPACE] (places token based on whos turn it is).
                if (tokenPositions[cursorPosY][cursorPosX] == 0) {
                    tokenPositions[cursorPosY][cursorPosX] = (turn + 1);
                    if (turn == 0)
                        turn = 1;
                    else
                        turn = 0;
                }
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
