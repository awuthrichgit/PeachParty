#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Board.h"
#include "Actor.h"
#include <string>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    //myFunctions
    int createBoard();
    void createObject(int xPixel, int yPixel);
    int convertToPixel(const int gridSquare);
    int endGame();
    void actorsDo();
    void deleteActors();
    void updateText();
    bool isSquareAt(const int xCoord, const int yCoord, Actor*& pointerToSquare);
    bool isPlayerAt(const int xCoord, const int yCoord);
    bool compareCoordinates(int x1, int y1, int x2, int y2) const;
    Player* pointerPeach() const { return m_Peach; }
    Player* pointerYoshi() const { return m_Yoshi; }
    int updateBank(int amount, bool withdraw);
    Actor* randomSquare();
    void DropSquare(int xCoord, int yCoord);
    Actor* impactableObjAt(int xCoord, int yCoord);
    bool inRange(int xVortex, int yVortex, int xActor, int yActor);
    void createVortexAt(int xCoord, int yCoord, int currMoveAng);
    //bool playerIsOnSquare(Player* currentPlayer, Actor* currentSquare) const;
    //bool playerOnNewSquare(Player* currentPlayer, Actor* currentSquare) const;
    //void updateLastSquareOn(Player* currentPlayer);

private:
    //List of pointers to Actors in the Game
    std::list<Actor*> m_actors;
    
    Player* m_Peach; //player1 on left side of keyboard
    Player* m_Yoshi; //player2 on right side of keyboard
    
    //FIXME: Board for the GameWorld
    Board m_board;
    //Bank Account for bank squares in game
    int m_bank;
};

inline
int StudentWorld::convertToPixel(const int gridSquare)
{
    //gridSquare is 0-15 inclusive
    //return pixel value btwn 0-255 inclusive
    return gridSquare * SPRITE_WIDTH;
}

#endif // STUDENTWORLD_H_
