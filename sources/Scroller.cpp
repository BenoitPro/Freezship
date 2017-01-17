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