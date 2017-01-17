// mydrawengine.cpp	 4/4/06 Version 10.1	
// 


// Version 10		8/12/03
// The definition file for the methods in MyDrawEngine - declared in mydrawengine.h
// Version 10 -added error loggers and updated to IDirectDraw7
// 
// Version 10.1
// Fixed silly negatives on DrawDouble and DrawInt

#include "mydrawengine.h"

MyDrawEngine* MyDrawEngine::instance=NULL;

// Constructor ********************************************************

MyDrawEngine* MyDrawEngine::Start(int width, int height, int depth, HWND hwnd)
{
	if(instance)		// If an instance already exists
	{
		instance->Terminate();
	}
	instance= new MyDrawEngine(width, height, depth, hwnd);

	return instance;
}

MyDrawEngine* MyDrawEngine::GetInstance()
{
	return instance;
}

ErrorType MyDrawEngine::Terminate()
{
	if(instance)
	{
		delete instance;
		instance=NULL;
		return SUCCESS;
	}
	else
		return FAILURE;
}

MyDrawEngine::MyDrawEngine(int width, int height, int bitsPerPixel, HWND hwnd)
{
	// Set attributes
	bytesPerPixel = bitsPerPixel/8;
	ZeroMemory(&ddsd, sizeof(ddsd));
	lpBackBuffer=NULL;
	lpPrimarySurface=NULL;
	memoryPitch=0;
	screenHeight=height;
	screenWidth=width;
	videoAddress=NULL;

	HRESULT err;

	// to do: add defensive programming
	err=DirectDrawCreateEx(0,(void**)&lpdd,IID_IDirectDraw7,0);
	if (FAILED(err))
	{
		ErrorLogger::Writeln("Could not connect to IDirectDraw7");
		ErrorLogger::Writeln(ErrorString(err));
		lpdd=NULL;
		return;		// No point going any further
	}
	
	// set cooperation level 
	err=lpdd->SetCooperativeLevel(hwnd,
			  DDSCL_ALLOWMODEX | DDSCL_FULLSCREEN | 
			  DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT);
	if (FAILED(err))
	{
		ErrorLogger::Writeln("Could not set cooperative level");
		ErrorLogger::Writeln(ErrorString(err));
		lpdd->Release();
		lpdd=NULL;
		return;		// No point going any further
	}

	// set display mode
	err=lpdd->SetDisplayMode(width,height,bytesPerPixel*8,0,0);
	if (FAILED(err))
	{
		ErrorLogger::Writeln("Could not set display mode");
		ErrorLogger::Writeln(ErrorString(err));
		lpdd->Release();
		lpdd=NULL;
		return;		// No point going any further
	}

	// Create the primary surface

	// First, make out an "order form" for the surface
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
	ddsd.dwBackBufferCount = 1;

	// Now get the primarySurface
	
	err=lpdd->CreateSurface(&ddsd,&lpPrimarySurface,NULL);
	if(FAILED(err))
	{
		ErrorLogger::Writeln("Failed to create primary surface");
		ErrorLogger::Writeln(ErrorString(err));
		lpdd->Release();
		lpdd=NULL;
		return;		// No point going any further
	}

	// Now get the buffer
	
	DDSCAPS2 ddsCaps;			// Generic surface capabilities struct
	ZeroMemory(&ddsCaps, sizeof(ddsCaps));
	ddsCaps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_FLIP;

	err=lpPrimarySurface->GetAttachedSurface(&ddsCaps,&lpBackBuffer);

	if(FAILED(err))
	{
		ErrorLogger::Writeln("Failed to obtain back surface");
		ErrorLogger::Writeln(ErrorString(err));
		lpdd->Release();
		lpdd=NULL;
		return;		// No point going any further
	}

	// Set up a standard clipper. User can always override.
	RECT rgCliprects[1];
	rgCliprects[0].left=0;
	rgCliprects[0].right=screenWidth;
	rgCliprects[0].top=0;
	rgCliprects[0].bottom=screenHeight;
	SetClipper(1,rgCliprects);

	// clear primary and secondary surfaces
	ClearBackBuffer();
	Flip();
	ClearBackBuffer();
}

// *******************************************************************

MyDrawEngine::~MyDrawEngine()
{
	// Just make damn sure that directx is released
	Release();
}



// *******************************************************************

ErrorType MyDrawEngine::WriteText(int x, int y, const char text[])
{
	HDC hdc;
		HRESULT err=lpBackBuffer->GetDC(&hdc);

	if(FAILED(err))
	{
		ErrorLogger::Writeln("Could not get device context to write text");
		ErrorLogger::Writeln(ErrorString(err));
		return FAILURE;
	}


	SetTextColor(hdc, RGB(255,255,255));

	SetBkMode(hdc, TRANSPARENT);

	TextOut(hdc, x,y, text, (int)strlen(text));


	err=lpBackBuffer->ReleaseDC(hdc);
	if(FAILED(err))
	{
		ErrorLogger::Writeln("Could not release a device context after writing text.");
		ErrorLogger::Writeln(ErrorString(err));
		return FAILURE;
	}

	return SUCCESS;	
}

// **********************************************************

ErrorType MyDrawEngine::WriteDouble(double num, int x, int y)
{
	char text[25];	// Big enough, even for a 64-bit integer, plus 3 s.f.
	if(num<=-0.001)
	{
		text[0]='-';
		num=-num;
	}
	else
	{
		text[0]=' ';
	}

	// Negatives dealt with. Num is positive.

	int iNum=int(num);	// Get the integer for the pre-decimal place part

	int dig=1;	// Digits written

	// Work out digits before the decimal
	if(iNum==0)			// For zero
	{
		text[1] = '0';
		dig++;
	}
	else				// For non-zero
	{
		int temp = iNum;

		while (temp>0)		// Count the digits
		{
			++dig;
			temp/=10;
		}

		for(int i= dig;i>1;i--)			// Enter the letters, working up the digits
		//for(int i=3;i>0;i--)
		{
			text[i-1] = (iNum%10)+'0';
			iNum/=10;
		}

	}
	// Now add the decimal part

	iNum = int(num*1000)%1000;		// Get the 3 sig figs

	if(iNum>0)
	{
		text[dig]='.';

		dig++;

		while(iNum>0)
		{
			text[dig]=(iNum/100)+'0';

			iNum = (iNum*10)%1000;

			dig++;
		}
	
	}

	text[dig]='\0';

	

	return WriteText(x,y, text);
}

// **********************************************************
ErrorType MyDrawEngine::WriteInt(int num, int x, int y)
{
	if(num==0) return WriteText(x,y, "0");	// Special case
	char text[21];	// Big enough, even for a 64-bit integer
	int dig=0;
	int temp = num;
	if (temp<0) temp = -temp;
	while (temp>0)		// Count the digits
	{
		dig++;
		temp/=10;
	}
	temp = num;
	if (temp<0) 
	{
		dig++;
		temp = -temp;
	}

	text[dig]='\0';

	for(int i= dig;i>0;i--)
	{
		text[i-1] = temp%10+'0';
		temp/=10;
	}
	if(num<0) text[0] ='-';

	return WriteText(x,y, text);
}

// *******************************************************************

ErrorType MyDrawEngine::ClearBackBuffer()
{
	DDBLTFX ddbltfx; // FX order form

	// clear out the structure and set the size field 
	memset(&ddbltfx,0,sizeof(ddbltfx));
	ddbltfx.dwSize=sizeof(ddbltfx);

	// Put colour wanted in order form
	ddbltfx.dwFillColor = 0; 


	HRESULT err=lpBackBuffer->Blt(NULL,			// no destination - all
			   NULL,							// no source           
			   NULL,							// no source
			   DDBLT_COLORFILL | DDBLT_WAIT,	// fill and wait                   
			   &ddbltfx);						// order form

	if(FAILED(err))
	{
		ErrorLogger::Writeln("Could not clear the back buffer.");
		ErrorLogger::Writeln(ErrorString(err));
		return FAILURE;
	}

	return SUCCESS;
}



// *****************************************************************

ErrorType MyDrawEngine::Flip()
{
	
	HRESULT err=lpPrimarySurface->Flip(NULL, DDFLIP_WAIT);
	if(FAILED(err))
	{
		ErrorLogger::Writeln("Could not flip.");
		ErrorLogger::Writeln(ErrorString(err));
		return FAILURE;
	}
	return SUCCESS;

}

// ******************************************************************

ErrorType MyDrawEngine::LockBackSurface()
{

	if (!lpBackBuffer)
	   return FAILURE;

	// lock the surface
	memset(&ddsd,0,sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);

	HRESULT err=lpBackBuffer->Lock(NULL,&ddsd,DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,NULL);

	if(FAILED(err))
	{
		ErrorLogger::Writeln("Could not lock the back surface.");
		ErrorLogger::Writeln(ErrorString(err));
		return FAILURE;
	}
	videoAddress = (BYTE*) ddsd.lpSurface;
	memoryPitch = ddsd.lPitch;	// lPitch is the number of bytes occupied
	
	return SUCCESS;
}

// ********************************************************************

ErrorType MyDrawEngine::Release()
{

// release the secondary surface
if (lpBackBuffer)
{
    lpBackBuffer->Release();
	lpBackBuffer=NULL;
}

// release the primary surface
if (lpPrimarySurface)
{
   lpPrimarySurface->Release();
   lpPrimarySurface=NULL;
}

// finally, the main dd object
if (lpdd)
{
    lpdd->Release();
	lpdd=NULL;
}

// return success
return SUCCESS;
}

// ****************************************************************


ErrorType MyDrawEngine::UnlockBackSurface()
{

	// check actually exists
	if (!lpBackBuffer)
	   return FAILURE;

	// unlock it
	HRESULT err=lpBackBuffer->Unlock(NULL);
	if(FAILED(err))
	{
		ErrorLogger::Writeln("Could not unlock the back buffer.");
		ErrorLogger::Writeln(ErrorString(err));
		return FAILURE;
	}

	// return success
	return SUCCESS;
}

// ****************************************************************


ErrorType MyDrawEngine::Validate()
{
	if (FAILED(lpPrimarySurface->IsLost()))		// See if primary surface is lost
	{	
		if(FAILED(lpPrimarySurface->Restore()))	// Try to reaquire
			return FAILURE;						// If it is still postal
		else
			return SUCCESS;						// If it recovered
	}
	return SUCCESS;								// If it never went away
}


// ***************************************************************

ErrorType MyDrawEngine::SetClipper(int numRects, LPRECT clipList)
{
	int i;								// For loops
	LPDIRECTDRAWCLIPPER theDDClipper;	// The DirectDraw clipper
	LPRGNDATA regionData;				// Pointer to region data

	// Create the clipper, and if it fails, return zero
	HRESULT err=lpdd->CreateClipper(0,&theDDClipper,NULL);
	if(FAILED(err))
	{
		ErrorLogger::Writeln("Failed to create clipper.");
		ErrorLogger::Writeln(ErrorString(err));
		return FAILURE;
	}

	// Allocate some memory for the region data, and store location in regionData
	regionData = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER)+numRects*sizeof(RECT));

	// Copy the rects into that memory area
	memcpy(regionData->Buffer, clipList, sizeof(RECT)*numRects);

	// Set up the header data in the regionData
	regionData->rdh.dwSize = sizeof(RGNDATAHEADER); // Set the size of the header
	regionData->rdh.iType  = RDH_RECTANGLES;		// Region stores data about rectangles
	regionData->rdh.nCount = numRects;				// Set number of regions
	regionData->rdh.nRgnSize = numRects*sizeof(RECT);	// Storing the size of the region data

	// Set some impossible bounding values for the regionData's bounding box
	regionData->rdh.rcBound.left   =  50000;
	regionData->rdh.rcBound.right  = -50000;
	regionData->rdh.rcBound.top	   =  50000;
	regionData->rdh.rcBound.bottom = -50000;

	// Now cycle through the rects sent to refine these values, so that they
	//  specify a box that surrounds all the individual rects
	for (i=0;i<numRects;i++)
	{
		if (clipList[i].left < regionData->rdh.rcBound.left)
			regionData->rdh.rcBound.left = clipList[i].left;
		if (clipList[i].right > regionData->rdh.rcBound.right)
			regionData->rdh.rcBound.right = clipList[i].right;
		if (clipList[i].top < regionData->rdh.rcBound.top)
			regionData->rdh.rcBound.top = clipList[i].top;
		if (clipList[i].bottom > regionData->rdh.rcBound.bottom)
			regionData->rdh.rcBound.bottom = clipList[i].bottom;
	}

	// Set the clip list (and return error if failed)
	err=theDDClipper->SetClipList(regionData,0);
	
	if(FAILED(err))
	{
		ErrorLogger::Writeln("Failed to send clip list.");
		ErrorLogger::Writeln(ErrorString(err));
		free(regionData);
		return FAILURE;
	}


	// Attach clipper to the surface
	err=lpBackBuffer->SetClipper(theDDClipper);
	if(FAILED(err))
	{
		ErrorLogger::Writeln("Failed to set clipper.");
		ErrorLogger::Writeln(ErrorString(err));
		free(regionData);
		return FAILURE;
	}

	// All done
	free(regionData);
	return SUCCESS;
}	

// ****************************************************************

char* MyDrawEngine::ErrorString(HRESULT err)
{
	switch(err)
	{
	case DD_OK:
		return("The request completed successfully.");
	case DDERR_ALREADYINITIALIZED:
		return("The object has already been initialized.");
	case DDERR_BLTFASTCANTCLIP:
		return "Can't clip. A DirectDrawClipper object is attached to a source surface that has passed into a call to the IDirectDrawSurface7::BltFast method.";
	case DDERR_CANNOTATTACHSURFACE:
		return "A surface cannot be attached.";
	case DDERR_CANNOTDETACHSURFACE:
		return "A surface cannot be detached.";
	case DDERR_CANTCREATEDC:
		return "Windows cannot create a device context."; 
	case DDERR_CANTLOCKSURFACE:
		return "Can't lock a surface";
	case DDERR_CLIPPERISUSINGHWND:
		return "An attempt was made to set a clip list for a DirectDrawClipper object that is already monitoring a window handle.";
	case DDERR_COLORKEYNOTSET:
		return "The colour key is not set"; 
	case DDERR_CURRENTLYNOTAVAIL:
		return "SNAFU. No support is currently available.";
	case DDERR_DDSCAPSCOMPLEXREQUIRED :
		return "DDSCAPS_COMPLEX flag is needed";
	case DDERR_DCALREADYCREATED:
		return "A device context (DC) has already been created for this surface.";
	case DDERR_DEVICEDOESNTOWNSURFACE:
		return "The device does not own this surface"; 
	case DDERR_DIRECTDRAWALREADYCREATED:
		return "A DirectDraw object representing this driver has already been created for this process.";
	case DDERR_EXCEPTION:
		return "An exception was encountered while performing the requested operation. ";
	case DDERR_EXCLUSIVEMODEALREADYSET:
		return "An attempt was made to set the cooperative level when it was already set to exclusive.";
	case DDERR_EXPIRED:
		return "Data has expired and is therefore no longer valid.";
	case DDERR_GENERIC:
		return "There is an undefined error condition."; 
	case DDERR_HWNDALREADYSET:
		return "The DirectDraw cooperative-level window handle has already been set. It cannot be reset while the process has surfaces or palettes created. ";
	case DDERR_HWNDSUBCLASSED:
		return "DirectDraw is prevented from restoring state because the DirectDraw cooperative-level window handle has been subclassed. ";
	case DDERR_IMPLICITLYCREATED:
		return "The surface cannot be restored because it is an implicitly created surface.";
	case DDERR_INCOMPATIBLEPRIMARY:
		return "The primary surface creation request does not match the existing primary surface. ";
	case DDERR_INVALIDCAPS:
		return "One or more of the capability bits passed to the callback function are incorrect. ";
	case DDERR_INVALIDCLIPLIST :
		return "DirectDraw does not support the provided clip list. ";
	case DDERR_INVALIDDIRECTDRAWGUID :
		return "The globally unique identifier (GUID) passed to the DirectDrawCreate function is not a valid DirectDraw driver identifier. ";
	case DDERR_INVALIDMODE :
		return "DirectDraw does not support the requested mode. ";
	case DDERR_INVALIDOBJECT :
		return "DirectDraw received a pointer that was an invalid DirectDraw object. ";
	case DDERR_INVALIDPARAMS :
		return "One or more of the parameters passed to the method are incorrect. ";
	case DDERR_INVALIDPIXELFORMAT :
		return "The pixel format was invalid as specified. ";
	case DDERR_INVALIDPOSITION :
		return "The position of the overlay on the destination is no longer legal. ";
	case DDERR_INVALIDRECT :
		return "The provided rectangle was invalid. ";
	case DDERR_INVALIDSTREAM :
		return "The specified stream contains invalid data. ";
	case DDERR_INVALIDSURFACETYPE :
		return "The surface was of the wrong type. ";
	case DDERR_LOCKEDSURFACES :
		return "One or more surfaces are locked, causing the failure of the requested operation. ";
	case DDERR_MOREDATA :
		return "There is more data available than the specified buffer size can hold. ";
	case DDERR_NEWMODE :
		return "New for DirectX 7.0. When IDirectDraw7::StartModeTest is called with the DDSMT_ISTESTREQUIRED flag, it may return this value to denote that some or all of the resolutions can and should be tested. IDirectDraw7::EvaluateMode returns this value to indicate that the test has switched to a new display mode. ";
	case DDERR_NO3D :
		return "No 3-D hardware or emulation is present. ";
	case DDERR_NOALPHAHW :
		return "No alpha-acceleration hardware is present or available, causing the failure of the requested operation. ";
	case DDERR_NOBLTHW :
		return "No blitter hardware is present. ";
	case DDERR_NOCLIPLIST :
		return "No clip list is available. ";
	case DDERR_NOCLIPPERATTACHED :
		return "No DirectDrawClipper object is attached to the surface object. ";
	case DDERR_NOCOLORCONVHW :
		return "No color-conversion hardware is present or available. ";
	case DDERR_NOCOLORKEY :
		return "The surface does not currently have a color key. ";
	case DDERR_NOCOLORKEYHW :
		return "There is no hardware support for the destination color key. ";
	case DDERR_NOCOOPERATIVELEVELSET :
		return "A create function was called without the IDirectDraw7::SetCooperativeLevel method. ";
	case DDERR_NODC :
		return "No device context (DC) has ever been created for this surface. ";
	case DDERR_NODDROPSHW :
		return "No DirectDraw raster-operation (ROP) hardware is available. ";
	case DDERR_NODIRECTDRAWHW :
		return "Hardware-only DirectDraw object creation is not possible; the driver does not support any hardware. ";
	case DDERR_NODIRECTDRAWSUPPORT :
		return "DirectDraw support is not possible with the current display driver. ";
	case DDERR_NOEMULATION :
		return "Software emulation is not available. ";
	case DDERR_NOEXCLUSIVEMODE :
		return "The operation requires the application to have exclusive mode, but the application does not have exclusive mode. ";
	case DDERR_NOFLIPHW :
		return "Flipping visible surfaces is not supported.  ";
	case DDERR_NOGDI :
		return "No GDI is present. ";
	case DDERR_NOHWND :
		return "Clipper notification requires a window handle, or no window handle has been previously set as the cooperative level window handle. ";
	case DDERR_NOMIRRORHW :
		return "No mirroring hardware is present or available. ";
	case DDERR_NOROTATIONHW :
		return "No rotation hardware is present or available. ";
	case DDERR_NOSTRETCHHW :
		return "There is no hardware support for stretching. ";
	case DDERR_NOTAOVERLAYSURFACE :
		return "An overlay component is called for a nonoverlay surface. ";
	case DDERR_NOTEXTUREHW :
		return "The operation cannot be carried out because no texture-mapping hardware is present or available. ";
	case DDERR_NOTFLIPPABLE :
		return "An attempt was made to flip a surface that cannot be flipped. ";
	case DDERR_NOTFOUND :
		return "The requested item was not found. ";
	case DDERR_NOTINITIALIZED :
		return "An attempt was made to call an interface method of a DirectDraw object created by CoCreateInstance before the object was initialized.  ";
	case DDERR_NOTLOCKED :
		return "An attempt was made to unlock a surface that was not locked. ";
	case DDERR_NOTPALETTIZED :
		return "The surface being used is not a palette-based surface. ";
	case DDERR_NOVSYNCHW :
		return "There is no hardware support for vertical blanksynchronized operations. ";
	case DDERR_NOZBUFFERHW :
		return "The operation to create a z-buffer in display memory or to perform a blit, using a z-buffer cannot be carried out because there is no hardware support for z-buffers. ";
	case DDERR_NOZOVERLAYHW :
		return "The overlay surfaces cannot be z-layered, based on the z-order because the hardware does not support z-ordering of overlays. ";
	case DDERR_OUTOFCAPS :
		return "The hardware needed for the requested operation has already been allocated. ";
	case DDERR_OUTOFMEMORY :
		return "DirectDraw does not have enough memory to perform the operation. ";
	case DDERR_OUTOFVIDEOMEMORY :
		return "DirectDraw does not have enough display memory to perform the operation. ";
	case DDERR_PRIMARYSURFACEALREADYEXISTS :
		return "This process has already created a primary surface. ";
	case DDERR_REGIONTOOSMALL :
		return "The region passed to the IDirectDrawClipper::GetClipList method is too small. ";
	case DDERR_SURFACEALREADYATTACHED :
		return "An attempt was made to attach a surface to another surface to which it is already attached. ";
	case DDERR_SURFACEBUSY :
		return "Access to the surface is refused because the surface is locked by another thread. ";
	case DDERR_SURFACELOST :
		return "Access to the surface is refused because the surface memory is gone. Call the IDirectDrawSurface7::Restore method on this surface to restore the memory associated with it. ";
	case DDERR_SURFACENOTATTACHED :
		return "The requested surface is not attached.  ";
	case DDERR_TOOBIGHEIGHT :
		return "The height requested by DirectDraw is too large. ";
	case DDERR_TOOBIGSIZE :
		return "The size requested by DirectDraw is too large. However, the individual height and width are valid sizes. ";
	case DDERR_TOOBIGWIDTH :
		return "The width requested by DirectDraw is too large. ";
	case DDERR_UNSUPPORTED :
		return "The operation is not supported.  ";
	case DDERR_UNSUPPORTEDMODE :
		return "The display is currently in an unsupported mode. ";
	case DDERR_VERTICALBLANKINPROGRESS :
		return "A vertical blank is in progress. ";
	case DDERR_VIDEONOTACTIVE :
		return "The video port is not active. ";
	case DDERR_WASSTILLDRAWING :
		return "The previous blit operation that is transferring information to or from this surface is incomplete. ";
}

return "No error message for this error from MyDrawEngine";
 
}

// ***************************************************************


