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