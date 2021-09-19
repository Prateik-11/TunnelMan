#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include "Actor.h"
#include <vector>
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	//Constructor
	StudentWorld(std::string assetDir);

	//Destructor
	~StudentWorld();

	virtual int init();

	virtual int move();
	
	virtual void cleanUp();
	
	//removes earth from the location x,y if it exists
	int removeEarthAtxy(int x, int y);

	//returns true if Tunnelman is <= radius distance away from (x,y)
	bool checkForTunnelMan(int x, int y, int radius);

	//bribe protester if it is <= radius distance away from (x,y)
	bool bribeProtester(int x, int y, int radius);

	//kill protesters that are <= radius distanc away from (x,y)
	bool annoyProtesters(int x, int y, int radius, int damage);

	//returns true if there is a boulder in the 1x4 area in the given direction
	bool checkForBoulder(int x, int y, std::string direction);

	//returns true if x, y is within radius of a boulder
	bool checkForBoulderProximity(int x, int y, int radius);

	//returns true if earth exists at x,y
	bool checkForEarth(int x, int y);

	//Sets the text at the top of the screen
	void setDisplayText();

	//returns true if the object at x,y facing in the given direction is facing TunnelMan
	bool facingTunnelMan(int x, int y, int direction);

	//returns true if TunnelMan is in line of sight of x,y
	int TunnelManInLineOfSight(int x, int y);

	//returns true if the actor at x,y can move in the specified direction, assuming the object has size 1.0 (4x4 squares)
	bool canMove(int x, int y, std::string s);

	int useTunnelManPath(int x, int y, int m);

	int useExitPath(int x, int y, Person* protester);

	void useSonar();

	////used by TunnelMan to add squirt to the listOfActors when spacebar is pressed
	//void addSquirt(Squirt* sq)
	//{
	//	listOfActors.push_back(sq);
	//}

	////Used by TunnelMan to add nugget when tab is pressed
	//void addNugget(GoldNugget* g)
	//{
	//	listOfActors.push_back(g);
	//}

	void addActor(Actor* g);
	
	//keeps track of how many barrels are picked
	void incrementBarrels();

	//returns how many barrels are picked
	unsigned int getBarrelsPicked();

	//returns TunnelMan address
	TunnelMan* getTunnelMan();

private:

	TunnelMan* TM;
	Earth* earthTracker[64][64];
	std::vector<Actor*> listOfActors;
	unsigned int barrelsPicked;
	bool visited[64][64];
	int distance[64][64];
	int previous[64][64];
	int ProtesterTimer;
	int ProtesterTimerMax;

	//uses breadth first search to find an exit path
	int protesterExitPath(int x, int y);

	//uses breadth first search to find the tunnelman
	int protesterTunnelManPath(int x, int y);
};

#endif // STUDENTWORLD_H_
