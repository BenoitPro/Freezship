// BulletItem.h
// Header file 
// 
// BulletItem class provides methods to manage and draw a Bullet
// Inherit of ObstacleItem class
//
// Author : Benoit Marechal

#ifndef BULLETITEM_H
#define BULLETITEM_H

/////////////////////////
// Includes
/////////////////////////

#include "ObstacleItem.h"


///////////////////////
// Class definition
////////////////////////
class BulletItem:public ObstacleItem {
public :

	bool mbIsLookForFoe; // If the target look for foe
	bool mbGravityOn; // If is gravity is on
    BackgroundItem* mpTarget; // The target aimed
	double mdTime; // count the time
	int miCountFrame; // count the number of frame 


public :
	// Constructor
	BulletItem(int item);
	// Destructor
	~BulletItem();

	// Initialize method
	void init();

	// Move the bullet
	void move(void);

	// Draw the bullet
	void draw(void);


	// setters
	void setCountFrame(int cf);
	void setGravity(bool g);
	void setTarget(BackgroundItem* b);
	void setAlive(bool b);

	// To aim the target
	void aimTarget();

};

#endif