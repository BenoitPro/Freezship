
// Animation.h
// Header file 
// 
// Animation class provides methods to manage and draw a picture
// or a sprite

#ifndef ANIMATION_H
#define ANIMATION_H

#include "mydrawengine.h"


extern MyDrawEngine* pTheDrawEngine;


class Animation {

public : 

	MyPicture* mpPicture; // Pointer to the picture
	double mdXCoord;	// X coord
	double mdYCoord;	// Y coord

	char msFilename[255]; // Path of the file on the hard drive wich hold the picture

	int miWidth; // width of the picture
	int miHeight;// height of the picture

	int miFrameWidth; // width of a frame in the picture (for sprite)
	int miFrameHeight;// height of a frame in the picture (for sprite)

	int miColumnsNumber;// number of columns in the sprite
	int miLinesNumber; // number of lines in the sprite

	bool mbLoop; // true if the sprite must be played in loop
	bool mbIsFinished; // boolean to know if the sprite if finished

	double mdCurrentFrame; // numbers of the current frame drawed

	int miFPS; // frame per second

	// Source rect in the picture file
	RECT mSourceRect ;

	// Destination on the screen
	RECT mDestRect ;


public :

	// Constructor
	Animation(char * filename);
	// Destructor
	~Animation();
	
	// Methods
	void init(double x,double y,int width,int height,int columnsnumber,int linesnumber);

	void draw(void); // display all frames
	void draw(int frame); // display the specified frame

	// Getters
//	double getXCoord(void);
//	double getYCoord(void);

	RECT getDestRect(void);
	RECT getBoundRect(void);

	// Setters
	void setXCoord(double xcoord);
	void setYCoord(double ycoord);
	void setFPS(int fps);

	void center(void);

	void setLoop(bool loop);

};
#endif