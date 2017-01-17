// class Animation
#include "Animation.h"
#include <time.h>


// extern MyDrawEngine* pTheDrawEngine;
extern double frame_time;

// Constructor
Animation::Animation(char * f)
{
	mdCurrentFrame = 0;

	mbLoop = false;
	mbIsFinished = false;
	miWidth = 0;
	miHeight = 0;
	miFrameHeight = 0;
	miFrameWidth = 0;

	miColumnsNumber = 1;
	miLinesNumber = 1;
	
	mdXCoord=0;
	mdYCoord=0;

	miFPS = 12;

	// Load the picture 
	strcpy(msFilename,f);	
	mpPicture = new MyPicture();
	mpPicture->LoadBitmap(msFilename);
}

Animation::~Animation()
{
	delete mpPicture;
}

// Init
void Animation::init(double xcoord,double ycoord,int width,int height,int columnsnumber,int linesnumber)
{
mbIsFinished=false;
	mbLoop = false;
	miWidth = width;
	miHeight = height;
	mdCurrentFrame=0;
	miColumnsNumber = columnsnumber;
	miLinesNumber = linesnumber;
	
	miFrameHeight =(int) miHeight/miLinesNumber;
	miFrameWidth =(int) miWidth/miColumnsNumber;

	mdXCoord=xcoord;
	mdYCoord=ycoord;
}


// Draw method
void Animation::draw(void)
{
	mdCurrentFrame+=miFPS*frame_time;
		if(mdCurrentFrame >= miColumnsNumber*miLinesNumber)
		{
			mdCurrentFrame = 0;
			if (mbLoop==false) 
				mbIsFinished = true;
		}

		if(!mbIsFinished)
		draw((int)mdCurrentFrame);
		
}

void Animation::draw(int frame){


	int line =(int) frame / miColumnsNumber;
	int column =(int) frame - (line*miColumnsNumber);

	mSourceRect.top =  line*miFrameHeight;
	mSourceRect.bottom = mSourceRect.top + miFrameHeight;

	mSourceRect.left = column*miFrameWidth;
	mSourceRect.right = mSourceRect.left + miFrameWidth;

	mDestRect.top = (int) mdYCoord;
	mDestRect.bottom =(int) mDestRect.top + miFrameHeight;
	mDestRect.left = (int)mdXCoord;
	mDestRect.right =(int) mDestRect.left + miFrameWidth;

	pTheDrawEngine->Blit(mDestRect, mSourceRect, mpPicture);


}

// GETTERS
//double Animation::getX(void){ return x; }
//double Animation::getY(void){ return y; }

RECT Animation::getDestRect(void){ return mDestRect; }
RECT Animation::getBoundRect(void){ return mDestRect; }


// SETTERS
void Animation::setXCoord(double xcoord) { mdXCoord = xcoord;}
void Animation::setYCoord(double ycoord) { mdYCoord = ycoord;}
void Animation::setLoop(bool loop){mbLoop = loop;}
void Animation::setFPS(int fps){	miFPS = fps;}

void Animation::center(void){ 
	mdXCoord = mdXCoord -miFrameWidth/2;
	mdYCoord = mdYCoord -miFrameHeight/2;
}
