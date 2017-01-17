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