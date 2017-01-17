// FoeItem.h
// Header file 
// 
// FoeItem class provides methods to manage and draw a Foe
// Inherit of a bullet class
//
// Author : Benoit Marechal

#ifndef FOEITEM_H
#define FOEITEM_H

/////////////////////////
// Includes
/////////////////////////
#include "BulletItem.h" 
#include "ItemCollection.h" 


///////////////////////
// Class definition
////////////////////////
class FoeItem:public BulletItem {
public :

	ItemCollection<BulletItem>* mpBullets; // Collection of bullets
	double mdFireFrequency; // temp entre 2 tire
	double mdFireTime; // Time during the foe fire bullets
	double mdLazyTime; // Time during the foe do nothing
	double mdMotion1Time; // Time during the foe go to the motion1
	double mdMotion2Time; // Time during the foe go to the motion1
	double mdTimeFrequency; // count the time between to fire
	double mdTimeAction; // time between 2 action
	double mdTimeLazy; // time between 2 action
	double mdTimeMotion; // current time during the motion

	BulletItem* mpFoe; // Pointer to the foe (inherit of a BulletItem)
	
public:

// Constructor
	FoeItem(int item);

// Destructor
	~FoeItem();

// Methods

	// Initialize the enemy
	void init();
	// Move the enemy
	void move(void);
	
	// Draw method, to display the Enemy 
	void draw(void);

};
#endif