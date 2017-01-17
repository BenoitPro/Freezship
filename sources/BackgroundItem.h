// BackgroundItem.h
// Header file 
// 
// BackgroundItem class provides methods to manage and draw a BackgroundItem like
// the background and the snow and the stars
//
// Author : Benoit Marechal

#ifndef BACKGROUNDITEM_H
#define BACKGROUNDITEM_H

/////////////////////////
// Includes
/////////////////////////
#include <math.h>
#include <time.h>
#include "Animation.h"
#include "Consts.h"
#include "gamecode.h"

/////////////////////////
// Extern variables
/////////////////////////
extern double frame_time;
extern double gravity;
extern MyDrawEngine* pTheDrawEngine;
extern double scroll_speed;

///////////////////////
// Class definition
////////////////////////
class BackgroundItem {

public :

	double mdXCoord;	// X coord
	double mdYCoord;	// Y coord

	double mdDirectionAngle; // direction angle of the object
    double mdDirectionSpeed; // Speed of the object
	bool mbAlive; // to know if it's alive
	int miItem; // kind of item, to init with the adapted parameters (see Consts.h)

    Animation* mpAnim;	// Pointer to the Animation object, which manage the picture
						// of the Item

public :

	// Constructor
	BackgroundItem(int item);
	// Destructor
	~BackgroundItem();

	// Initialize method
	void init(); // ramdom values (i is the seed of the srand function)

	// Move the object
	void move(void);

	// Draw the object
	void draw(void);

	// getter
	bool isAlive(void);
	double getXCoord(void);
	double getYCoord(void);
	RECT getBoundRect(void);

	// setters
	void setAlive(bool b);
	void setXCoord(double xcoord);
	void setYCoord(double ycoord);
	void setDirectionAngle(double angle);



};
#endif