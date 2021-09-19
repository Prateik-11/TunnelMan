#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <vector>

class StudentWorld;

//Base class for all of the game's objects, derived from GraphObject
class Actor : public GraphObject
{
public:

	Actor(int imageID, int x, int y, Direction d, double size, unsigned int depth, StudentWorld* worldpointer);
	//destructor
	~Actor();

	virtual std::string getType();

	//Accesor function for corresponding StudentWorld 
	StudentWorld* getWorld();

	//virtual method called doSomething() that can be called by the World to get one of the game’s actors to do something.
	// does nothing by default so actors like Earth don't have to override it
	virtual void doSomething();

	void setLife(bool x);

	bool getLife();

	//Since we can't use IsVisible as per the spec, we'll use this instead
	void makeVisible(bool x);

	bool getVisible();

private:
	
	//Pointer to object corresponding StudentWorld
	StudentWorld* world;
	bool alive;
	bool visible;
};

//Earth class, doesn't really do much
class Earth : public Actor
{
public:

	//passes parameters to the corresponding base classes
	Earth(int x, int y, StudentWorld* world);

	~Earth();
};

class Boulder : public Actor
{
public:
	Boulder(int x, int y, StudentWorld* world);
	
	~Boulder();

	virtual std::string getType();

	virtual void doSomething();

private:
	int state;
	int timer;
};

class Squirt : public Actor
{
public:
	Squirt(int x, int y, Direction d, StudentWorld* world);

	~Squirt();

	virtual std::string getType();

	virtual void doSomething();

private:
	int travelDistance;
};

class Goodies : public Actor
{
public:
	Goodies(int imageID, int x, int y, StudentWorld* world);
	~Goodies();
};

class Barrel : public Goodies
{
public:
	Barrel(int x, int y, StudentWorld* world);
	~Barrel();
	virtual std::string getType();
	virtual void doSomething();
};

class GoldNugget : public Goodies
{
public:
	GoldNugget(int x, int y, bool visible, bool protesterORtunnelman, bool permanentOrTemporary, StudentWorld* world, int Ntimer = 0);
	~GoldNugget();
	virtual std::string getType();
	virtual void doSomething();
private:
	bool canBePickedUpByTunnelMan; //if false, can be picked up by protestor
	bool permanent;
	int timer;
};

class WaterPool: public Goodies
{
public:
	WaterPool(int x, int y, StudentWorld* world);
	~WaterPool();
	virtual std::string getType();
	virtual void doSomething();
private:
	int timer;
};

class Sonar : public Goodies
{
public:
	Sonar(StudentWorld* world);
	~Sonar();
	virtual std::string getType();
	virtual void doSomething();
private:
	int tickCounter;
	int counterMax;
};

//PERSON CLASS THAT ACTS AS A BASE CLASS FOR TUNNELMAN AND ALL PROTESTERS
class Person : public Actor
{
public:

	//Passes along parameters to the appropriate base class
	Person(int imageID, int x, int y, Direction d, int newHP, StudentWorld* world);

	~Person();

	//Accessor and mutator for HP
	void setHP(int x);
	int getHP();
	virtual bool annoyed(int x) = 0;
	virtual void addToExitStrategy(int);
	virtual void pickUpGold();

private:

	int HP;
};

class TunnelMan : public Person
{
public:

	//Passes along parameters to the appropriate base class
	TunnelMan(StudentWorld* world);
	//Destructor
	~TunnelMan();
	//Overriden function for TunnelMan's actions to be called by StudentWorld::move() each tick
	int getSquirts();
	void setSquirts(int x);
	int getSonarCharges();
	void setSonarCharges(int x);
	void setGoldNuggets(int x);
	int getGoldNuggets();
	virtual void doSomething();
	virtual bool annoyed(int x);
private:
	int numberOfSquirts;
	int sonarCharges;
	int goldNuggets;
};

class regularProtester : public Person
{
public:
	regularProtester(StudentWorld* world);
	~regularProtester();
	virtual std::string getType();
	virtual void doSomething();
	virtual bool annoyed(int x);
	virtual void pickUpGold();
	void addToExitStrategy(int x);
	std::vector<int> exitStrategy;
private:
	bool leaveTheOilField;
	int numSquaresToMoveInCurrentDirection;
	int ticksToWaitBetweenMoves;
	int tickCounter;
	int shoutCounter;
	int turnCounter;
	int stunCounter;
	
};

class hardcoreProtester : public Person
{
public:
	hardcoreProtester(StudentWorld* world);
	~hardcoreProtester();
	virtual std::string getType();
	virtual void doSomething();
	virtual bool annoyed(int x);
	virtual void pickUpGold();
	void addToExitStrategy(int x);
	std::vector<int> exitStrategy;
private:
	bool leaveTheOilField;
	int numSquaresToMoveInCurrentDirection;
	int ticksToWaitBetweenMoves;
	int tickCounter;
	int shoutCounter;
	int turnCounter;
	int stunCounter;

};

#endif // ACTOR_H_