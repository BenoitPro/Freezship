// Blaster.cpp
// Definition file 
// 
// Blaster class provides methods to manage and draw a Blast/Explosion
//
// Author : Benoit Marechal

#include "BlasterItem.h"

// Constructor
Blaster::Blaster(int style):BackgroundItem(style)
{
	mdTimeLife=0;
	switch(miItem)
	{
	case MIDDLEBLAST:
			mpSmallParticules = new ItemCollection<ParticuleItem>(100,PARTICULE1);
			mpAnimParticules = new ItemCollection<ParticuleItem>(30,PARTICULE2);

			mpAnim = new Animation("explosion1.bmp");

	break;
	case SMALLBLAST:
			mpSmallParticules = new ItemCollection<ParticuleItem>(50,PARTICULE1);
			mpAnimParticules = new ItemCollection<ParticuleItem>(1,PARTICULE2);
			mpAnim = new Animation("explosion1.bmp");

	break;
	case ASTEROID1BLAST:
			mpSmallParticules = new ItemCollection<ParticuleItem>(10,PARTICULE1);
			mpAnimParticules = new ItemCollection<ParticuleItem>(5,PARTICULE3);
			mpAnim = new Animation("explosion1.bmp");

	break;

	}
}

// Destructor
Blaster::~Blaster(){
	delete mpSmallParticules;
	delete mpAnimParticules;
}

// Initialize method
void Blaster::init()
{
	mdTimeLife=0;
		
	mpSmallParticules->init();
	mpSmallParticules->setItemsXCoord(mdXCoord);
	mpSmallParticules->setItemsYCoord(mdYCoord);

	mpAnimParticules->init();
	mpAnimParticules->setItemsXCoord(mdXCoord);
	mpAnimParticules->setItemsYCoord(mdYCoord);
	switch(miItem)
	{
	case MIDDLEBLAST:
		
		mpAnim->init(mdXCoord,mdYCoord,512,64,8,1);
		mpSmallParticules->AddItems(199);
		mpAnimParticules->AddItems(49);
		
	break;
	case SMALLBLAST:
		
		mpAnim->init(mdXCoord,mdYCoord,512,64,8,1);
		mpSmallParticules->AddItems(49);
	break;
	
	case ASTEROID1BLAST:
		
		mpAnim->init(mdXCoord,mdYCoord,512,64,8,1);
		mpSmallParticules->AddItems(9);
		mpAnimParticules->AddItems(4);
	break;
	}	
}

// Move the blast
void Blaster::move()
{
	BackgroundItem::move();

	mpSmallParticules->move();
	mpAnimParticules->move();
	mdTimeLife+=(1*frame_time);

	if(mdTimeLife>=3.5)
		mbAlive=false;
}

// Draw the blast
void Blaster::draw()
{
	BackgroundItem::draw();

	mpSmallParticules->draw();
	mpAnimParticules->draw();
}

// Setters
void Blaster::setXCoord(double x){
	mdXCoord=x;}
void Blaster::setYCoord(double y){mdYCoord = y;}
