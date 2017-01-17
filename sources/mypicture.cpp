// mypicture.cpp	Version 7		8/12/03
// The definition file for the methods in MyPicture, declared in mypicture.h


#include "Mypicture.h"
#include "mydrawengine.h"

#define BITMAP_ID 0x4D42
#define MAX_COLORS_PALETTE 256

// *****************************************************************

MyPicture::MyPicture()
{	
	MyDrawEngine* pTheDrawEngine = MyDrawEngine::GetInstance();

	if(!pTheDrawEngine)
	{
		ErrorLogger::Writeln("Error creating MyPicture, no MyDrawEngine");
	}
	ZeroMemory(&ddsd, sizeof(ddsd));
	height=0;
	width=0;
	this->lpdd=pTheDrawEngine->lpdd;
	lpTheSurface=NULL;
	memoryPitch=0;
	videoAddress=NULL;
	mpszSourceFileName = NULL;
	bytesPerPixel=0;
}

MyPicture::~MyPicture()
{
	// Make sure memory and ddraw have been released
	if (lpTheSurface)
	{
		lpTheSurface->Release();
	}

	if(mpszSourceFileName)
	{
		delete mpszSourceFileName;
		mpszSourceFileName=NULL;
	}
}

bool MyPicture::Validate()
{
	if (FAILED(lpTheSurface->IsLost()))		// See if primary surface is lost
	{	
		if(FAILED(lpTheSurface->Restore()))	// Try to reaquire
			return FAILURE;					// If it is still postal
		else
		{
			// Need to reload. Damn. Is this going to be churning all the time I have ALT-TABBED? Don't think so.
			if(mpszSourceFileName)	// If it has a file name
			{
				if(FAILED(LoadBitmap(mpszSourceFileName))) // Reload
					return FAILURE;	
			}
			else					// Must be a blank one. Hope it hasn't been drawn on.
			{
				// Blank out?
			}
			return SUCCESS;						// If it recovered
		}
	}
	return SUCCESS;								// If it never went away
}

void MyPicture::Release()
{ 
	// Set everything to zero or null
	height=0;
	width=0;
	lpdd=NULL;

	// If it is still valid, release the surface
	if (lpTheSurface)
	{
		lpTheSurface->Release();
		lpTheSurface=NULL;
	}
}

ErrorType MyPicture::LoadBitmap(char *pszFilename, int flags)
{
	// Check MyDrawEngine has been started
	if(MyDrawEngine::GetInstance()==NULL)
	{
		ErrorLogger::Writeln("Can't load image - draw engine not initialised\n");
		  return FAILURE;
	}


	// Keep a permanent record of the filename

	mpszSourceFileName = new char[strlen(pszFilename)+1];

	strcpy(mpszSourceFileName, pszFilename);

	HBITMAP hbm;
	BITMAP bmInfo;
	DDSURFACEDESC2 ddsd;
	HDC hdcImage = NULL;
	HDC surfaceDC = NULL;

	flags=0;

	// Load an image from file to a bitmap handle
	hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), pszFilename, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);

	// If it didn't load, bug out.
	if (hbm == NULL)
	{
		ErrorLogger::Writeln("File didn't load\n");
		ErrorLogger::Writeln(*pszFilename);
		  return FAILURE;
	}

	// Put the info about the bitmap into a struct
	GetObject(hbm,sizeof(bmInfo),&bmInfo);

	// Select Image into a device context

	hdcImage = CreateCompatibleDC(NULL);

	if(hdcImage == NULL)
	{
		  ErrorLogger::Writeln("Couldn't create DC\n");
		  return FAILURE;
	}


	SelectObject(hdcImage, hbm);

	// now to create the DDraw surface so I can load the image onto it
	
	this->height=bmInfo.bmHeight;
	this->width=bmInfo.bmWidth;

	// Fill in the order form
	memset(&ddsd,0,sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags=DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT;		// Set capablilties
	ddsd.dwHeight	=this->height;						// Set height
	ddsd.dwWidth	=this->width;						// Set width
	ddsd.ddsCaps.dwCaps=DDSCAPS_OFFSCREENPLAIN|flags;	// Set as offscreen surface, also any
														// flags requested by call - such as
														// a request for video memory storage

	// Create the surface

	int err;
	(err=(lpdd->CreateSurface(&ddsd, &lpTheSurface, NULL)));
	if (FAILED(err))
	{
		ErrorLogger::Writeln("Could not create a surface");
		ErrorLogger::Writeln(*pszFilename);
		ErrorLogger::Writeln(MyDrawEngine::ErrorString(err));

		  return FAILURE;
	}

	// Set colour key to 0
	DDCOLORKEY ddColourKey;
	ddColourKey.dwColorSpaceHighValue=0;
	ddColourKey.dwColorSpaceLowValue=0;

	lpTheSurface->SetColorKey(DDCKEY_SRCBLT, &ddColourKey);

	// Copy the bitmap info into it.

	if (FAILED(lpTheSurface->GetDC(&surfaceDC)))
	{
		  OutputDebugString("Couldn't get DC of bitmap\n");
		  return FAILURE;
	}


	StretchBlt(surfaceDC,0,0,width,height,hdcImage,0,0,width, height, SRCCOPY);
	lpTheSurface->ReleaseDC(surfaceDC);

	DeleteDC(hdcImage);

	bytesPerPixel = MyDrawEngine::GetInstance()->bytesPerPixel;

	return SUCCESS;
}

ErrorType MyPicture::MakeBlank(int width, int height, int flags)
{

	// Check MyDrawEngine has been started
	if(MyDrawEngine::GetInstance()==NULL)
	{
		ErrorLogger::Writeln("Can't make blank MyPicture - draw engine not initialised\n");
		  return FAILURE;
	}
	DDSURFACEDESC2 ddsd;				// An "order form" for the surface

	// create the DDraw surface
	
	this->height=height;
	this->width=width;

	// Fill in the order form
	memset(&ddsd,0,sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags=DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT;		// Set capablilties
	ddsd.dwHeight	=this->height;						// Set height
	ddsd.dwWidth	=this->width;						// Set width
	ddsd.ddsCaps.dwCaps=DDSCAPS_OFFSCREENPLAIN|flags;	// Set as offscreen surface, also any
														// flags requested by call - such as
														// a request for video memory storage
	// Create the surface

	int err;
	(err=(lpdd->CreateSurface(&ddsd, &lpTheSurface, NULL)));
	if (err!=DD_OK)
	{
		  OutputDebugString("Couldn't funk up theSurface\n");
		  if (err==DDERR_INVALIDOBJECT)
			  	OutputDebugString("Invalid object\n");
		  if (err==DDERR_INVALIDCAPS)
			  	OutputDebugString("Invalid caps\n");
		  if (err==DDERR_INVALIDPARAMS)
			  	OutputDebugString("Invalid params\n");
		  if (err==DDERR_NOCOOPERATIVELEVELSET)
			  	OutputDebugString("Coops not set\n");

		  return FAILURE;
	}

	// Set colour key to 0
	DDCOLORKEY ddColourKey;
	ddColourKey.dwColorSpaceHighValue=0;
	ddColourKey.dwColorSpaceLowValue=0;

	lpTheSurface->SetColorKey(DDCKEY_SRCBLT, &ddColourKey);

	// Blank the surface with black pixels

	RECT tempRect;
	tempRect.top=0;
	tempRect.bottom=height-1;
	tempRect.left=0;
	tempRect.right=width-1;

	FillRect(tempRect, 0);

	bytesPerPixel = MyDrawEngine::GetInstance()->bytesPerPixel;

	return SUCCESS;

}	// End function


ErrorType MyPicture::Blit(RECT& destinationRect, RECT& sourceRect, MyPicture *sourcePicture)
{
	// Adjust the sourceRect to make sure it does not go beyond the size of the sourcePicture
	if (sourceRect.left<0)	sourceRect.left		= 0;
	if (sourceRect.top<0)	sourceRect.top		= 0;
	if (sourceRect.right>sourcePicture->width)		sourceRect.right	= sourcePicture->width;
	if (sourceRect.bottom>sourcePicture->height)	sourceRect.bottom	= sourcePicture->height;

	// Adjust the destinationRect to make sure it does not go beyond the size of this MyPicture	
	if (destinationRect.left<0) destinationRect.left=0;
	if (destinationRect.top<0) destinationRect.top=0;
	if (destinationRect.right<0) destinationRect.right=width;
	if (destinationRect.bottom<0) destinationRect.bottom=height;

	// Attempt to blit and return failure if DDraw reports failure
	HRESULT err=lpTheSurface->Blt(&destinationRect, sourcePicture->lpTheSurface, &sourceRect, (DDBLT_WAIT | DDBLT_KEYSRC),NULL);
	if (FAILED(err))
	{
		ErrorLogger::Writeln("Blit failed");
		ErrorLogger::Writeln(MyDrawEngine::ErrorString(err));
		if(FAILED(sourcePicture->Validate()))
		{
			ErrorLogger::Writeln("Failed to restore.");
			return FAILURE;
		}
		else
		{
			ErrorLogger::Writeln("Restored image.");
		}
	}

	return SUCCESS;
}	// End function

// **************************************************************

inline ErrorType MyPicture::DrawPoint(int x, int y, int colour)
{
	*(ULONG*)(videoAddress+x*bytesPerPixel+y*memoryPitch)=colour;
	return SUCCESS;
}

// *************************************************************

int MyPicture::GetPoint(int x, int y)
{
	return *(ULONG*)(videoAddress+x*bytesPerPixel+y*memoryPitch);
}

// **************************************************************

inline ErrorType MyPicture::DrawLine(int x1, int y1, int x2, int y2, int colour)
{
	// Set up variables
	int i, deltax, deltay, numpixels,
		d, dinc1, dinc2,
		x, xinc1, xinc2,
		y, yinc1, yinc2;

  deltax = abs(x2 - x1);
  deltay = abs(y2 - y1);

  // Which variable is the independant one?
  if (deltax >= deltay)
  {
      // x is independent variable 
      numpixels = deltax + 1;
      d = (2 * deltay) - deltax;
      dinc1 = deltay << 1;
      dinc2 = (deltay - deltax) << 1;
      xinc1 = 1;
      xinc2 = 1;
      yinc1 = 0;
      yinc2 = 1;
  }
  else
  {
      // y is independent variable 
      numpixels = deltay + 1;
      d = (2 * deltax) - deltay;
      dinc1 = deltax << 1;
      dinc2 = (deltax - deltay) << 1;
      xinc1 = 0;
      xinc2 = 1;
      yinc1 = 1;
      yinc2 = 1;
  }

  // Make sure x and y move in the right directions 
  if (x1 > x2)
  {
      xinc1 = - xinc1;
      xinc2 = - xinc2;
	}
if (y1 > y2)
{
      yinc1 = - yinc1;
      yinc2 = - yinc2;
}

  // Start drawing  
  x = x1;
  y = y1;

  // Draw the pixels 
  for (i = 0;i< numpixels;i++)
  {
      DrawPoint(x, y, colour);	// Plot the point
      if (d < 0 )				// Increment the fixed variable and count up the time for next increment of free variable
	  {
          d = d + dinc1;	
          x = x + xinc1;
          y = y + yinc1;
	  }
      else
	  {
          d = d + dinc2;		// Increase the free variable
          x = x + xinc2;
          y = y + yinc2;
	}
  }
  return SUCCESS;
}
// *************************************************************

ErrorType MyPicture::Lock()
{
	if (!lpTheSurface)
	{
		ErrorLogger::Write("Couldn't lock a secondary surface - surface not found.");
	   return FAILURE;
	}
	// lock the surface
	memset(&ddsd,0,sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	HRESULT err =lpTheSurface->Lock(NULL,&ddsd,DDLOCK_WAIT |
		DDLOCK_SURFACEMEMORYPTR,NULL);
	if (FAILED(err))
	{
		ErrorLogger::Writeln("Couldn't lock a secondary surface.");
		ErrorLogger::Writeln(MyDrawEngine::ErrorString(err));
		return FAILURE;
	}

	videoAddress	= (USHORT*)ddsd.lpSurface;
	memoryPitch		= ddsd.lPitch;
	
	return SUCCESS;
}

// ******************************************************************

ErrorType MyPicture::Unlock()
{
	// check actually exists
	if (!lpTheSurface)
	   return FAILURE;

	// unlock it
	if FAILED(lpTheSurface->Unlock(NULL))
		return FAILURE;

	// return success
	return SUCCESS;
}

// ****************************************************************

ErrorType MyPicture::FillRect(RECT& destinationRect, int colour)
{
	// to do: clip this rectangle - or is it done by the DDclipper?

	DDBLTFX ddbltfx;	// FX order form

	memset(&ddbltfx,0,sizeof(ddbltfx));
	ddbltfx.dwSize=sizeof(ddbltfx);


	// set FX to the colour requested
	ddbltfx.dwFillColor = colour; 


	// Fill in the surface
	lpTheSurface->Blt(&destinationRect, 
			   NULL,							// nothing to copy from            
			   NULL,							// nothing to copy from
			   DDBLT_COLORFILL | DDBLT_WAIT,	// fill and wait for complete
			   &ddbltfx);						// the order form

	return SUCCESS;
}	// End function

