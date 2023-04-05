#include <cstdlib>

#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//ACTOR
Actor::Actor(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth)
: GraphObject(imageID, posX, posY, startDir, depth /*, size = 1.0*/)
{
    m_pStudentWorld = sWorldPtr;
}

Actor::~Actor(){}

int Actor::randomNumber(const int range)
{
    //further randomize (running into problems when RNG is so close together in time)
    if(rand() % 2 == 1)
        srand(time(nullptr));
    return rand() % range;
}

//SQUARE
Square::Square(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth)
: Actor(imageID, posX, posY, sWorldPtr, startDir, depth)
{
    m_aliveSquare = true;
    m_newPlayerOn = false;
}

Square::~Square(){}

void Square::doSomething()
{}

bool Square::peachIsOn() const
{
    int xCoord, yCoord;
    
    //get peach's x and y value
    xCoord = getWorld()->pointerPeach()->getX();
    yCoord = getWorld()->pointerPeach()->getY();
    
    //check if those values == the squares values and peach no more ticks to move
    if (xCoord == getX() && yCoord == getY())
        return true;
    
    return false;
}

bool Square::yoshiIsOn() const
{
    int xCoord, yCoord;
    
    //get Yoshi's x and y value
    xCoord = getWorld()->pointerYoshi()->getX();
    yCoord = getWorld()->pointerYoshi()->getY();
    
    //check if those values == the squares values and yoshi no more ticks to move
    if (xCoord == getX() && yCoord == getY())
        return true;
    
    return false;
}

bool Square::peachMovedOnto() const
{
    if(peachIsOn() && getWorld()->pointerPeach()->ticks() > 0)
        return true;
    return false;
}

bool Square::yoshiMovedOnto() const
{
    if (yoshiIsOn() && getWorld()->pointerYoshi()->ticks() > 0)
        return true;
    return false;
}

Player* Square::playerMovedOn() const
{
    //details which player has moved on specifically
    //ptr: player moved onto but won't land on
    //nullptr: player has not moved onto square OR player landed on square
    
    Player* currPlayer = nullptr;
    
    //check if player is on and that their m_tickstomove > 0
    if(peachMovedOnto())
    {
        return currPlayer = getWorld()->pointerPeach();
    }
    else if (yoshiMovedOnto())
    {
        return currPlayer = getWorld()->pointerYoshi();
    }
    else
        return currPlayer;
}

Player* Square::playerOn() const
{
    //returns pointer to new player that landed on square (NOT Passed over top)
    if(NewPlayerOn() && peachIsOn() && getWorld()->pointerPeach()->ticks() == 0 && getWorld()->pointerPeach()->justFinishedMoving())
        return getWorld()->pointerPeach();
    else if(NewPlayerOn() && yoshiIsOn() && getWorld()->pointerYoshi()->ticks() == 0 && getWorld()->pointerYoshi()->justFinishedMoving())
        return getWorld()->pointerYoshi();
    return nullptr;
}

//DIR SQUARE
DirSquare::DirSquare(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth)
: Square(imageID, posX, posY, sWorldPtr, startDir, depth)
{
    m_forcingDir = startDir;
}
DirSquare::~DirSquare() {}

void DirSquare::doSomething()
{
    //check if player has landed on OR moved onto square
    Player* playerLandOn = playerOn();
    Player* playerMoveOn = playerMovedOn();
    if(playerLandOn != nullptr)
    {
        //player landed on dir square
        //update walking dir
        playerLandOn->changeMoveAngle(m_forcingDir);
        //changeSpriteDir(playerLandOn);
    }
    if(playerMoveOn != playerLandOn && playerMoveOn != nullptr)
    {
        //player moved onto directional square
        //update walking dir
        playerMoveOn->changeMoveAngle(m_forcingDir);
        //changeSpriteDir(playerMoveOn);
    }
}

//void DirSquare::changeSpriteDir(Player* currPlayer)
//{
//    if(m_forcingDir == left)
//        currPlayer->setDirection(left);
//    else
//        currPlayer->setDirection(right);
//}

//GIVE-TAKE SQUARE
GiveTakeSquare::GiveTakeSquare(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth /*bool giveSquare*/)
: Square(imageID, posX, posY, sWorldPtr, startDir, depth)
{
    //m_giveSquare = giveSquare;
}

GiveTakeSquare::~GiveTakeSquare(){}

void GiveTakeSquare::interactWith(Player* currPlayer)
{
    if(dealsWithStars())
        starsTo(currPlayer);
    else
    {
        coinsTo(currPlayer);
    }
}

void GiveTakeSquare::takeCoins(Player* currPlayer, int amount)
{
    if(currPlayer->coins() < amount && currPlayer->coins() >= 0)
    {
        currPlayer->updateCoins(-(currPlayer->coins()));
    }
    else if(currPlayer->coins() < 0)
        ; //do nothing because player can't have negative coins from red square
    else
    {
        currPlayer->updateCoins(-amount);
    }
}

//DROPPING SQUARE
DroppingSquare::DroppingSquare(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth)
: GiveTakeSquare(imageID, posX, posY, sWorldPtr, startDir, depth)
{}

DroppingSquare::~DroppingSquare() {}

void DroppingSquare::doSomething()
{
    Player* currPlayer = playerOn();
    //check if new player landed on square
    if(currPlayer != nullptr)
    {
        //if so, interact with them
        interactWith(currPlayer);
        getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
    }
}

void DroppingSquare::coinsTo(Player* currPlayer)
{
    takeCoins(currPlayer, 10);
}
void DroppingSquare::starsTo(Player* currPlayer)
{
    if(currPlayer->stars() > 0)
        currPlayer->updateStars(-1);
}

//COIN SQUARE
CoinSquare::CoinSquare(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth)
: GiveTakeSquare(imageID, posX, posY, sWorldPtr, startDir, depth)
{}

CoinSquare::~CoinSquare(){}

void CoinSquare::doSomething()
{
    //check if still alive
    if (this->alive() == false)
        return;
    //check newPlayerOn value and if player is at location
    Player* currPlayer = playerOn();
    if(currPlayer != nullptr)
        interactWith(currPlayer);
    //reset newplayeron to be false after square interacts with characters
    updateNewPlayerOn(false);
}

void CoinSquare::coinsTo(Player* currPlayer)
{
    if(gives()) //blue square check
    {
        currPlayer->updateCoins(3);
        getWorld()->playSound(SOUND_GIVE_COIN);
        return;
    }
    takeCoins(currPlayer, 3);
    getWorld()->playSound(SOUND_TAKE_COIN);
    return;
}

//RED SQUARE
RedSquare::RedSquare(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth)
: CoinSquare(imageID, posX, posY, sWorldPtr, startDir, depth)
{}

RedSquare::~RedSquare(){}

//STAR SQUARE
StarSquare::StarSquare(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth)
: GiveTakeSquare(imageID, posX, posY, sWorldPtr, startDir, depth)
{}
StarSquare::~StarSquare(){}

void StarSquare::doSomething()
{
    //check if player has moved onto OR landed on square AND has not yet activated square
    
    //check newPlayerOn value and if player is at location
    Player* playerLandOn = playerOn();
    Player* playerMoveOn = playerMovedOn();

    if(playerMoveOn != nullptr)
    {
        //check if player has >= 20 coins
        if(playerMoveOn->coins() >= 20)
        {
            interactWith(playerMoveOn);
        }
    }
    if (playerLandOn != nullptr)
    {
        //check if player has >= 20 coins
        if(playerLandOn->coins() >= 20)
        {
            interactWith(playerLandOn);
        }
    }
    
}
void StarSquare::starsTo(Player* currPlayer)
{
    currPlayer->updateCoins(-20);
    currPlayer->updateStars(1);
    getWorld()->playSound(SOUND_GIVE_STAR);
}

//BANK SQUARE
BankSquare::BankSquare(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth)
: GiveTakeSquare(imageID, posX, posY, sWorldPtr, startDir, depth)
{}
BankSquare::~BankSquare() {}

void BankSquare::doSomething()
{
    //check if square is in give mode and playerOn
    Player* playerLandOn = playerOn();
    if(gives() && playerLandOn != nullptr)
    {
        //player landed on square so give bank balance and play music
        interactWith(playerLandOn);
    }
    //means player passed over the square so take up to 5 coins
    //check if player passing over
    Player* playerMoveOn = playerMovedOn();
    if(playerMoveOn != nullptr)
    {
        interactWith(playerMoveOn);
    }
}

void BankSquare::coinsTo(Player* currPlayer)
{
    if(gives())
    {
        int withdrawAmount = getWorld()->updateBank(0, true);
        if(withdrawAmount > 0)
        {
            currPlayer->updateCoins(withdrawAmount);
            getWorld()->playSound(SOUND_WITHDRAW_BANK);
        }
        return;
    }
    
    int balance = (currPlayer->coins() < 5) ? currPlayer->coins() : 5; //FIXME: trying out new syntax
    
    if (balance <= 0)
        return;
    else if(balance < 5)
    {
        //add coins to bank
        getWorld()->updateBank(balance, false);
        //subtract coins from player
        currPlayer->updateCoins(-balance);
    }
    else
    {
        //add coins to bank
        getWorld()->updateBank(balance, false);
        //subtract coins from player
        currPlayer->updateCoins(-balance);
    }
    getWorld()->playSound(SOUND_DEPOSIT_BANK);
    return;
}

bool BankSquare::gives()
{
    Player* currPlayer = playerMovedOn();
    //check if player has moved onto square: take
    if(currPlayer != nullptr)
        return false;
    //otherwise, player lands on, so gives
    return true;
}

//EVENT SQUARE
EventSquare::EventSquare(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth)
: GiveTakeSquare(imageID, posX, posY, sWorldPtr, startDir, depth)
{}

EventSquare::~EventSquare(){}

void EventSquare::doSomething()
{
    Player* playerLandOn = playerOn();
    if(playerLandOn != nullptr)
    {
        //player has landedOn the square then 3 options:
        //randomly pick btwn 3 options
        switch (randomNumber(3)) {
            case 0:
                //inform player they've been teleported to random square on board
                playerLandOn->teleported();
                //play teleport music
                getWorld()->playSound(SOUND_PLAYER_TELEPORT);
                break;
            case 1:
                //Have player swap position and movement with other player
                playerLandOn->swapOtherPlayer();
                //ensure that square doesn't activate on new player
                updateNewPlayerOn(false);
                //play tele music
                getWorld()->playSound(SOUND_PLAYER_TELEPORT);
                break;
            case 2:
                //check if player has vortex projectile
                if(playerLandOn->hasVortex() == false)
                {
                //if not, give them one
                    playerLandOn->updateHasVortex(true);
                //play sound
                    getWorld()->playSound(SOUND_GIVE_VORTEX);
                }
                break;
                
            default:
                break;
        }
    }
    //prevent double actions
    updateNewPlayerOn(false);
}

//AVATAR
Avatar::Avatar(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth, int ticks_to_move)
: Actor(imageID, posX, posY, sWorldPtr, startDir, depth)
{
    m_ticks_to_move = ticks_to_move;
    m_currentMoveAngle = right; //FIXME: Should this be variable depending on Avatar?
}

Avatar::~Avatar(){}

int Avatar::changeTicks(int newTicks){
    return m_ticks_to_move = newTicks;
}

bool Avatar::canMoveCurrentAngle() const
{
    //get current x,y coord of avatar
    int newX, newY;
    
    //check 16 pixels ahead and see if a square exists there
    getPositionInThisDirection(currentMoveAngle(), SPRITE_WIDTH, newX, newY);
    
    Actor* dontUse = nullptr;
    
    if(getWorld()->isSquareAt(newX, newY, dontUse))
        return true;
    return false;
}

void Avatar::changeToAppropriateAngle()
{
    //check avatar direction to find perpendicular angle that works
    switch (currentMoveAngle()) {
        case right:
        case left:
        {
            //character is either moving right (0 degrees) or left (180 degrees)
            //change avatar's move angle
            changeMoveAngle(up);
            if(canMoveCurrentAngle() == false)
                changeMoveAngle(down);
        }
            break;
        case up:
        case down:
        {
            //character is either moving up (90 degrees) or down (270 degrees)
            //change avatar's direction
            changeMoveAngle(right);
            if(canMoveCurrentAngle() == false)
                changeMoveAngle(left);
        }
            break;
        default:
            std::cerr << "Error in Character Direction. Not 0, 90, 180, 270." << std::endl;
            break;
    }
    return;
}

bool Avatar::isOnSquare() const
{
    //check if avatar is on a square currently
    if (getX() % SPRITE_WIDTH == 0 && getY() % SPRITE_WIDTH == 0)
    {
        return true;
    }
    return false;
}

void Avatar::changeToAppropriateSpriteDir()
{
    //update sprite direction. Sprite dir face left if moving left, otherwise face right
    if (currentMoveAngle() == left)
        setDirection(left);
    else
        setDirection(right);
}

bool Avatar::fork() const
{
    //determine if multiple directions to move in from given square
    //NOTE: does not assume that you are on a square. Check before calling function

    //check if degrees of freedom > 3 from current location
    int degOfFree = 0;
    
    //Right
    if(legalMove(right))
        degOfFree++;
    
    //Up
    if(legalMove(up))
        degOfFree++;
    
    //Left
    if(legalMove(left))
        degOfFree++;
    
    //Down
    if(legalMove(down))
        degOfFree++;
    
    return (degOfFree > 2) ? true : false;
}

bool Avatar::legalMove(int dir) const
{
    int xCoord = getX();
    int yCoord = getY();
    Actor* dontUse = nullptr;
    
    switch (dir) {
        case 0: //right
            if(getWorld()->isSquareAt(xCoord+SPRITE_WIDTH, yCoord, dontUse))
                return true;
            break;
        case 90: //up
            if(getWorld()->isSquareAt(xCoord, yCoord+SPRITE_WIDTH, dontUse))
                return true;
            break;
        case 180: //left
            if(getWorld()->isSquareAt(xCoord-SPRITE_WIDTH, yCoord, dontUse))
                return true;
            break;
        case 270: //down
            if(getWorld()->isSquareAt(xCoord, yCoord-SPRITE_WIDTH, dontUse))
                return true;
            break;
        default:
            break;
    }
    return false;
}

bool Avatar::turningPoint()
{
    //check if avatar is on square and can move at current angle for one square on gameboard
    if(isOnSquare() && canMoveCurrentAngle() == false)
    {
        //can't move forward in current angle so change
        changeToAppropriateAngle();
        changeToAppropriateSpriteDir();
        return true;
    }
    return false;
}

void Avatar::teleported()
{
    //move player to square they aren't on
    
    //get random square from get world
    Actor* p = getWorld()->randomSquare();
    //if random square happens to be same square, get another
    while (p->getX() == getX() && p->getY() == getY())
    {
        p = getWorld()->randomSquare();
    }
    
    //teleport player by changing their x,y values to that of the new square
    int diffX = getX() - (p->getX());
    int diffY = getY() - (p->getY());
    
    //move
    moveTo(getX()-diffX, getY()-diffY);
    
    //invalidated player's angle of movement, so they randomly pick a new one
    changeToAppropriateAngle();
}

//VORTEX
Vortex::Vortex(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth, int moveAngle)
: Actor(imageID, posX, posY, sWorldPtr, startDir, depth)
{
    m_activeState = true; //initialize state to true
    m_moveAngle = moveAngle; //given by player
}
Vortex::~Vortex() {}

void Vortex::doSomething()
{
    //check if active state
    if(active() == false)
        return;
    
    //move two pixels in current dir
    moveAtAngle(moveAngle(), 2);
    
    //check if vortex leaves bounds of screen and make nonactive if so (deleted at end of tick)
    if(outsideBounds(getX(), getY()))
        updateActive(false);
    
    //check if vortex overlaps with impactable object
    Actor* ptrObj = getWorld()->impactableObjAt(getX(), getY());
    if(ptrObj != nullptr)
    {
        //found an impactable object overlap
        ptrObj->impactedByVortex();
        //deactivate vortex and play hit sound
        updateActive(false);
        getWorld()->playSound(SOUND_HIT_BY_VORTEX);
    }
}

bool Vortex::outsideBounds(int xCoord, int yCoord)
{
    if(xCoord < 0 || xCoord >= VIEW_WIDTH || yCoord < 0 || yCoord >= VIEW_HEIGHT)
        return true;
    return false;
}

//BADDIES
Baddies::Baddies(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth, int ticks_to_move)
: Avatar(imageID, posX, posY, sWorldPtr, startDir, depth, ticks_to_move)
{
    m_squaresToMove = 0; //starts at 0
    m_walkState = false; //stars as false
    m_pauseCounter = 180; //stars at 180
    m_activatedOnSquare = false;
}
Baddies::~Baddies() {}

void Baddies::standingAction(int squareMoveFactor)
{
    //baddie is standing still
    //check if a player is also standing on square
    if(activatedOnSquare() == false && getWorld()->pointerPeach()->ticks() == 0 && getWorld()->compareCoordinates(getX(), getY(),
    getWorld()->pointerPeach()->getX(), getWorld()->pointerPeach()->getY()))
    {
        //peach is on the square
        interactPlayer(getWorld()->pointerPeach());
        changeActivatedOnSquare(true);
    }
    else if(activatedOnSquare() == false && getWorld()->pointerYoshi()->ticks() == 0 && getWorld()->compareCoordinates(getX(), getY(),
    getWorld()->pointerYoshi()->getX(), getWorld()->pointerYoshi()->getY()))
    {
        //yoshi is on the square
        interactPlayer(getWorld()->pointerYoshi());
        changeActivatedOnSquare(true);
    }
    
    //decrement pause counter
    changePauseCounterBy(-1);
    
    if(pauseCounter() == 0)
    {
        //reset squares to move by
        changeSquaresToMoveBy(-squaresToMove());
        //change squares to move
        changeSquaresToMoveBy(randomNumber(squareMoveFactor) + 1);
        //change ticks to move
        changeTicks(squaresToMove() * 8);
        //change move direction
        changeToAppropriateAngle();
        //change sprite orientation
        changeToAppropriateSpriteDir();
        //change baddie to Walking state
        changeWalkState(true);
    }
}

void Baddies::movingAction()
{
    //if on square and multiple dir to choose from
    if(isOnSquare() && fork())
    {
        switch (currentMoveAngle())
        {
            case 0: //right
            case 180: //left
                //left is reverse, so only need to check if up or down is option
                if(legalMove(up) && legalMove(down))
                    changeMoveAngle((randomNumber(2)) ? up : down);
                else if(legalMove(up))
                    changeMoveAngle(up);
                else
                    changeMoveAngle(down);
                break;
            case 90: //up
            case 270: //down
                if(legalMove(right) && legalMove(left))
                    changeMoveAngle((randomNumber(2)) ? right : left);
                else if(legalMove(right))
                    changeMoveAngle(right);
                else
                    changeMoveAngle(left);
                break;
            default:
                break;
        }
        //change sprite direction to match
        if(currentMoveAngle() == left)
            setDirection(left);
        else
            setDirection(right);
    }
    else if(turningPoint())
        ;
    
    //moving to new square so reset if has activated on square
    changeActivatedOnSquare(false);
    
    //move 2 pixels in current direction
    moveAtAngle(currentMoveAngle(), 2);
    
    changeTicks(ticks()-1);
    //check if new ticks is 0
    if (ticks() == 0)
    {
        //set bowser to standing
        changeWalkState(false);
        //update pause counter
        changePauseCounterBy(180);
        //special action
        specialAction();
    }
}

void Baddies::impactedByVortex()
{
    //called when notified of being hit by vortex
    //randomly teleport to new square
    teleported();
    //moveangle of right && spritedir of 0
    changeMoveAngle(right);
    setDirection(0);
    //put in standing state
    changeWalkState(false);
    //reset pause counter and set pause ticks to 180
    changePauseCounterBy((-pauseCounter()) + 180);
}

//BOWSER
Bowser::Bowser(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth, int ticks_to_move)
: Baddies(imageID, posX, posY, sWorldPtr, startDir, depth, ticks_to_move)
{}

Bowser::~Bowser() {}

void Bowser::doSomething()
{
    //check state of bowser
    if(walkState() == false)
    {
        standingAction(10);
    }
    //bowser in walking state
    if(walkState() == true)
    {
        movingAction();
    }
}

void Bowser::interactPlayer(Player* currPlayer)
{
    //50% chance he takes coins
    if(randomNumber(2))
    {
        //take all coins
        currPlayer->updateCoins(-(currPlayer->coins()));
        //take all stars
        currPlayer->updateStars(-(currPlayer->stars()));
        getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
    }
}

void Bowser::specialAction()
{
    //1/4 chance bowser creates drop square
    if(randomNumber(4) == 1)
    {
        //ask studentworld to delete square at location and create drop square
        getWorld()->DropSquare(getX(), getY());
        getWorld()->playSound(SOUND_DROPPING_SQUARE_CREATED);
    }
}


//BOO
Boo::Boo(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int startDir, int depth, int ticks_to_move)
: Baddies(imageID, posX, posY, sWorldPtr, startDir, depth, ticks_to_move)
{}

Boo::~Boo() {}

void Boo::doSomething()
{
    //check state of boo
    if(walkState() == false)
    {
        standingAction(3);
    }
    else if(walkState() == true)
    {
        movingAction();
    }
}

void Boo::swapCoins(Player* p1, Player* p2)
{
    int temp = p1->coins();
    //whipe out p1's coins
    p1->updateCoins(-temp);
    //replace with p2's
    p1->updateCoins(p2->coins());
    
    //whipe out p2's coins
    p2->updateCoins(-p1->coins());
    //replace with temp
    p2->updateCoins(temp);
}

void Boo::swapStars(Player* p1, Player* p2)
{
    int temp = p1->stars();
    //whipe out p1's coins
    p1->updateStars(-temp);
    //replace with p2's
    p1->updateStars(p2->stars());
    
    //whipe out p2's coins
    p2->updateStars(-p1->stars());
    //replace with temp
    p2->updateStars(temp);
}

void Boo::interactPlayer(Player* currPlayer)
{
    Player* p2;
    if(currPlayer == getWorld()->pointerPeach())
        p2 = getWorld()->pointerYoshi();
    else
        p2 = getWorld()->pointerPeach();
    
    
    if(randomNumber(2))
    {
        //swap player's coins with other
        swapCoins(currPlayer, p2);
    }
    else
    {
        //swap player's stars with other
        swapStars(currPlayer, p2);
    }
    getWorld()->playSound(SOUND_BOO_ACTIVATE);
}


//PLAYER
Player::Player(int imageID, int posX, int posY, StudentWorld* sWorldPtr, int playerNum)
: Avatar(imageID, posX, posY, sWorldPtr, /*startdir*/ right, /*depth*/ 0, /*ticks to move*/ 0)
{
    //initialize player specific priavte data members
    m_waiting_to_roll = true;
    m_playerNum = playerNum;
    m_stars = 0;
    m_coins = 0;
    m_dieRoll = 0;
    m_hasVortex = false;
    m_justFinishedMoving = false; //need to prevent aliasing on a square with two characters
}

Player::~Player(){}

void Player::doSomething()
{
    int playerAction = getWorld()->getAction(m_playerNum);
    //check if waiting to roll
    if (m_waiting_to_roll == true)
    {
        //check direction in case of teleportation (my teleported function takes care of this)
//        if(legalMove(currentMoveAngle()) == false)
//        {
//            changeToAppropriateAngle();
//            changeToAppropriateSpriteDir();
//        }
        
        //update just finished moving to false because no longer true
        m_justFinishedMoving = false;

        //check if player made an action
        if (playerAction != ACTION_NONE)
        {
            //check what action is
            switch (playerAction)
            {
                case ACTION_ROLL:
                {
                    //random number btwn 1 and 10 inclusive
                    m_dieRoll = randomNumber(10) + 1;
                    
                    //change ticks to dieRoll * 8
                    changeTicks(m_dieRoll * 8);
                    
                    //put Player in walking state
                    m_waiting_to_roll = false;
                    break;
                }
                case ACTION_FIRE:
                {
                    //check player has Vortex to be able to shoot
                    if(hasVortex())
                    {
                        //create vortex at next square in front of player
                        createVortexAtSquareFront();
                        //play fire sound
                        getWorld()->playSound(SOUND_PLAYER_FIRE);
                        //update player so no longer has vortex
                        updateHasVortex(false);
                    }
                    break;
                }
                default:
                {
                    //action is neither Roll nor Fire so return
                    return;
                    break;
                }
            }
        }
    }
    //check if player in walking state
    if (m_waiting_to_roll == false)
    {
        //check if on DirSquare
        int dir = -1;
        Actor* squareBelow = nullptr;
        
        if(getWorld()->isSquareAt(getX(), getY(), squareBelow) && squareBelow->forcingDir(dir))
        {
            changeMoveAngle(dir);
            changeToAppropriateSpriteDir();
        }
        
        //check if player is on a fork
        else if(isOnSquare() && fork())
        {
            //see if user made action
            int angle = actionToAngle(playerAction);
            //if player pressed key, is legal direction to move, and is not direction that player came from
            if(playerAction != ACTION_NONE && legalMove(angle) && angle != oppositeAngle())
            {
                //change to angle
                changeMoveAngle(angle);
                changeToAppropriateSpriteDir();
            }
            //invalid direction or no key pressed, so return
            else
                return;
        }
        //check if player must automatically change angle while walking due to no choices
        else if(turningPoint())
            ;
        
        //move 2 pixels in current direction
        moveAtAngle(currentMoveAngle(), 2);
        
        bool lastMove = false;
        if(ticks()>0)
            lastMove = true;
        
        //decrement ticks to move by 1
        changeTicks(ticks()-1);
        
        if(ticks() % 8 == 0 && dieRoll() > 0)
            updateDieRoll(-1);
        
        //check if new ticks is 0
        if (ticks() == 0)
        {
            //change to waiting to roll
            m_waiting_to_roll = true;
            //update just finished moving
            m_justFinishedMoving = true;
            if(lastMove)
            {
                //find square that player is now on and update newplayeron value to true
                Actor* p = nullptr;
                getWorld()->isSquareAt(getX(), getY(), p);
                if(p != nullptr)
                {
                    p->updateNewPlayerOn(true);
                }
            }
                
        }

    }
    return;
}

int Player::actionToAngle(int keyboardAction)
{
    switch (keyboardAction) {
        case ACTION_RIGHT: //0
            return right;
            break;
        case ACTION_UP: //90
            return up;
            break;
        case ACTION_LEFT: //180
            return left;
            break;
        case ACTION_DOWN: //270
            return down;
            break;
        default:
            break;
    }
    //FIXME: what should default be?
    return -1;
}

int Player::oppositeAngle()
{
    switch (currentMoveAngle()) {
        case right:
            return left;
            break;
        case up:
            return down;
            break;
        case left:
            return right;
            break;
        case down:
            return up;
            break;
        default:
            break;
    }
    //FIXME: what should default be?
    return -1;
}

void Player::swapOtherPlayer()
{
    Player* p2;
    if(this == getWorld()->pointerPeach())
        p2 = getWorld()->pointerYoshi();
    else
        p2 = getWorld()->pointerPeach();
    
    int temp[2];
    
    //x, y coords
    temp[0] = this->getX();
    temp[1] = this->getY();
    
    this->moveTo(p2->getX(), p2->getY());
    p2->moveTo(temp[0], temp[1]);
    
    //ticks to roll
    temp[0] = this->ticks();
    
    this->changeTicks(p2->ticks());
    p2->changeTicks(temp[0]);
    
    //walk dir && sprite dir
    temp[0] = this->currentMoveAngle();
    temp[1] = this->getDirection();
    
    this->changeMoveAngle(p2->currentMoveAngle());
    this->setDirection(p2->getDirection());
    
    p2->changeMoveAngle(temp[0]);
    p2->setDirection(temp[1]);
    
    //roll/walk state
    bool btemp;
    btemp = rollState();
    
    this->changeRollState(p2->rollState());
    p2->changeRollState(btemp);
    
}

void Player::createVortexAtSquareFront()
{
    switch (currentMoveAngle())
    {
        case right:
            getWorld()->createVortexAt(getX()+SPRITE_WIDTH, getY(), right);
            break;
        case up:
            getWorld()->createVortexAt(getX(), getY()+SPRITE_WIDTH, up);
            break;
        case left:
            getWorld()->createVortexAt(getX()-SPRITE_WIDTH, getY(), left);
            break;
        case down:
            getWorld()->createVortexAt(getX(), getY()-SPRITE_WIDTH, down);
            break;
            
        default:
            break;
    }
}

