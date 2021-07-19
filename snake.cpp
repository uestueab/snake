#include "snake.h"
#include <cstddef>
#include <cstdlib>
#include <curses.h>
#include <iostream>
#include <unistd.h>
#include <string>

snakepart::snakepart(int col, int row){
    x = col;
    y = row;
}

snakepart::snakepart(){
    x = 0;
    y = 0;
}

game::game(){

    /* The initscr code determines the terminal type and initializes all curses data */
    /* structures. initscr also causes the first call to refresh to clear the screen */
    initscr();
    //continue program even without any keypress (true: ->no delay). stdscr = default window.
    nodelay(stdscr, true); 
    /* initialize the keyboard */
    keypad(stdscr,true);
    //don't write keypress to screen
    noecho();
    //make cursor invisible
    curs_set(0);
    //make use of colors
    start_color();
    init_pair(FOOD_COLOR, COLOR_GREEN, COLOR_BLACK);
    init_pair(BOX_COLOR, COLOR_RED, COLOR_BLACK);
    getmaxyx(stdscr, maxheight, maxwidth);


    /* initialize game variables */
    partchar = 'x';
    gameBorder = (char)219;
    foodChar = '*';
    boxChar = (char)254;


    /* makes the snake 5 characters long at start */
    /* and sets their x and y coordinates on screen */
    for (int i=0; i<5; i++) {
        snake.push_back(snakepart(40+i,10));
    }

    points = 0;
    delay = 50000;
    hitFood = false;
    /* default moving path of snake */
    direction = 'l';
    srand(time(0));



    /* put the food on screen and restrict the places where it can spawn */
    spawnGamePart();


    /* prints the upper horizontal end of the screen */
    for (int i=0; i<maxwidth-1; i++) {
        /* move the cursor.. */
        /* tells ncurses where on the screen something should happen */
        
        move(0,i);
        /* place a character into the current or specified window at the current or specified position */
        addch(gameBorder);

    }

    /* prints the upper horizontal end of the screen */
    for (int i=0; i<maxwidth-1; i++) {
        move(maxheight-2,i);
        addch(gameBorder);
    }

    /* prints the left vertical end of the screen */
    for (int i=0; i<maxheight-1; i++) {
        move(i,0);
        addch(gameBorder);
    }

    /* prints the right vertical end of the screen */
    for (int i=0; i<maxheight-1; i++) {
        move(i,maxwidth-2);
        addch(gameBorder);
    }

    /* print the snake on screen */
    for (int i=0; i<snake.size(); i++) {
        move(snake[i].y,snake[i].x);
        addch(partchar);

    }

    /* print the score on screen */
    move(maxheight-1, 0);
    /* since addch only prints a single character on screen, */
    /* we need to use printw to write formatted output on screen */
    printGameInfo(points,delay);

    /* we specified the food location earlier, now we can print it on screen */
    /* move(food.y,food.x); */
    /* addch(foo); */

    /* must be called to get actual output to the terminal */
    refresh();
}

void game::printGameInfo(int points, int delay){
    move(maxheight-1,0);
    printw("Score: %d \t speed: %d", points,delay);
    refresh();
}

void game::printGameInfo(int points){
    move(maxheight-1,0);
    printw("Score: %d \t speed: %d", points);
    refresh();
}

game::~game(){
    nodelay(stdscr, false);
    getch();
    endwin();
}

/* - change speed */
/* - set limit */
/* - print speed on screen */
void game::speedUp(){
    if (delay > 22000) {
        delay*=(1-0.25);
        printGameInfo(points,delay);
    }
}

void game::speedDown(){
    if (delay < 120000) {
        delay*=(1+0.25);
        printGameInfo(points,delay);
    }
}

void game::spawnGamePart(){
    while(true){
        int temp_x = rand()%maxwidth+1;
        int temp_y = rand()%maxheight+1;

        // don't spawn food within the snake
        for (int i=0; i<snake.size(); i++) {
            if(snake[i].x == temp_x && snake[i].y == temp_y)
                continue;
        }

        // don't spawn food at the border
        if(temp_x >= maxwidth-2 || temp_y > maxheight-3)
            continue;

        food.x = temp_x;
        food.y = temp_y;
        break;
    }

    attron(COLOR_PAIR(FOOD_COLOR));

    move(food.y,food.x);
    addch(foodChar);
    attroff(COLOR_PAIR(FOOD_COLOR));
    refresh();
}

bool game::collision(){

    //game border collision
    if(snake[0].x==0 || snake[0].x == maxwidth-1
            || snake[0].y == 0 || snake[0].y == maxheight-2){
        return true;
    }

    /* collision of snake with itself */
    for (int i=2; i<snake.size(); i++) {
        if(snake[0].x == snake[i].x && snake[0].y == snake[i].y)
            return true;
    }

    /* collision with food */
    if(snake[0].x == food.x && snake[0].y == food.y){
        hitFood = true;
        /* print new food to screen */
        spawnGamePart();
        /* increment the score since snake ate the food */
        points+=10;

        /* print new score to screen */
        move(maxheight-1,0);
        printGameInfo(points,delay);

        /* every 50 scores the speed increases by 1.25 */
        if((points%50)==0){
            delay*=(1-0.25);
        }
    }else{
        hitFood = false;
    }

    return false;
}


void game::movesnake(){
    int tmp = getch();

    // Do not allow the snake to move through itself!
    switch (tmp) {
        case KEY_LEFT:
            if(direction != 'r'){
                if(direction == 'l')
                    speedUp();
                else
                    direction = 'l';
            }
            else
                speedDown();
            break;
        case KEY_UP:
            if(direction != 'd'){
                if(direction == 'u')
                    speedUp();
                else
                    direction = 'u';
            }
            else
                speedDown();
            break;
        case KEY_DOWN:
            if(direction != 'u'){
                if(direction == 'd')
                    speedUp();
                else
                    direction = 'd';
            }
            else
                speedDown();
            break;
        case KEY_RIGHT:
            if(direction != 'l'){
                if(direction == 'r')
                    speedUp();
                else
                    direction = 'r';
            }
            else
                speedDown();
            break;
        case KEY_BACKSPACE:
            direction = 'q';
            break;
    }

    // as long as the snake moves and doesnt hit the food
    if(!hitFood){
        move(snake[snake.size()-1].y, snake[snake.size()-1].x);
        addch(' ');

        refresh();
        /* remove last part of the snake */
        snake.pop_back();
    }

    /* we removed the last part of the snake with pop_back */
    /* now we want to attach a new head, depending on the current direction */
    if(direction == 'l')
        snake.insert(snake.begin(),snakepart(snake[0].x-1,snake[0].y));
    else if (direction == 'r')
        snake.insert(snake.begin(),snakepart(snake[0].x+1,snake[0].y));
    else if (direction == 'u')
        snake.insert(snake.begin(),snakepart(snake[0].x,snake[0].y-1));
    else if (direction == 'd')
        snake.insert(snake.begin(),snakepart(snake[0].x,snake[0].y+1));

    //print new head
    move(snake[0].y,snake[0].x);
    addch(partchar);
    refresh();
}

void game::start(){
    while(true){
        if(collision()){
            move(maxheight/2, (maxwidth/2)-4);
            printw("GAME OVER");
            break;
        }
        movesnake();
        if(direction == 'q')
            break;
        usleep(delay);
    }
}
