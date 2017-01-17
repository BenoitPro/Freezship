#include "Gamer.h"
#include "ObstacleItem.h"

#include <math.h>

// Constructor
Gamer::Gamer()
{
	miLifeLeft=3;
	mdXCoord = 200;
	mdYCoord = 300;
	mdDeltaX = 0;
	mdDeltaY = 0;

	mdLevel = 1;
	miMotion = 2;
	mdTime = 0.99;
	mdTime2 = 0.27;
	mdTime3 = 0.14;
	
	mdAcceleration = 40;
	mdFriction = 6.0;

	mdAngle = 0;

	mdLife=1000;
	miScore = 0;

	mpAnim = new Animation("vantoo.bmp");
	// x y width height columnnuber linenumber
	mpAnim->init(mdXCoord,mdYCoord,780,1300,6,10);
	mpAnim->setLoop(false);

	mpLife= new Animation("life2.bmp");
	mpLife->init(100,10,2000,50,10,1);
	
	mpLifeLeft= new Animation("heart.bmp");
	mpLifeLeft->init(100,50,360,48,3,1);
	
	
	mdLevel = 2;// TEMPORAIRE *****************
	
	mpBullets = new ItemCollection<BulletItem>(10,BULLET1);
	mpBullets->init();
	mpBullets->setItemsXCoord(mdXCoord);
	mpBullets->setItemsYCoord(mdYCoord);

	mpBullets2 = new ItemCollection<BulletItem>(12,BULLET2);
	mpBullets2->init();
	mpBullets2->setItemsXCoord(mdXCoord);
	mpBullets2->setItemsYCoord(mdYCoord);

	mpBullets3 = new ItemCollection<BulletItem>(12,BULLET2);
	mpBullets3->init();
	mpBullets3->setItemsXCoord(mdXCoord);
	mpBullets3->setItemsYCoord(mdYCoord);

	mpBulletsBomb  = new ItemCollection<BulletItem>(10,BULLET3);
	mpBulletsBomb->init();
    mpBulletsBomb->setItemsXCoord(mdXCoord);
	mpBulletsBomb->setItemsYCoord(mdYCoord);

}

// Move

void Gamer::move(void)
{	
	
	mpBullets->move();
	mpBullets2->move();
	mpBullets3->move();
	mpBulletsBomb->move();
	
	
pTheInputs->SampleKeyboard(); // COMMON

if (pTheInputs->KeyPressed(DIK_M)) {
	mdTime3+=1*frame_time;
	if(mdTime3>0.15)
	{
		miMotion = (miMotion==1) ? 2:1 ; mdAngle=0;
		mdTime3=0;
	}
}


	move(miMotion);

		mdDeltaX -= mdDeltaX * mdFriction * frame_time;
		mdDeltaY -= mdDeltaY * mdFriction * frame_time;

		mdXCoord +=	mdDeltaX;
		mdYCoord +=	mdDeltaY;

	// COMMON
	
		if (mdXCoord+mpAnim->miFrameWidth >= SCREENWIDTH-100)
			mdXCoord = SCREENWIDTH-100-mpAnim->miFrameWidth;

		if (mdXCoord < 0)
			mdXCoord = 0;

		if (mdYCoord >= SCREENHEIGHT)
			mdYCoord = SCREENHEIGHT;

		if (mdYCoord < 0)
			mdYCoord = 0;

}
void Gamer::move(int style)
{

if(style==1) {
	// Input handler

	if (pTheInputs->KeyPressed(DIK_SPACE)) {	
 			mdTime=mdTime+1*frame_time;
		
			if (mdTime>=0.1){

					if(mdLevel==1){
						BulletItem* b = mpBullets->getDeathItem();
						b->setAlive(1);
						gpTheSoundFX->playBullet1();
						b->setXCoord(mdXCoord+115);
						b->setYCoord(mdYCoord+83);
					}
					else if(mdLevel ==2){
						BulletItem* b2 = mpBullets->getDeathItem();
						b2->setAlive(1);
						b2->setXCoord(mdXCoord+115);
						b2->setYCoord(mdYCoord+83);
						b2->setTarget(gpScroll->mpAsteroids->getNearItem(mdXCoord,mdYCoord));
						gpTheSoundFX->playBullet1();
						//b2->setLookForFoe(true);
						//b2->aimTarget();

						b2 = mpBullets2->getDeathItem();
						b2->setAlive(1);
						b2->setDirectionAngle(-5);
						b2->setXCoord(mdXCoord+115);
						b2->setYCoord(mdYCoord+73);

						b2 = mpBullets3->getDeathItem();
						b2->setAlive(1);
						b2->setXCoord(mdXCoord+115);
						b2->setDirectionAngle(5);
						b2->setYCoord(mdYCoord+93);

					
					}
				mdTime=0;
			}
		}


		if (pTheInputs->KeyPressed(DIK_B)){
			
 				mdTime2=mdTime2+1*frame_time;
			if (mdTime2>=0.3){

					BulletItem* b2 = mpBulletsBomb->getDeathItem();
						b2->setAlive(1);
						b2->setXCoord(mdXCoord+115);
						b2->setDirectionAngle(90);
						b2->setCountFrame(0);
						b2->setGravity(true);
						b2->setYCoord(mdYCoord+93);

						mdTime2 = 0;
			}
		}

		if (pTheInputs->KeyPressed(DIK_DOWN))
			mdDeltaY += mdAcceleration * frame_time ;
		if (pTheInputs->KeyPressed(DIK_UP))
			mdDeltaY -= mdAcceleration * frame_time ;
		if (pTheInputs->KeyPressed(DIK_LEFT))
			mdDeltaX -= mdAcceleration * frame_time ;
		if (pTheInputs->KeyPressed(DIK_RIGHT))
		{	mdDeltaX += mdAcceleration * frame_time ;
		gpTheSoundFX->playThrust();
		}

}

// ****************************************
	if(style==2){


		if (pTheInputs->KeyPressed(DIK_SPACE))
	{	
 		mdTime=mdTime+1*frame_time;
	
		if (mdTime>=0.1){
				//BulletItem* b = mpBullets->getDeathItem();
			if(mdLevel==1){
					BulletItem* b = mpBullets->getDeathItem();
					b->setAlive(1);
					b->setXCoord(mdXCoord+115);
					b->setYCoord(mdYCoord+83);gpTheSoundFX->playBullet1();
				}
			else if(mdLevel ==2){
					BulletItem* b = mpBullets2->getDeathItem();
					b->setAlive(1);
					gpTheSoundFX->playBullet1();
					b->setDirectionAngle(mdAngle);
				if(mdAngle<90 || mdAngle>270){
					b->setXCoord(mdXCoord+65+cos( (360-(mdAngle+20)) * 3.14159/180)*60);
					b->setYCoord(mdYCoord+65+sin( -(360-(mdAngle+20)) * 3.14159/180)*60);
				}
				else {
					b->setXCoord(mdXCoord+65+cos( (360-(mdAngle-20)) * 3.14159/180)*60);
					b->setYCoord(mdYCoord+65+sin( -(360-(mdAngle-20)) * 3.14159/180)*60);
				}
		

			}
			mdTime=0;
		}
	}

	if (pTheInputs->KeyPressed(DIK_RIGHT))
	{
	mdAngle += 3;
	if (mdAngle>=360)
		mdAngle = 0;
	}
			
	if (pTheInputs->KeyPressed(DIK_LEFT)){
        mdAngle -= 3;
		if(mdAngle<=  0 )
			mdAngle=359;
	}
		
	if (pTheInputs->KeyPressed(DIK_DOWN))
	{		mdDeltaX -= mdAcceleration *cos((mdAngle)*3.14159/180)* frame_time ;
			mdDeltaY -= mdAcceleration *sin((mdAngle)*3.14159/180) * frame_time ;	
	}
	if (pTheInputs->KeyPressed(DIK_UP))
	{
		gpTheSoundFX->playThrust();
		mdDeltaX += mdAcceleration *cos((mdAngle)*3.14159/180)* frame_time ;
		mdDeltaY += mdAcceleration *sin((mdAngle)*3.14159/180) * frame_time ;	
	}
	
	}

	

	msBoundRect.top=mdYCoord+40;
	msBoundRect.bottom=msBoundRect.top+50;
	msBoundRect.left=mdXCoord+20;
	msBoundRect.right=msBoundRect.left+100;



}

// Draw
void Gamer::draw(void)
{
	
	mpBullets->draw();
	mpBullets2->draw();
	mpBullets3->draw();
	mpBulletsBomb->draw();
	// update animation
	mpAnim->setXCoord(mdXCoord);
	mpAnim->setYCoord(mdYCoord);

	// draw animation
	mpAnim->draw((int) ((mdAngle)  / 6));

	// Draw information about the Gamer
	// Life amount of the gamer
	if(mdLife>=10)
	{mpLife->draw(10-(int) mdLife/100);
	// Display the life of the gamer
	pTheDrawEngine->WriteText(10,25,"Life amount: ");
	pTheDrawEngine->WriteDouble(mdLife,150,25);
	}else{mdLife=0;}

	// Nb of life left
	pTheDrawEngine->WriteText(10,70,"Life left: ");
	mpLifeLeft->draw(miLifeLeft-1);

	//

		


}


// Return the bound Rect
RECT Gamer::getBoundRect(void){

	if(mdAngle==0)
	{
		return msBoundRect;
	}
	else
	return mpAnim->getBoundRect();
}

// Setters
void Gamer::setLevel(int level){ mdLevel= level;}
