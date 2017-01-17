// FoeItem.cpp
//
// FoeItem class provides methods to manage and draw a Foe
// Inherit of the BulletItem class
//
// Author : Benoit Marechal

#include "FoeItem.h"

// Constructeur
FoeItem::FoeItem(int item):BulletItem(item)
{
	// Init value
	mdFireTime=0;
	mdLazyTime=0;
	mdTimeFrequency = 0;
	mdFireFrequency = 1;
	mdTimeMotion= 0;
	mdTimeAction = 0;
	mdTimeLazy =0;

	// switch kind of enemy (defined ins Const.h file)
	switch(miItem)
	{
		case MINI_FOE:
			mpAnim = new Animation("foe1.bmp");
			mpBullets = new ItemCollection<BulletItem>(4,BULLET4);
			mpBullets->init();
			mdLifeAmount=200;

		break;
		case SMALL_FOE:
			mpAnim = new Animation("foe2.bmp");
			mpBullets = new ItemCollection<BulletItem>(5,BULLET5);
			mpBullets->init();
			mdLifeAmount=400;
		break;

		
		case BOSS_FOE:
			mpAnim = new Animation("foe7.bmp");
			mpBullets = new ItemCollection<BulletItem>(20,BULLET1);
			mpBullets->init();
			mdLifeAmount=20000;
		break;
	}
		
}

// Destructor
FoeItem::~FoeItem(){
	delete mpAnim;
	delete mpBullets;
}

// Init method to initialize the parameters of the foe
void FoeItem::init()
{
	BulletItem::init();
	switch(miItem)
	{
		case MINI_FOE:
			mpAnim->init(mdXCoord,mdYCoord,100,72,1,1);
			mdFireTime = 0.2;
			mdLazyTime = 2;
			mdFireFrequency = 0.1;
			mdMotion1Time = 3;
			mdMotion2Time = 1.5;
			mdDirectionSpeed = 70;
			mdDirectionAngle = 180; 
			mpAnim->setLoop(true);
			mdLifeAmount=200;
			miDamage=50;

			mdXCoord=SCREENWIDTH-5;
			mdYCoord=rand()%(SCREENHEIGHT-170);
		break;	

		case SMALL_FOE:
			mpAnim->init(mdXCoord,mdYCoord,200,139,1,1);
			mdFireTime = 1;
			mdLazyTime = 1;
			mdMotion1Time = 1;
			mdMotion2Time = 2;
			mdDirectionSpeed = 50;
			mdDirectionAngle = 180; 
			mdLifeAmount=100;
			mdFireFrequency = 0.2;
			mpAnim->setLoop(true);
			miDamage=100;
			mdXCoord=SCREENWIDTH-5;
			mdYCoord=rand()%(SCREENHEIGHT-170);
			mdLifeAmount=400;
		break;	

		case BOSS_FOE:
			mpAnim->init(mdXCoord,mdYCoord,800,270,1,1);
			mdFireTime = 5;
			mdLazyTime = 5;
			mdDirectionSpeed = 0;
			mdDirectionAngle = 180; 
			mdLifeAmount=100;
			mdFireFrequency = 0.1;
			mpAnim->setLoop(true);
			mdLifeAmount=3000;
		break;	
	}	
}

// Move the enemy
void FoeItem::move()
{
	if(mbAlive){
		mdTimeMotion=mdTimeMotion+1*frame_time;
		if(mdTimeMotion<mdMotion1Time)
		{
			mdDirectionAngle = 190;
			mdDirectionSpeed = 200;
		}
		else if(mdTimeMotion<(mdMotion1Time+mdMotion2Time))
		{
			mdDirectionAngle = 170;
				mdDirectionSpeed=400;
		}
		else
			mdTimeMotion=0;


	BulletItem::move();

	mdTimeFrequency=mdTimeFrequency+1*frame_time;
	mdTimeAction = mdTimeAction+1*frame_time;

	if(mdTimeAction<=mdFireTime) {
		if (mdTimeFrequency>=mdFireFrequency){
			
			BulletItem* b = mpBullets->getDeathItem();
			b->setAlive(1);
			b->setXCoord(mdXCoord);
			b->setYCoord(mdYCoord+30);
			b->setDirectionAngle(180);
		
			mdTimeFrequency=0;
			}
		}
		else {
			mdTimeLazy=mdTimeLazy+1*frame_time;
			if(mdTimeLazy>=mdLazyTime)
			{	mdTimeAction = 0;
				mdTimeLazy = 0;
			}
		}
		mpBullets->move();
	}
}

// Draw method, to display the Enemy 
void FoeItem::draw()
{
	mpBullets->draw();
	mpAnim->setXCoord(mdXCoord);
	mpAnim->setYCoord(mdYCoord);
	mpAnim->draw();
}


