// gamecode.cpp
// project : MyGame
// Author : Benoit Marechal
//
// This file define the main functions to manage and display the game.


// Includes

#include "gamecode.h"
#include "mydrawengine.h"
#include "mypicture.h"
#include "mysoundengine.h"
#include "mysound.h"
#include "myinputs.h"
#include <time.h>
#include "gametimer.h" 
#include "errorlogger.h"
#include <math.h>
  
// My Consts
#include "Consts.h"

// My class
#include "Animation.h" 
#include "Gamer.h" 
#include "SoundFX.h"
#include "BackgroundItem.h"
#include "ParticuleItem.h"
#include "ItemCollection.h"
#include "ObstacleItem.h"
#include "BlasterItem.h"
#include "BulletItem.h"
#include "FoeItem.h"
#include "Scroller.h"


////////////////////////////////////
// Global Instanses 
////////////////////////////////////

// Direct X instanse
MyDrawEngine* pTheDrawEngine;	// A pointer to the drawing engine
MySoundEngine* pTheSoundEngine;	// A pointer to the sound engine
MyInputs* pTheInputs;			// A pointer to the inputs
GameTimer* gpTheGameTimer;		// A pointer to the game timer

// Gamer
Gamer* gpTheGamer ;				//  A pointer to the gamer

// Blast Collections
ItemCollection<Blaster>* gpSmallBlast; // Small Blast collection
ItemCollection<Blaster>* gpMiddleBlast; // Middle Blast collection
ItemCollection<Blaster>* gpAsteroid1Blast; // Asteroid 1 Blast collection

// Backgrounds
BackgroundItem* gpBackgroundFire;	// Background during the game
BackgroundItem* gpBackgroundStart;  // Start Background
BackgroundItem* gpBackgroundGameOver; // Game Over background

// Sounds
SoundFX* gpTheSoundFX; // Handle the sounds effects

// The Scroller class manage all the game entities and the scrolling
Scroller* gpScroll;

///////////////////////////////////
// Globals variables
//////////////////////////////////

double frame_time=0.002;		// init the frame times
double gravity=9;				// gravity fixed to 9 pixels per second 
double scroll_speed;			// hold the scroll speed
double current_position=0;		// hold the position in the game
double lastTimeCollision=0;		// to fix a bug with collisions

// to know if the Start background must be drawed
 bool gbStart=true;
 //  to know if the Game over background must be drawed
 bool gbGameOver=false;

 // The progress Bars
RECT gsProgress_background;
RECT gsCurrent_progress;

/////////////////////////
// Extern variables
////////////////////////
extern HWND gHwnd;				// A handle to the main window application (created and declared in wincode.cpp).
extern HINSTANCE g_hinstance;	// A handle to the program instance

 ///////////////////////////////////
// Globals Functions 
 ///////////////////////////////////

// Play a small blast (with a sound) to the specified position
int playSmallBlast(double x,double y)
{
		Blaster * b = gpSmallBlast->getDeathItem();
		b->setXCoord(x);
		b->setYCoord(y); 
		b->init(); 
		b->setAlive(1); 
		
		gpTheSoundFX->playSmallExplosion();
	return (SUCCESS);
}
// Play a Asteroid 1 blast (with a sound) to the specified position
int playAsteroid1Blast(double x,double y)
{
		Blaster * b = gpAsteroid1Blast->getDeathItem();
		b->setXCoord(x);
		b->setYCoord(y); 
		b->init(); 
		b->setAlive(1); 
		
		gpTheSoundFX->playSmallExplosion();
	return (SUCCESS);
}

// Play a middle blast (with a sound) to the specified position
int playMiddleBlast(double x,double y)
{
		Blaster * b = gpMiddleBlast->getDeathItem();
		b->setXCoord(x);
		b->setYCoord(y); 
		b->init(); 
		b->setAlive(1); 
	
		gpTheSoundFX->playMiddleExplosion();
	return (SUCCESS);
}

// Check for collisions between 2 rect
bool Collided ( RECT  rectA, RECT rectB) {
	if (rectA.top >	rectB.bottom ||
		rectA.bottom <	rectB.top ||
		rectA.left >	rectB.right ||
		rectA.right <	rectB.left )
	{
	return false;
	}
	return true;
}


// Check Gamer <-> Obstacles Collection and display an explosion
int checkGamerWithObstacles(ItemCollection<ObstacleItem>* ic)
{
	for(int i=0;i<ic->miAmount;i++)
	{
		ObstacleItem* o = ic->getItem(i);
		if(o->isAlive())
		{
			if(Collided(o->getBoundRect(),
						gpTheGamer->getBoundRect()))
			{
				if(o->miItem!=FIR)
				{
				o->setAlive(false);
				
				gpTheGamer->mdLife-=o->miDamage;
				if(gpTheGamer->mdDeltaX<=0)
				gpTheGamer->mdDeltaX-=8;
				else{
				gpTheGamer->mdDeltaX*=-1;
				gpTheGamer->mdDeltaY*=-1;
				}
				
					playMiddleBlast(o->getXCoord(),o->getYCoord());	
				}
				else
				{
				
				if(lastTimeCollision>0.1)
					{	
						gpTheGamer->mdLife-=10;
							if(gpTheGamer->mdDeltaX<10 && gpTheGamer->mdDeltaX>-10)
								gpTheGamer->mdDeltaX*=-2;
							if(gpTheGamer->mdDeltaY<10 && gpTheGamer->mdDeltaY>-10)
								gpTheGamer->mdDeltaY*=-2;
						lastTimeCollision=0;
					}
				else
				gpTheGamer->mdYCoord-=10;
				}
			}
		}
	}
	return (SUCCESS);
}

// Check a bullet collection with a foe collection and display an explosion
int checkBulletsWithFoes(ItemCollection<BulletItem>* bullets,ItemCollection<FoeItem>* ic)
{
	// For all the gamer bullets
	for(int i=0;i<bullets->miAmount;i++)
	{
		BulletItem* bull = bullets->getItem(i);
		if (bull->isAlive())
		{
			for(int i=0;i<ic->miAmount;i++)
			{
				FoeItem* f = ic->getItem(i);

				if(f->isAlive())
				{
					// Check Gamer Bullet <-> Foe Collision
					if(Collided(bull->getBoundRect(),
						f->getBoundRect()))
					{
						// Bullet
						bull->setAlive(false);
						// Foe
						
						f->setLifeAmount(f->getLifeAmount()-bull->miDamage);
						// Bullet Explosion
						playSmallBlast(
							bull->getXCoord()+(bull->mpAnim->miFrameWidth),
							bull->getYCoord()+(bull->mpAnim->miFrameHeight));	

						if(f->getLifeAmount()<=0)
						{
							f->setAlive(false);
							gpTheGamer->miScore+=(int)f->miDamage;
							playMiddleBlast(f->getXCoord()+(f->mpAnim->miFrameWidth/2),f->getYCoord()+(f->mpAnim->miFrameHeight/2));
						}
					}					
				}
			}
		}
	}
	return (SUCCESS);
}

// Check a bullet collection with an Obstacle collection and display an explosion
int checkBulletsWithObstables(ItemCollection<BulletItem>* bullets,ItemCollection<ObstacleItem>* ic)
{
	// For all the gamer bullets
	for(int i=0;i<bullets->miAmount;i++)
	{
		BulletItem* bull = bullets->getItem(i);
		if (bull->isAlive())
		{
			for(int i=0;i<ic->miAmount;i++)
			{
				ObstacleItem* f = ic->getItem(i);

				if(f->isAlive())
				{
					// Check Gamer Bullet <-> Foe Collision
					if(Collided(bull->getBoundRect(),
						f->getBoundRect()))
					{
						// Bullet
						bull->setAlive(false);

						// Foe
						if(f->miItem!=FIR)
						f->setLifeAmount(f->getLifeAmount()-bull->miDamage);
						// Bullet Explosion

						playSmallBlast(
							bull->getXCoord()+(bull->mpAnim->miFrameWidth),
							bull->getYCoord()+(bull->mpAnim->miFrameHeight));	

						if(f->getLifeAmount()<=0  && f->miItem!=FIR)
						{
							f->setAlive(false);
							
							gpTheGamer->miScore+=(int)f->miDamage;
							if(f->miItem==ASTEROID1)
								playAsteroid1Blast(f->getXCoord(),f->getYCoord());
							else
								playMiddleBlast(f->getXCoord()+(f->mpAnim->miFrameWidth/2),f->getYCoord()+(f->mpAnim->miFrameHeight/2));
						}
					}
				}
			}
		}
	}
	return (SUCCESS);
}


// Check an Obstacle collection with the static obstacles and display an explosion
int checkObstacleStaticWithObstable(ItemCollection<ObstacleItem>* ic)
{
	for(int i=0;i<ic->miAmount;i++)
	{
		ObstacleItem* o = ic->mpTheItems[i];
		
		if(o->isAlive())
		{
			for(int j=0;j<gpScroll->mpFir->miAmount ; j++)
			{
				ObstacleItem* f = gpScroll->mpFir->mpTheItems[j];
				if(f->isAlive())
				{
					if( Collided(f->getBoundRect(),o->getBoundRect()) )
					{	o->setAlive(false);
					playMiddleBlast(o->getXCoord(),o->getYCoord());
					}
				}
			}
			for(int j=0;j<gpScroll->mpFloor->miAmount ; j++)
			{
				ObstacleItem* f = gpScroll->mpFloor->mpTheItems[j];
				if(f->isAlive())
				{
					if( Collided(f->getBoundRect(),o->getBoundRect()) )
					{	o->setAlive(false);
					playMiddleBlast(o->getXCoord(),o->getYCoord());
					}
				}
			}
		}
	}
	return (SUCCESS);
}

// Check a Bullets collection with the final Boss
int checkBulletsWithBoss(ItemCollection<BulletItem>* ic)
{
	if(gpScroll->mpBossFoe->isAlive())
	for(int i=0;i<ic->miAmount;i++)
	{
		BulletItem* bull = ic->getItem(i);
		if(bull->isAlive())
		{
			if( Collided(bull->getBoundRect(),gpScroll->mpBossFoe->getBoundRect()))
			{
				bull->setAlive(false);
				playSmallBlast(
							bull->getXCoord()+(bull->mpAnim->miFrameWidth),
							bull->getYCoord()+(bull->mpAnim->miFrameHeight));	

				gpScroll->mpBossFoe->setLifeAmount(gpScroll->mpBossFoe->getLifeAmount()-bull->miDamage);

				if(gpScroll->mpBossFoe->getLifeAmount()<=0)
				{
					gpScroll->mpBossFoe->setAlive(false);
					playMiddleBlast(gpScroll->mpBossFoe->mdXCoord+20,
						gpScroll->mpBossFoe->mdYCoord+20);
					playMiddleBlast(gpScroll->mpBossFoe->mdXCoord+150,
						gpScroll->mpBossFoe->mdYCoord+50);
					playMiddleBlast(gpScroll->mpBossFoe->mdXCoord+10,
						gpScroll->mpBossFoe->mdYCoord+10);
					gbGameOver=true; // set the end of the Game
				}
			}
		}
	}
	return (SUCCESS);
}

// The process all the game collisions 
int processCollision()
{
	
	// Collision with the boss
	checkBulletsWithBoss(gpTheGamer->mpBullets);
	checkBulletsWithBoss(gpTheGamer->mpBullets2);
	checkBulletsWithBoss(gpTheGamer->mpBullets3);
	checkBulletsWithBoss(gpTheGamer->mpBulletsBomb);
	checkGamerWithObstacles((ItemCollection<ObstacleItem>*)gpScroll->mpBossFoe->mpBullets);

	// Check the collision between the Gamer and the floor
	RECT floor;
	floor.top = 680;
	floor.bottom = 768 ;
	floor.left =0;
	floor.right =1024;
	if ( Collided(gpTheGamer->getBoundRect(),floor))
	{
		gpTheGamer->mdYCoord-=10;
		gpTheGamer->mdDeltaY*=-1;
		gpTheGamer->mdLife-=20;
	}

	// Check the static obstable with the moving obstacle
	checkObstacleStaticWithObstable(gpScroll->mpAsteroids);
	checkObstacleStaticWithObstable(gpScroll->mpAsteroids2);
	checkObstacleStaticWithObstable((ItemCollection<ObstacleItem>*)gpScroll->mpMiniFoe);
	checkObstacleStaticWithObstable((ItemCollection<ObstacleItem>*)gpScroll->mpSmallFoe);

	// Check the Gamer with 
	checkGamerWithObstacles(gpScroll->mpAsteroids);
	checkGamerWithObstacles(gpScroll->mpFir);
	checkGamerWithObstacles(gpScroll->mpAsteroids2);
	checkGamerWithObstacles((ItemCollection<ObstacleItem>*)gpScroll->mpMiniFoe);
	checkGamerWithObstacles((ItemCollection<ObstacleItem>*)gpScroll->mpSmallFoe);



	// Check Gamer with the Bullets of each Enemy
	for(int i=0;i<gpScroll->mpMiniFoe->miAmount;i++)
	{checkGamerWithObstacles((ItemCollection<ObstacleItem>*)gpScroll->mpMiniFoe->getItem(i)->mpBullets);	}
	for(int i=0;i<gpScroll->mpSmallFoe->miAmount;i++)
	{checkGamerWithObstacles((ItemCollection<ObstacleItem>*)gpScroll->mpSmallFoe->getItem(i)->mpBullets);	}


	// CHeck the Gamer's  Bullets with the enemies
	checkBulletsWithFoes(gpTheGamer->mpBullets, gpScroll->mpMiniFoe);
	checkBulletsWithFoes(gpTheGamer->mpBullets2, gpScroll->mpMiniFoe);
	checkBulletsWithFoes(gpTheGamer->mpBullets3, gpScroll->mpMiniFoe);
	checkBulletsWithFoes(gpTheGamer->mpBulletsBomb, gpScroll->mpMiniFoe);

	checkBulletsWithFoes(gpTheGamer->mpBullets, gpScroll->mpSmallFoe);
	checkBulletsWithFoes(gpTheGamer->mpBullets2, gpScroll->mpSmallFoe);
	checkBulletsWithFoes(gpTheGamer->mpBullets3, gpScroll->mpSmallFoe);
	checkBulletsWithFoes(gpTheGamer->mpBulletsBomb, gpScroll->mpSmallFoe);

	checkBulletsWithObstables(gpTheGamer->mpBullets, gpScroll->mpFir);
	checkBulletsWithObstables(gpTheGamer->mpBullets2, gpScroll->mpFir);
	checkBulletsWithObstables(gpTheGamer->mpBullets3, gpScroll->mpFir);
	checkBulletsWithObstables(gpTheGamer->mpBulletsBomb, gpScroll->mpFir);

	checkBulletsWithObstables(gpTheGamer->mpBullets,gpScroll->mpAsteroids);
	checkBulletsWithObstables(gpTheGamer->mpBullets2,gpScroll->mpAsteroids);
	checkBulletsWithObstables(gpTheGamer->mpBullets3,gpScroll->mpAsteroids);
	checkBulletsWithObstables(gpTheGamer->mpBulletsBomb,gpScroll->mpAsteroids);

	checkBulletsWithObstables(gpTheGamer->mpBullets,gpScroll->mpAsteroids2);
	checkBulletsWithObstables(gpTheGamer->mpBullets2,gpScroll->mpAsteroids2);
	checkBulletsWithObstables(gpTheGamer->mpBullets3,gpScroll->mpAsteroids2);
	checkBulletsWithObstables(gpTheGamer->mpBulletsBomb,gpScroll->mpAsteroids2);


	// If the player do not have life left, I kill it
	if(gpTheGamer->mdLife<100)
	{
		playMiddleBlast(gpTheGamer->mdXCoord,gpTheGamer->mdYCoord);

		gpTheGamer->mdXCoord=200;
		gpTheGamer->mdYCoord=300;
		gpTheGamer->mdLife=1000;
		gpTheGamer->miLifeLeft-=1;
		// If the player used all the 3 life, the game is over
		if(gpTheGamer->miLifeLeft<=0)
			gbGameOver=true;
	}
	return (SUCCESS);
}


///////////////////////////////////////////////////////
//						GameInit
///////////////////////////////////////////////////////
int GameInit()
{
	// Begin with the Start Background, thus gpStart is switched to true
	gbStart=true;

// ******************
// DirectX instanses
// ******************

		// engine
	pTheDrawEngine = MyDrawEngine::Start(SCREENWIDTH, SCREENHEIGHT, COLOURDEPTH, gHwnd);
		// sounds
	pTheSoundEngine = MySoundEngine::Start(gHwnd);
		// inputs
	pTheInputs = MyInputs::Start(g_hinstance, gHwnd);

// ****************
//	Game object
// ****************

	// Background Picture
	gpBackgroundFire = new BackgroundItem(BACKGROUND_FIRE);
	gpBackgroundFire->init();
	gpBackgroundFire->setXCoord(0);
	gpBackgroundFire->setYCoord(0);

	// Background Start Picture
	gpBackgroundStart = new BackgroundItem(BACKGROUND_START);
	gpBackgroundStart->init();
	gpBackgroundStart->setXCoord(0);
	gpBackgroundStart->setYCoord(0);

	// Background Game over Picture
	gpBackgroundGameOver = new BackgroundItem(BACKGROUND_GAMEOVER);
	gpBackgroundGameOver->init();
	gpBackgroundGameOver->setXCoord(0);
	gpBackgroundGameOver->setYCoord(0);
	
	// The game timer (for the dynamic frame rate)
	gpTheGameTimer = new GameTimer();

	// global pointer to the Gamer
	gpTheGamer = new Gamer();

	// Global pointer to the Small blast collection
	gpSmallBlast = new ItemCollection<Blaster>(20,SMALLBLAST);
	gpSmallBlast->init();

	// Global pointer to the Middle blast collection
	gpMiddleBlast = new ItemCollection<Blaster>(6,MIDDLEBLAST);
	gpMiddleBlast->init();

	gpAsteroid1Blast = new ItemCollection<Blaster>(6,ASTEROID1BLAST);
	gpAsteroid1Blast->init();

	// Sound object 
	gpTheSoundFX = new SoundFX();

	// Global pointer to the Scroller
	gpScroll = new Scroller();

	// Progress Bars (Show the progress of the Gamer in the Game)
		gsProgress_background.top=110;
		gsProgress_background.bottom=gsProgress_background.top+20;
		gsProgress_background.left=100;
		gsProgress_background.right=gsProgress_background.left+200;


		gsCurrent_progress.top=gsProgress_background.top;
		gsCurrent_progress.bottom=gsProgress_background.bottom;
		gsCurrent_progress.left=gsProgress_background.left;

	return (SUCCESS);
}


///////////////////////////////////////////////////////
//						GameMain
///////////////////////////////////////////////////////
int GameMain()
{
	
	// Begin to timer (for dynamic frame rate)
	gpTheGameTimer->mark();

	//  validation routine - should be able to alt-tab out of program
	// (Done in game main just to show that it needs to be done - better to do in MyDrawEngine. really)
	if(pTheDrawEngine->Validate()==FAILURE) return FAILURE;

	// Clear screen
	pTheDrawEngine->ClearBackBuffer();

	// gpStart set to true, display the Start Background 
	if(gbStart==true){
		gpBackgroundStart->draw();
		pTheInputs->SampleKeyboard(); 
		if (pTheInputs->KeyPressed(DIK_SPACE)) {
			gbStart=false;
		} 

	}	
	// gpGameOver set to true, display the game over background
	else if(gbGameOver==true){
		gpBackgroundGameOver->move();
		gpBackgroundGameOver->draw();
		pTheDrawEngine->WriteDouble(gpTheGamer->miScore,500,395);
	}
	// else the game can be displayed
	else{

		// Calcul the scroll speed (in function of the gamer position)
		scroll_speed=( (gpTheGamer->mdXCoord-200) /1)  ;

		// stop the Gamer to go before the 0 position
		if(current_position<=0 && scroll_speed<0)
			scroll_speed = 0;

	
		
		// ****************
		//	Move objects
		// ****************
		gpBackgroundFire->move();
		gpSmallBlast->move();
		gpMiddleBlast->move();
		gpTheGamer->move();
		gpScroll->move();

		// To look the game Background in a cycle way
		if(gpBackgroundFire->getXCoord()< -2048)
			gpBackgroundFire->setXCoord(SCREENWIDTH);

		// ****************
		//	Draw objects
		// ****************
		gpBackgroundFire->draw();
		gpSmallBlast->draw();
		gpMiddleBlast->draw();
		gpTheGamer->draw();
		gpScroll->draw();	
		
		// Display the score of the gamer
		pTheDrawEngine->WriteText(10,150,"Score :");
		pTheDrawEngine->WriteInt(gpTheGamer->miScore,100,150);
		// Show the motion type mod activated
		if(gpTheGamer->miMotion==2)
			pTheDrawEngine->WriteText(10,185,"Normal mode (press M to change)");
		else
		{pTheDrawEngine->WriteText(10,185,"Shoot Them up mode (press M to change)");
		pTheDrawEngine->WriteText(10,205,"Press B to drop Bomb");
		}

		// Display the position of the Gamer within the Game
		pTheDrawEngine->WriteText(10,110,"Progress:" );		
		gsCurrent_progress.right=gsCurrent_progress.left+current_position/gpScroll->miBossFoeBegin*200;
		pTheDrawEngine->FillRect(gsProgress_background,_XRGB(0,0,100));
		pTheDrawEngine->FillRect(gsCurrent_progress,_XRGB(0,150,200));
		pTheDrawEngine->WriteInt(current_position/gpScroll->miBossFoeBegin*100,gsProgress_background.left+90,gsProgress_background.top+3);
		pTheDrawEngine->WriteText(gsProgress_background.left+110,gsProgress_background.top+3,"%" );
		
		// Display the name of the zone, in which the Gamer is.
		if(current_position>gpScroll->miAste1Begin && current_position<gpScroll->miAste1End)
		pTheDrawEngine->WriteText(440,30,"White Asteroid Zone");		
		else if(current_position>gpScroll->miAste2Begin && current_position<gpScroll->miAste2End)
		pTheDrawEngine->WriteText(440,30,"Red Asteroid Zone");
		else if(current_position>gpScroll->miMiniFoeBegin && current_position<gpScroll->miMiniFoeEnd)
		pTheDrawEngine->WriteText(440,30,"Small Enemy Zone");
		else if(current_position>gpScroll->miSmallFoeBegin && current_position<gpScroll->miSmallFoeEnd)
		pTheDrawEngine->WriteText(440,30,"Medium Enemy Zone");
		else if(current_position>gpScroll->miBossFoeBegin)
		pTheDrawEngine->WriteText(440,30,"Boss Zone");
		
	}

	// Process all collision
	processCollision();

	// Flip and clear buffers
	pTheDrawEngine->Flip();


	// Stop the timer 
    gpTheGameTimer->mark();

	// Get the frame time and put it in the frame_time global variable
	frame_time = gpTheGameTimer->mdFrameTime;

	// Update the current_position
	current_position += scroll_speed*frame_time;
	// Update the lasTimeCollision
	lastTimeCollision+=1*frame_time;

	// Stop the gamer to go before the 0 position
	if(current_position<0){
		current_position=0;
		scroll_speed=0;
	}
	if(current_position>=gpScroll->miBossFoeBegin)
		gbGameOver=true;

	return SUCCESS;
}

///////////////////////////////////////////////////////
//					GameShutdown
///////////////////////////////////////////////////////


// Function called to shutdown the game
// Delete all resource allocated during the game
void GameShutdown()
{
	
	delete gpTheGamer;
	delete gpScroll;

	delete gpSmallBlast; // Small Blast collection
	delete gpMiddleBlast; // Middle Blast collection
	delete gpAsteroid1Blast; // Asteroid 1 Blast collection

	delete gpBackgroundFire;	// Background during the game
	delete gpBackgroundStart;  // Start Background
	delete gpBackgroundGameOver; // Game Over background

	delete gpTheGameTimer;		// A pointer to the game timer

	// (engines must be terminated last)
	pTheDrawEngine->Terminate();
	pTheSoundEngine->Terminate();
	pTheInputs->Terminate();
}




// Scroller.h
// Manage all the moving object during to game
//
// Author : Benoit Marechal


#ifndef SCROLLER_H
#define SCROLLER_H

// Includes
#include "BackgroundItem.h"
#include "FoeItem.h"
#include "ObstacleItem.h"


// Extern variables
extern double scroll_speed;
extern double current_position;

////////////////////////////////
// Class header
////////////////////////////////
class Scroller
{ 
public :

ItemCollection<BackgroundItem>* mpStars1;
ItemCollection<BackgroundItem>* mpStars2;
ItemCollection<FoeItem>* mpSmallFoe;
ItemCollection<FoeItem>* mpMiniFoe;
FoeItem* mpBossFoe;

ItemCollection<ObstacleItem>* mpAsteroids;
ItemCollection<ObstacleItem>* mpAsteroids2;
ItemCollection<ObstacleItem>* mpFloor;
ItemCollection<ObstacleItem>* mpFir;

int miAste1Begin;
int miAste1End;
double mdDstAste1;
double mdTimeAste1;

int miAste2Begin;
int miAste2End;
double mdDstAste2;
double mdTimeAste2;

int miMiniFoeBegin;
int miMiniFoeEnd;
double mdDstMiniFoe;
double mdTimeMiniFoe;

int miSmallFoeBegin;
int miSmallFoeEnd;
double mdDstSmallFoe;
double mdTimeSmallFoe;

int miBossFoeBegin;



double mdTime;
double mdTime2;
double mdItemTimeStar1; // Time between 2 new items
double mdItemTimeStar2; // Time between 2 new items

public :
Scroller();
~Scroller();

void move();
void draw();

};

#endif




#include "Scroller.h"

Scroller::Scroller()
{

//Set up Scrolling
	// Asteroid 1
miAste1Begin= 300;
miAste1End = 3000;
mdDstAste1 = 80;

	// Asteroid 2
miAste2Begin= 6000;
miAste2End = 9000;
mdDstAste2 = 120;
	
	// Mini Foe
miMiniFoeBegin = 3000;
miMiniFoeEnd = 6000;
mdDstMiniFoe = 200;
mdTimeMiniFoe = 0;

	// Small Foe
miSmallFoeBegin = 9000;
miSmallFoeEnd = 11000;
mdDstSmallFoe = 200;
mdTimeSmallFoe = 0;

	// Boss Foe
miBossFoeBegin = 12500;


	mdTime = 0;
	mdTime2 = 0;
	mdTimeAste1 = 0;
	mdTimeAste2 = 0;
	mdItemTimeStar1 = 0.2;
	mdItemTimeStar2 = 0.5;

	mpStars1 = new ItemCollection<BackgroundItem>(50,STAR1);
	mpStars1->init();
	mpStars2 = new ItemCollection<BackgroundItem>(6,STAR3);
	mpStars2->init();
	mpSmallFoe = new ItemCollection<FoeItem>(12,SMALL_FOE);
	mpSmallFoe->init();
	mpMiniFoe = new ItemCollection<FoeItem>(12,MINI_FOE);
	mpMiniFoe->init();


	mpAsteroids = new ItemCollection<ObstacleItem>(20,ASTEROID1);
	mpAsteroids->init();
	mpAsteroids2 = new ItemCollection<ObstacleItem>(20,ASTEROID2);
	mpAsteroids2->init();


	//* STATIC OBSTACLE generation
	mpFloor = new ItemCollection<ObstacleItem>(20,FLOOR);
	mpFloor->init();

	for(int i=0;i<mpFloor->miAmount;i++)
	{
		ObstacleItem* obs = mpFloor->getDeathItem();
		obs->setAlive(1);
		obs->setXCoord(i*1000);
		obs->setYCoord(680);
		obs->mdDirectionSpeed = 0;
	}

	mpFir = new ItemCollection<ObstacleItem>(20,FIR);
	mpFir->init();

	for(int i=0;i<mpFir->miAmount;i++)
	{
		ObstacleItem* obs = mpFir->getDeathItem();
		obs->setAlive(1);
		obs->setXCoord(i*400+(rand()%320));
		obs->setYCoord(539);
	}

	mpBossFoe = new FoeItem(BOSS_FOE);
	mpBossFoe->init();
	mpBossFoe->setXCoord(miBossFoeBegin);
	mpBossFoe->setYCoord(200);
	mpBossFoe->setAlive(1);
	mpBossFoe->mdLifeAmount=10000;

	//*END generation of Static ob8stacle
}

Scroller::~Scroller()
{
	delete mpStars1;	
	delete mpStars2;

	delete mpSmallFoe;
	delete mpMiniFoe;

	delete mpAsteroids;
	delete mpAsteroids2;

	delete mpFloor;
	delete mpFir;

	delete mpBossFoe;
}


void Scroller::move()
{
	mdTime=mdTime+1*frame_time;
	mdTime2=mdTime2+1*frame_time;
	mdTimeSmallFoe+=1*frame_time;
	mdTimeMiniFoe=mdTimeMiniFoe+1*frame_time;


	mdTimeAste1=mdTimeAste1+1*frame_time;

	mdTimeAste2=mdTimeAste2+1*frame_time;

	if (mdTime>=mdItemTimeStar1)
	{
		BackgroundItem* b = mpStars1->getDeathItem();
		b->setXCoord(rand()%(SCREENWIDTH+30));
		b->setYCoord(-5);
		b->setAlive(1);
		
		mdTime=0;
	}
	else if (mdTime2>=mdItemTimeStar2)
	{
		BackgroundItem* b = mpStars2->getDeathItem();
		b->setXCoord(SCREENWIDTH+10);
		b->setYCoord(100+rand()%(SCREENHEIGHT-100));
		b->setAlive(1);
		
		mdTime2=0;
	}

	if (current_position>=miAste1Begin && current_position<=miAste1End)
	{
		double rap = (mdDstAste1/scroll_speed);
		rap = (rap>=0) ? rap : -rap;

		
		if (mdTimeAste1>rap && rap >=0.01)
		{
			ObstacleItem* b5 = mpAsteroids->getDeathItem();
			b5->init();
			b5->setAlive(1);

			mdTimeAste1=0;
		}

	}

	if (current_position>=miAste2Begin && current_position<=miAste2End)
	{
		double rap = (mdDstAste2/scroll_speed);
		rap = (rap>=0) ? rap : -rap;
		

		if (mdTimeAste2>rap && rap >=0.01)
		{

			mdTimeAste2=0;
			ObstacleItem* b6 = mpAsteroids2->getDeathItem();
			b6->init();
			b6->setXCoord(SCREENWIDTH);
			b6->setYCoord(rand()%(SCREENHEIGHT-170));
			b6->setAlive(1);
		}

	}

	if (current_position>=miMiniFoeBegin && current_position<=miMiniFoeEnd)
	{

		double rap = (mdDstMiniFoe/scroll_speed);
		rap = (rap>=0) ? rap : -rap;
		

	
		if (mdTimeMiniFoe>rap && rap >=0.01)
		{
			
			mdTimeMiniFoe=0;
			
			FoeItem* b7 = mpMiniFoe->getDeathItem();
			b7->init();
			b7->setAlive(1);
		}
	}
	if (current_position>=miSmallFoeBegin && current_position<=miSmallFoeEnd)
	{
		double rap = (mdDstSmallFoe/scroll_speed);
		rap = (rap>=0) ? rap : -rap;
		
		if (mdTimeSmallFoe>rap && rap >=0.01)
		{
			mdTimeSmallFoe=0;
			FoeItem* b8 = mpSmallFoe->getDeathItem();
			b8->init();
			b8->setXCoord(SCREENWIDTH);
			b8->setDirectionAngle(180);
			b8->setYCoord(rand()%(SCREENHEIGHT-170));
			b8->setAlive(1);
		}
	}
	mpStars1->move(); 
	mpStars2->move(); 
	mpSmallFoe->move(); 
	mpMiniFoe->move(); 
	mpAsteroids->move(); 
	mpAsteroids2->move(); 
	mpFloor->move(); 
	mpFir->move();
	if(mpBossFoe->isAlive())
	mpBossFoe->move();
}

void Scroller::draw()
{	
	mpStars1->draw();
	mpStars2->draw();

	mpSmallFoe->draw();
	mpMiniFoe->draw();

	mpAsteroids->draw();
	mpAsteroids2->draw();

	mpFloor->draw();
	mpFir->draw();

	if(mpBossFoe->isAlive())
	mpBossFoe->draw();
	//pTheDrawEngine->WriteDouble(mpBossFoe->mdLifeAmount,50,400);
}














