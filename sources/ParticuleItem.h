// ParticuleItem.h
// Header file 
// 
// ParticuleItem class provides methods to manage and draw a Particule
// Inherit of a BackgroundItem class
//
// Author : Benoit Marechal

#ifndef PARTICULEITEM_H
#define PARTICULEITEM_H

/////////////////////////
// Includes
/////////////////////////
#include "BackgroundItem.h"

////////////////////////
// Class definition
////////////////////////
class ParticuleItem : public BackgroundItem
{
protected :

	double mdTimeLife; // Particule's time life
	int miCountFrame; // Count the numbers of frame

public :

	// Constructor
	ParticuleItem(int item);

	// Initialize (with random values)
	void init(); 

	// Move the particule
	void move(void);

	// Draw the particule
	void draw(void);
};
#endif