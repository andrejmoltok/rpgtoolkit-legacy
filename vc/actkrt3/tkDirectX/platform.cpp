//------------------------------------------------------------------------
//All contents copyright 2003, 2004, Christopher Matthews or Contributors
//All rights reserved.  YOU MAY NOT REMOVE THIS NOTICE.
//Read LICENSE.txt for licensing info
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// DirectX interface
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Inclusions
//------------------------------------------------------------------------
#include "platform.h"							// Symbols for this file
#include "..\dmutil\include\dmutil.h"			// DirectMusic foundation classes
#include "..\dmutil\include\dxutil.h"			// DirectX utilities

//------------------------------------------------------------------------
// Globals
//------------------------------------------------------------------------
bool g_bUseDirectX = false;						// Using DirectX?
HWND ghWndMain = NULL;							// Handle to host window
HINSTANCE ghInstance;							// Handle of instance to app
HDC ghDCLocked = NULL;							// HDC of locked surface
DXINFO gDXInfo;									// DirectX info structure.
HRGN g_Clipper = NULL;							// Clipping region
CGDICanvas* g_pBackBuffer = NULL;				// Non-DirectX backbuffer
static CMusicManager* g_pMusicManager = NULL;	// DirectMusic manager
static CMusicSegment* g_pMusicSegment = NULL;	// A DirectMusic segment

//------------------------------------------------------------------------
// Initiate DirectDraw
//------------------------------------------------------------------------
int APIENTRY DXInitGfxMode(int hostHwnd, int nScreenX, int nScreenY, int nUseDirectX, int nColorDepth, int nFullScreen)
{

	// Initiate the gfx engine
	return InitGraphicsMode(HWND(hostHwnd), nScreenX, nScreenY, bool(nUseDirectX), nColorDepth, bool(nFullScreen));

}

//------------------------------------------------------------------------
// Initiate DirectMusic
//------------------------------------------------------------------------
int APIENTRY DXInitMusic(HWND hWnd)
{

	// Create the music manager
	g_pMusicManager = new CMusicManager();

	// Initialize the music manager
	return SUCCEEDED(g_pMusicManager->Initialize(hWnd));

}

//------------------------------------------------------------------------
// Kill DirectMusic
//------------------------------------------------------------------------
void APIENTRY DXKillMusic(void)
{

	// Delete the music manager
	SAFE_DELETE(g_pMusicManager);

	// Delete the music segment
	SAFE_DELETE(g_pMusicSegment);

}

//------------------------------------------------------------------------
// Play a MIDI
//------------------------------------------------------------------------
void APIENTRY DXPlayMidi(TCHAR* strFileName, VARIANT_BOOL bLoop)
{

	// If DirectMusic hasn't been initiated
	if (!g_pMusicManager)
	{

		// Cannot play a MIDI
		return;

	}

	// Stop any current segment
	DXStopMidi();

	// Create a new segment
	if (FAILED(g_pMusicManager->CreateSegmentFromFile(&g_pMusicSegment, strFileName, TRUE, TRUE)))
	{

		// Could not load the file
		return;

	}

	if (bLoop)
	{

		// Loop the segment
		g_pMusicSegment->SetRepeats(DMUS_SEG_REPEAT_INFINITE);

	}
	else
	{

		// Do not loop the segment
		g_pMusicSegment->SetRepeats(0);

	}

	// Play the segment
	g_pMusicSegment->Play();

}

//------------------------------------------------------------------------
// Stop the playing MIDI
//------------------------------------------------------------------------
void APIENTRY DXStopMidi(void)
{

	// If a segment exists
	if (g_pMusicSegment)
	{

		// Stop the segment
		g_pMusicSegment->Stop();

		// Delete the segment
		SAFE_DELETE(g_pMusicSegment);

		// Collect any garbage
		g_pMusicManager->CollectGarbage();

	}

}

//------------------------------------------------------------------------
// Determine if a MIDI is playing
//------------------------------------------------------------------------
BOOL APIENTRY DXIsMidiPlaying(void)
{

	// If a segment exists
	if (g_pMusicSegment)
	{

		// Return if the MIDI is playing
		return g_pMusicSegment->IsPlaying();

	}
	else
	{

		// No segment - MIDI cannot be playing
		return FALSE;

	}

}

//------------------------------------------------------------------------
// Kill DirectDraw
//------------------------------------------------------------------------
int APIENTRY DXKillGfxMode()
{
	return KillGraphicsMode();
}

//------------------------------------------------------------------------
// Plot a pixel
//------------------------------------------------------------------------
int APIENTRY DXDrawPixel(int x, int y, long clr)
{
	return DrawPixel(x, y, clr);
}

//------------------------------------------------------------------------
// Page flip
//------------------------------------------------------------------------
int APIENTRY DXRefresh(CNV_HANDLE cnvHandle)
{
	return Refresh((CGDICanvas*)cnvHandle);
}

//------------------------------------------------------------------------
// Lock the screen
//------------------------------------------------------------------------
int APIENTRY DXLockScreen()
{
	ghDCLocked = OpenDC();
	return int(ghDCLocked);
}

//------------------------------------------------------------------------
// Unlock the screen
//------------------------------------------------------------------------
int APIENTRY DXUnlockScreen()
{
	return UnlockScreen();
}

//------------------------------------------------------------------------
// Draw a canvas
//------------------------------------------------------------------------
int APIENTRY DXDrawCanvas(CNV_HANDLE cnv, int x, int y, long lRasterOp)
{
	CGDICanvas* pCnv = (CGDICanvas*)cnv;
	return DrawCanvas(pCnv, x, y, lRasterOp);
}

//------------------------------------------------------------------------
// Draw a canvas transparently
//------------------------------------------------------------------------
int APIENTRY DXDrawCanvasTransparent(CNV_HANDLE cnv, int x, int y, long crTransparentColor)
{
	CGDICanvas* pCnv = (CGDICanvas*)cnv;
	return DrawCanvasTransparent(pCnv, x, y, crTransparentColor);
}

//------------------------------------------------------------------------
// Draw a canvas translucently
//------------------------------------------------------------------------
int APIENTRY DXDrawCanvasTranslucent(CNV_HANDLE cnv, int x, int y, double dIntensity, long crUnaffectedColor, long crTransparentColor)
{
	CGDICanvas* pCnv = (CGDICanvas*)cnv;
	return DrawCanvasTranslucent(pCnv, x, y, dIntensity, crUnaffectedColor, crTransparentColor);
}

//------------------------------------------------------------------------
// Clear the screen to a color
//------------------------------------------------------------------------
int APIENTRY DXClearScreen(long crColor)
{
	ClearScreen(crColor);
	return 1;
}

//------------------------------------------------------------------------
// Draw text
//------------------------------------------------------------------------
int APIENTRY DXDrawText(int x, int y, char* strText, char* strTypeFace, int size, long clr, int bold, int italics, int underline, int centred, int outlined)
{
	return DrawText(x, y, strText, strTypeFace, size, clr, (bool)bold, (bool)italics, (bool)underline, (bool)centred, (bool)outlined);
}

//------------------------------------------------------------------------
// Draw part of a canvas
//------------------------------------------------------------------------
int APIENTRY DXDrawCanvasPartial(CNV_HANDLE cnv, int destx, int desty, int srcx, int srcy, int width, int height, long lRasterOp)
{
	CGDICanvas* pCnv = (CGDICanvas*)cnv;
	return DrawCanvasPartial(pCnv, destx, desty, srcx, srcy, width, height, lRasterOp);
}

//------------------------------------------------------------------------
// Draw part of a canvas transparently
//------------------------------------------------------------------------
int APIENTRY DXDrawCanvasTransparentPartial(CNV_HANDLE cnv, int destx, int desty, int srcx, int srcy, int width, int height, long crTransparentColor)
{
	CGDICanvas* pCnv = (CGDICanvas*)cnv;
	return DrawCanvasTransparentPartial(pCnv, destx, desty, srcx, srcy, width, height, crTransparentColor);
}

//------------------------------------------------------------------------
// Copy the back buffer to a canvas
//------------------------------------------------------------------------
int APIENTRY DXCopyScreenToCanvas(CNV_HANDLE cnv)
{
	CGDICanvas* pCnv = (CGDICanvas*)cnv;
	return CopyScreenToCanvas(pCnv);
}

//------------------------------------------------------------------------
// Initiate the graphics engine
//------------------------------------------------------------------------
inline bool InitGraphicsMode(HWND handle, int nWidth, int nHeight, bool bUseDirectX, long nColorDepth, bool bFullScreen)
{
	//Initiate the DirectX info structure
	g_bUseDirectX = bUseDirectX;
	gDXInfo.lpdd = NULL;
	gDXInfo.lpddsSecond = NULL;
	gDXInfo.windowedMode.lpddClip = NULL;
	gDXInfo.bFullScreen = false;
	gDXInfo.nColorDepth = 0;

	//Store the main window's handle
	ghWndMain = handle;

	if (!ghWndMain) return false;

	//initialize direct draw, but only of we're using DirectX
	if(g_bUseDirectX)
	{

		gDXInfo = InitDirectX(ghWndMain, nWidth, nHeight, nColorDepth, bFullScreen);

		if (!gDXInfo.lpddsPrime)
		{
			//problems initing...
			KillGraphicsMode();
			return false;
		}

	}
	else
	{
		//make a back buffer...
		g_pBackBuffer = CreateCanvas(nWidth, nHeight, false);

		//clear back buffer...
		DrawFilledRect(0, 0, nWidth, nHeight, 0);
	}

	return true;
}

//------------------------------------------------------------------------
// Initiate DirectX
//------------------------------------------------------------------------
inline DXINFO InitDirectX(HWND hWnd, int nWidth, int nHeight, long nColorDepth, bool bFullScreen)
{

	//Initiate the return structure
	DXINFO dxInfo;
	dxInfo.lpdd = NULL;
	dxInfo.lpddsPrime = NULL;
	dxInfo.lpddsSecond = NULL;
	dxInfo.windowedMode.lpddClip = NULL;
	dxInfo.nWidth = nWidth;
	dxInfo.nHeight = nHeight;
	dxInfo.bFullScreen = bFullScreen;
	gDXInfo.nColorDepth = nColorDepth;

	//create the direct draw object...
	if (FAILED(DirectDrawCreateEx(NULL, (LPVOID *)&dxInfo.lpdd, IID_IDirectDraw7, NULL))) return dxInfo;

	//initiate the primary surface structure
	DDSURFACEDESC2 ddsd;
	memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);

	if (bFullScreen)
	{
		//full-screen mode
		if (FAILED(dxInfo.lpdd->SetCooperativeLevel(hWnd,DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT))) return dxInfo;
		if (FAILED(dxInfo.lpdd->SetDisplayMode(nWidth, nHeight, nColorDepth, 0, 0))) return dxInfo;
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.dwBackBufferCount = 1;		//Make a *real* backbuffer
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP;
	}
	else
	{
		//windowed mode
		if (FAILED(dxInfo.lpdd->SetCooperativeLevel(hWnd,DDSCL_NORMAL))) return dxInfo;
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;	//this will be the primary surface
	}

	//Create the primary surface
	if (FAILED(dxInfo.lpdd->CreateSurface(&ddsd, &dxInfo.lpddsPrime, NULL))) return dxInfo;

	// create rectangles for the window and for the surface
	SetRect(&dxInfo.windowedMode.surfaceRect, 0, 0, dxInfo.nWidth, dxInfo.nHeight);

	//get the back buffer
	if (bFullScreen)
	{
		ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
		if (FAILED(dxInfo.lpddsPrime->GetAttachedSurface(&ddsd.ddsCaps, &dxInfo.lpddsSecond))) return dxInfo;
	}
	else
	{

		// create clipper
		dxInfo.lpdd->CreateClipper(0, &dxInfo.windowedMode.lpddClip, NULL);

		// set clipper window
		dxInfo.windowedMode.lpddClip->SetHWnd(0, hWnd);

		// attach clipper
		dxInfo.lpddsPrime->SetClipper(dxInfo.windowedMode.lpddClip);

		//setup the effects to blt with
		memset(&dxInfo.windowedMode.bltFx, 0, sizeof(DDBLTFX));
		dxInfo.windowedMode.bltFx.dwSize = sizeof(DDBLTFX);
		dxInfo.windowedMode.bltFx.dwROP = SRCCOPY;

		memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		ddsd.dwWidth = nWidth;
		ddsd.dwHeight = nHeight;

		if (FAILED(dxInfo.lpdd->CreateSurface(&ddsd, &dxInfo.lpddsSecond, NULL)))
		{
			//not enough video memory.
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
			if (FAILED(dxInfo.lpdd->CreateSurface(&ddsd, &dxInfo.lpddsSecond, NULL))) return dxInfo;
		}

	}

	//Black out screen
	DrawFilledRect(0, 0, nWidth, nHeight, 0);

	return dxInfo;
}

//------------------------------------------------------------------------
// Kill the graphics engine
//------------------------------------------------------------------------
inline bool KillGraphicsMode()
{
	//shut down direct draw, but only if we're using it...
	if(g_bUseDirectX)
	{
		//kill clipper
		if(gDXInfo.windowedMode.lpddClip)
		{
			if (FAILED(gDXInfo.windowedMode.lpddClip->Release())) return false;;
			gDXInfo.windowedMode.lpddClip = NULL;
		}

		//kill backbuffer
		if (gDXInfo.lpddsSecond)
		{
			if FAILED(gDXInfo.lpddsSecond->Release()) return false;
			gDXInfo.lpddsSecond = NULL;
		}

		//kill primary surface...
		if(gDXInfo.lpddsPrime)
		{
			if (FAILED(gDXInfo.lpddsPrime->Release())) return false;
			gDXInfo.lpddsPrime = NULL;
		}

		//kill direct draw...
		if(gDXInfo.lpdd)
		{
			if (FAILED(gDXInfo.lpdd->Release())) return false;
			gDXInfo.lpdd = NULL;
		}

	}

	else

		//kill back buffer...
		delete g_pBackBuffer;

	return true;
}

//------------------------------------------------------------------------
// Get DC of the screen
//------------------------------------------------------------------------
inline HDC OpenDC()
{
	//first check if the screen is locked...
	if (ghDCLocked)
	{
		return ghDCLocked;
	}

	HDC hdc = 0;
	if (g_bUseDirectX)
	{
		//use DirectX
		if (gDXInfo.lpdd)
		{
			gDXInfo.lpddsSecond->GetDC(&hdc);
		}
	}
	else
	{
		if (g_pBackBuffer)
		{
			hdc = g_pBackBuffer->OpenDC();
		}
		else
		{
			hdc = GetDC(ghWndMain);
		}
	}

	return hdc;
}

//------------------------------------------------------------------------
// Close the screen's DC
//------------------------------------------------------------------------
inline void CloseDC(HDC hdc)
{
	//check if screen is locked
	if (ghDCLocked)
	{
		return;
	}

	if (g_bUseDirectX)
	{
		if (gDXInfo.lpdd && hdc)
		{
			gDXInfo.lpddsSecond->ReleaseDC(hdc);
		}
	}
	else
	{
		if (hdc)
		{
			if (g_pBackBuffer)
			{
				//don't have to release it.
				g_pBackBuffer->CloseDC(hdc);				
			}
			else
			{
				//use the GDI...
				ReleaseDC(ghWndMain, hdc);
			}
		}
	}
}

//------------------------------------------------------------------------
// Plot a pixel onto the screen
//------------------------------------------------------------------------
inline bool DrawPixel(int x, int y, long clr)
{
	HDC hdc = OpenDC();

	SetPixelV(hdc, x, y, clr);

	CloseDC(hdc);
	return true;
}

//------------------------------------------------------------------------
// Draw a line on the screen
//------------------------------------------------------------------------
inline bool DrawLine(int x1, int y1, int x2, int y2, long clr)
{
	HDC hdc = OpenDC();

	POINT point;
	HPEN brush = CreatePen(0, 1, clr);
	HGDIOBJ m = SelectObject(hdc, brush);
	MoveToEx(hdc, x1, y1, &point);
	LineTo(hdc, x2, y2);
	SelectObject(hdc, m);
	DeleteObject(brush);

	CloseDC(hdc);
	return true;
}

//------------------------------------------------------------------------
// Get the pixel at x, y
//------------------------------------------------------------------------
inline long GetPixelColor(int x, int y)
{
	HDC hdc = OpenDC();
	long lRet = GetPixel(hdc, x, y);
	CloseDC(hdc);
	return lRet;
}

//------------------------------------------------------------------------
// Draw a filled rectangle
//------------------------------------------------------------------------
inline bool DrawFilledRect(int x1, int y1, int x2, int y2, long clr)
{
	HDC hdc = OpenDC();
	RECT r;
	r.left = x1;
	r.top = y1;
	r.bottom = y2+1;
	r.right = x2+1;
	HPEN pen = CreatePen(0, 1, clr);
	HGDIOBJ l = SelectObject(hdc, pen);
	HBRUSH brush = CreateSolidBrush(clr);
	HGDIOBJ m = SelectObject(hdc, brush);
	FillRect(hdc, &r, brush);
	SelectObject(hdc, m);
	SelectObject(hdc, l);
	DeleteObject(brush);
	DeleteObject(pen);
	CloseDC(hdc);
	return true;
}

//------------------------------------------------------------------------
// Flip back buffer onto the screen
//------------------------------------------------------------------------
inline bool Refresh(CGDICanvas* cnv)
{
	if(g_bUseDirectX && gDXInfo.lpdd)
	{

		if (gDXInfo.bFullScreen)

			if (cnv)

				// Blt to a canvas
				cnv->GetDXSurface()->BltFast(0, 0, gDXInfo.lpddsSecond, &gDXInfo.windowedMode.surfaceRect, NULL);

			else

				// Page flip
				while (FAILED(gDXInfo.lpddsPrime->Flip(NULL, DDFLIP_WAIT)));

		else
		{

			// Get the point of the window outside of the title bar and border
			POINT ptPrimeBlt;
			ptPrimeBlt.x = ptPrimeBlt.y = 0;
			ClientToScreen(ghWndMain, &ptPrimeBlt);

			// Now offset the top/left of the window rect by the distance from the
			// title bar / border
			SetRect(&gDXInfo.windowedMode.destRect, 0, 0, gDXInfo.nWidth, gDXInfo.nHeight);
			OffsetRect(&gDXInfo.windowedMode.destRect, ptPrimeBlt.x, ptPrimeBlt.y);

			if (cnv)

				// Blt to a canvas
				cnv->GetDXSurface()->BltFast(0, 0, gDXInfo.lpddsSecond, &gDXInfo.windowedMode.surfaceRect, DDBLTFAST_NOCOLORKEY);

			else

				// Blt to the screen
				gDXInfo.lpddsPrime->Blt(&gDXInfo.windowedMode.destRect, gDXInfo.lpddsSecond, &gDXInfo.windowedMode.surfaceRect, DDBLT_WAIT | DDBLT_ROP, &gDXInfo.windowedMode.bltFx);

		}
	}
	else if (g_pBackBuffer)
	{
		//if offscreen buffer exists, blit it formward.
		HDC hdc = GetDC(ghWndMain);
		HDC hdcBuffer = g_pBackBuffer->OpenDC();
		BitBlt(hdc, 0, 0, g_pBackBuffer->GetWidth(), g_pBackBuffer->GetHeight(), hdcBuffer, 0, 0, SRCCOPY);
		g_pBackBuffer->CloseDC(hdcBuffer);
		ReleaseDC(ghWndMain, hdc);
	}

	return true;
}

//------------------------------------------------------------------------
// Draw text onto the screen
//------------------------------------------------------------------------
inline bool DrawText(int x, int y, std::string strText, std::string strTypeFace, int size, long clr, bool bold, bool italics, bool underline, bool centred, bool outlined)
{

  int nWeight = 0;
  if (bold)
	{
		nWeight = FW_BOLD;
	}
	else
	{
		nWeight = FW_NORMAL;
	}

	HFONT hFont = 0;
	hFont = CreateFont(size, 0, 0, 0, nWeight, italics, underline, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, strTypeFace.c_str());
	if (hFont)
	{
		HDC hdc = OpenDC();

		SetBkMode(hdc, TRANSPARENT);
		
		HGDIOBJ hOld = SelectObject(hdc, hFont);

		SetTextColor(hdc, clr);

		if ( centred )
		{
			SetTextAlign(hdc, TA_CENTER);
		} 
		else 
		{
			SetTextAlign(hdc, TA_LEFT);
		}


		HGDIOBJ hOldBrush,hNewBrush;
		hNewBrush = CreateSolidBrush(clr);
		hOldBrush = SelectObject(hdc, hNewBrush);

		if (outlined)
		{
			BeginPath(hdc);
			TextOut(hdc, x, y, strText.c_str(), strText.length());
			EndPath(hdc);
        
			SetBkColor(hdc, clr);
			SetBkMode(hdc, OPAQUE);
			StrokeAndFillPath(hdc);
		} 
		else
		{
			TextOut(hdc, x, y, strText.c_str(), strText.length());
		}

		SelectObject(hdc, hOldBrush);
		DeleteObject(hNewBrush);

		//put the old font back...
		SelectObject(hdc, hOld);
		DeleteObject(hFont);

		CloseDC(hdc);
	}
	else
	{
		return false;
	}

	return true;
}

//------------------------------------------------------------------------
// Make a back buffer with GDI
//------------------------------------------------------------------------
inline CGDICanvas* CreateCanvas(int nWidth, int nHeight, bool bUseDX)
{
	CGDICanvas* pToRet = NULL;

	//get dc of screen...
	HDC hdc = GetDC(ghWndMain);

	pToRet = new CGDICanvas();
	if (pToRet)
	{
		pToRet->CreateBlank(hdc, nWidth, nHeight, bUseDX);
	}

	//release dc...
	if (hdc)
	{
		//use the GDI...
		ReleaseDC(ghWndMain, hdc);
	}

	return pToRet;
}

//------------------------------------------------------------------------
// Draw a canvas onto the back buffer
//------------------------------------------------------------------------
inline bool DrawCanvas(CGDICanvas* pCanvas, int x, int y, long lRasterOp)
{
	return DrawCanvasPartial(pCanvas, x, y, 0, 0, pCanvas->GetWidth(), pCanvas->GetHeight(), lRasterOp);
}

//------------------------------------------------------------------------
// Draw a canvas with transparency
//------------------------------------------------------------------------
inline bool DrawCanvasTransparent(CGDICanvas* pCanvas, int x, int y, long crTransparentColor)
{
	return DrawCanvasTransparentPartial(pCanvas, x, y, 0, 0, pCanvas->GetWidth(), pCanvas->GetHeight(), crTransparentColor);
}

//------------------------------------------------------------------------
// Draw a canvas with translucency
//------------------------------------------------------------------------
inline bool DrawCanvasTranslucent(CGDICanvas* pCanvas, int x, int y, double dIntensity, long crUnaffectedColor, long crTransparentColor)
{
	bool bToRet = false;
	if (pCanvas)
	{
		if(g_bUseDirectX)
		{
			bToRet = pCanvas->BltTranslucent(gDXInfo.lpddsSecond, x, y, dIntensity, crUnaffectedColor, crTransparentColor);
		}
		else
		{
			//use GDI
			HDC hdc = OpenDC();

			int n = pCanvas->BltTranslucent(hdc, x, y, dIntensity, crUnaffectedColor, crTransparentColor);
			if (n)
			{
				bToRet = true;
			}
			
			CloseDC(hdc);
		}
	}

	return bToRet;
}

//------------------------------------------------------------------------
// Partially draw a canvas
//------------------------------------------------------------------------
inline bool DrawCanvasPartial(CGDICanvas* pCanvas, int destx, int desty, int srcx, int srcy, int width, int height, long lRasterOp)
{
	bool bToRet = false;
	if (pCanvas)
	{
		if(g_bUseDirectX)
		{
			//use DirectX
			int n = pCanvas->BltPart(gDXInfo.lpddsSecond, destx, desty, srcx, srcy, width, height, lRasterOp);
			if (n)
			{
				bToRet = true;
			}
		}
		else
		{
			//use GDI
			HDC hdc = OpenDC();

			int n = pCanvas->BltPart(hdc, destx, desty, srcx, srcy, width, height, lRasterOp);
			if (n)
			{
				bToRet = true;
			}
			
			CloseDC(hdc);
		}
	}

	return bToRet;
}

//------------------------------------------------------------------------
// Draw part of a canvas with transparency
//------------------------------------------------------------------------
inline bool DrawCanvasTransparentPartial(CGDICanvas* pCanvas, int destx, int desty, int srcx, int srcy, int width, int height, long crTransparentColor)
{
	bool bToRet = false;
	if (pCanvas)
	{
		if(g_bUseDirectX)
		{
			//use DirectX
			int n = pCanvas->BltTransparentPart(gDXInfo.lpddsSecond, destx, desty, srcx, srcy, width, height, crTransparentColor);
			if (n)
			{
				bToRet = true;
			}
		}
		else
		{
			HDC hdc = OpenDC();

			int n = pCanvas->BltTransparentPart(hdc, destx, desty, srcx, srcy, width, height, crTransparentColor);
			if (!n)
			{
				bToRet = true;
			}
			
			CloseDC(hdc);
		}
	}

	return bToRet;
}

//------------------------------------------------------------------------
// Copy contents of screen to a canvas
//------------------------------------------------------------------------
inline BOOL CopyScreenToCanvas(CGDICanvas* pCanvas)
{
	BOOL nRet = FALSE;
	if (pCanvas)
	{
		HDC hdcSrc = OpenDC();
		HDC hdcDest = pCanvas->OpenDC();
		nRet = BitBlt(hdcDest, 0, 0, pCanvas->GetWidth(), pCanvas->GetHeight(), hdcSrc, 0, 0, SRCCOPY);
		pCanvas->CloseDC(hdcDest);
		CloseDC(hdcSrc);
	}
	return nRet;
}

//------------------------------------------------------------------------
// Clear the screen to a color
//------------------------------------------------------------------------
inline void ClearScreen(long crColor)
{
	HDC hdc = OpenDC();
	RECT r;
	r.left = 0;
	r.top = 0;

	if(g_bUseDirectX)
	{
		r.right = gDXInfo.nWidth+1;
		r.bottom = gDXInfo.nHeight+1;
	}
	else
	{
		r.right = g_pBackBuffer->GetWidth() + 1;
		r.bottom = g_pBackBuffer->GetHeight() + 1;
	}

	HPEN pen = CreatePen(0, 1, crColor);
	HGDIOBJ l = SelectObject(hdc, pen);
	HBRUSH brush = CreateSolidBrush(crColor);
	HGDIOBJ m = SelectObject(hdc, brush);

	FillRect(hdc, &r, brush);

	SelectObject(hdc, m);
	SelectObject(hdc, l);
	DeleteObject(brush);
	DeleteObject(pen);

	CloseDC(hdc);
}

//------------------------------------------------------------------------
// Unlock the screen
//------------------------------------------------------------------------
inline bool UnlockScreen()
{
	HDC hdc = ghDCLocked;
	ghDCLocked = NULL;
	CloseDC(hdc);
	return true;
}
