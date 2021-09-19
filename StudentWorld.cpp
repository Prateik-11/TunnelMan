#include "StudentWorld.h"
#include "GraphObject.h"
#include <string>
#include <random>
#include <vector>
#include <queue>
#include <list>
#include <iostream>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir)
{
	//initialise all pointers to nullptr
	//Dynamic allocation takes place in the init function to prevent dynamically allocating twice in a row
	//as this would cause a memory leak

	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			earthTracker[i][j] = nullptr;
		}
	}

	TM = nullptr;

	//Assign values for each constant
	barrelsPicked = 0;
	//The actual values for these are provided in init()
	ProtesterTimerMax = 0;
	ProtesterTimer = 0;

	//The 2D arrays distance, visited and previous are iniatialised before they are used
	//there is no point of initialising them here, they'll have to be re-initialised whenever they are used anyway
	//They might as well have been dynamically allocated, but doing that made my program run slower when their correspondign function was called
}

StudentWorld::~StudentWorld()
{
	//delete all the Earth objects
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			delete earthTracker[i][j];
		}
	}

	//delete all actors
	listOfActors.clear();

	//delete TunnelMan
	delete TM;
}

//#######################################################################################################
//										MAIN FUNCTIONS
//#######################################################################################################

int StudentWorld::init()
{
	//Initialise Tunnelman
	TM = new TunnelMan(this);

	//Initialise Earth
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			//Dont place Earth in the top 4 rows and in the central shaft
			if ((i >= 30 && i <= 33 && j > 3) || (j > 59))
			{
				continue;
			}
			else
			{
				earthTracker[i][j] = new Earth(i, j, this);
			}
		}
	}

	vector<int> positions;

	//Boulders, Barrels and Nuggets position will be randomly picked from slots on the oilfield arranged like a checkerboard with each box spanning a 6x6 base.
	//There will be 50 such slots, this will ensure both the randomness and that no 2 objects are within a 6 box radius of each other. Since there can
	// only be 32 items at max, this is more than enough
	//They will have x coordinates of (i/5)*12, pre-adjustement for the shaft in the middle and y coordinates of (i%5)*6 where i is a number from
	//0 to 49. x will be shifted to the right on rows with even values of y to create the checkerboard effect which preserves the 6 box gap.
	//i will only span 24 to 49 on the first loop because boulders cant exist below y = 20.
	//these elements will be taken out of the vector to ensure the same one is not picked twice.
	for (int i = 24; i <= 49; i++)
	{
		positions.push_back(i);
	}

	//ADD BOULDERS
	//bouldersDropped = 0;
	for (size_t i = 1; i <= min(getLevel() / 2 + 2, (unsigned) 9); i++)
	{
		//pick a random element from the array through a random index
		int placement = rand() % positions.size();
		int slot = positions[placement];
		//remove that element once picked
		positions.erase(positions.begin() + placement);;
		
		//tranform that element into coordinates according to instructionos noted above
		int x = slot % 5;
		int y = slot / 5;
		x *= 12;
		if (y % 2 != 0)
			x += 6;
		if (x >= 30)
			x += 4;
		y *= 6;

		//add boulder at those coordinates
		listOfActors.push_back(new Boulder(x, y, this));
	}

	//Now that we can add items below y=20, adding more coordinates to the vector
	for (int i = 0; i <= 23; i++)
	{
		positions.push_back(i);
	}

	//ADD BARRELS
	barrelsPicked = 0;
	for (size_t i = 1; i <= min( 2 + getLevel(), (unsigned) 21 ); i++)
	{
		//pick a random element from the array through a random index
		int placement = rand() % positions.size();
		int slot = positions[placement];
		//remove that element once picked
		positions.erase(positions.begin() + placement);;

		//tranform that element into coordinates according to instructionos noted above
		int x = slot % 5;
		int y = slot / 5;
		x *= 12;
		if (y % 2 != 0)
			x += 6;
		if (x >= 30)
			x += 4;
		y *= 6;

		//add barrel at those coordinates
		listOfActors.push_back(new Barrel(x, y, this));
		
	}

	//ADD NUGGETS
	for (size_t i = 1; i <= max(5 - getLevel()/ 2, (unsigned)2); i++)
	{
		//pick a random element from the array through a random index
		int placement = rand() % positions.size();
		int slot = positions[placement];
		//remove that element once picked
		positions.erase(positions.begin() + placement);;

		//tranform that element into coordinates according to instructionos noted above
		int x = slot % 5;
		int y = slot / 5;
		x *= 12;
		if (y % 2 != 0)
			x += 6;
		if (x >= 30)
			x += 4;
		y *= 6;

		//add barrel at those coordinates
		listOfActors.push_back(new GoldNugget(x, y, false, true,true,this));
	}


	//Set value for when protesters should be spawned
	if (200 - (int)getLevel() > 25)
	{
		ProtesterTimerMax = 200 - (int)getLevel();
	}
	else
		ProtesterTimerMax = 25;

	//Set value such that first protester is called on the first tick
	ProtesterTimer = ProtesterTimerMax;

	//used for testing
	//listOfActors.push_back(new hardcoreProtester(this));
	/*listOfActors.push_back(new Boulder(50, 55, this));
	listOfActors.push_back(new Boulder(50, 25, this));*/
	
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	//Updating the status screen at the top
	setDisplayText();

	//make TunnelMan do something each tick
	//TM->doSomething();

	//Make all the actors do something each tick
	for (unsigned int i = 0; i < listOfActors.size(); i++)
	{
		listOfActors[i]->doSomething();
		
		//Remove all dead actors
		if (!listOfActors[i]->getLife())
		{
			delete listOfActors[i];
			listOfActors.erase(listOfActors.begin() + i);
			i--;
		}

		//End level if TunnelMan dies
		if (!TM->getLife())
		{
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
		
		//End level if all Barrels are picked up
		if (getBarrelsPicked() == min(2 + getLevel(), (unsigned) 21))
		{
			playSound(SOUND_FINISHED_LEVEL);
			return GWSTATUS_FINISHED_LEVEL;
		}
	}

	TM->doSomething();

	for (unsigned int i = 0; i < listOfActors.size(); i++)
	{
		//Remove all dead actors
		if (!listOfActors[i]->getLife())
		{
			delete listOfActors[i];
			listOfActors.erase(listOfActors.begin() + i);
			i--;
		}
	}


	//End level if TunnelMan dies
	if (!TM->getLife())
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	//End level if all Barrels are picked up
	if (getBarrelsPicked() == min(2 + getLevel(), (unsigned)21))
	{
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}

	//If it is time to add a protester
	if (ProtesterTimer >= ProtesterTimerMax)
	{
		int numberOfProtesters = 0;
		//Calculate number of protesters
		for(size_t i = 0; i < listOfActors.size(); i++)
		{
			if (listOfActors[i]->getType() == "Regular Protester" || listOfActors[i]->getType() == "Hardcore Protester")
				numberOfProtesters++;
		}
		
		//If there arent enough protesters yet
		if (numberOfProtesters < min((double)15, 2 + getLevel() * 1.5))
		{
			if (rand() % min((unsigned)90, getLevel() * 10 + 30) == 0)
			{
				//spawn HardcoreProtester with the probability given in spec
				listOfActors.push_back(new hardcoreProtester(this));
			}
			else
			{
				//spawn regularProtester
				listOfActors.push_back(new regularProtester(this));
			}
			ProtesterTimer = 0;
		}
	}
	else
	{
		//If it is not yet time to add a protester
		ProtesterTimer++;
	}
	
	//Add Goodies
	if (rand() % (getLevel() * 25 + 300) == 0)
	{
		if (rand() % 5 == 0)
		{
			//create sonar
			listOfActors.push_back(new Sonar(this));
		}
		else
		{
			//create water, assigned to a random empty 4*4 slot of earth
			vector<int>* emptySpots = new vector<int>;
			for (int i = 0; i <= 60; i++)
			{
				for (int j = 0; j <= 60; j++)
				{
					//Dont know if water and sonar are allowed to spawn in the same place
					if (i == 0 && j == 60)
						continue;

					if (canMove(i - 4, j, "right") && canMove(i - 3, j, "right") && canMove(i - 2, j, "right") && canMove(i - 1, j, "right"))
					{
						//this is an open 4x4 slot
						emptySpots->push_back((100 * i) + j);
					}
				}
			}
			int z = rand() % emptySpots->size();
			int x = (*emptySpots)[z] / 100;
			int y = (*emptySpots)[z] % 100;
			listOfActors.push_back(new WaterPool(x, y, this));
			emptySpots->clear();
			delete emptySpots;
		}
	}

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	//delete all Earth objects
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			delete earthTracker[i][j];
		}
	}

	//delete all actors
	for (size_t i = 0; i < listOfActors.size(); i++)
	{
		delete listOfActors[i];
	}

	listOfActors.clear();

	//delete TunnelMan
	delete TM;
}

//#######################################################################################################
//										HELPER FUNCTIONS
//#######################################################################################################

int StudentWorld::removeEarthAtxy(int x, int y)
{
	if (earthTracker[x][y] == nullptr || !earthTracker[x][y]->getVisible())
	{
		//If the Earth object is non-existant or invisible
		return 0;
	}
	else
	{
		//If the Earth object is visible and needs to be removed
		earthTracker[x][y]->setVisible(false);
		earthTracker[x][y]->makeVisible(false);
		return 1;
	}
}

bool StudentWorld::checkForTunnelMan(int x, int y, int radius)
{
	return (x - TM->getX()) * (x - TM->getX()) + (y - TM->getY()) * (y - TM->getY()) <= radius*radius;
}

bool StudentWorld::bribeProtester(int x, int y, int radius)
{
	for (size_t i = 0; i < listOfActors.size(); i++)
	{
		if (listOfActors[i]->getType() == "Regular Protester" || listOfActors[i]->getType() == "Hardcore Protester")
		{
			Person* protester = dynamic_cast<Person*>(listOfActors[i]);
			
			// (x_1 - x_2)^2 + (y_1 - y_2)^2 <= radius^2
			if ((x - protester->getX()) * (x - protester->getX()) + (y - protester->getY()) * (y - protester->getY()) <= radius * radius)
			{
				//if a protester is within the radius
				protester->pickUpGold();
				return true;
			}
		}
	}

	return false;
}

bool StudentWorld::annoyProtesters(int x, int y, int radius, int damage)
{
	bool hit = false;

	for (size_t i = 0; i < listOfActors.size(); i++)
	{
		if (listOfActors[i]->getType() == "Regular Protester" || listOfActors[i]->getType() == "Hardcore Protester")
		{
			// (x_1 - x_2)^2 + (y_1 - y_2)^2 <= radius^2
			if ((x - listOfActors[i]->getX()) * (x - listOfActors[i]->getX()) + (y - listOfActors[i]->getY()) * (y - listOfActors[i]->getY()) <= radius * radius)
			{
				Person* protester = dynamic_cast<Person*>(listOfActors[i]);
				//if a protester is within the radius
				if (protester->annoyed(damage))
				{
					//If the protester get damaged
					if (damage == 100)
					{
						//If the protester was killed by a boulder
						increaseScore(500);
					}
					if (damage == 2 && (protester->getHP() <= 0))
					{
						//If a regular protester was killed by a squirt
						if (protester->getType() == "Regular Protester")
							increaseScore(100);
						else if (protester->getType() == "Hardcore Protester")
							increaseScore(250);
					}
					//Do not return, squirt can hurt multiple protesters

					hit = true;
				}
			}
		}
	}

	return hit;
}

bool StudentWorld::checkForBoulder(int x, int y, string direction)
{
	if (direction == "left")
	{
		for (size_t i = 0; i < listOfActors.size(); i++)
		{
			if (listOfActors[i]->getType() == "Boulder" && listOfActors[i]->getX() >= (x - 4) && listOfActors[i]->getX() <= (x - 1) && listOfActors[i]->getY() <= (y + 3) && listOfActors[i]->getY() >= (y - 3))
				return true;
		}

		//older method
		/*for (size_t i = 0; i < min(getLevel() / 2 + 2, (unsigned)9) - bouldersDropped; i++)
		{
			if (listOfActors[i]->getX() >= (x - 4) && listOfActors[i]->getX() <= (x - 1) && listOfActors[i]->getY() <= (y + 3) && listOfActors[i]->getY() >= (y - 3))
				return true;
		}*/
	}
	else if (direction == "right")
	{
		for (size_t i = 0; i < listOfActors.size(); i++)
		{
			if (listOfActors[i]->getType() == "Boulder" && listOfActors[i]->getX() >= (x + 1) && listOfActors[i]->getX() <= (x + 4) && listOfActors[i]->getY() <= (y + 3) && listOfActors[i]->getY() >= (y - 3))
				return true;
		}
	}
	else if (direction == "up")
	{
		for (size_t i = 0; i < listOfActors.size(); i++)
		{
			if (listOfActors[i]->getType() == "Boulder" && listOfActors[i]->getX() >= (x - 3) && listOfActors[i]->getX() <= (x + 3) && listOfActors[i]->getY() <= (y + 4) && listOfActors[i]->getY() >= (y + 1))
				return true;
		}
	}
	else if (direction == "down")
	{
		for (size_t i = 0; i < listOfActors.size(); i++)
		{
			if (listOfActors[i]->getType() == "Boulder" && listOfActors[i]->getX() >= (x - 3) && listOfActors[i]->getX() <= (x + 3) && listOfActors[i]->getY() <= (y - 1) && listOfActors[i]->getY() >= (y - 4))
				return true;
		}
	}

	return false;
}

bool StudentWorld::checkForBoulderProximity(int x, int y, int radius)
{
	//All boulder have been dropped
	/*if (bouldersDropped == min(getLevel() / 2 + 2, (unsigned)9))
		return false;*/

	bool BoulderIsClose = false;

	//Check each boulder's position against the supplied coordinates
	for (size_t i = 0; i < listOfActors.size(); i++)
	{
		if (listOfActors[i]->getType() == "Boulder")
		{
			BoulderIsClose = (listOfActors[i]->getX() - x) * (listOfActors[i]->getX() - x) + (listOfActors[i]->getY() - y) * (listOfActors[i]->getY() - y) <= radius * radius;
			if (BoulderIsClose)
				return BoulderIsClose;
		}
	}
	
	return BoulderIsClose;
}

bool StudentWorld::checkForEarth(int x, int y)
{
	if (earthTracker[x][y] != nullptr)
	{
		//If it is not nullptr
		if (earthTracker[x][y]->getVisible())
		{
			//If it is visible
			return true;
		}
	}

	return false;
}

bool StudentWorld::facingTunnelMan(int x, int y, int direction)
{
	if (x < TM->getX() && direction == 4)
		return true;
	if (x > TM->getX() && direction == 3)
		return true;
	if (y > TM->getY() && direction == 2)
		return true;
	if (y < TM->getY() && direction == 1)
		return true;

	return false;
}

void StudentWorld::setDisplayText()
{
	//Format:
	// Lvl: 52 Lives: 3 Hlth: 80% Wtr: 20 Gld: 3 Oil Left: 2 Sonar: 1 Scr: 321000
	
	//add level text
	string s = "Lvl: ";
	
	//add extra space if level is only 1 digit
	if (getLevel() < 10)
		s = s + " ";
	
	//add level number
	s = s + to_string(getLevel());

	//add double space
	s = s + "  ";

	//add lives text and number
	s = s + " Lives: " + to_string(getLives());
	
	//add double space
	s = s + "  ";

	//add health text
	s = s + " Hlth: " ;

	//add leading space if health is not 3 digits
	if ((int)(((double)TM->getHP() / (double)10) * 100) < 100)
		s = s + " ";
	
	//add health number and %
	s = s + to_string((int)(((double)TM->getHP() / (double)10) * 100)) + "%";

	//add double space

	s = s + "  ";

	//add water text and number, with leading space as required
	s = s + "Wtr:";
	if (TM->getSquirts() < 10)
		s = s + " ";
	s = s + to_string(TM->getSquirts());

	//add double space
	s = s + "  ";

	//add gold text and number, with leading space as required
	s = s + "Gld:";
	if (TM->getGoldNuggets() < 10)
		s = s + " ";
	s = s + to_string(TM->getGoldNuggets());

	//add double space
	s = s + "  ";

	//add oil left text and number, with leading space as required
	s = s + "Oil Left:";
	if (min(2 + getLevel(), (unsigned)21) - getBarrelsPicked() < 10)
		s = s + " ";
	s = s + to_string(min(2 + getLevel(), (unsigned)21) - getBarrelsPicked());

	//add double space
	s = s + "  ";

	//add sonar text and number, with leading space as required
	s = s + "Sonar:";
	if (TM->getSonarCharges() < 10)
		s = s + " ";
	s = s + to_string(TM->getSonarCharges());

	//add double space
	s = s + "  ";

	//add text for score:
	s = s + "Scr: ";

	string x = to_string(getScore());
	
	for (size_t i = 1; i <= 6 - x.size(); i++)
		s += "0";

	s += x;

	setGameStatText(s);

	/*setGameStatText(
               "Lvl: "      + to_string(getLevel()) + 
		      " Lives: "    + to_string(getLives()) + 
		      " Hlth : "    + to_string((int)(((double)TM->getHP()/(double)10)*100)) + "%" +
		      " Wtr : "     + to_string(TM->getSquirts()) + 
	          " Gld: "      + to_string(TM->getGoldNuggets()) +        
		      " Oil Left: " + to_string(min(2 + getLevel(), (unsigned)21) -getBarrelsPicked()) + 
		      " Sonar: "    + to_string(TM->getSonarCharges()) + 
		      " Scr : "     + to_string(getScore())
				  );*/
}

//PRIVATE
int StudentWorld::protesterTunnelManPath(int x, int y)
{
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			visited[i][j] = false;
			previous[i][j] = -1;
			distance[i][j] = 9999;
		}
	}
	list<int> queue;
	queue.push_back(100 * x + y);
	distance[x][y] = 0;
	visited[x][y] = true;

	while (!queue.empty())
	{
		int u = queue.front();
		queue.pop_front();
		if (canMove(u / 100, u % 100, "up") && !visited[u / 100][u % 100 + 1])
		{
			visited[u / 100][u % 100 + 1] = true;
			distance[u / 100][u % 100 + 1] = 1 + distance[u / 100][u % 100];
			previous[u / 100][u % 100 + 1] = u;
			queue.push_back(u + 1);
			if ((u + 1) / 100 == TM->getX() && (u + 1) % 100 == TM->getY())
			{
				return distance[u / 100][u % 100 + 1];
			}
		}

		//The next part of the code throws up a bunch of warnings because it thinks I may enter an index beyond the size of the array
		//but I make sure that it wont search outside the outfield in the canMove() function, so these warnings are useless and can be ignored
		//Furthermore, there is no need to have a control path for when there is no path, as the protester can always just backtrack there is 
		//always a path

		if (canMove(u / 100, u % 100, "down") && !visited[u / 100][u % 100 - 1])
		{
			visited[u / 100][u % 100 - 1] = true;
			distance[u / 100][u % 100 - 1] = 1 + distance[u / 100][u % 100];
			previous[u / 100][u % 100 - 1] = u;
			queue.push_back(u - 1);
			if ((u - 1) / 100 == TM->getX() && (u - 1) % 100 == TM->getY())
			{
				return distance[u / 100][u % 100 - 1];
			}
		}

		if (canMove(u / 100, u % 100, "left") && !visited[u / 100 - 1][u % 100])
		{
			visited[u / 100 - 1][u % 100] = true;
			distance[u / 100 - 1][u % 100] = 1 + distance[u / 100][u % 100];
			previous[u / 100 - 1][u % 100] = u;
			queue.push_back(u - 100);
			if ((u - 100) / 100 == TM->getX() && (u - 100) % 100 == TM->getY())
			{
				return distance[u / 100 - 1][u % 100];
			}
		}

		if (canMove(u / 100, u % 100, "right") && !visited[u / 100 + 1][u % 100])
		{
			visited[u / 100 + 1][u % 100] = true;
			distance[u / 100 + 1][u % 100] = 1 + distance[u / 100][u % 100];
			previous[u / 100 + 1][u % 100] = u;
			queue.push_back(u + 100);
			if ((u + 100) / 100 == TM->getX() && (u + 100) % 100 == TM->getY())
			{
				return distance[u / 100 + 1][u % 100];
			}
		}
	}

	//Should never reach here, this statement is to avoid the empty control path warning
	return -1;
}

//PRIVATE
int StudentWorld::protesterExitPath(int x, int y)
{
	//These arrays might as well have been dynamically allocated, but doing that made my program run slower when this function was called
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			visited[i][j] = false;
			previous[i][j] = -1;
			distance[i][j] = 9999;
		}
	}
	list<int> queue;
	queue.push_back(100 * x + y);
	distance[x][y] = 0;
	visited[x][y] = true;

	while (!queue.empty())
	{
		int u = queue.front();
		queue.pop_front();
		if (canMove(u / 100, u % 100, "up") && !visited[u / 100][u % 100 + 1])
		{
			visited[u / 100][u % 100 + 1] = true;
			distance[u / 100][u % 100 + 1] = 1 + distance[u / 100][u % 100];
			previous[u / 100][u % 100 + 1] = u;
			queue.push_back(u + 1);
			if ((u + 1) / 100 == 60 && (u + 1) % 100 == 60)
			{
				return distance[u / 100][u % 100 + 1];
			}
		}

		//The next part of the code throws up a bunch of warnings because it thinks I may enter an index beyond the size of the array
		//but I make sure that it wont search outside the outfield in the canMove() function, so these warnings are useless and can be ignored
		//Furthermore, there is no need to have a control path for when there is no path, as the protester can always just backtrack there is 
		//always a path

		if (canMove(u / 100, u % 100, "down") && !visited[u / 100][u % 100 - 1])
		{
			visited[u / 100][u % 100 - 1] = true;
			distance[u / 100][u % 100 - 1] = 1 + distance[u / 100][u % 100];
			previous[u / 100][u % 100 - 1] = u;
			queue.push_back(u - 1);
			if ((u - 1) / 100 == 60 && (u - 1) % 100 == 60)
			{
				return distance[u / 100][u % 100 - 1];
			}
		}

		if (canMove(u / 100, u % 100, "left") && !visited[u / 100 - 1][u % 100])
		{
			visited[u / 100 - 1][u % 100] = true;
			distance[u / 100 - 1][u % 100] = 1 + distance[u / 100][u % 100];
			previous[u / 100 - 1][u % 100] = u;
			queue.push_back(u - 100);
			if ((u - 100) / 100 == 60 && (u - 100) % 100 == 60)
			{
				return distance[u / 100 - 1][u % 100];
			}
		}

		if (canMove(u / 100, u % 100, "right") && !visited[u / 100 + 1][u % 100])
		{
			visited[u / 100 + 1][u % 100] = true;
			distance[u / 100 + 1][u % 100] = 1 + distance[u / 100][u % 100];
			previous[u / 100 + 1][u % 100] = u;
			queue.push_back(u + 100);
			if ((u + 100) / 100 == 60 && (u + 100) % 100 == 60)
			{
				return distance[u / 100 + 1][u % 100];
			}
		}
	}

	//Should never reach here, this statement is to avoid the empty control path warning
	return -1;
}

int StudentWorld::useExitPath(int x, int y, Person* protester)
{
	int dist = protesterExitPath(x, y);
	//path.push_back(6060);
	int crawl = 6060;
	while (crawl != -1)
	{
		protester->addToExitStrategy(crawl);
		crawl = previous[crawl/100][crawl%100];
	}
	//for (int i = path.size() - 1; i >= 0; i--)
	//	cout << path[i] << " ";

	return dist;
}

int StudentWorld::useTunnelManPath(int x, int y, int m)
{
	int dist = protesterTunnelManPath(x, y);
	if (dist > m)
	{
		//If the distance is too great, return -1 to indicate that the TunnelMan is out of range
		return -1;
	}
	vector<int>* path = new vector<int>;
	int crawl = 100 * TM->getX() + TM->getY();
	while (crawl != -1)
	{
		path->push_back(crawl);
		crawl = previous[crawl / 100][crawl % 100];
	}
	//The path is stored in reverse, so reverse it
	reverse(path->begin(), path->end());
	//The first element of the path now is the current position, so we move to the next element's position
	int toReturn = -1;
	if(path->size()>=2)
		toReturn = (*path)[1];
	delete path;
	return toReturn;
}

int StudentWorld::TunnelManInLineOfSight(int x, int y)
{
	int itX = x;
	int itY = y;
	if (x == TM->getX())
	{
		for (int i = 0; i <= 3; i++)
		{
			if (y < TM->getY())
			{
				while (itY < TM->getY())
				{
					if (checkForEarth(x + i, itY))
						return 0;
					itY++;
				}
				
				itY = y;
			}
			else
			{
				while (itY > TM->getY())
				{
					if (checkForEarth(x + i, itY))
						return 0;
					itY--;
				}

				itY = y;
			}
		}	

		if (y < TM->getY())
			return 3;
		else
			return 4;
	}

	if (y == TM->getY())
	{
		for (int i = 0; i <= 3; i++)
		{
			if (x < TM->getX())
			{
				while (itX < TM->getX())
				{
					if (checkForEarth(itX, y + i))
						return 0;
					itX++;
				}
				
				itX = x;
				return 1;
			}
			else
			{
				while (itX > TM->getX())
				{
					if (checkForEarth(itX, y + i))
						return 0;
					itX--;
				}

				itX = x;
				return 2;
			}
		}		

		if (x < TM->getX())
			return 1;
		else
			return 2;
	}

	return 0;
}

bool StudentWorld::canMove(int x, int y, std::string s)
{
	if (checkForBoulder(x, y, s))
	{
		return false;
	}

	if (s == "left")
	{
		if (x <= 0)
		{
			return false;
		}
		for (int i = 0; i <= 3; i++)
		{
			if (checkForEarth(x - 1, y + i))
				return false;
		}
		return true;
	}
	
	else if (s == "right")
	{
		if (x >= 60)
			return false;
		for (int i = 0; i <= 3; i++)
		{
			if (checkForEarth(x + 4, y + i))
				return false;
		}
		return true;
	}
	
	else if (s == "up")
	{
		if (y >= 60)
			return false;
		for (int i = 0; i <= 3; i++)
		{
			if(checkForEarth(x + i, y + 4))
				return false;
		}
		return true;
	}
	
	else if (s == "down")
	{
		if (y <= 0)
			return false;
		for (int i = 0; i <= 3; i++)
		{
			if (checkForEarth(x + i, y - 1))
				return false;
		}
		return true;

	}

	//Should never reach here, this statement is only to stop getting a warning for empty control paths
	return false;
}

void StudentWorld::useSonar()
{
	for (size_t i = 0; i < listOfActors.size(); i++)
	{
		if ((listOfActors[i]->getX() - TM->getX()) * (listOfActors[i]->getX() - TM->getX()) + (listOfActors[i]->getY() - TM->getY()) * (listOfActors[i]->getY() - TM->getY()) <= 144)
		{
			listOfActors[i]->setVisible(true);
			listOfActors[i]->makeVisible(true);
		}
	}
}

void StudentWorld::addActor(Actor* g)
{
	listOfActors.push_back(g);
}

void StudentWorld::incrementBarrels()
{
	barrelsPicked++;
}

unsigned int StudentWorld::getBarrelsPicked()
{
	return barrelsPicked;
}

TunnelMan* StudentWorld::getTunnelMan()
{
	return TM;
}