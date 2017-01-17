// ParticuleItem.h
// Definition file
// 
// ParticuleItem class provides methods to manage and draw a Particule
// Inherit of a BackgroundItem class
//
// Author : Benoit Marechal


#include "ParticuleItem.h"

// Constructor
ParticuleItem::ParticuleItem(int item):BackgroundItem(item)
{
    miCountFrame=0;
	switch(miItem)
	{
		case PARTICULE2:
			mpAnim = new Animation("explosion1.bmp");
			break;
		case PARTICULE3:
			mpAnim = new Animation("asteroid-exp.bmp");
			break;
	}
}

// Initialise the particule
void ParticuleItem::init()
{
	BackgroundItem::init();

	mdTimeLife = rand()%5;
	miCountFrame=0;

	switch(miItem)
	{
		case PARTICULE2:
			mpAnim->init(mdXCoord,mdYCoord,512,64,8,1);
			mpAnim->center();
			mpAnim->setFPS(20);
			break;  
		case PARTICULE3:
			mpAnim->init(mdXCoord,mdYCoord,22,17,1,1);
			mpAnim->center();
			mpAnim->setLoop(true);
			break;  
	}

}

// Move the particule
void ParticuleItem::move()
{
	BackgroundItem::move();
	
	mdYCoord+=(gravity*frame_time)*miCountFrame;	
	mdTimeLife-=frame_time;

	if(mdTimeLife<=0)
		mbAlive=0;

	miCountFrame++;
}

// Draw the particule
void ParticuleItem::draw(void)
{
	switch(miItem)
	{
	case PARTICULE1:

		pTheDrawEngine->LockBackSurface();

		if(mdXCoord>0 && mdXCoord<SCREENWIDTH && mdYCoord>0 && mdYCoord<SCREENHEIGHT)
		pTheDrawEngine->DrawPoint((int)mdXCoord,(int)mdYCoord,_XRGB(255,255,255));

		pTheDrawEngine->UnlockBackSurface();
		break;
	
	case PARTICULE2:
		BackgroundItem::draw();
		break;
	}
}



