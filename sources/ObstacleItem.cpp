// ObstacleItem.h
// Definition file 
// 
// ObstacleItem class provides methods to manage and draw a Obstacle
// Inherit of a BackgroundItem class
//
// Author : Benoit Marechal


#include "ObstacleItem.h"

// Constructor
ObstacleItem::ObstacleItem(int item):BackgroundItem(item)
{
	mdLifeAmount=0;
	miDamage=0;

	switch(miItem)
	{
		case OBSTACLE1:
			mpAnim = new Animation("foe4.bmp");
			break;
		case ASTEROID1: 
			mpAnim = new Animation("asteroid.bmp");
			mdLifeAmount = 200;
			miDamage=50;
			break;
		case ASTEROID2:
			mpAnim = new Animation("asteroid-red.bmp");
			mdLifeAmount = 100;
			miDamage = 200;
			break;
		case FLOOR:
			mpAnim = new Animation("floor6.bmp");
			mdLifeAmount=10000;
			break;

			break;
		case FIR:
			mpAnim = new Animation("fir2.bmp");
			mdLifeAmount=10;
			break;

			
	}
}

// Destructor
ObstacleItem::~ObstacleItem()
{
	BackgroundItem::~BackgroundItem();
}

// Methods
void ObstacleItem::init()
{
	BackgroundItem::init();
    
	switch(miItem)
	{
		case OBSTACLE1:
			mpAnim->init(mdXCoord,mdYCoord,1100,346,1,1);
			mdLifeAmount = 200;
			break;
		case ASTEROID1:
			mpAnim->init(mdXCoord,mdYCoord,50,34,1,1);
			mpAnim->setLoop(1);
			mdDirectionAngle=180;
			mdDirectionSpeed=300-(rand()%200);
			mdLifeAmount = 200;
			mdXCoord=SCREENWIDTH-5;
			mdYCoord=rand()%(SCREENHEIGHT-170);
			

			break;
		case ASTEROID2:
			mpAnim->init(mdXCoord,mdYCoord,160,38,4,1);
			mpAnim->setLoop(1);
			mpAnim->setFPS(6);
			mdDirectionAngle=180;
			mdDirectionSpeed=300;
			mdLifeAmount = 100;
			break;
		case FLOOR:
			mpAnim->init(mdXCoord,mdYCoord,1000,121,1,1);
			mpAnim->setLoop(1);
			mdDirectionAngle=180;
			mdDirectionSpeed=0;
			mdLifeAmount = 10000;
			break;
		case FIR:
			mpAnim->init(mdXCoord,mdYCoord,80,140,1,1);
			mpAnim->setLoop(1);
			mdDirectionAngle=180;
			mdDirectionSpeed=0;
			mdLifeAmount = 10;

			break;
	}
}

// Move the Obstacle
void ObstacleItem::move()
{
	if(mdLifeAmount<=0)
		mbAlive=false;
	BackgroundItem::move();
}

// Draw the Obstacle
void ObstacleItem::draw(void)
{
	BackgroundItem::draw();
}

// Getters
double ObstacleItem::getLifeAmount(void){ return mdLifeAmount;}

// Setters
void  ObstacleItem::setLifeAmount(double life){ mdLifeAmount = life;}

