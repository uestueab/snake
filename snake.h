#include <iostream>
#include <vector>
#include <ncurses.h>
#include <cstdlib>

#ifndef SNAKE_H
#define SNAKE_H

#define FOOD_COLOR 1 
#define BOX_COLOR 2
#define BORDER_COLOR 3


struct snakepart{
    int x,y;
    snakepart(int col, int row);
    snakepart();
};

class game{
    int points, delay, maxwidth, maxheight;
    char direction, partchar, gameBorder, foodChar, boxChar;
    //hit food?
    bool hitFood;

    snakepart food;
    snakepart box;
    std::vector<snakepart> snake;

    void spawnGamePart();
    bool collision();
    void movesnake();

    void speedUp();
    void speedDown();
    void printGameInfo(int score, int delay);
    void printGameInfo(int score);



    public:
        game();
        ~game();
        void start();
        

};
#endif
