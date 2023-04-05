#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <iterator>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_Peach = nullptr;
    m_Yoshi = nullptr;
    m_bank = 0;
}

StudentWorld::~StudentWorld()
{
    //call cleanup to destroy Actors in world
    cleanUp();
}

int StudentWorld::init()
{
    //initialize the world through createBoard(). Returns either Board Error or Continue Game
	startCountdownTimer(99);  // FIXME: replace 5 with 99 when turning in
    return createBoard();
}

//move is called once per tick
int StudentWorld::move()
{
    //check if game is over
    if (timeRemaining() <= 0)
        return endGame();
    
    //ask each actor to do something
    actorsDo();
    
    //delete actors that died (blocks due to droppings) or have gone inactive (vortex) on this tick
    deleteActors();
    
    //update status text on top of screen with new info
    updateText();
    
    //return status continue game
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    //must delete all actors that still exist
    
    //iterate through list deleting each actor
    for(list<Actor*>::iterator li = m_actors.begin(); li != m_actors.end(); li++)
    {
        //protect against undefined behavior
        if(*li != nullptr)
        {
            delete *li;
            //reset pointer
            *li = nullptr;
        }
            
    }
    
    //delete peach and yoshi (check not null!)
    if(m_Peach != nullptr) //FIXME: why is this being called multiple times?
    {
        delete m_Peach;
        //reset pointer
        m_Peach = nullptr;
    }
        
    if(m_Yoshi != nullptr)
    {
        delete m_Yoshi;
        //reset pointer
        m_Yoshi = nullptr;
    }
        
}

int StudentWorld::createBoard()
{
    //get board number
    string num = to_string(getBoardNumber());
    
    //find pathway to boardfile
    string board_file = (assetPath() + "board0") + (num + ".txt");
    
    //instantiate board object with pathway
    Board::LoadResult result = m_board.loadBoard(board_file);
    if (result == Board::load_fail_file_not_found || result == Board::load_fail_bad_format)
    {
        //cerr << "Board error\n";
        return GWSTATUS_BOARD_ERROR;
    }
    else if (result == Board::load_success)
    {
        //iterate through all the rows and columns of 16x16 board
        for(int col = 0; col < BOARD_WIDTH; col++)
        {
            for(int row = 0; row < BOARD_HEIGHT; row++)
            {
                //its col, row based on how Carey wrote it...
                createObject(col, row);
            }
        }
        //cerr << "Successfully loaded board\n";
        return GWSTATUS_CONTINUE_GAME;
    }
    
    //default
    return GWSTATUS_BOARD_ERROR;
}

void StudentWorld::createObject(int xGrid, int yGrid)
{
    Board::GridEntry ge = m_board.getContentsOf(xGrid, yGrid);
    
    //convert board grid values to pixel values
    xGrid = convertToPixel(xGrid);
    yGrid = convertToPixel(yGrid);
    
    //check what the board entry is supposed to be and create new object
    switch (ge)
    {
        case Board::empty:
            break;
        case Board::boo:
            m_actors.push_front(new Boo(IID_BOO, xGrid, yGrid, this, 0, 1, 0));
            //a blue coin square created under boo
            m_actors.push_front(new CoinSquare(IID_BLUE_COIN_SQUARE, xGrid, yGrid, this, 0, 1));
            break;
        case Board::bowser:
            m_actors.push_front(new Bowser(IID_BOWSER, xGrid, yGrid, this, 0, 1, 0));
            //a blue coin square created under bowser
            m_actors.push_front(new CoinSquare(IID_BLUE_COIN_SQUARE, xGrid, yGrid, this, 0, 1));
            break;
        case Board::player:
            //new player and coin square at location
            if(m_Peach == nullptr && m_Yoshi == nullptr)
            {
                m_Peach = new Player(IID_PEACH, xGrid, yGrid, this, 1);
                m_Yoshi = new Player(IID_YOSHI, xGrid, yGrid, this, 2);
                
                //create Blue coin square underneath them and push onto front of list
                m_actors.push_front(new CoinSquare(IID_BLUE_COIN_SQUARE, xGrid, yGrid, this, 0, 1));
            }
            break;
        case Board::red_coin_square:
            //create Red Coin Square that takes coins from player
            m_actors.push_front(new RedSquare(IID_RED_COIN_SQUARE, xGrid, yGrid, this, 0, 1));
            break;
        case Board::blue_coin_square:
        {
            //create Blue coin square and push onto front of list
            m_actors.push_front(new CoinSquare(IID_BLUE_COIN_SQUARE, xGrid, yGrid, this, 0, 1));
        }
            break;
        case Board::left_dir_square:
            m_actors.push_front(new DirSquare(IID_DIR_SQUARE, xGrid, yGrid, this, 180, 1));
            break;
        case Board::right_dir_square:
            m_actors.push_front(new DirSquare(IID_DIR_SQUARE, xGrid, yGrid, this, 0, 1));
            break;
        case Board::up_dir_square:
            m_actors.push_front(new DirSquare(IID_DIR_SQUARE, xGrid, yGrid, this, 90, 1));
            break;
        case Board::down_dir_square:
            m_actors.push_front(new DirSquare(IID_DIR_SQUARE, xGrid, yGrid, this, 270, 1));
            break;
        case Board::event_square:
            m_actors.push_front(new EventSquare(IID_EVENT_SQUARE, xGrid, yGrid, this, 0, 1));
            break;
        case Board::bank_square:
            m_actors.push_front(new BankSquare(IID_BANK_SQUARE, xGrid, yGrid, this, 0, 1));
            break;
        case Board::star_square:
            //give square
            m_actors.push_front(new StarSquare(IID_STAR_SQUARE, xGrid, yGrid, this, 0, 1));
            break;
    }
    return;
}

int StudentWorld::endGame()
{
    //play end sound
    playSound(SOUND_GAME_FINISHED);
    
    if(m_Peach->stars() > m_Yoshi->stars())
    {
        //gets their stats set
        setFinalScore(m_Peach->stars(), m_Peach->coins());
        return GWSTATUS_PEACH_WON;
    }
    else if (m_Yoshi->stars() > m_Peach->stars())
    {
        setFinalScore(m_Yoshi->stars(), m_Yoshi->coins());
        return GWSTATUS_YOSHI_WON;
    }
    else if (m_Peach->coins() > m_Yoshi->coins())
    {
        setFinalScore(m_Peach->stars(), m_Peach->coins());
        return GWSTATUS_PEACH_WON;
    }
    else if (m_Yoshi->coins() > m_Peach->coins())
    {
        setFinalScore(m_Yoshi->stars(), m_Yoshi->coins());
        return GWSTATUS_YOSHI_WON;
    }
    else
    {
        //else pick winner randomly
        if(pointerYoshi()->randomNumber(2))
            return GWSTATUS_PEACH_WON;
        else
            return GWSTATUS_YOSHI_WON;
    }
}

void StudentWorld::actorsDo()
{
    m_Peach->doSomething();
    m_Yoshi->doSomething();
    
    //iterate through all other actors...
    for(list<Actor*>::iterator li = m_actors.begin(); li != m_actors.end(); li++)
    {
        //then have actors do something
        (*li)->doSomething();
    }
    
    return;
}

void StudentWorld::deleteActors()
{
    //iterate through list of actors in game
    for(list<Actor*>::iterator li = m_actors.begin(); li != m_actors.end(); li++)
    {
        //all actors are default alive, but blocks and vortexes have overload for other case
        if((*li)->alive() == false)
        {
            //denotes block or votex that must be deleted
            delete (*li);
            //erase ptr from list to save space
            li = m_actors.erase(li);
        }
    }
}

void StudentWorld::updateText()
{
    ostringstream oss;
    //Player1 Portion
    oss << "P1 Roll: " << to_string(m_Peach->dieRoll()) << " Stars: " << to_string(m_Peach->stars())
    << " $$: " << to_string(m_Peach->coins()) << ((m_Peach->hasVortex()) ? " VOR" : "");
    
    //General Game Stats Portion
    oss << " | Time: " << to_string(timeRemaining()) << " | Bank: " << to_string(m_bank) << " | ";
    
    //Player2 Portion
    oss << "P2 Roll: " << to_string(m_Yoshi->dieRoll()) << " Stars: " << to_string(m_Yoshi->stars())
    << " $$: " << to_string(m_Yoshi->coins()) << ((m_Yoshi->hasVortex()) ? " VOR" : "");
    
    setGameStatText(oss.str());
    
    return;
}

bool StudentWorld::isSquareAt(const int xCoord, const int yCoord, Actor*& pointerToSquare)
{
    //iterate through list of actors for squares that have particular x,y coord
    for(list<Actor*>::iterator li = m_actors.begin(); li != m_actors.end(); li++)
    {
        if((*li)->isSquare() && (*li)->getX() == xCoord && (*li)->getY() == yCoord)
        {
            pointerToSquare = (*li);
            return true;
        }
    }
    return false;
}

bool StudentWorld::isPlayerAt(const int xCoord, const int yCoord)
{
    if (m_Peach->getX() == xCoord && m_Peach->getY() == yCoord)
        return true;
    if (m_Yoshi->getX() == xCoord && m_Yoshi->getY() == yCoord)
        return true;
    return false;
}

int StudentWorld::updateBank(int amount, bool withdraw)
{
    if(withdraw == false)
    {
        return m_bank += amount;
    }
    //else everything out of the bank
    int withdrawAmount = m_bank;
    m_bank = 0;
    return withdrawAmount;
}

Actor* StudentWorld::randomSquare()
{
    //assign at beginning for safety
    list<Actor*>::iterator li = m_actors.begin();
    
    //find random actor and ensure they're a square (if not, perform randomness again)
    do{
        //reset li every loop in case
        li = m_actors.begin();
        //get random number within range [0, m_actors.size()-1] inclusive
        int num = pointerYoshi()->randomNumber(m_actors.size());
        
        //iterate through actors for random actor
        
        advance(li, num);
    } while((*li)->isSquare() == false);
    
    //return pointer to square
    return (*li);
}

void StudentWorld::DropSquare(int xCoord, int yCoord)
{
    Actor* ptrSquare = nullptr;
    //delete square at location
    isSquareAt(xCoord, yCoord, ptrSquare);
    if(ptrSquare != nullptr)
    {
        //delete dynam alloc square object
        delete ptrSquare;
        //ptrSquare = nullptr;
        //remove ptrSquare from list
        m_actors.remove(ptrSquare); //FIXME: Not sure if this works
    }
    //make a drop square at location
    m_actors.push_front(new DroppingSquare(IID_DROPPING_SQUARE, xCoord, yCoord, this, 0, 1));
}

Actor* StudentWorld::impactableObjAt(int xCoord, int yCoord)
{
    //iterate through list of actors looking for ones that can be impactable
    for(list<Actor*>::iterator li = m_actors.begin(); li != m_actors.end(); li++)
    {
        if((*li)->impactableByVortex() == true && inRange(xCoord, yCoord, (*li)->getX(), (*li)->getY()))
        {
            //found the first overlap, so return a pointer to that object
            return (*li);
        }
    }
    //default case is nullptr, so be careful calling function
    return nullptr;
}

bool StudentWorld::inRange(int xVortex, int yVortex, int xActor, int yActor)
{
    if(xActor >= xVortex && xActor <= xVortex + SPRITE_WIDTH &&
       yActor >= yVortex && yActor <= yVortex + SPRITE_HEIGHT)
        return true;
    return false;
}

void StudentWorld::createVortexAt(int xCoord, int yCoord, int currMoveAng)
{
    //create new vortex at square right in front of player moving in player's direction
    m_actors.push_front(new Vortex(IID_VORTEX, xCoord, yCoord, this, 0, 0, currMoveAng));
}

bool StudentWorld::compareCoordinates(int x1, int y1, int x2, int y2) const
{
    if (x1 == x2 && y1 == y2)
        return true;
    return false;
}

//void StudentWorld::updateLastSquareOn(Player* currentPlayer)
//{
//    //check if player is on square currently
//    if (isSquareAt(currentPlayer->getX(), currentPlayer->getY()))
//    {
//        //if so, update last Square On to be current square
//        currentPlayer->updateLastSquareOn(currentPlayer->getX(), currentPlayer->getY());
//    }
//}

//bool StudentWorld::playerIsOnSquare(Player* currentPlayer, Actor* currentSquare) const
//{
//
//    return compareCoordinates(currentPlayer->getX(), currentPlayer->getY(), currentSquare->getX(), currentSquare->getY());
//}
//
//bool StudentWorld::playerOnNewSquare(Player* currentPlayer, Actor* currentSquare) const
//{
//    //check that square is new square for peach
//    int xCoord, yCoord;
//    xCoord = *(currentPlayer->lastSquareOn());
//    yCoord = *(currentPlayer->lastSquareOn() + 1);
//
//    return (compareCoordinates(xCoord, yCoord, currentSquare->getX(), currentSquare->getY()) == false);
//}
