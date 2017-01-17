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