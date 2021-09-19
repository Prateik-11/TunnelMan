#include "Actor.h"
#include "StudentWorld.h"
#include <random>
#include <vector>
#include <algorithm>

Actor::Actor(int imageID, int x, int y, Direction d, double size, unsigned int depth, StudentWorld* worldpointer) :GraphObject(imageID, x, y, d, size, depth)
{
	//Sets itself visible when called
	setVisible(true);
	visible = true;
	//Stores a pointer to the StudentWorld object that calls it
	world = worldpointer;
	alive = true;
}

Actor::~Actor()
{
}

std::string Actor::getType()
{
	return "";
}

StudentWorld* Actor::getWorld()
{
	return world;
}

void Actor::doSomething()
{
}

void Actor::setLife(bool x)
{
	alive = x;
}

bool Actor::getLife()
{
	return alive;
}

void Actor::makeVisible(bool x)
{
	visible = x;
}

bool Actor::getVisible()
{
	return visible;
}

Earth::Earth(int x, int y, StudentWorld* world) : Actor(TID_EARTH, x, y, right, 0.25, 3, world)
{
}
Earth::~Earth()
{
}

Boulder::~Boulder()
{
}

std::string Boulder::getType()
{
	return "Boulder";
}

Squirt::~Squirt()
{
}

std::string Squirt::getType()
{
	return "Squirt";
}

Goodies::Goodies(int imageID, int x, int y, StudentWorld* world) : Actor(imageID, x, y, right, 1.0, 2, world)
{

}
Goodies::~Goodies()
{

}

Barrel::Barrel(int x, int y, StudentWorld* world) : Goodies(TID_BARREL, x, y, world)
{
	setVisible(false);
	makeVisible(false);
}
Barrel::~Barrel()
{
}
std::string Barrel::getType()
{
	return "Barrel";
}

GoldNugget::GoldNugget(int x, int y, bool visible, bool protesterORtunnelman, bool permanentOrTemporary, StudentWorld* world, int Ntimer) :Goodies(TID_GOLD, x, y, world)
{
	setVisible(visible);
	makeVisible(visible);
	canBePickedUpByTunnelMan = protesterORtunnelman;
	permanent = permanentOrTemporary;
	timer = Ntimer;
}
GoldNugget::~GoldNugget()
{

}
std::string GoldNugget::getType()
{
	return "Gold Nugget";
}

WaterPool::~WaterPool()
{
}
std::string WaterPool::getType()
{
	return "Water Pool";
}

Sonar::~Sonar()
{
}
std::string Sonar::getType()
{
	return "Sonar";
}

Person::Person(int imageID, int x, int y, Direction d, int newHP, StudentWorld* world) : Actor(imageID, x, y, d, 1.0, 0, world)
{
	HP = newHP;
}

Person::~Person()
{
}

//Accessor and mutator for HP
void Person::setHP(int x)
{
	HP = x;
	if (HP <= 0)
	{
		setLife(false);
	}

}
int Person::getHP()
{
	return HP;
}
void Person::addToExitStrategy(int)
{
}
void Person::pickUpGold()
{
}

TunnelMan::TunnelMan(StudentWorld* world) : Person(TID_PLAYER, 30, 60, right, 10, world)
{
	numberOfSquirts = 5;
	sonarCharges = 1;
	goldNuggets = 0;
}
//Destructor
TunnelMan::~TunnelMan()
{
}
//Overriden function for TunnelMan's actions to be called by StudentWorld::move() each tick
int TunnelMan::getSquirts()
{
	return numberOfSquirts;
}
void TunnelMan::setSquirts(int x)
{
	numberOfSquirts = x;
}
int TunnelMan::getSonarCharges()
{
	return sonarCharges;
}
void TunnelMan::setSonarCharges(int x)
{
	sonarCharges = x;
}
void TunnelMan::setGoldNuggets(int x)
{
	goldNuggets = x;
}
int TunnelMan::getGoldNuggets()
{
	return goldNuggets;
}

regularProtester::~regularProtester()
{
}
std::string regularProtester::getType()
{
	return "Regular Protester";
}
void regularProtester::addToExitStrategy(int x)
{
	exitStrategy.push_back(x);
}

hardcoreProtester::~hardcoreProtester()
{
}
std::string hardcoreProtester::getType()
{
	return "Hardcore Protester";
}
void hardcoreProtester::addToExitStrategy(int x)
{
	exitStrategy.push_back(x);
}
void Boulder::doSomething()
{
	if (!getLife())
	{
		//return immediately if the boulder is dead
		return;
	}

	if (state == 0)
	{
		//stable state
		if (!getWorld()->canMove(getX(), getY(),"down"))
		{
			//nothing to be done if the boulder remains stable
		}
		else
		{
			//set state to waiting if there is no longer earth beneath the boulder
			state = 1;
		}
	}
	else if (state == 1)
	{
		//waiting state
		if (timer == 30)
		{
			//If 30 ticks have passed, we move on to the falling state and reset the timer
			state = 2;
			timer = 0;
			getWorld()->playSound(SOUND_FALLING_ROCK);
		}
		else
		{
			//If 30 ticks have not passed, increment the timer
			timer++;
		}
	}
	else if (state == 2)
	{
		//falling state
		moveTo(getX(), getY()-1);

		//if ( getY() == 0 || getWorld()->thereIsEarthBelowTheBoulder(getX(), getY()) || getWorld()->checkForBoulder(getX(), getY(), "down") )
		if(!getWorld()->canMove(getX(),getY(),"down"))
		{
			//If the boulder hit a floor of some sort
			setLife(false);
			return;
		}

		//Kill protesters in its way
		//if (getWorld()->checkForProtester(getX(), getY(), 3))
		//{
			/*std::cout << "Reached" << std::endl;*/
			//If the boulder is crushing the protester,
			//kill the protester
			getWorld()->annoyProtesters(getX(),getY(),3,100);
			//getWorld()->incrementBoulderDropped();
			/*std::cout << "Reached 2" << std::endl;*/
		//}

			//std::cout << "reached" << std::endl;

		if (getWorld()->checkForTunnelMan(getX(), getY(),3))
		{
			//If the boulder is crushing the tunnelman,
			//kill the tunnelman
			//getWorld()->damageTunnelMan(100);
			getWorld()->getTunnelMan()->annoyed(100);
			//getWorld()->incrementBoulderDropped();
			setLife(false);
		}

		//std::cout << "reached2" << std::endl;
	}

	
	return;
}

void TunnelMan::doSomething()
{
	//If he's dead then simply return the function
	if (!getLife()) 	
	{
		return;
	}

	//int to check if digging has ocurred during this tick, this ran faster than using a bool to check
	int dig = 0;

	for (int i = getX(); i <= getX() + 3; i++)
	{
		for (int j = getY(); j <= getY() + 3; j++)
		{
			dig += getWorld()->removeEarthAtxy(i, j);
		}
	}

	//If any digging occured, play the sound and end this tick's actions
	
	if (dig>0)
	{
		getWorld()->playSound(SOUND_DIG);
		return;
	}

	//detect keypresses if there is no digging done this turn
	int ch;
	if (getWorld()->getKey(ch) == true)
	{
		switch (ch)
		{
			//For each directional input, set direction to that input
			//If they are the same and we are not at the edge of the field, move forward
			//Check if moving in the specified direction would cause a collision with a boulder, in which case do not move forward

		case KEY_PRESS_UP:
			
			if (getDirection() != up)
			{
				setDirection(up);
			}
			else
			{
				//Older version, uncomment if new one has a problem: if (getY() == 60 || getWorld()->checkForBoulderProximity(getX(), getY()+1, 3))
				if (getY() == 60 || getWorld()->checkForBoulder(getX(), getY(), "up"))
					break;
				else
					moveTo(getX(), getY() + 1);
			}
			break;

		case KEY_PRESS_DOWN:

			if (getDirection() != down)
			{
				setDirection(down);
			}
			else
			{
				//Older version, uncomment if new one has a problem: if (getY() == 0 || getWorld()->checkForBoulderProximity(getX(), getY()-1, 3))
				if (getY() == 0 || getWorld()->checkForBoulder(getX(), getY(), "down"))
					break;
				else
					moveTo(getX(), getY() - 1);
			}
			break;
			
		case KEY_PRESS_LEFT:
			
			if (getDirection() != left)
			{
				setDirection(left);
			}
			else
			{
				//Older version, uncomment if new one has a problem: if (getX() == 0 || getWorld()->checkForBoulderProximity(getX()-1,getY(), 3))
				if (getX() == 0 || getWorld()->checkForBoulder(getX(), getY(), "left"))
					break;
				else
					moveTo(getX() - 1, getY());
			}
			break;

		case KEY_PRESS_RIGHT:

			if (getDirection() != right)
			{
				setDirection(right);
			}
			else
			{
				//if (getX() == 60 || getWorld()->checkForBoulderProximity(getX() + 1, getY(), 3))
				if (getX() == 60 || getWorld()->checkForBoulder(getX(), getY(), "right"))
					break;
				else
					moveTo(getX() + 1, getY());
			}
			break;

		case KEY_PRESS_ESCAPE:

			//Kill the player
			setHP(0);
			break;

		case KEY_PRESS_SPACE:

			//If the player has squirts remaining,
			if (numberOfSquirts > 0)
			{
				switch (getDirection())
				{
				case up:
					getWorld()->addActor(new Squirt(getX(), getY() + 4, up, getWorld()));
					break;
				case down:
					getWorld()->addActor(new Squirt(getX(), getY() - 4, down, getWorld()));
					break;
				case left:
					getWorld()->addActor(new Squirt(getX() - 4, getY(), left, getWorld()));
					break;
				case right:
					getWorld()->addActor(new Squirt(getX() + 3, getY(), right, getWorld()));
					break;
				}

				getWorld()->playSound(SOUND_PLAYER_SQUIRT);
				numberOfSquirts--;
			}
			break;
			
		case KEY_PRESS_TAB:
			if (goldNuggets > 0)
			{
				getWorld()->addActor(new GoldNugget(getX(), getY(), true, false, false, getWorld(), 100));
				goldNuggets--;
			}
			break;
		
		case 'z':
		case 'Z':
			if (sonarCharges > 0)
			{
				getWorld()->useSonar();
				sonarCharges--;
			}
			break;
		}
	}
}

bool TunnelMan::annoyed(int x)
{
	setHP(getHP() - x);
	if (getHP() <= 0)
	{
		getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
		setLife(false);
	}
	return true;
}

void Barrel::doSomething()
{
	if (!getLife())
	{
		return;
	}

	if (!getVisible() && getWorld()->checkForTunnelMan(getX(), getY(), 4))
	{
		//If the barrel is invisible and the tunnel is <= 4 units away
		setVisible(true);
		makeVisible(true);
		return;
	}

	if (getWorld()->checkForTunnelMan(getX(), getY(), 3))
	{
		setLife(false);
		getWorld()->playSound(SOUND_FOUND_OIL);
		getWorld()->increaseScore(1000);
		getWorld()->incrementBarrels();
	}

	return;

}

Boulder::Boulder(int x, int y, StudentWorld* world) : Actor(TID_BOULDER, x, y, down, 1, 1, world)
{
	state = 0;
	timer = 0;

	for (int i = x; i <= x + 3; i++)
	{
		for (int j = y; j <= y + 3; j++)
		{
			getWorld()->removeEarthAtxy(i, j);
		}
	}
}

Squirt:: Squirt(int x, int y, Direction d, StudentWorld* world) : Actor(TID_WATER_SPURT, x, y, d, 1.0, 1, world)
{
	travelDistance = 4;
	if (x < 3 && d == left)
		setLife(false);
	if (x > 60 && d == right)
		setLife(false);
	if (y < 3 && d == down)
		setLife(false);
	if (y > 60 && d == up)
		setLife(false);

	if (getWorld()->canMove(x - 4, y, "right") && getWorld()->canMove(x - 3, y, "right") && getWorld()->canMove(x - 2, y, "right") && getWorld()->canMove(x - 1, y, "right"))
	{
		//We're good
	}
	else
	{
		setLife(false);
	}
}

void Squirt::doSomething()
{
	if (getLife() == false)
	{
		return;
	}
	
	if (getWorld()->annoyProtesters(getX(), getY(), 3, 2))
	{
		setLife(false);
	}
	
	if (travelDistance <= 0)
	{
		setLife(false);
	}
	
	switch (getDirection())
	{
	case up:
		if (getWorld()->canMove(getX(), getY(), "up"))
		{
			moveTo(getX(), getY() + 1);
			travelDistance--;
		}
		else
		{
			setLife(false);
			return;
		}
		break;
	case down:
		if (getWorld()->canMove(getX(), getY(), "down"))
		{
			moveTo(getX(), getY() - 1);
			travelDistance--;
		}
		else
		{
			setLife(false);
			return;
		}
		break;
	case left:
		if (getWorld()->canMove(getX(), getY(), "left"))
		{
			moveTo(getX()-1, getY());
			travelDistance--;
		}
		else
		{
			setLife(false);
			return;
		}
		break;
	case right:
		if (getWorld()->canMove(getX(), getY(), "right"))
		{
			moveTo(getX()+1, getY());
			travelDistance--;
		}
		else
		{
			setLife(false);
			return;
		}
		break;
	}
	return;
}

WaterPool::WaterPool(int x, int y, StudentWorld* world) : Goodies(TID_WATER_POOL, x, y, world)
{
	timer = 100;
	if (timer < (300 - 10 * (int)getWorld()->getLevel()))
		timer = (300 - 10 * (int)getWorld()->getLevel());
}

void WaterPool::doSomething()
{
	if (!getLife())
		return;
	if (getWorld()->checkForTunnelMan(getX(), getY(), 3))
	{
		setLife(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getTunnelMan()->setSquirts(5 + getWorld()->getTunnelMan()->getSquirts());
		getWorld()->increaseScore(100);
	}
	if (timer <= 0)
		setLife(false);
	else
		timer--;
}

void GoldNugget::doSomething()
{
	if (!getLife())
	{
		//Immediately return if not alive
		return;
	}
	if (!getVisible() && getWorld()->checkForTunnelMan(getX(), getY(), 4))
	{
		setVisible(true);
		makeVisible(true);
		return;
	}
	if (canBePickedUpByTunnelMan && getWorld()->checkForTunnelMan(getX(), getY(), 3))
	{
		setLife(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(10);
		getWorld()->getTunnelMan()->setGoldNuggets(1 + getWorld()->getTunnelMan()->getGoldNuggets());
	}
	if (!canBePickedUpByTunnelMan)
	{
		if (getWorld()->bribeProtester(getX(), getY(), 3))
		{
			setLife(false);
		}
	}
	if (!permanent)
	{
		if (timer == 0)
			setLife(false);
		else
			timer--;
	}
}

Sonar::Sonar(StudentWorld* world) : Goodies(TID_SONAR, 0, 60, world)
{
	tickCounter = 0;
	//counterMax = max(100, 300 – 10 * current_level_number)
	if (300  - 10 * getWorld()->getLevel() > 100)
		counterMax = 300 - 10 * getWorld()->getLevel();
	else
		counterMax = 100;
}

void Sonar::doSomething()
{
	if (!getLife())
		return;
	
	if (getWorld()->checkForTunnelMan(getX(), getY(), 3))
	{
		setLife(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getTunnelMan()->setSonarCharges(2 + getWorld()->getTunnelMan()->getSonarCharges());
		getWorld()->increaseScore(75);
		return;
	}
	
	if (tickCounter >= counterMax)
		setLife(false);
	else
		tickCounter++;
}

regularProtester::regularProtester(StudentWorld* world) : Person(TID_PROTESTER, 60, 60, left, 5, world)
{
	leaveTheOilField = false;
	numSquaresToMoveInCurrentDirection = 8 + (rand() % 53);

	//ticksToWaitBetweenMoves = max(0, 3 – getWorld()->getLevel() / 4);
	if (3 - getWorld()->getLevel() / 4 > 0)
		ticksToWaitBetweenMoves = 3 - getWorld()->getLevel() / 4;
	else
		ticksToWaitBetweenMoves = 0;

	tickCounter = 0;
	shoutCounter = 0;
	turnCounter = 0;
	stunCounter = 0;
}

void regularProtester::doSomething()
{
	if (!getLife())
	{
		//return immediately if its already dead
		return;
	}

	if (stunCounter >= 0)
	{
		stunCounter--;
		return;
	}

	if (tickCounter >= ticksToWaitBetweenMoves)
	{
		//we can continue with the rest of the function
		//Since this is a non-resting tick:
		//increment shout counter 
		//increment turn counter
		//reset tick counter
		
		shoutCounter++;
		turnCounter++;
		tickCounter = 0;

		//Uncomment below line to see if Protester is turning at the right time
		//std::cerr << turnCounter << std::endl;
	}
	else
	{
		//increment tickcounter if we are not done waiting for our move
		tickCounter++;
		return;
	}

	if (leaveTheOilField)
	{
		if (getX() == 60 && getY() == 60)
		{
			setLife(false);
			return;
		}
		
		//empty out vector containing path to exit
		exitStrategy.clear();
		//fill vector with new path from given location
		getWorld()->useExitPath(getX(),getY(),this);
		//reverse order since the returned path is backwards
		std::reverse(exitStrategy.begin(), exitStrategy.end());
		
		//change direction and move in the appropriate direction

		//In some cases, the falling boulder will make all exit paths impossible. This is resolved by waiting for a few ticks
		if (exitStrategy.size() < 2)
			return;

		if (exitStrategy[1] / 100 == (getX() + 1))
			setDirection(right);
		else if (exitStrategy[1] / 100 == (getX() - 1))
			setDirection(left);
		else
		{
			if (exitStrategy[1] % 100 == (getY() + 1))
				setDirection(up);
			else if (exitStrategy[1] % 100 == (getY() - 1))
				setDirection(down);
		}
		moveTo(exitStrategy[1] / 100, exitStrategy[1] % 100);

		//end this turn
		return;
	}

	if (getWorld()->checkForTunnelMan(getX(), getY(), 4) && getWorld()->facingTunnelMan(getX(), getY(), getDirection()))
	{
		//If the tunnelman is within 4 blocks and the protester is facing the right direction
		if (shoutCounter >= 15)
		{
			//Shout at the TunnelMan and return 
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			//getWorld()->damageTunnelMan(2);
			getWorld()->getTunnelMan()->annoyed(2);
			shoutCounter = 0;
		}
		return;
	}

	//If tunnelman is > 4 units away
	if (!getWorld()->checkForTunnelMan(getX(), getY(), 4))
	{
		//If he is in line of sight
		int dir = getWorld()->TunnelManInLineOfSight(getX(), getY());
		if (dir !=0 )
		{
			if (dir == 3)
			{
				setDirection(up);
				moveTo(getX(), getY() + 1);
			}
			else if (dir == 4)
			{
				setDirection(down);
				moveTo(getX(), getY() - 1);
			}
			else if (dir == 1)
			{
				setDirection(right);
				moveTo(getX()+1, getY());
			}
			else if (dir == 2)
			{
				setDirection(left);
				moveTo(getX() -1 , getY());
			}

			numSquaresToMoveInCurrentDirection = 0;
			return;
		}
	}

	numSquaresToMoveInCurrentDirection--;

	if (numSquaresToMoveInCurrentDirection <= 0)
	{
		bool repeat;

		do
		{
			repeat = true;
			int dir = 1 + rand() % 4;
			switch (dir)
			{
			case 1:
				setDirection(left);
				if (getWorld()->canMove(getX(), getY(), "left") && !getWorld()->checkForBoulderProximity(getX()-1, getY(), 3))
					repeat = false;
					break;
			case 2:
				setDirection(right);
				if (getWorld()->canMove(getX(), getY(), "right") && !getWorld()->checkForBoulderProximity(getX() + 1, getY(), 3))
					repeat = false;
				break;
			case 3:
				setDirection(down);
				if (getWorld()->canMove(getX(), getY(), "down") && !getWorld()->checkForBoulderProximity(getX(), getY()-1, 3))
					repeat = false;
				break;
			case 4:
				setDirection(up);
				if (getWorld()->canMove(getX(), getY(), "up") && !getWorld()->checkForBoulderProximity(getX(), getY() + 1, 3))
					repeat = false;
				break;
			};

		} while (repeat);

		numSquaresToMoveInCurrentDirection = 8 + rand() % 53;
	}

	if (turnCounter >= 200)
	{
		if ( (getDirection() == left || getDirection() == right) )
		{
			//If currently facing left or right
			bool goUp = false;
			bool goDown = false;
			//Check if possible to go up and/or down
			if (getWorld()->canMove(getX(), getY(), "up") && !getWorld()->checkForBoulderProximity(getX(), getY() + 1, 3))
				goUp = true;
			if (getWorld()->canMove(getX(), getY(), "down") && !getWorld()->checkForBoulderProximity(getX(), getY() - 1, 3))
				goDown = true;
			
			if (goUp & goDown)
			{
				//If possible to go both up and down then decide randomly by checking if a random number is odd or even
				if (rand() % 2 == 0)
					setDirection(up);
				else
					setDirection(down);

				numSquaresToMoveInCurrentDirection = 8 + rand() % 53;
				turnCounter = 0;
			}
			else if (goUp)
			{
				//if only possible to go up then go up
				setDirection(up);

				numSquaresToMoveInCurrentDirection = 8 + rand() % 53;
				turnCounter = 0;
			}
			else if (goDown)
			{
				//if only possible to go up then go up
				setDirection(down);

				numSquaresToMoveInCurrentDirection = 8 + rand() % 53;
				turnCounter = 0;
			}
		}
		else if ((getDirection() == up || getDirection() == down))
		{
			//If currently facing up or down
			bool goLeft = false;
			bool goRight = false;
			//Check if possible to go left and/or right
			if (getWorld()->canMove(getX(), getY(), "left") && !getWorld()->checkForBoulderProximity(getX()-1, getY(), 3))
				goLeft = true;
			if (getWorld()->canMove(getX(), getY(), "right") && !getWorld()->checkForBoulderProximity(getX()+1, getY(), 3))
				goRight = true;

			if (goLeft & goRight)
			{
				//If possible to go both left and right then decide randomly by checking if a random number is odd or even
				if (rand() % 2 == 0)
					setDirection(left);
				else
					setDirection(right);

				numSquaresToMoveInCurrentDirection = 8 + rand() % 53;
				turnCounter = 0;
			}
			else if (goLeft)
			{
				//if only possible to go left then go left
				setDirection(left);

				numSquaresToMoveInCurrentDirection = 8 + rand() % 53;
				turnCounter = 0;
			}
			else if (goRight)
			{
				//if only possible to go right then go right
				setDirection(right);

				numSquaresToMoveInCurrentDirection = 8 + rand() % 53;
				turnCounter = 0;
			}
		}

		
	}

	switch (getDirection())
	{
	case up:
		if (getWorld()->canMove(getX(), getY(), "up") && !getWorld()->checkForBoulderProximity(getX(), getY() + 1, 3))
			moveTo(getX(), getY() + 1);
		else
			numSquaresToMoveInCurrentDirection = 0;
		return;
		break;
	case down:
		if (getWorld()->canMove(getX(), getY(), "down") && !getWorld()->checkForBoulderProximity(getX(), getY() - 1, 3))
			moveTo(getX(), getY() - 1);
		else
			numSquaresToMoveInCurrentDirection = 0;
		return;
		break;
	case left:
		if (getWorld()->canMove(getX(), getY(), "left") && !getWorld()->checkForBoulderProximity(getX() - 1, getY(), 3))
			moveTo(getX() - 1, getY());
		else
			numSquaresToMoveInCurrentDirection = 0;
		return;
		break;
	case right:
		if (getWorld()->canMove(getX(), getY(), "right") && !getWorld()->checkForBoulderProximity(getX() + 1, getY(), 3))
			moveTo(getX() + 1, getY());
		else
			numSquaresToMoveInCurrentDirection = 0;
		return;
		break;
	}

}

void regularProtester::pickUpGold()
{
	getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	getWorld()->increaseScore(25);
	leaveTheOilField = true;
}

bool regularProtester::annoyed(int x)
{
	if (leaveTheOilField)
	{
		return false;
	}

	setHP(getHP()-x);
	if (getHP() > 0)
	{
		//If the protester survived
		getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
		
		if ((100 - (getWorld()->getLevel() * 10)) > 50)
			stunCounter = (100 - (getWorld()->getLevel() * 10));
		else
			stunCounter = 50;
	}
	else
	{
		//Doesn't die till it reaches 60,60; make sure its alive and losing HP didn't kill it
		setLife(true);
		//If the protester died
		leaveTheOilField = true;
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);

		//ensure it will do something on the next move:
		//This implementation is different from the spec but functionally identical
		//the spec said to set tickCounter to 0, but my program counts upwards not downwards, so I set it to the limit
		//The spec didn't actually say that tickCounter had to count down to 0, it just assumed it would
		tickCounter = ticksToWaitBetweenMoves;
		stunCounter = 0;
		//Incrementing player's score accordingly depending on whether protester was killed by a boulder or squirt is done by the boulder and squirt
		//classes respectively
	}

	return true;
}

hardcoreProtester::hardcoreProtester(StudentWorld* world) : Person(TID_HARD_CORE_PROTESTER, 60, 60, left, 20, world)
{
	leaveTheOilField = false;
	numSquaresToMoveInCurrentDirection = 8 + (rand() % 53);

	//ticksToWaitBetweenMoves = max(0, 3 – getWorld()->getLevel() / 4);
	if (3 - getWorld()->getLevel() / 4 > 0)
		ticksToWaitBetweenMoves = 3 - getWorld()->getLevel() / 4;
	else
		ticksToWaitBetweenMoves = 0;

	tickCounter = 0;
	shoutCounter = 0;
	turnCounter = 0;
	stunCounter = 0;
}

void hardcoreProtester::doSomething()
{
	if (!getLife())
	{
		//return immediately if its already dead
		return;
	}
	
	if (stunCounter >= 0)
	{
		stunCounter--;
		return;
	}

	if (tickCounter >= ticksToWaitBetweenMoves)
	{
		//we can continue with the rest of the function
		//Since this is a non-resting tick:
		//increment shout counter 
		//increment turn counter
		//reset tick counter

		shoutCounter++;
		turnCounter++;
		tickCounter = 0;

		//Uncomment below line to see if Protester is turning at the right time
		//std::cerr << turnCounter << std::endl;
	}
	else
	{
		//increment tickcounter if we are not done waiting for our move
		tickCounter++;
		return;
	}

	if (leaveTheOilField)
	{
		if (getX() == 60 && getY() == 60)
		{
			setLife(false);
			return;
		}

		//empty out vector containing path to exit
		exitStrategy.clear();
		//fill vector with new path from given location
		getWorld()->useExitPath(getX(), getY(), this);
		//reverse order since the returned path is backwards
		std::reverse(exitStrategy.begin(), exitStrategy.end());

		//In some cases, the falling boulder will make all exit paths impossible. This is resolved by waiting for a few ticks
		if (exitStrategy.size() < 2)
			return;

		//change direction and move in the appropriate direcction
		if (exitStrategy[1] / 100 == (getX() + 1))
			setDirection(right);
		else if (exitStrategy[1] / 100 == (getX() - 1))
			setDirection(left);
		else
		{
			if (exitStrategy[1] % 100 == (getY() + 1))
				setDirection(up);
			else if (exitStrategy[1] % 100 == (getY() - 1))
				setDirection(down);
		}
		moveTo(exitStrategy[1] / 100, exitStrategy[1] % 100);

		//end this turn
		return;
	}

	if (getWorld()->checkForTunnelMan(getX(), getY(), 4) && getWorld()->facingTunnelMan(getX(), getY(), getDirection()))
	{
		//If the tunnelman is within 4 blocks and the protester is facing the right direction
		if (shoutCounter >= 15)
		{
			//Shout at the TunnelMan and return 
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			//getWorld()->damageTunnelMan(2);
			getWorld()->getTunnelMan()->annoyed(2);
			shoutCounter = 0;
		}
		return;
	}

	//If tunnelman is > 4 units away
	if (!getWorld()->checkForTunnelMan(getX(), getY(), 4))
	{
		//Use this line instead so Hardcore Protester will follow TunnelMan anywhere for testing the method
		//int nextMove = getWorld()->useTunnelManPath(getX(), getY(), 999);
		int nextMove = getWorld()->useTunnelManPath(getX(), getY(), 16 + getWorld()->getLevel() * 2);
		if (nextMove != -1)
		{
			if (nextMove / 100 == (getX() + 1))
				setDirection(right);
			else if (nextMove / 100 == (getX() - 1))
				setDirection(left);
			else
			{
				if (nextMove % 100 == (getY() + 1))
					setDirection(up);
				else if (nextMove % 100 == (getY() - 1))
					setDirection(down);
			}
			moveTo(nextMove / 100, nextMove % 100);
			return;
		}
	}

	if (!getWorld()->checkForTunnelMan(getX(), getY(), 4))
	{
		//If he is in line of sight
		int dir = getWorld()->TunnelManInLineOfSight(getX(), getY());
		if (dir != 0)
		{
			if (dir == 3)
			{
				setDirection(up);
				moveTo(getX(), getY() + 1);
			}
			else if (dir == 4)
			{
				setDirection(down);
				moveTo(getX(), getY() - 1);
			}
			else if (dir == 1)
			{
				setDirection(right);
				moveTo(getX() + 1, getY());
			}
			else if (dir == 2)
			{
				setDirection(left);
				moveTo(getX() - 1, getY());
			}

			numSquaresToMoveInCurrentDirection = 0;
			return;
		}
	}

	numSquaresToMoveInCurrentDirection--;

	if (numSquaresToMoveInCurrentDirection <= 0)
	{
		bool repeat;

		do
		{
			repeat = true;
			int dir = 1 + rand() % 4;
			switch (dir)
			{
			case 1:
				setDirection(left);
				if (getWorld()->canMove(getX(), getY(), "left") && !getWorld()->checkForBoulderProximity(getX() - 1, getY(), 3))
					repeat = false;
				break;
			case 2:
				setDirection(right);
				if (getWorld()->canMove(getX(), getY(), "right") && !getWorld()->checkForBoulderProximity(getX() + 1, getY(), 3))
					repeat = false;
				break;
			case 3:
				setDirection(down);
				if (getWorld()->canMove(getX(), getY(), "down") && !getWorld()->checkForBoulderProximity(getX(), getY() - 1, 3))
					repeat = false;
				break;
			case 4:
				setDirection(up);
				if (getWorld()->canMove(getX(), getY(), "up") && !getWorld()->checkForBoulderProximity(getX(), getY() + 1, 3))
					repeat = false;
				break;
			};

		} while (repeat);

		numSquaresToMoveInCurrentDirection = 8 + rand() % 53;
	}

	if (turnCounter >= 200)
	{
		if ((getDirection() == left || getDirection() == right))
		{
			//If currently facing left or right
			bool goUp = false;
			bool goDown = false;
			//Check if possible to go up and/or down
			if (getWorld()->canMove(getX(), getY(), "up") && !getWorld()->checkForBoulderProximity(getX(), getY() + 1, 3))
				goUp = true;
			if (getWorld()->canMove(getX(), getY(), "down") && !getWorld()->checkForBoulderProximity(getX(), getY() - 1, 3))
				goDown = true;

			if (goUp & goDown)
			{
				//If possible to go both up and down then decide randomly by checking if a random number is odd or even
				if (rand() % 2 == 0)
					setDirection(up);
				else
					setDirection(down);

				numSquaresToMoveInCurrentDirection = 8 + rand() % 53;
				turnCounter = 0;
			}
			else if (goUp)
			{
				//if only possible to go up then go up
				setDirection(up);

				numSquaresToMoveInCurrentDirection = 8 + rand() % 53;
				turnCounter = 0;
			}
			else if (goDown)
			{
				//if only possible to go up then go up
				setDirection(down);

				numSquaresToMoveInCurrentDirection = 8 + rand() % 53;
				turnCounter = 0;
			}
		}
		else if ((getDirection() == up || getDirection() == down))
		{
			//If currently facing up or down
			bool goLeft = false;
			bool goRight = false;
			//Check if possible to go left and/or right
			if (getWorld()->canMove(getX(), getY(), "left") && !getWorld()->checkForBoulderProximity(getX() - 1, getY(), 3))
				goLeft = true;
			if (getWorld()->canMove(getX(), getY(), "right") && !getWorld()->checkForBoulderProximity(getX() + 1, getY(), 3))
				goRight = true;

			if (goLeft & goRight)
			{
				//If possible to go both left and right then decide randomly by checking if a random number is odd or even
				if (rand() % 2 == 0)
					setDirection(left);
				else
					setDirection(right);

				numSquaresToMoveInCurrentDirection = 8 + rand() % 53;
				turnCounter = 0;
			}
			else if (goLeft)
			{
				//if only possible to go left then go left
				setDirection(left);

				numSquaresToMoveInCurrentDirection = 8 + rand() % 53;
				turnCounter = 0;
			}
			else if (goRight)
			{
				//if only possible to go right then go right
				setDirection(right);

				numSquaresToMoveInCurrentDirection = 8 + rand() % 53;
				turnCounter = 0;
			}
		}


	}

	switch (getDirection())
	{
	case up:
		if (getWorld()->canMove(getX(), getY(), "up") && !getWorld()->checkForBoulderProximity(getX(), getY() + 1, 3))
			moveTo(getX(), getY() + 1);
		else
			numSquaresToMoveInCurrentDirection = 0;
		return;
		break;
	case down:
		if (getWorld()->canMove(getX(), getY(), "down") && !getWorld()->checkForBoulderProximity(getX(), getY() - 1, 3))
			moveTo(getX(), getY() - 1);
		else
			numSquaresToMoveInCurrentDirection = 0;
		return;
		break;
	case left:
		if (getWorld()->canMove(getX(), getY(), "left") && !getWorld()->checkForBoulderProximity(getX() - 1, getY(), 3))
			moveTo(getX() - 1, getY());
		else
			numSquaresToMoveInCurrentDirection = 0;
		return;
		break;
	case right:
		if (getWorld()->canMove(getX(), getY(), "right") && !getWorld()->checkForBoulderProximity(getX() + 1, getY(), 3))
			moveTo(getX() + 1, getY());
		else
			numSquaresToMoveInCurrentDirection = 0;
		return;
		break;
	}

}

void hardcoreProtester::pickUpGold()
{
	getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	getWorld()->increaseScore(50);
	//ticks_to_stare = max(50, 100 – current_level_number * 10)
	if (50 > (100 - getWorld()->getLevel() * 10))
		stunCounter = 50;
	else
		stunCounter = (100 - getWorld()->getLevel() * 10);
}

bool hardcoreProtester::annoyed(int x)
{
	if (leaveTheOilField)
		return false;
	setHP(getHP() - x);
	if (getHP() > 0)
	{
		//If the protester survived, play the appropriate sound
		getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
		
		//set stunCounter to stun the protester by the amount provided in spec:  max(50, 100 – current_level_number * 10)
		if ((100 - (getWorld()->getLevel() * 10)) > 50)
			stunCounter = (100 - (getWorld()->getLevel() * 10));
		else
			stunCounter = 50;
	}
	else
	{
		//If the protester died,
		//Protester doesn't actually die (get deleted) till it reaches (60,60); make sure its alive and setHP() function didn't kill it
		setLife(true);
		leaveTheOilField = true;
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);

		//ensure it will do something on the next move:
		//This implementation is different from the spec but functionally identical
		//the spec said to set tickCounter to 0, but my program counts upwards not downwards, so I set it to the limit
		//The spec didn't actually say that tickCounter had to count down to 0, it just assumed it would
		tickCounter = ticksToWaitBetweenMoves;
		stunCounter = 0;
		//Incrementing player's score accordingly depending on whether protester was killed by a boulder or squirt is done by the boulder and squirt
		//classes respectively
	}

	return true;

}

