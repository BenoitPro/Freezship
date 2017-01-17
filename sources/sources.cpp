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



// FoeItem.h
// Header file 
// 
// FoeItem class provides methods to manage and draw a Foe
// Inherit of a bullet class
//
// Author : Benoit Marechal

#ifndef FOEITEM_H
#define FOEITEM_H

/////////////////////////
// Includes
/////////////////////////
#include "BulletItem.h" 
#include "ItemCollection.h" 


///////////////////////
// Class definition
////////////////////////
class FoeItem:public BulletItem {
public :

	ItemCollection<BulletItem>* mpBullets; // Collection of bullets
	double mdFireFrequency; // temp entre 2 tire
	double mdFireTime; // Time during the foe fire bullets
	double mdLazyTime; // Time during the foe do nothing
	double mdMotion1Time; // Time during the foe go to the motion1
	double mdMotion2Time; // Time during the foe go to the motion1
	double mdTimeFrequency; // count the time between to fire
	double mdTimeAction; // time between 2 action
	double mdTimeLazy; // time between 2 action
	double mdTimeMotion; // current time during the motion

	BulletItem* mpFoe; // Pointer to the foe (inherit of a BulletItem)
	
public:

// Constructor
	FoeItem(int item);

// Destructor
	~FoeItem();

// Methods

	// Initialize the enemy
	void init();
	// Move the enemy
	void move(void);
	
	// Draw method, to display the Enemy 
	void draw(void);

};
#endif


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


// BulletItem.h
// Header file 
// 
// BulletItem class provides methods to manage and draw a Bullet
// Inherit of ObstacleItem class
//
// Author : Benoit Marechal

#ifndef BULLETITEM_H
#define BULLETITEM_H

/////////////////////////
// Includes
/////////////////////////

#include "ObstacleItem.h"


///////////////////////
// Class definition
////////////////////////
class BulletItem:public ObstacleItem {
public :

	bool mbIsLookForFoe; // If the target look for foe
	bool mbGravityOn; // If is gravity is on
    BackgroundItem* mpTarget; // The target aimed
	double mdTime; // count the time
	int miCountFrame; // count the number of frame 


public :
	// Constructor
	BulletItem(int item);
	// Destructor
	~BulletItem();

	// Initialize method
	void init();

	// Move the bullet
	void move(void);

	// Draw the bullet
	void draw(void);


	// setters
	void setCountFrame(int cf);
	void setGravity(bool g);
	void setTarget(BackgroundItem* b);
	void setAlive(bool b);

	// To aim the target
	void aimTarget();

};

#endif


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




// ObstacleItem.h
// Header file 
// 
// ObstacleItem class provides methods to manage and draw a Obstacle
// Inherit of a BackgroundItem class
//
// Author : Benoit Marechal

#ifndef OBSTACLEITEM_H
#define OBSTACLEITEM_H


/////////////////////////
// Includes
/////////////////////////
#include "BackgroundItem.h"

////////////////////////
// Class definition
////////////////////////
class ObstacleItem:public BackgroundItem {
public :

	double mdLifeAmount ;	// Life Amount
	int miDamage;			// Damage caused by the Obstacle

public :
// Constructor
	ObstacleItem(int item);
// Destructor
	~ObstacleItem();

//Methods 

	// Initialize method
	void init();

	// Move the Obstacle
	void move(void);

	// Draw the Obstacle
	void draw(void);

	// getters
	double getLifeAmount(void);

	// setters
	void setLifeAmount(double life);

};
#endif



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






// BackgroundItem.h
// Header file 
// 
// BackgroundItem class provides methods to manage and draw a BackgroundItem like
// the background and the snow and the stars
//
// Author : Benoit Marechal

#ifndef BACKGROUNDITEM_H
#define BACKGROUNDITEM_H

/////////////////////////
// Includes
/////////////////////////
#include <math.h>
#include <time.h>
#include "Animation.h"
#include "Consts.h"
#include "gamecode.h"

/////////////////////////
// Extern variables
/////////////////////////
extern double frame_time;
extern double gravity;
extern MyDrawEngine* pTheDrawEngine;
extern double scroll_speed;

///////////////////////
// Class definition
////////////////////////
class BackgroundItem {

public :

	double mdXCoord;	// X coord
	double mdYCoord;	// Y coord

	double mdDirectionAngle; // direction angle of the object
    double mdDirectionSpeed; // Speed of the object
	bool mbAlive; // to know if it's alive
	int miItem; // kind of item, to init with the adapted parameters (see Consts.h)

    Animation* mpAnim;	// Pointer to the Animation object, which manage the picture
						// of the Item

public :

	// Constructor
	BackgroundItem(int item);
	// Destructor
	~BackgroundItem();

	// Initialize method
	void init(); // ramdom values (i is the seed of the srand function)

	// Move the object
	void move(void);

	// Draw the object
	void draw(void);

	// getter
	bool isAlive(void);
	double getXCoord(void);
	double getYCoord(void);
	RECT getBoundRect(void);

	// setters
	void setAlive(bool b);
	void setXCoord(double xcoord);
	void setYCoord(double ycoord);
	void setDirectionAngle(double angle);



};
#endif




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



// Blaster.h
// Header file 
// 
// BackgroundItem class provides methods to manage and draw a BackgroundItem like
// the background and the snow and the stars
//
// Author : Benoit Marechal

#ifndef BLASTERITEM_H
#define BLASTERITEM_H

/////////////////////////
// Includes
/////////////////////////
#include "BackgroundItem.h"
#include "ItemCollection.h"
#include "ParticuleItem.h"


///////////////////////
// Class definition
////////////////////////
class Blaster: public BackgroundItem {

protected :

	// Collection of small particules
ItemCollection<ParticuleItem>* mpSmallParticules;
	// Collection of animated particules
ItemCollection<ParticuleItem>* mpAnimParticules;

	// Time life of the blast
double mdTimeLife;

public :

	// Constructor
	Blaster(int style);

	// Destructor
	~Blaster();

	// Initialize Methods
	void init();
	
	// Move the object
	void move(void);

	// Draw
	void draw(void);

	// Setters
	void setXCoord(double x);
	void setYCoord(double y);

};

#endif


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





// ParticuleItem.h
// Header file 
// 
// ParticuleItem class provides methods to manage and draw a Particule
// Inherit of a BackgroundItem class
//
// Author : Benoit Marechal

#ifndef PARTICULEITEM_H
#define PARTICULEITEM_H

/////////////////////////
// Includes
/////////////////////////
#include "BackgroundItem.h"

////////////////////////
// Class definition
////////////////////////
class ParticuleItem : public BackgroundItem
{
protected :

	double mdTimeLife; // Particule's time life
	int miCountFrame; // Count the numbers of frame

public :

	// Constructor
	ParticuleItem(int item);

	// Initialize (with random values)
	void init(); 

	// Move the particule
	void move(void);

	// Draw the particule
	void draw(void);
};
#endif


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




// SoundFX.h
// Header file 
// 
// SoundFX class provides methods to manage and play a Sound
// Author : Benoit Marechal

#ifndef SOUNDFX_H
#define SOUNDFX_H

/////////////////////////
// Includes
/////////////////////////
#include "mysound.h"

////////////////////////
// Class definition
////////////////////////
class SoundFX
{

private:

	int	miIndexSmallExplosion; // position of the last sound played
	int miIndexMiddleExplosion;// position of the last sound played
	int miIndexBullet1;// position of the last sound played
	int miIndexThrust;// position of the last sound played

	// Array holding the sounds
	MySound*  mpSmallExplosion[10]; // hold the small explosions sound
	MySound*  mpMiddleExplosion[10]; // hold the Middle explosions sound
	MySound*  mpBullet1[15]; // hold the Bullet sound
	MySound*  mpThrust[5]; // hold the Thrust sound
public :

	// Constructor
	SoundFX();
	// Destructor
	~SoundFX();


	// Methods
	void playSmallExplosion(void); // play a small explosion sound
	void playMiddleExplosion(void);// play a middle explosion sound
	void playBullet1(void); // play a bullet explosion sound
	void playThrust(void); // play the thrust sound
};

#endif




// SoundFX.cpp
// Defintion file 
// 
// SoundFX class provides methods to manage and play a Sound
//
// Author : Benoit Marechal

/////////////////////////
// Includes
/////////////////////////
#include "SoundFX.h"


// Constructor
SoundFX::SoundFX()
{

miIndexSmallExplosion=0;
miIndexMiddleExplosion=0;
miIndexBullet1=0;
miIndexThrust=0;

	for(int i=0;i<20;i++)
	{
		mpSmallExplosion[i] = new MySound();
		mpSmallExplosion[i]->LoadWave("explosion1.wav");
	}

	for(int i=0;i<20;i++)
	{
		mpMiddleExplosion[i] = new MySound();
		mpMiddleExplosion[i]->LoadWave("explosion2.wav");
	}

	for(int i=0;i<50;i++)
	{
		mpBullet1[i] = new MySound();
		mpBullet1[i]->LoadWave("zap.wav");
	}

	for(int i=0;i<5;i++)
	{
		mpThrust[i] = new MySound();
		mpThrust[i]->LoadWave("thrust.wav");
	}

}

// Destructor
SoundFX::~SoundFX()
{
	delete mpSmallExplosion;
	delete mpMiddleExplosion;
	delete mpBullet1;
	delete mpThrust;
}

// play a small explosion sound
void SoundFX::playSmallExplosion()
{	
	miIndexSmallExplosion= (miIndexSmallExplosion>=20) ? 0:miIndexSmallExplosion;
	mpSmallExplosion[miIndexSmallExplosion++]->Play(0);
}

// play a middle explosion sound
void SoundFX::playMiddleExplosion()
{
	miIndexMiddleExplosion= (miIndexMiddleExplosion>=20) ? 0:miIndexMiddleExplosion;
	mpMiddleExplosion[miIndexMiddleExplosion++]->Play(0);
}

// play a bullet explosion sound
void SoundFX::playBullet1()
{
	miIndexBullet1= (miIndexBullet1>=50) ? 0:miIndexBullet1;		
	mpBullet1[miIndexBullet1++]->Play(0);
}

// play the thrust sound
void SoundFX::playThrust()
{
	miIndexThrust= (miIndexThrust>=5) ? 0:miIndexThrust;		
	mpThrust[miIndexThrust++]->Play(0);
}






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



// Gamer.h	Version 1		10/4/07

// Gamer class
#pragma once

#include "Animation.h"
#include "gametimer.h"
#include "myinputs.h"
#include "mydrawengine.h"
#include "gamecode.h" 
#include "gametimer.h"
#include "SoundFX.h"


#include <windows.h>

#include "ItemCollection.h"
#include "BulletItem.h"
#include "Scroller.h"

extern GameTimer* pTheGameTimer;
extern double frame_time;
extern double gravity;
extern MyDrawEngine* pTheDrawEngine;
extern MyInputs* pTheInputs;
extern ObstacleItem* obs;
extern SoundFX* gpTheSoundFX;

extern Scroller* gpScroll;

class Gamer{
public :
	double mdXCoord;
	double mdYCoord;
	
	double mdDeltaX;		// 
	double mdDeltaY;		//

	double mdAngle;

	double mdAcceleration;

	int miMotion;
	double mdFriction;

	int miLifeLeft;
	//int lThrust;			// Use an enum? each thrust corespond to an acceleration value

	//int lState;			// Use an enum?

	Animation* mpAnim;	// pointer to an animation which represent the Gamer

	Animation* mpLife;
	Animation* mpLifeLeft;

	ItemCollection<BulletItem>* mpBullets;
	ItemCollection<BulletItem>* mpBullets2;
	ItemCollection<BulletItem>* mpBullets3;
	ItemCollection<BulletItem>* mpBulletsBomb;

	int mdLevel;
	double mdTime;
	int miScore;

	double mdTime2;
	double mdTime3;

	double mdLife;
	RECT msBoundRect;

public :
	Gamer();

	void move();
	void move(int style);
	void draw(void);

	// getter
	RECT getBoundRect(void);

	void setLevel(int level);
};



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



// ItemCollection.h
// Header file 
// 
// ItemCollection class provides methods to manage a set (an array)
// of all objects that inherited by the BackgroundItem class, like
// Blaster, ObstacleItem, BulletItem and FoeItem.
// 
// To perfom that, it use the template mecanism of C++
//
// Author : Benoit Marechal

#ifndef ITEMCOLLECTION_H
#define ITEMCOLLECTION_H


template <class T> 
class ItemCollection {

public :

	T* mpTheItems[1024]; // Array holding the object
	int miAmount; // Amount of object created

public :

	// Constructor
	ItemCollection(int amount,int item);
	// Destructor
	~ItemCollection();

	// Initialize the collection
	void init();					
	// Move alive items
	void move();					
	// Draw alive items				
	void draw();

	// Allow to define the position of all the item
	void setItemsXCoord(double xcoord);
	void setItemsYCoord(double ycoord);

	// Search a death item, init and turn to alive
	T* getDeathItem(void);
	// Give the item specified (for checking collision)
	T* getItem(int i);
	
	// Return of a near item of the Gamer
	T* getNearItem(double x,double y);

	// Set to alive the specified amount of item
	void AddItems(int amount); 

};

///////////////////////////////////////////
// Definition of the ItemCollection class
///////////////////////////////////////////

// Constructor
template <class T> 
ItemCollection<T>::ItemCollection(int amount,int item)
{
	miAmount = amount;
	int i=0;
	for (i=0 ; i<miAmount ; i++)
	{
		mpTheItems[i] = new T(item);
	}

}
// Destructor
template <class T> 
ItemCollection<T>::~ItemCollection()
{
int i=0;
	for (i=0 ; i<miAmount ; i++)
	{
		delete mpTheItems[i];
	}
}


// Initialize the collection
template <class T> 
void ItemCollection<T>::init()
{
	int i=0;
	for (i=0 ; i<miAmount ; i++)
	{
			mpTheItems[i]->init();
	}
}

// Move alive items
template <class T> 
void ItemCollection<T>::move()
{
	int i=0;
	for (i=0 ; i<miAmount ; i++)
	{
		if (mpTheItems[i]->isAlive())
			mpTheItems[i]->move();
	}

}

// Draw alive items
template <class T> 
void ItemCollection<T>::draw()
{
int i=0;
	for (i=0 ; i<miAmount ; i++)
	{
		if (mpTheItems[i]->isAlive())
			mpTheItems[i]->draw();
	}
}

//  define the position (X here) of all the item
template <class T> 
void ItemCollection<T>::setItemsXCoord(double xcoord)
{
	int i=0;
	for (i=0 ; i<miAmount ; i++)
	{
			mpTheItems[i]->setXCoord(xcoord);
	}
}

//  define the position (Y here) of all the item
template <class T> 
void ItemCollection<T>::setItemsYCoord(double ycoord)
{
	int i=0;
	for (i=0 ; i<miAmount ; i++)
	{
			mpTheItems[i]->setYCoord(ycoord);
	}
}

// get the first item that is not alive
template <class T> 
T* ItemCollection<T>::getDeathItem()
{
int i=0;
	for (i=0 ; i<miAmount ; i++)
	{
		if (!mpTheItems[i]->isAlive())
			return mpTheItems[i];
	}
mpTheItems[0]->setAlive(false);
	return mpTheItems[0];
}

// Return an item which is near the Gamer
template <class T> 
T* ItemCollection<T>::getNearItem(double x,double y)
{
int i=0;
double min=2000;
T* item;
	for (i=0 ; i<miAmount ; i++)
	{
		if (mpTheItems[i]->isAlive())
			if( (mpTheItems[i]->getXCoord()-x)>200 && mpTheItems[i]->getXCoord()< min)
			{
				min = mpTheItems[i]->getXCoord();
				item = mpTheItems[i];
			}
	}
	if(min==2000)
		return NULL;
	return item;
}


// Give the item specified (for checking collision)
template <class T> 
T* ItemCollection<T>::getItem(int i)
{
	return mpTheItems[i];
}

// Set to alive the specified amount of item
template <class T> 
void ItemCollection<T>::AddItems(int amount)
{
	int i=0;
	for (i=0 ; i<amount ; i++)
	{
		getDeathItem()->setAlive(1);
	}
}
#endif



// Consts.h
// Header file 
// 
// hold the Constant for the Game
//
// Author : Benoit Marechal

#define PARTICULE1			2
#define PARTICULE2			3
#define PARTICULE3			4

#define STAR1			10
#define STAR2			11
#define STAR3			12

#define MIDDLEBLAST		30
#define SMALLBLAST		31
#define ASTEROID1BLAST  32
#define OBSTACLE1		40
#define ASTEROID1		41
#define ASTEROID2		42


#define BULLET1			60
#define BULLET2			61
#define BULLET3			62
#define BULLET4			63
#define BULLET5			64
#define MINI_FOE		80
#define SMALL_FOE		81
#define BOSS_FOE		82

#define BACKGROUND_FIRE	100
#define BACKGROUND_START	101
#define BACKGROUND_GAMEOVER	102

#define FLOOR		200
#define FIR 	201









