// ObstacleItem.h
// Header file 
// 
// ObstacleItem class provides methods to manage and draw a Obstacle
// Inherit of a BackgroundItem class
//
// Author : Benoit Marechal

#ifndef OBSTACLEITEM_H
#define OBSTACLEITEM_H


/////////////////////////
// Includes
/////////////////////////
#include "BackgroundItem.h"

////////////////////////
// Class definition
////////////////////////
class ObstacleItem:public BackgroundItem {
public :

	double mdLifeAmount ;	// Life Amount
	int miDamage;			// Damage caused by the Obstacle

public :
// Constructor
	ObstacleItem(int item);
// Destructor
	~ObstacleItem();

//Methods 

	// Initialize method
	void init();

	// Move the Obstacle
	void move(void);

	// Draw the Obstacle
	void draw(void);

	// getters
	double getLifeAmount(void);

	// setters
	void setLifeAmount(double life);

};
#endif