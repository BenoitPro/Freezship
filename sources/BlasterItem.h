// BlasterItem.h
// Header file 
// 
// BackgroundItem class provides methods to manage and draw a BackgroundItem like
// the background and the snow and the stars
//
// Author : Benoit Marechal

#ifndef BLASTERITEM_H
#define BLASTERITEM_H

/////////////////////////
// Includes
/////////////////////////
#include "BackgroundItem.h"
#include "ItemCollection.h"
#include "ParticuleItem.h"


///////////////////////
// Class definition
////////////////////////
class Blaster: public BackgroundItem {

protected :

	// Collection of small particules
ItemCollection<ParticuleItem>* mpSmallParticules;
	// Collection of animated particules
ItemCollection<ParticuleItem>* mpAnimParticules;

	// Time life of the blast
double mdTimeLife;

public :

	// Constructor
	Blaster(int style);

	// Destructor
	~Blaster();

	// Initialize Methods
	void init();
	
	// Move the object
	void move(void);

	// Draw
	void draw(void);

	// Setters
	void setXCoord(double x);
	void setYCoord(double y);

};

#endif
