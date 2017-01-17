
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
