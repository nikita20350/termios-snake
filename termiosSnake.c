#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>

#define SIZE_X 30
#define SIZE_Y 15
#define FPS 60
#define tick 1000000 / FPS
#define WINNING_SCORE 10
#define TICKS_PER_MOVEMENT 7
typedef struct snakePos {
    int x;
    int y;
    struct snakePos *next;
} snakePos;
typedef struct {
    int x;
    int y;
} eggPos;

char prevTrajectory;
char trajectory;
int score;
char input;
char (*field)[SIZE_X];
struct termios old_t, new_t;
snakePos *snakeHead;
snakePos *tail;
eggPos eggCoords;


void displayField(void);
char getInput(void);
void restoreNormalInput(void);
void moveToTrajectory(void);
void handleInput(void);
void init(void);
void placeEgg(void);
void createNextNode(void);
void updateField(void);
void freeSnake(void);
void moveSnake(void);
void cleanup(void);

int main(void) {
    init();

    int numberOfTicks = 0;
    displayField();
    do {
        handleInput();
        if (numberOfTicks == TICKS_PER_MOVEMENT) {
            numberOfTicks = 0;
            moveToTrajectory();
            if (input == 'q') {
                break;
            }
            updateField();
            displayField();
            
        }
        numberOfTicks++;
        usleep(tick);
    } while (input != 'q');
    if (score == WINNING_SCORE) {
        updateField();
        displayField();
        printf("YOU WON!!!\n");
    }
    else {
        printf("Not enough\n");
    }
    cleanup();
    return 0;
}

char getInput(void) {
    // read old terminal settings

    char ch;
    tcgetattr(STDIN_FILENO, &old_t);
    new_t = old_t;
    int flags = fcntl(STDIN_FILENO, F_GETFL);

    // set nonblock, disable line by line input and echo
    
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    new_t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_t);

    // validate input

    if (read(STDIN_FILENO, &ch, 1) == -1) {
        ch = 0;
    }

    // turn off nonblock

    fcntl(STDIN_FILENO, F_SETFL, flags);
    return ch;
}

void displayField(void) {
    system("clear");
    printf("Press \"w\", \"a\", \"s\", \"d\" for movement\n");
    printf("Press \"q\" to stop the game\n");
    
    for (int i = 0; i < SIZE_Y; i++) {
        for (int j = 0; j < SIZE_X; j++) {
            printf("%c", field[i][j]);
        }
        printf("\n");
    }
    printf("\n\nScore: %d\n", score);
    printf("Score %d to win\n", WINNING_SCORE);
}

void restoreNormalInput(void) {
    // revert terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &old_t);
}

void moveToTrajectory(void) {
    field[tail-> y][tail -> x] = '.';
    moveSnake();
    prevTrajectory = trajectory;
    if (trajectory == 'w') {
        snakeHead -> y--;
    }
    if (trajectory == 's') {
        snakeHead -> y++;
    }
    if (trajectory == 'a') {
        snakeHead -> x--;
    }
    if (trajectory == 'd') {
        snakeHead -> x++;
    }
    // check out of bounds
    if (snakeHead -> x >= SIZE_X || snakeHead -> x < 0 || snakeHead -> y >= SIZE_Y || snakeHead -> y < 0) {
        input = 'q';
    }
    // check collected egg
    if (snakeHead -> x == eggCoords.x && snakeHead -> y == eggCoords.y) {
        score++;
        if (score == WINNING_SCORE) {
            input = 'q';
        }
        else {
            createNextNode();
            placeEgg();
        }
    }
}

void handleInput(void) {
    input = getInput();    
    
    if (input == 'w' && prevTrajectory != 's') {
        trajectory = 'w';
    }
    else if (input == 'a' && prevTrajectory != 'd') {
        trajectory = 'a';
    }
    else if (input == 's' && prevTrajectory != 'w') {
        trajectory = 's';
    }
    else if (input == 'd' && prevTrajectory != 'a') {
        trajectory = 'd';
    }

}

void init(void) {
    // init field

    field =  malloc(sizeof(char[SIZE_X]) * SIZE_Y);
    if (field == NULL) {
        cleanup();
        return;
    }
    for (int i = 0; i < SIZE_Y; i++) {
        for (int j = 0; j < SIZE_X; j++) {
            field[i][j] = '.';
        }
        
    }
    // init snake
    snakeHead = malloc(sizeof(snakePos));
    if (snakeHead == NULL) {
        cleanup();
        return;
    }
    snakeHead -> next = NULL;
    snakeHead -> y = 0;
    snakeHead -> x = 5;
    field[snakeHead -> y][snakeHead -> x] = '#';
    trajectory = 'd';
    prevTrajectory = 'd';

    tail = snakeHead;

    // init egg placements

    srand(time(NULL));
    placeEgg();
    
    // init score
    score = 0;
}

void placeEgg(void) {
    eggCoords.y = rand() % SIZE_Y;
    eggCoords.x = rand() % SIZE_X;
   }

void createNextNode(void) {
    snakePos *node = malloc(sizeof(snakePos));
    if (node == NULL) {
        cleanup();
        return;
    }
    node -> next = tail;
    tail = node;
    if (trajectory == 'w') {
        node -> y = node -> next -> y + 1;
        node -> x = node -> next -> x;
    }
    else if (trajectory == 's') {
        node -> y = node -> next -> y - 1;
        node -> x = node -> next -> x;
    }
    else if (trajectory == 'a') {
        node -> x = node -> next -> x + 1;
        node -> y = node -> next -> y;
    }
    else if (trajectory == 'd') {
        node -> x = node -> next -> x - 1;
        node -> y = node -> next -> y;
    }
}

void updateField(void) {
    field[eggCoords.y][eggCoords.x] = '0';
    if (field[snakeHead -> y][snakeHead -> x] == '#') {
        input = 'q';
    }
    for (snakePos *trav = tail; trav != NULL; trav = trav -> next) {
        field[trav -> y][trav -> x] = '#';
    }
}

void freeSnake(void) {
    snakePos *trav = tail;
    while (trav != NULL) {
        snakePos *nxt = trav->next;
        free(trav);
        trav = nxt;
    }
}

void moveSnake(void) {
    for (snakePos *trav = tail; trav -> next != NULL; trav = trav -> next) {
        trav -> y = trav -> next -> y;
        trav -> x = trav -> next -> x;
    }
}
void cleanup(void) {
    // Free the field
    free(field);
    // Free the snake
    freeSnake();
    // Restore normal terminal behaviour
    restoreNormalInput();
}