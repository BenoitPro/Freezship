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
