// BackgroundItem.h
// Definition file 
// 
// BackgroundItem class provides methods to manage and draw a BackgroundItem like
// the background and the snow and the stars
//
// Author : Benoit Marechal

/////////////////////////
// Includes
/////////////////////////
#include "BackgroundItem.h"
#include "gamecode.h" 


///////////////////////
// Definition
//////////////////////
BackgroundItem::BackgroundItem(int item)
{
	// Init the variables
	mdXCoord=0;
	mdYCoord=0;
	mdDirectionAngle=0;
	mdDirectionSpeed=0;
	mbAlive=0;
	mpAnim=NULL;
	miItem=item;

	switch(miItem)
	{
	case STAR1 :
		mpAnim = new Animation("star.bmp");
		break;
	case STAR2 :
		mpAnim = new Animation("star2.bmp");
		break;
	case STAR3 :
		mpAnim = new Animation("star3.bmp");
		break;
	case BACKGROUND_FIRE :
		mpAnim = new Animation("background.bmp");
		break;
	case BACKGROUND_START :
		mpAnim = new Animation("start.bmp");
		break;
	case BACKGROUND_GAMEOVER :
		mpAnim = new Animation("Gameover.bmp");
		break;
	}

}

// destructor
BackgroundItem::~BackgroundItem()
{}


// Initialize the object 
void BackgroundItem::init()
{

	mdXCoord = rand()%SCREENWIDTH;
	mdYCoord = rand()%SCREENHEIGHT;
	mdDirectionAngle = rand()%360;

	mdDirectionSpeed=rand()%500;


	mbAlive=0;
	switch(miItem){
		case STAR1:
			mpAnim->init(mdXCoord,mdYCoord,10,10,1,1);
			mdDirectionAngle=130;
			mdDirectionSpeed=120;
			mpAnim->setLoop(true);
		break;
		case STAR2:
			mpAnim->init(mdXCoord,mdYCoord,50,10,1,1);
			mdDirectionAngle=180;
			mdDirectionSpeed=1000;
			mpAnim->setLoop(true);
		break;
		case STAR3:
			mpAnim->init(mdXCoord,mdYCoord,33,28,1,1);
			mdDirectionAngle=180;
			mdDirectionSpeed=700;
			mpAnim->setLoop(true);
		break;
		case BACKGROUND_FIRE:
			mpAnim->init(0,0,2048,768,1,1);
			mdDirectionAngle=180;
			mdDirectionSpeed=0;
			mpAnim->setLoop(true);
		break;
		case BACKGROUND_START:
			mpAnim->init(0,0,1024,768,1,1);
			mdDirectionAngle=180;
			mdDirectionSpeed=0;
			mpAnim->setLoop(true);
		break;
		case BACKGROUND_GAMEOVER:
			mpAnim->init(0,0,1024,1000,1,1);
			mdDirectionAngle=270;
			mdDirectionSpeed=0;
			mpAnim->setLoop(true);
		break;
	}
}


// Move the object
void BackgroundItem::move()
{
	mdXCoord +=  mdDirectionSpeed * cos(mdDirectionAngle*3.14159/180) * frame_time ;
	mdYCoord +=  mdDirectionSpeed * sin(mdDirectionAngle*3.14159/180) * frame_time ;

	if(miItem!=BACKGROUND_GAMEOVER &&miItem!=BULLET1 &&miItem!=BULLET2)
	mdXCoord -=scroll_speed*frame_time;

	if(miItem<200 && miItem!=BOSS_FOE){
	if(mpAnim!=NULL)
	{
		if ((mdXCoord+mpAnim->miFrameWidth)<-100 ||
		mdYCoord+mpAnim->miFrameHeight<-100 ||
		mdXCoord>SCREENWIDTH+100 ||
		mdYCoord>SCREENHEIGHT+100)
			mbAlive = false;
	}
	else
	{ if ((mdXCoord)<-200 || mdYCoord<-100 || mdYCoord>SCREENHEIGHT+80)
		mbAlive = false;
	}}
}

// Draw the object
void BackgroundItem::draw()
{
	mpAnim->setXCoord(mdXCoord);
	mpAnim->setYCoord(mdYCoord);
	mpAnim->draw();
}


// Setters
void BackgroundItem::setXCoord(double xcoord)
{ mdXCoord = xcoord;}
void BackgroundItem::setYCoord(double ycoord)
{mdYCoord = ycoord;}
void BackgroundItem::setAlive(bool b)
{mbAlive = b;}
void BackgroundItem::setDirectionAngle(double angle){mdDirectionAngle = angle;}


// Getters
bool BackgroundItem::isAlive() {return mbAlive;}
double BackgroundItem::getXCoord() { return mdXCoord;}
double BackgroundItem::getYCoord() { return mdYCoord;}
RECT BackgroundItem::getBoundRect() { return mpAnim->getBoundRect();}
