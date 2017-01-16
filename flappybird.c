#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <linux/limits.h>

#define GRAVITY 0.7
#define INITIAL_VELOCITY 0
#define JUMP_VELOCITY 2.1
#define T_WIDTH 80 //terminal width
#define T_HEIGHT 24  //terminal height
#define FRAME_INTERVAL 100000  //microseconds, 1e-6 seconds

#define TUBE_WIDTH 3
#define TUBE_DISTANCE 8
#define TUBE_HEIGHT 5 //distance between upper and lower tube
#define TUBE_INTERVAL 19 // 0.1 seconds

typedef struct {
    int x;
    int y;
    float yvel;  // y velocity
} bird;

//tubes list
typedef struct Tube {
    int x;
    int y;
    struct Tube* next;
} tube;
typedef tube* Tubes;
//global pointer at first element of the tubes list
Tubes gTubes = NULL;

//init score
int score = 0;
//highscore
FILE * highscoreFile;
long int highscore;

//function declarations
void updateBird(bird* b, int jumpBool);
void generateTube();
Tubes updateTube(tube* t, bird* b); 
static void finish(int sig);
int KeyPressed(int* pKeyCode);

int main() {
    bird flappy;  // Our not-so flappy bird!
    int commandO=32, command; //32=spacebar, see below
    long htime = 0; //time since star

    srand(time(0)); //init random

    (void) signal(SIGINT, finish);  //to handle program kill right

    (void) initscr();
    keypad(stdscr, TRUE);  //enable keyboard input
    (void) cbreak();  //don't wait for ENTER on new characters
    (void) noecho();  //don't print input chars to screen
    (void) curs_set(0); //hide cursor

    if (has_colors()) {  //COLORS AND RAINBOWS
        start_color();
                     //fore       back
        init_color(COLOR_MAGENTA, 0, 608, 270);
        init_pair(1, COLOR_BLACK, COLOR_YELLOW); //bird
        init_pair(2, COLOR_RED, COLOR_YELLOW); //bird
        init_pair(3, COLOR_BLACK, COLOR_GREEN); //tubes
        init_pair(5, COLOR_BLACK, COLOR_MAGENTA);
        init_pair(4, COLOR_CYAN, COLOR_CYAN); //sky
        init_pair(6, COLOR_WHITE, COLOR_BLACK); //score
    }

    (void) bkgd(COLOR_PAIR(4));

    //initialise bird
    flappy.x = 2;
    flappy.y = 1;
    flappy.yvel = INITIAL_VELOCITY; 
    

    generateTube(gTubes); //generate first tube

    //testing
    while(1) {
        usleep(FRAME_INTERVAL);
        htime += FRAME_INTERVAL / 100000; //add time in 0.1 seconds
        if (htime%TUBE_INTERVAL==0) {
            generateTube(gTubes);
        }
        command = KeyPressed(&commandO);
        updateTube(gTubes, &flappy);
        if (command==32) 
            updateBird(&flappy, 1);
        else 
            updateBird(&flappy, 0);
        command = 0;

        //print score
        move(5, T_WIDTH/2-1);
        attrset(COLOR_PAIR(6));
        printw("%3d", score);
        
        refresh();
    }

    //finish(0);
}

void generateTube(Tubes tubes) { //attach tube to tail
    int x, y, curr_x, i, j;
    Tubes cur;

    tube* t = (tube*) malloc(sizeof(tube));
    t->y = rand()%21;
    t->x = T_WIDTH - 6;
    t->next = NULL;
    if (tubes != NULL) {
        for (cur = tubes; cur->next != NULL; cur = cur->next) {}
        cur->next = t;
    } else gTubes = t;
}

Tubes updateTube(Tubes t, bird* b) { //recursively update all the tubes
    int curr_x, i, j;
    int colorC = 3;
    Tubes tmp;
    t->x -= 1; //move the tube left

    if (t==NULL) return gTubes;
    
    for (i=0; i<T_HEIGHT; i++) {
        //make the tube look nicer
        if (i==t->y || i==t->y+TUBE_HEIGHT+1) colorC=5;
        //draw tube
        if (i<=t->y || i>t->y + TUBE_HEIGHT) {
            for (j=0; j<TUBE_WIDTH+1; j++) {
                curr_x = t->x + j;
                //check for collision with bird
                if ((curr_x == b->x || curr_x == b->x + 1) && 
                        (b->y <= t->y || b->y > t->y+TUBE_HEIGHT)) {
                    finish(0);
                }
                //actually draw bird
                if (j==TUBE_WIDTH) {
                    move(i, curr_x);
                    attrset(COLOR_PAIR(4));
                    addch(' ');
                    addch(' ');
                } else if (curr_x >= 0 && curr_x < T_WIDTH) {
                    move(i, curr_x);
                    attrset(COLOR_PAIR(colorC));
                    addch(' ');
                }
                //remove tube
                if (j==TUBE_WIDTH && curr_x<0) { //remove from head
                    tmp = t->next;
                    free(t);
                    gTubes = tmp;
                    score++; //update score when tube gets off screen
                    return tmp;
                }
            }
        }
        colorC = 3;
    }
    if(t->next == NULL) return gTubes;
    return updateTube(t->next, b);
}

void updateBird(bird* b, int jumpBool) {
    float y;  //where fx and fy are old coordinates, y is new height
    int fx, fy, jumpC = 74;
    fx = b->x;
    fy = b->y;

    //bird physics
    if (jumpBool) 
        b->yvel = INITIAL_VELOCITY - JUMP_VELOCITY;
    b->yvel += GRAVITY;
    y = b->y + b->yvel;

    //DRAW BIRD SECTION
    //check for offscreen
    if (y >= T_HEIGHT-1) {
        y = T_HEIGHT-1;
        b->yvel = INITIAL_VELOCITY;
    }
    if (y <= 0) {
        y = 0;
        b->yvel = INITIAL_VELOCITY;
    }

    //del old bird sprite
    move(fy, fx);
    attrset(COLOR_PAIR(4));
    addch(' ');
    addch(' ');

    //draw new bird sprite
    move((int) y, fx);
    attrset(COLOR_PAIR(2));
    addch('>');
    attrset(COLOR_PAIR(1));
    addch('`');

    //update flappy y
    b->y = y;
}

static void finish(int sig) {  //kill program the right way
    char hpath[PATH_MAX]; //highscore path
    int previousHighscore=0;
    char hRelativePath[23] = "/.flappybird_highscore\0"; //highscore relative path

    endwin(); //kill graphics, go back to terminal prompt

    strcat(hpath, getenv("HOME"));
    strcat(hpath, hRelativePath);

    highscoreFile = fopen(hpath, "r");
    if (highscoreFile != NULL) {
        fscanf(highscoreFile, "%d", &previousHighscore);
        fclose(highscoreFile);
    }

    if(score>previousHighscore) {
        highscoreFile = fopen(hpath, "w");
        printf("New HIGHSCORE!!\n");
        fprintf(highscoreFile, "%d\n", score);
        fclose(highscoreFile);
    }

    printf("Game Over\nScore: %4d\n", score);
    printf("Last highscore: %4d\n\n", previousHighscore);

    exit(0);
}

int KeyPressed(int* pKeyCode) { //function I found on the internet for checking for key pressed
	int KeyIsPressed = 0;
	struct timeval tv;
	fd_set rdfs;

	tv.tv_sec = tv.tv_usec = 0;

	FD_ZERO(&rdfs);
	FD_SET(STDIN_FILENO, &rdfs);

	select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);

	if(FD_ISSET(STDIN_FILENO, &rdfs)) {
		int KeyCode = getchar();
		if(pKeyCode != NULL) *pKeyCode = KeyCode;
		KeyIsPressed = KeyCode;
	}

	return KeyIsPressed;
}
