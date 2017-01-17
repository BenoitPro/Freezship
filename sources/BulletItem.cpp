// BulletItem.cpp
//
// BulletItem class provides methods to manage and draw a bullet
// Inherit of the ObstacleItem class
//
// Author : Benoit Marechal

#include "BulletItem.h"

// Constructor
BulletItem::BulletItem(int item):ObstacleItem(item)
{
	mdLifeAmount=0;
	miCountFrame=0;
	mbGravityOn=false;
	switch(miItem)
	{
		case BULLET1:
			mpAnim = new Animation("bullet1.bmp");
			miDamage=100;

			break;
		case BULLET2:
			mpAnim = new Animation("bullet3.bmp");
			miDamage=50;
			break;
		case BULLET3:
			mpAnim = new Animation("bomb1.bmp");
			miDamage=2000;
			break;
		case BULLET4:
			mpAnim = new Animation("bullet5.bmp");
			miDamage=50;
			break;
		case BULLET5:
			mpAnim = new Animation("bullet7.bmp");
			miDamage=200;
			break;
	}
}

// Destructor
BulletItem::~BulletItem()
{
	ObstacleItem::~ObstacleItem();
}

// Initialize method
void BulletItem::init()
{
	ObstacleItem::init();
	miCountFrame=0;
	mbIsLookForFoe=false;
	mpTarget=NULL;
	switch(miItem)
	{
		case BULLET1:
			mdDirectionSpeed=900;
			mdDirectionAngle=0;
			mpAnim->init(mdXCoord,mdYCoord,50,8,1,1);
			mpAnim->setLoop(true);
			mdLifeAmount=100;
			break;
		case BULLET2:
			mdDirectionSpeed=900;
			mdDirectionAngle=0;
			mpAnim->init(mdXCoord,mdYCoord,100,34,3,1);
			mpAnim->setFPS(30);
			mpAnim->setLoop(true);
			mdLifeAmount=100;
			break;
		case BULLET3:
			mdDirectionSpeed=230;
			mdDirectionAngle=90;
			mpAnim->init(mdXCoord,mdYCoord,60,22,1,1);
			mpAnim->setLoop(true);
			mdLifeAmount=100;
			break;
		case BULLET4:
			mdDirectionSpeed=600;
			mdDirectionAngle=90;
			mpAnim->init(mdXCoord,mdYCoord,30,32,1,1);
			mpAnim->setLoop(true);
			mdLifeAmount=100;
			break;
		case BULLET5:
			mdDirectionSpeed=900;
			mdDirectionAngle=90;
			mpAnim->init(mdXCoord,mdYCoord,20,20,1,1);
			mpAnim->setLoop(true);
			mdLifeAmount=100;
			break;
	}
}

// Move the bullet
void BulletItem::move()
{

	if (mbIsLookForFoe &&mdTime>0.2) 
	{	aimTarget();
	mdTime=0;
	}

	ObstacleItem::move();

	if (mbGravityOn)
		mdYCoord+=gravity*miCountFrame*frame_time;

	miCountFrame++;

	mdTime+=(1*frame_time);
}

// Setters
void BulletItem::setCountFrame(int cf)
{	miCountFrame = cf;}
void BulletItem::setGravity(bool g)
{	mbGravityOn=g ;}
void BulletItem::setTarget(BackgroundItem* b)
{	mpTarget = b;}
void BulletItem::setAlive(bool b)
{
	ObstacleItem::setAlive(b);
	if(!mbAlive)
		mpTarget=NULL;	
}

// To aim the target
void BulletItem::aimTarget()
{
	if(mpTarget!=NULL){
		double dX = mpTarget->getXCoord() - mdXCoord;
		double dY = mpTarget->getYCoord() - mdYCoord;

		double theta = 0;
		
		if(dX>0)
		theta = ( atan(dY/dX) * 180 / 3.14159);
		else
		theta = ( atan(dY/dX) * 180 / 3.14159)+180;

		mdDirectionAngle=theta;
	}
}

// Draw the bullet
void BulletItem::draw(void)
{	ObstacleItem::draw(); }