#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
 
class StudentWorld;
class Player;

//Base class that is an actor object
//make it pure virtual
class Actor : public GraphObject
{
public: //FIXME: not sure I need startDir
    Actor(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth);
    virtual ~Actor();

    virtual void doSomething() = 0; //pure virtual
    virtual bool alive() const { return true; }
    virtual bool isSquare() const { return false; }
    virtual StudentWorld* getWorld() const { return m_pStudentWorld; } //connects Actors to their world
    virtual void updateNewPlayerOn(bool update){;}
    int randomNumber(const int range);
    virtual bool forcingDir(int& dir) { return false; }
    virtual bool impactableByVortex() { return false; }
    virtual void impactedByVortex() {}
    
private:
    StudentWorld* m_pStudentWorld;
};

//Derived: Vortex class derived from Actor
class Vortex : public Actor
{
public:
    Vortex(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth, int moveAngle);
    virtual ~Vortex();
    
    virtual void doSomething();
    bool active() const { return m_activeState; }
    void updateActive(bool update) { m_activeState = update; }
    bool outsideBounds(int xCoord, int yCoord);
    int moveAngle() const { return m_moveAngle; }
    virtual bool alive() const { return m_activeState; }
    
private:
    bool m_activeState; //either active or not. Starts out in active state
    int m_moveAngle; //given at creation by player
};

//Derived: Square class from Actor
class Square: public Actor
{
public:
    Square(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth);
    virtual ~Square();
    
    virtual void doSomething() = 0; //Pure Virtual
    virtual void updateNewPlayerOn(bool update){ m_newPlayerOn = update; }
    virtual bool NewPlayerOn() const { return m_newPlayerOn; }
    virtual bool peachIsOn() const;
    virtual bool yoshiIsOn() const;
    virtual Player* playerOn() const;
    virtual bool alive() const { return m_aliveSquare; } //overrides true at Actor base
    virtual void updateAlive(bool update){ m_aliveSquare = update; }
    virtual bool isSquare() const { return true; }
    virtual Player* playerMovedOn() const;
    bool peachMovedOnto() const;
    bool yoshiMovedOnto() const;
    
private:
    //true denotes player is newly on square for that tick
    //false denotes either player not on square or player occupied square for more than one tick
    bool m_newPlayerOn;
    
    //true denotes square is alive/active
    //false denotes square is no longer alive probably due to Bowser dropping
    bool m_aliveSquare;
};

//Derived: Directional Square class from Square
class DirSquare: public Square
{
public:
    DirSquare(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth);
    ~DirSquare();
    
    void doSomething();
    bool forcingDir(int& dir) { dir = m_forcingDir; return true; }
private:
    //void changeSpriteDir(Player* currPlayer);
    //forcing direction of Directional Square. Either right, up, left, down
    //forcing direction is the same as sprite orientation (startDir)
    int m_forcingDir;
};

//Derived: GiveTakeSquare from Square
//for squares that give or take things like coins from players (coin, star, bank, dropping)
//need in order to differentiate doSomething from other squares
class GiveTakeSquare: public Square
{
public:
    GiveTakeSquare(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth /* bool giveSquare*/);
    virtual ~GiveTakeSquare();

    virtual void doSomething() = 0; //Pure Virtual
    //virtual void interactWithPlayer(Player* currPlayer);
    virtual void interactWith(Player* currPlayer);
    //virtual void takeFrom(Player* currPlayer);
    virtual void coinsTo(Player* currPlayer) {} //FIXME: Implement?
    virtual void starsTo(Player* currPlayer) {}
    virtual void takeCoins(Player* currPlayer, int amount);
    
    //functionality to override that differentiate squares
    virtual bool gives() { return true; } //either gives or takes (as first action)
    virtual bool dealsWithStars() { return true; } //either deals with stars or doesn't
private:
};

class DroppingSquare: public GiveTakeSquare
{
public:
    DroppingSquare(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth);
    ~DroppingSquare();
    bool dealsWithStars() { return (randomNumber(2) == 0) ? true : false; } //determine randomly
    void coinsTo(Player* currPlayer);
    void starsTo(Player* currPlayer);
    
    void doSomething();
private:
};

//Derived: Coin Square
class CoinSquare: public GiveTakeSquare
{
public:
    CoinSquare(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth);
    virtual ~CoinSquare();
    
    virtual void doSomething();
    void coinsTo(Player* currPlayer);
    virtual bool dealsWithStars() { return false; } //differentiates Blue Coin Square to have proper behavior calling give
private:
};

//Derived: Red Coin Square
class RedSquare: public CoinSquare
{
public:
    RedSquare(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth);
    virtual ~RedSquare();
    bool gives() { return false; }
private:
};

//Derived: Star Square
class StarSquare: public GiveTakeSquare
{
public:
    StarSquare(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth);
    virtual ~StarSquare();
    
    virtual void doSomething();
    void starsTo(Player* currPlayer);
    //deals with stars and gives: true
private:
};

//Derived: Bank Square
class BankSquare: public GiveTakeSquare
{
public:
    BankSquare(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth);
    virtual ~BankSquare();
    
    bool gives();
    virtual void doSomething();
    bool dealsWithStars() { return false; }
    void coinsTo(Player* currPlayer);
private:
};

//Derived: Event Square
class EventSquare: public GiveTakeSquare
{
public:
    EventSquare(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth);
    virtual ~EventSquare();
    
    //bool gives();
    virtual void doSomething();
    bool dealsWithStars() { return false; }
    //void interactWith(Player* currPlayer); //FIXME: override this function so that gives vortex or whatnot
private:
};

//Derived: Avatars class from Actor
class Avatar : public Actor
{
    //This is a class to house all Actors that can MOVE (Players, Baddies, etc)
public:
    Avatar(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth, int ticks_to_move);
    virtual ~Avatar();

    virtual void doSomething() = 0; //pure virtual
    
    int ticks() const { return m_ticks_to_move; }
    int changeTicks(int newTicks);
    bool canMoveCurrentAngle() const;
    void changeToAppropriateAngle();
    void changeToAppropriateSpriteDir();
    int currentMoveAngle() const { return m_currentMoveAngle; }
    void changeMoveAngle(int newAngle) { m_currentMoveAngle = newAngle; }
    bool isOnSquare() const;
    bool fork() const;
    bool legalMove(int dir) const;
    bool turningPoint();
    void teleported();

private:
    int m_ticks_to_move; //represents number of ticks to wait before Avatar can do something again
    int m_currentMoveAngle; //represents current angle of movement: right (0), up (90), left (180), down (270)
};

//Derived: Players Class from Avatar
class Player : public Avatar {
    //This is a class for Yoshi and Peach
public:
    //spriteDir = 0, 90, 180, 270 and is direction sprite image faces but in this case will only be 0 or 180
    Player(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int playerNum);
    virtual ~Player();

    virtual void doSomething();
    int stars() const { return m_stars; }
    int coins() const { return m_coins; }
    void updateCoins(int number) { m_coins+=number; }
    void updateStars(int number) { m_stars+=number; }
    int dieRoll() const { return m_dieRoll; }
    void updateDieRoll(int number) { m_dieRoll+= number; }
    void swapOtherPlayer();
    bool rollState() const { return m_waiting_to_roll; }
    void changeRollState(bool update) { m_waiting_to_roll = update; }
    bool hasVortex() { return m_hasVortex; }
    void updateHasVortex(bool update) { m_hasVortex = update; }
    bool justFinishedMoving() { return m_justFinishedMoving; }
    //void changeNewPlayerOnSquare();
    
private:
    int actionToAngle(int keyboardAction);
    int oppositeAngle();
    void createVortexAtSquareFront();
    
    //Player Numbers: 1 = peach (left keyboard), 2 = yoshi (right keyboard)
    int m_playerNum;
    bool m_waiting_to_roll; //true means the player is standing on a square and can take a keyboard input
    int m_stars;
    int m_coins;
    int m_dieRoll; //0 for no roll or 1-10 inclusive for roll
    bool m_hasVortex; //has vortex projectile. Initalized false
    bool m_justFinishedMoving;
};

//Derived: Baddies Class from Avatar
class Baddies : public Avatar
{
public:
    Baddies(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth, int ticks_to_move);
    virtual ~Baddies();
    
    virtual void doSomething() = 0; //Pure Virtual
    bool walkState() const { return m_walkState; }
    void changeWalkState(bool state) { m_walkState = state; }
    bool activatedOnSquare() const { return m_activatedOnSquare; }
    void changeActivatedOnSquare(bool state) { m_activatedOnSquare = state; }
    int pauseCounter() { return m_pauseCounter; }
    void changePauseCounterBy(int number) { m_pauseCounter += number; }
    int squaresToMove() const { return m_squaresToMove; }
    void changeSquaresToMoveBy(int number) { m_squaresToMove += number; }
    virtual void standingAction(int squareMoveFactor);
    virtual void movingAction();
    virtual void interactPlayer(Player* currPlayer) = 0;
    virtual void specialAction() {}
    virtual bool impactableByVortex() { return true; }
    virtual void impactedByVortex();
    
private:
    int m_squaresToMove; //starts at 0
    bool m_walkState; //true: walking, false: standing
    int m_pauseCounter; //range: [0, 180]
    bool m_activatedOnSquare; //keep track of whether baddie activated on a given square
};

//Derived: Bowser Class from Baddies
class Bowser : public Baddies
{
public:
    Bowser(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth, int ticks_to_move);
    virtual ~Bowser();
    
    void doSomething();
    void specialAction();
    //void takeCoins(Player* currPlayer);
    void interactPlayer(Player* currPlayer);
    
private:
};

//Derived: Boo Class from Baddies
class Boo : public Baddies
{
public:
    Boo(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth, int ticks_to_move);
    virtual ~Boo();
    
    void doSomething();
    void interactPlayer(Player* currPlayer); //FIXME: implement!
    
private:
    void swapCoins(Player* p1, Player* p2);
    void swapStars(Player* p1, Player* p2);
};

//Derived: Vortex class??

#endif // ACTOR_H_
