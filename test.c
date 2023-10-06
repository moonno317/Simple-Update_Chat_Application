#include <stdio.h>
#include <stdlib.h>
#include <string.h>awdawdawdawdawd
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ncurses.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
#define BUFFER_SIZE 1024

void wrapText(WINDOW* win, int starty, int startx, int width, const char* str) {
int x, y, i;
float temp;
temp = (strlen(str) / width) + 1;
y = starty;
x = startx;
for (i = 0; i < temp; i++) {
    mvwprintw(win, y, x, "%.*s", width - 2, str + i * width);
    y++;
}
}

void clearInput(WINDOW* win, int startY, int startX, int width) {
int i;
for (i = 0; i < width - 2; i++) {
mvwprintw(win, startY, startX + i, " ");
}
}

void printMessage(WINDOW* win, const char* sender, const char* message, int* currentLine) {
int maxy, maxx;
getmaxyx(win, maxy, maxx);
int messageLength = strlen(message);
int remainingWidth = maxx - 2 - strlen(sender) - 3; // 2 for box borders, 3 for spacing

if (messageLength <= remainingWidth) {
    mvwprintw(win, *currentLine, 1, "%s: %s", sender, message);
    (*currentLine)++;
} else {
    int i;
    for (i = 0; i < messageLength; i += remainingWidth) {
        char line[remainingWidth + 1];
        strncpy(line, message + i, remainingWidth);
        line[remainingWidth] = '\0';
        mvwprintw(win, *currentLine, 1, "%s: %s", sender, line);
        (*currentLine)++;
    }
}

wrefresh(win);
}

int main() {
initscr();
cbreak();
noecho();
keypad(stdscr, TRUE);
int screenHeight, screenWidth;
getmaxyx(stdscr, screenHeight, screenWidth);

int smallBoxHeight = 3;
int bigBoxHeight = screenHeight - smallBoxHeight - 3;
int bigBoxWidth = screenWidth - 4;

WINDOW* smallBoxWin = newwin(smallBoxHeight, bigBoxWidth, 1, 2);
WINDOW* bigBoxWin = newwin(bigBoxHeight, bigBoxWidth, smallBoxHeight + 2, 2);
scrollok(bigBoxWin, TRUE);

box(smallBoxWin, 0, 0);
box(bigBoxWin, 0, 0);

refresh();
wrefresh(smallBoxWin);
wrefresh(bigBoxWin);

int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
if (clientSocket == -1) {
    perror("Socket creation failed");
    exit(1);
}

struct sockaddr_in serverAddress;
serverAddress.sin_family = AF_INET;
serverAddress.sin_port = htons(SERVER_PORT);
if (inet_pton(AF_INET, SERVER_IP, &(serverAddress.sin_addr)) <= 0) {
    perror("Invalid address/Address not supported");
    exit(1);
}

if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
    perror("Connection failed");
    exit(1);
}

char input[BUFFER_SIZE];
int ch;
int inputLength = 0;
int currentLine = 1;

while ((ch = getch()) != KEY_F(1)) {
    if (ch == ERR)
        continue;

    if (ch == '\n' || ch == '\r') {
        if (inputLength > 0) {
            if (send(clientSocket, input, inputLength, 0) == -1) {
                perror("Sending failed");
                exit(1);
            }

            char message[BUFFER_SIZE];
            memset(message, 0, sizeof(message));
            if (recv(clientSocket, message, BUFFER_SIZE - 1, 0) == -1) {
                perror("Receiving failed");
                exit(1);
            }

            printMessage(bigBoxWin, "Client", input, &currentLine);

            clearInput(smallBoxWin, 1,1, bigBoxWidth);

            memset(input, 0, sizeof(input));
            inputLength = 0;
        }
    } else if (ch == KEY_BACKSPACE || ch == 127) {
        if (inputLength > 0) {
            input[inputLength - 1] = '\0';
            inputLength--;

            mvwprintw(smallBoxWin, 1, 1, "%s", input);
            wrefresh(smallBoxWin);
        }
    } else {
        if (inputLength < BUFFER_SIZE - 1) {
            input[inputLength] = ch;
            inputLength++;

            mvwprintw(smallBoxWin, 1, 1, "%s", input);
            wrefresh(smallBoxWin);
        }
    }
}

delwin(smallBoxWin);
delwin(bigBoxWin);
endwin();
close(clientSocket);

return 0; }
