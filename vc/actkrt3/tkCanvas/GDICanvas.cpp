//--------------------------------------------------------------------------
// All contents copyright 2003, 2004, Christopher Matthews or Contributors
// All rights reserved. YOU MAY NOT REMOVE THIS NOTICE.
// Read LICENSE.txt for licensing info
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Inclusions
//--------------------------------------------------------------------------
#include "GDICanvas.h"		// Contains stuff for this file

//--------------------------------------------------------------------------
// Externs
//--------------------------------------------------------------------------
extern DXINFO gDXInfo;		// DirectX info structure.

//--------------------------------------------------------------------------
// Default constructor
//--------------------------------------------------------------------------
CGDICanvas::CGDICanvas(VOID)
{
	// Initialize members
	m_hdcMem = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	m_lpddsSurface = NULL;
	m_bUseDX = FALSE;
	m_hdcLocked = NULL;
}

//--------------------------------------------------------------------------
// Copy constructor
//--------------------------------------------------------------------------
CGDICanvas::CGDICanvas(
	CONST CGDICanvas &rhs
		)
{

	// First, create an equal sized canvas
	CreateBlank(rhs.m_hdcMem, rhs.m_nWidth, rhs.m_nHeight, rhs.m_bUseDX);

	// Now blt the image over
	if (rhs.m_bUseDX)
	{

		// Create rectangles
		RECT destRect = {0, 0, m_nWidth, m_nHeight};
		RECT rect = {0, 0, m_nWidth, m_nHeight};

		// Setup blt effects
		DDBLTFX bltFx;
		DD_INIT_STRUCT(bltFx);
		bltFx.dwROP = SRCCOPY;

		// Execute the blt
		m_lpddsSurface->Blt(&destRect, rhs.m_lpddsSurface, &rect, DDBLT_WAIT | DDBLT_ROP, &bltFx);

	}
	else
	{
		// Just use the incredibly slow BitBlt()
		BitBlt(m_hdcMem, 0, 0, rhs.m_nWidth, rhs.m_nHeight, rhs.m_hdcMem, 0, 0, SRCCOPY);
	}

	// HDC is not locked
	m_hdcLocked = NULL;

}

//--------------------------------------------------------------------------
// Assignment operator
//--------------------------------------------------------------------------
CGDICanvas &CGDICanvas::operator=(
	CONST CGDICanvas &rhs
		)
{

	// Destroy, if in use
	if (m_hdcMem) Destroy();

	// First, create an equal sized canvas
	CreateBlank(rhs.m_hdcMem, rhs.m_nWidth, rhs.m_nHeight, rhs.m_bUseDX);

	// Now blt the image over
	if (rhs.m_bUseDX)
	{

		// Create rectangles
		RECT destRect = {0, 0, m_nWidth, m_nHeight};
		RECT rect = {0, 0, m_nWidth, m_nHeight};

		// Setup blt effects
		DDBLTFX bltFx;
		DD_INIT_STRUCT(bltFx);
		bltFx.dwROP = SRCCOPY;

		// Execute the blt
		m_lpddsSurface->Blt(&destRect, rhs.m_lpddsSurface, &rect, DDBLT_WAIT | DDBLT_ROP, &bltFx);

	}
	else
	{
		// Just use the incredibly slow BitBlt()
		BitBlt(m_hdcMem, 0, 0, rhs.m_nWidth, rhs.m_nHeight, rhs.m_hdcMem, 0, 0, SRCCOPY);
	}

	// HDC is not locked
	m_hdcLocked = NULL;

	// Return the current object
	return *this;

}

//--------------------------------------------------------------------------
// Deconstructor
//--------------------------------------------------------------------------
INLINE CGDICanvas::~CGDICanvas(VOID)
{
	// Destroy existing canavs, if one
	if (usingDX())
	{
		if (m_lpddsSurface)
		{
			Destroy();
		}
	}
	else if (m_hdcMem)
	{
		Destroy();
	}
}

//--------------------------------------------------------------------------
// Create a canvas
//--------------------------------------------------------------------------
VOID FAST_CALL CGDICanvas::CreateBlank(
	CONST HDC hdcCompatible,
	CONST INT width,
	CONST INT height,
	CONST BOOL bDX
		)
{

	// If using DirectX
	if (!(m_bUseDX = (bDX && gDXInfo.lpdd)))
	{

		// Destroy existing canvas
		if (m_hdcMem) Destroy();

		// Create new canvas
		m_hdcMem = CreateCompatibleDC(hdcCompatible);

	}
	else
	{

		// Create a DirectX surface
		DDSURFACEDESC2 ddsd;
		DD_INIT_STRUCT(ddsd);
		ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		ddsd.dwWidth = width;
		ddsd.dwHeight = height;
		CONST HRESULT hr = gDXInfo.lpdd->CreateSurface(&ddsd, &m_lpddsSurface, NULL);
		if (FAILED(hr))
		{
			// Use RAM (slower...)
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
			gDXInfo.lpdd->CreateSurface(&ddsd, &m_lpddsSurface, NULL);
		}
	}

	// Record width and height
	m_nWidth = width;
	m_nHeight = height;

}

//--------------------------------------------------------------------------
// Resize the canvas
//--------------------------------------------------------------------------
VOID FAST_CALL CGDICanvas::Resize(
	CONST HDC hdcCompatible,
	CONST INT width,
	CONST INT height
		)
{
	CONST BOOL bDX = usingDX();
	Destroy();
	CreateBlank(hdcCompatible, width, height, bDX);
}

//--------------------------------------------------------------------------
// Destroy the canvas
//--------------------------------------------------------------------------
INLINE VOID CGDICanvas::Destroy(VOID)
{

	// If using GDI
	if (!(usingDX()))
	{
		// Delete the DC
		DeleteDC(m_hdcMem);
	}
	else if (m_lpddsSurface)
	{
		// Release the surface
		m_lpddsSurface->Release();
	}

	// Clear members
	m_hdcMem = NULL;
	m_lpddsSurface = NULL;
	m_nWidth = 0;
	m_nHeight = 0;

}

//--------------------------------------------------------------------------
// Set a pixel using GDI
//--------------------------------------------------------------------------
INLINE VOID CGDICanvas::SetPixel(
	CONST INT x,
	CONST INT y,
	CONST INT crColor
		)
{
	CONST HDC hdc = OpenDC();
	SetPixelV(hdc, x, y, crColor);
	CloseDC(hdc);
}

//--------------------------------------------------------------------------
// Get a pixel using GDI
//--------------------------------------------------------------------------
INT FAST_CALL CGDICanvas::GetPixel(
	CONST INT x,
	CONST INT y
		) CONST
{
	CONST HDC hdc = OpenDC();
	CONST INT nToRet = ::GetPixel(hdc, x, y);
	CloseDC(hdc);
	return nToRet;
}

//--------------------------------------------------------------------------
// Opaque blitter
//--------------------------------------------------------------------------

//
// HDC target
//
INT FAST_CALL CGDICanvas::BltPart(
	CONST HDC hdcTarget,
	CONST INT x,
	CONST INT y,
	CONST INT xSrc,
	CONST INT ySrc,
	CONST INT width,
	CONST INT height,
	CONST LONG lRasterOp
		) CONST
{
	CONST HDC hdc = OpenDC();
	CONST INT nToRet = BitBlt(hdcTarget, x, y, width, height, hdc, xSrc, ySrc, lRasterOp);
	CloseDC(hdc);
	return nToRet;
}

//
// Canvas target
//
INT FAST_CALL CGDICanvas::BltPart(
	CONST CGDICanvas *pCanvas,
	INT x,
	INT y,
	INT xSrc,
	INT ySrc,
	INT width,
	INT height,
	CONST LONG lRasterOp
		) CONST
{

	if (x < 0)
	{
		xSrc -= x;
		width += x;
		x = 0;
	}

	if (y < 0)
	{
		ySrc -= y;
		height += y;
		y = 0;
	}

	if (x + width > pCanvas->GetWidth())
	{
		width = pCanvas->GetWidth() - x;
	}

	if (y + height > pCanvas->GetHeight())
	{
		height = pCanvas->GetHeight() - y;
	}

	if (pCanvas->usingDX() && usingDX())
	{
		// Blt using DirectX blt call
		return BltPart(pCanvas->GetDXSurface(), x, y, xSrc, ySrc, width, height, lRasterOp);
	}
	else
	{
		// Use GDI
		CONST HDC hdc = pCanvas->OpenDC();
		CONST INT nToRet = BltPart(hdc, x, y, xSrc, ySrc, width, height, lRasterOp);
		pCanvas->CloseDC(hdc);
		return nToRet;
	}

}

//
// Surface target
//
INT FAST_CALL CGDICanvas::BltPart(
	CONST LPDIRECTDRAWSURFACE7 lpddsSurface,
	CONST INT x,
	CONST INT y,
	CONST INT xSrc,
	CONST INT ySrc,
	CONST INT width,
	CONST INT height,
	CONST LONG lRasterOp
		) CONST
{

	// If using DirectX
	if (lpddsSurface && usingDX())
	{

		// Setup the rects
		RECT destRect = {x, y, x + width, y + height};
		RECT rect = {xSrc, ySrc, xSrc + width, ySrc + height};

		// Execute the blt
		DDBLTFX bltFx;
		DD_INIT_STRUCT(bltFx);
		bltFx.dwROP = lRasterOp;
		return SUCCEEDED(lpddsSurface->BltFast(x, y, GetDXSurface(), &rect, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY));

	}
	else if (lpddsSurface)
	{
		// Use GDI
		HDC hdc = NULL;
		lpddsSurface->GetDC(&hdc);
		CONST INT nToRet = BltPart(hdc, x, y, xSrc, ySrc, width, height, lRasterOp);
		lpddsSurface->ReleaseDC(hdc);
		return nToRet;
	}

	// Else, we've failed
	return FALSE;

}

//
// Complete blt to an HDC
//
INT FAST_CALL CGDICanvas::Blt(
	CONST HDC hdcTarget,
	CONST INT x,
	CONST INT y,
	CONST LONG lRasterOp
		) CONST
{
	return BltPart(hdcTarget, x, y, 0, 0, GetWidth(), GetHeight(), lRasterOp);
}

//
// Complete blt to a canvas
//
INT FAST_CALL CGDICanvas::Blt(
	CONST CGDICanvas *pCanvas,
	CONST INT x,
	CONST INT y,
	CONST LONG lRasterOp
		) CONST
{
	return BltPart(pCanvas, x, y, 0, 0, GetWidth(), GetHeight(), lRasterOp);
}

//
// Complete blt to a strface
//
INT FAST_CALL CGDICanvas::Blt(
	CONST LPDIRECTDRAWSURFACE7 lpddsSurface,
	CONST INT x,
	CONST INT y,
	CONST LONG lRasterOp
		) CONST
{
	return BltPart(lpddsSurface, x, y, 0, 0, GetWidth(), GetHeight(), lRasterOp);
}

//--------------------------------------------------------------------------
// Transparent blitter
//--------------------------------------------------------------------------

//
// HDC target
//
INT FAST_CALL CGDICanvas::BltTransparent(
	CONST HDC hdcTarget,
	CONST INT x,
	CONST INT y,
	CONST LONG crTransparentColor
		) CONST
{
	return BltTransparentPart(hdcTarget, x, y, 0, 0, GetWidth(), GetHeight(), crTransparentColor);
}

//
// Canvas target
//
INT FAST_CALL CGDICanvas::BltTransparent(
	CONST CGDICanvas *pCanvas,
	CONST INT x,
	CONST INT y,
	CONST LONG crTransparentColor
		) CONST
{
	return BltTransparentPart(pCanvas, x, y, 0, 0, GetWidth(), GetHeight(), crTransparentColor);
}

//
// Surface target
//
INT FAST_CALL CGDICanvas::BltTransparent(
	CONST LPDIRECTDRAWSURFACE7 lpddsSurface,
	CONST INT x,
	CONST INT y,
	CONST LONG crTransparentColor
		) CONST
{
	return BltTransparentPart(lpddsSurface, x, y, 0, 0, GetWidth(), GetHeight(), crTransparentColor);
}

//
// Partial - HDC target
//
INT FAST_CALL CGDICanvas::BltTransparentPart(
	CONST HDC hdcTarget,
	CONST INT x,
	CONST INT y,
	CONST INT xSrc,
	CONST INT ySrc,
	CONST INT width,
	CONST INT height,
	CONST LONG crTransparentColor
		) CONST
{
	CONST HDC hdc = OpenDC();
	CONST INT nToRet = TransparentBlt(hdcTarget, x, y, width, height, hdc, xSrc, ySrc, width, height, crTransparentColor);
	CloseDC(hdc);
	return nToRet;
}

//
// Partial - canvas target
//
INT FAST_CALL CGDICanvas::BltTransparentPart(
	CONST CGDICanvas *pCanvas,
	INT x,
	INT y,
	INT xSrc,
	INT ySrc,
	INT width,
	INT height,
	LONG crTransparentColor
		) CONST
{

	if (x < 0)
	{
		xSrc -= x;
		width += x;
		x = 0;
	}

	if (y < 0)
	{
		ySrc -= y;
		height += y;
		y = 0;
	}

	if (x + width > pCanvas->GetWidth())
	{
		width = pCanvas->GetWidth() - x;
	}

	if (y + height > pCanvas->GetHeight())
	{
		height = pCanvas->GetHeight() - y;
	}

	if (pCanvas->usingDX() && usingDX())
	{
		// Use DirectX
		return BltTransparentPart(pCanvas->GetDXSurface(), x, y, xSrc, ySrc, width, height, crTransparentColor);
	}
	else
	{
		// Use GDI
		CONST HDC hdc = pCanvas->OpenDC();
		CONST INT nToRet = BltTransparentPart(hdc, x, y, xSrc, ySrc, width, height, crTransparentColor);
		pCanvas->CloseDC(hdc);
		return nToRet;
	}

}

//
// Partial - surface target
//
INT FAST_CALL CGDICanvas::BltTransparentPart(
	CONST LPDIRECTDRAWSURFACE7 lpddsSurface,
	CONST INT x,
	CONST INT y,
	CONST INT xSrc,
	CONST INT ySrc,
	CONST INT width,
	CONST INT height,
	CONST LONG crTransparentColor
		) CONST
{

	// If using DirectX
	if (lpddsSurface &&usingDX())
	{

		// Obtain RGB color
		CONST LONG rgb = GetRGBColor(crTransparentColor);

		// Setup color key
		DDCOLORKEY ddck = {rgb, rgb};
		GetDXSurface()->SetColorKey(DDCKEY_SRCBLT, &ddck);

		// Setup rectangles
		RECT destRect = {x, y, x + width, y + height};
		RECT rect = {xSrc, ySrc, xSrc + width, ySrc + height};

		// Execute the blt
		return SUCCEEDED(lpddsSurface->BltFast(x, y, this->GetDXSurface(), &rect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY));

	}
	else if (lpddsSurface)
	{
		// Use GDI
		HDC hdc = 0;
		lpddsSurface->GetDC(&hdc);
		CONST INT nToRet = BltTransparentPart(hdc, x, y, xSrc, ySrc, width, height, crTransparentColor);
		lpddsSurface->ReleaseDC(hdc);
		return nToRet;
	}

	// If here, we've failed
	return FALSE;

}

//--------------------------------------------------------------------------
// Translucent blitter
//--------------------------------------------------------------------------

//
// HDC target
//
INT FAST_CALL CGDICanvas::BltTranslucent(
	CONST HDC hdcTarget,
	CONST INT x,
	CONST INT y,
	CONST DOUBLE dIntensity,
	CONST LONG crUnaffectedColor,
	CONST LONG crTransparentColor
		) CONST
{

	// GDI translucent blts are way too slow - don't do it
	if (crTransparentColor == -1)
	{
		// Blt opaque
		return Blt(hdcTarget, x, y);
	}
	else
	{
		// Blt transp
		return BltTransparent(hdcTarget, x, y, crTransparentColor);
	}

}

//
// Canvas target
//
INT FAST_CALL CGDICanvas::BltTranslucent(
	CONST CGDICanvas *pCanvas,
	CONST INT x,
	CONST INT y,
	CONST DOUBLE dIntensity,
	CONST LONG crUnaffectedColor,
	CONST LONG crTransparentColor
		) CONST
{

	// If using DirectX
	if (pCanvas->usingDX() && usingDX())
	{
		// Use the DX blitter
		return BltTranslucent(pCanvas->GetDXSurface(), x, y, dIntensity, crUnaffectedColor, crTransparentColor);
	}
	else
	{
		// Blt using GDI
		CONST HDC hdc = pCanvas->OpenDC();
		CONST INT nToRet = BltTranslucent(hdc, x, y, dIntensity, crUnaffectedColor, crTransparentColor);
		pCanvas->CloseDC(hdc);
		return nToRet;
	}

}

//
// Surface target
//
INLINE INT CGDICanvas::BltTranslucent(
	CONST LPDIRECTDRAWSURFACE7 lpddsSurface,
	CONST INT x,
	CONST INT y,
	CONST DOUBLE dIntensity,
	CONST LONG crUnaffectedColor,
	CONST LONG crTransparentColor
		) CONST
{

	// Use the partial blitter
	return BltTranslucentPart(
		lpddsSurface,
		x, y, 0, 0,
		m_nWidth, m_nHeight,
		dIntensity, crUnaffectedColor, crTransparentColor
	);

}

//
// Partial - HDC target
//
INT FAST_CALL CGDICanvas::BltTranslucentPart(
	CONST HDC hdcTarget,
	CONST INT x,
	CONST INT y,
	CONST INT xSrc,
	CONST INT ySrc,
	CONST INT width,
	CONST INT height,
	CONST DOUBLE dIntensity,
	CONST LONG crUnaffectedColor,
	CONST LONG crTransparentColor
		) CONST
{

	// GDI translucent blts are way too slow - don't do it
	if (crTransparentColor == -1)
	{
		// Blt opaque
		return Blt(hdcTarget, x, y);
	}
	else
	{
		// Blt transp
		return BltTransparent(hdcTarget, x, y, crTransparentColor);
	}

}

//
// Partial - canvas target
//
INT FAST_CALL CGDICanvas::BltTranslucentPart(
	CONST CGDICanvas *pCanvas,
	CONST INT x,
	CONST INT y,
	CONST INT xSrc,
	CONST INT ySrc,
	CONST INT width,
	CONST INT height,
	CONST DOUBLE dIntensity,
	CONST LONG crUnaffectedColor,
	CONST LONG crTransparentColor
		) CONST
{

	if (pCanvas->usingDX() && usingDX())
	{
		// Blt using DirectX
		return BltTranslucentPart(
			pCanvas->GetDXSurface(),
			x, y, xSrc, ySrc,
			width, height,
			dIntensity, crUnaffectedColor, crTransparentColor
		);
	}
	else
	{
		// Blt using GDI
		CONST HDC hdc = pCanvas->OpenDC();
		CONST INT toRet = BltTranslucentPart(
			hdc,
			x, y, xSrc, ySrc,
			width, height,
			dIntensity, crUnaffectedColor, crTransparentColor
		);
		pCanvas->CloseDC(hdc);
		return toRet;
	}

}

//
// Partial - surface target
//
INT FAST_CALL CGDICanvas::BltTranslucentPart(
	CONST LPDIRECTDRAWSURFACE7 lpddsSurface,
	CONST INT x,
	CONST INT y,
	CONST INT xSrc,
	CONST INT ySrc,
	CONST INT width,
	CONST INT height,
	CONST DOUBLE dIntensity,
	CONST LONG crUnaffectedColor,
	CONST LONG crTransparentColor
		) CONST
{

	// If we have a valid surface ptr and we're using DirectX
	if (lpddsSurface && usingDX())
	{

		// Lock the destination surface
		DDSURFACEDESC2 destSurface;
		DD_INIT_STRUCT(destSurface);
		HRESULT hr = lpddsSurface->Lock(NULL, &destSurface, DDLOCK_SURFACEMEMORYPTR | DDLOCK_NOSYSLOCK | DDLOCK_WAIT, NULL);

		if (FAILED(hr))
		{
			// Return failed
			return FALSE;
		}

		// Lock the source surface
		DDSURFACEDESC2 srcSurface;
		DD_INIT_STRUCT(srcSurface);
		hr = GetDXSurface()->Lock(NULL, &srcSurface, DDLOCK_SURFACEMEMORYPTR | DDLOCK_NOSYSLOCK | DDLOCK_WAIT, NULL);

		if (FAILED(hr))
		{

			// Unlock destination surface
			lpddsSurface->Unlock(NULL);

			// Failed
			return FALSE;

		}

		// Obtain the pixel format
		DDPIXELFORMAT ddpfDest;
		DD_INIT_STRUCT(ddpfDest);
		lpddsSurface->GetPixelFormat(&ddpfDest);

		// (Could kill this check by saving color depth and using function pointer?)

		// Switch on pixel format
		switch (ddpfDest.dwRGBBitCount)
		{

			// 32-bit color depth
			case 32:
			{

				// Calculate pixels per row
				CONST INT nPixelsPerRow = destSurface.lPitch / (ddpfDest.dwRGBBitCount / 8);

				// Obtain pointers to the surfaces
				// (*CONST means that the address pointed to will not change, but
				// the data at that address can be changed freely)
				DWORD *CONST pSurfDest = reinterpret_cast<DWORD *>(destSurface.lpSurface);
				DWORD *CONST pSurfSrc = reinterpret_cast<DWORD *>(srcSurface.lpSurface);

				// For the y axis
				for (INT yy = ySrc; yy < height; yy++)
				{

					// Calculate index into destination and source, respectively
					INT idxd = (yy + y) * nPixelsPerRow + x;
					INT idx = yy * (srcSurface.lPitch / (ddpfDest.dwRGBBitCount / 8));

					// For the x axis
					for (INT xx = xSrc; xx < width; xx++)
					{

						// Obtain a pixel in RGB format
						CONST LONG srcRGB = ConvertDDColor(pSurfSrc[idx], &ddpfDest);

						// Check for unaffected color
						if (srcRGB == crUnaffectedColor)
						{
							// Directly copy
							pSurfDest[idxd] = ConvertColorRef(srcRGB, &ddpfDest);
						}

						// Check for opaque color
						else if (srcRGB != crTransparentColor)
						{

							// Obtain destination RGB
							CONST LONG destRGB = ConvertDDColor(pSurfDest[idxd], &ddpfDest);

							// Calculate translucent rgb value
							CONST INT r = (GetRValue(srcRGB) * dIntensity) + (GetRValue(destRGB) * (1 - dIntensity));
							CONST INT g = (GetGValue(srcRGB) * dIntensity) + (GetGValue(destRGB) * (1 - dIntensity));
							CONST INT b = (GetBValue(srcRGB) * dIntensity) + (GetBValue(destRGB) * (1 - dIntensity));

							// Lay down translucently
							pSurfDest[idxd] = ConvertColorRef(RGB(r, g, b), &ddpfDest);

						}

						// Increment position on surfaces
						idx++;
						idxd++;

					} // x axis
				} // y axis
			} break; // 32 bit blt

			// 24 bit color depth
			case 24:
			{

				// Modify RGB params by setting and getting a pixel
				CONST LONG crTemp = GetRGBPixel(&srcSurface, &ddpfDest, 1, 1);
				LONG rgbUnaffectedColor = -1, rgbTransparentColor = -1;
				if (crUnaffectedColor != -1)
				{
					// Modify unaffected color
					SetRGBPixel(&srcSurface, &ddpfDest, 1, 1, crUnaffectedColor);
					rgbUnaffectedColor = GetRGBPixel(&srcSurface, &ddpfDest, 1, 1);
				}
				if (crTransparentColor != -1)
				{
					// Modify transparent color
					SetRGBPixel(&srcSurface, &ddpfDest, 1, 1, crTransparentColor);
					rgbTransparentColor = GetRGBPixel(&srcSurface, &ddpfDest, 1, 1);
				}
				// Set back down pixel
				SetRGBPixel(&srcSurface, &ddpfDest, 1, 1, crTemp);

				// For the y axis
				for (INT yy = ySrc; yy < height; yy++)
				{

					// For the x axis
					for (INT xx = xSrc; xx < width; xx++)
					{

						// Get pixel on source surface
						CONST LONG srcRGB = GetRGBPixel(&srcSurface, &ddpfDest, xx, yy);

						// Check for unaffected color
						if (srcRGB == rgbUnaffectedColor)
						{
							// Just copy over
							SetRGBPixel(&destSurface, &ddpfDest, x + xx, y + yy, srcRGB);
						}

						// If color is not transparent
						else if (srcRGB != rgbTransparentColor)
						{

							// Obtain destination pixel
							CONST LONG destRGB = GetRGBPixel(&destSurface, &ddpfDest, xx + x, yy + y);

							// Calculate new rgb color
							CONST INT r = (GetRValue(srcRGB) * dIntensity) + (GetRValue(destRGB) * (1 - dIntensity));
							CONST INT g = (GetGValue(srcRGB) * dIntensity) + (GetGValue(destRGB) * (1 - dIntensity));
							CONST INT b = (GetBValue(srcRGB) * dIntensity) + (GetBValue(destRGB) * (1 - dIntensity));

							// Set the pixel
							SetRGBPixel(&destSurface, &ddpfDest, x + xx, y + yy, RGB(r, g, b));

						}

					} // x axis
				} // y axis
			} break; // 24 bit blt

			// 16 bit color depth
			case 16:
			{

				// Calculate pixels per row
				CONST INT nPixelsPerRow = destSurface.lPitch / (ddpfDest.dwRGBBitCount / 8);

				// Obtain pointers to the surfaces
				// (*CONST means that the address pointed to will not change, but
				// the data at that address can be changed freely)
				WORD *CONST pSurfDest = reinterpret_cast<WORD *>(destSurface.lpSurface);
				WORD *CONST pSurfSrc = reinterpret_cast<WORD *>(srcSurface.lpSurface);

				// For the y axis
				for (INT yy = ySrc; yy < height; yy++)
				{

					// Calculate index into destination and source, respectively
					INT idxd = (yy + y) * nPixelsPerRow + x;
					INT idx = yy * (srcSurface.lPitch / (ddpfDest.dwRGBBitCount / 8));

					// For the x axis
					for (INT xx = xSrc; xx < width; xx++)
					{

						// Obtain a pixel in RGB format
						CONST LONG srcRGB = ConvertDDColor(pSurfSrc[idx], &ddpfDest);

						// Check for unaffected color
						if (srcRGB == crUnaffectedColor)
						{
							// Directly copy
							pSurfDest[idxd] = ConvertColorRef(srcRGB, &ddpfDest);
						}

						// Check for opaque color
						else if (srcRGB != crTransparentColor)
						{

							// Obtain destination RGB
							CONST LONG destRGB = ConvertDDColor(pSurfDest[idxd], &ddpfDest);

							// Calculate translucent rgb value
							CONST INT r = (GetRValue(srcRGB) * dIntensity) + (GetRValue(destRGB) * (1 - dIntensity));
							CONST INT g = (GetGValue(srcRGB) * dIntensity) + (GetGValue(destRGB) * (1 - dIntensity));
							CONST INT b = (GetBValue(srcRGB) * dIntensity) + (GetBValue(destRGB) * (1 - dIntensity));

							// Lay down translucently
							pSurfDest[idxd] = ConvertColorRef(RGB(r, g, b), &ddpfDest);

						}

						// Increment position on surfaces
						idx++;
						idxd++;

					} // x axis
				} // y axis
			} break; // 16 bit blt

			// Unsupported color depth
			default:
			{

				// Unlock surfaces
				GetDXSurface()->Unlock(NULL);
				lpddsSurface->Unlock(NULL);

				// If a transp color is not set
				if (crTransparentColor == -1)
				{
					// Just do direct blt
					return Blt(lpddsSurface, x, y);
				}
				else
				{
					// Else, do transp blt
					return BltTransparent(lpddsSurface, x, y, crTransparentColor);
				}

			} break;

		} // Color depth switch

		// Unlock the surfaces
		GetDXSurface()->Unlock(NULL);
		lpddsSurface->Unlock(NULL);

		// All's good
		return TRUE;

	} // Can use DirectX

	else if (lpddsSurface)
	{

		// Not using DirectX, but we have a surface
		HDC hdc = 0;
		lpddsSurface->GetDC(&hdc);

		// Do super slow GDI blt
		CONST INT nToRet = BltTranslucent(hdc, x, y, dIntensity, crUnaffectedColor, crTransparentColor);
		lpddsSurface->ReleaseDC(hdc);

		// Return the result
		return nToRet;

	}

	// If we made it here, we failed
	return FALSE;

}

//--------------------------------------------------------------------------
// Obtain the contained DirectX surface
//--------------------------------------------------------------------------
INLINE LPDIRECTDRAWSURFACE7 CGDICanvas::GetDXSurface(VOID) CONST
{
	return m_lpddsSurface;
}

//--------------------------------------------------------------------------
// Are we using DirectX?
//--------------------------------------------------------------------------
INLINE BOOL CGDICanvas::usingDX(VOID) CONST
{
	return m_bUseDX;
}

//--------------------------------------------------------------------------
// Shift the canvas left
//--------------------------------------------------------------------------
INT FAST_CALL CGDICanvas::ShiftLeft(
	CONST INT nPixels
		)
{
	INT nToRet = 0;
	if (this->usingDX())
	{
		CGDICanvas temp = (*this);
		//blt them using directX blt call
		RECT destRect;
		SetRect(&destRect, 0, 0, m_nWidth-nPixels, m_nHeight);

		RECT rect;
		SetRect(&rect, nPixels, 0, m_nWidth, m_nHeight);

		//I'm going to use a raster operation witht he blt
		//it will be SRCCOPY (straight copy!)
		DDBLTFX bltFx;
		memset(&bltFx, 0, sizeof(DDBLTFX));
		bltFx.dwSize = sizeof(DDBLTFX);
		bltFx.dwROP = SRCCOPY;

		HRESULT hr = GetDXSurface()->Blt(&destRect, temp.GetDXSurface(), &rect, DDBLT_WAIT | DDBLT_ROP, &bltFx);
		if (FAILED(hr))
		{
			nToRet = 0;
		}
		else
		{
			nToRet = 1;
		}
	}
	else
	{
		HDC hdcMe = OpenDC();
		nToRet = BitBlt(hdcMe, -nPixels, 0, GetWidth(), GetHeight(), hdcMe, 0, 0, SRCCOPY);
		CloseDC(hdcMe);
	}
	return nToRet;
}

//--------------------------------------------------------------------------
// Shift the canvas right
//--------------------------------------------------------------------------
INT FAST_CALL CGDICanvas::ShiftRight(
	CONST INT nPixels
		)
{
	INT nToRet = 0;
	if (this->usingDX())
	{
		CGDICanvas temp = (*this);
		//blt them using directX blt call
		RECT destRect;
		SetRect(&destRect, nPixels, 0, m_nWidth, m_nHeight);

		RECT rect;
		SetRect(&rect, 0, 0, m_nWidth-nPixels, m_nHeight);

		//I'm going to use a raster operation witht he blt
		//it will be SRCCOPY (straight copy!)
		DDBLTFX bltFx;
		memset(&bltFx, 0, sizeof(DDBLTFX));
		bltFx.dwSize = sizeof(DDBLTFX);
		bltFx.dwROP = SRCCOPY;

		HRESULT hr = GetDXSurface()->Blt(&destRect, temp.GetDXSurface(), &rect, DDBLT_WAIT | DDBLT_ROP, &bltFx);
		if (FAILED(hr))
		{
			nToRet = 0;
		}
		else
		{
			nToRet = 1;
		}
	}
	else
	{
		HDC hdcMe = OpenDC();
		nToRet = BitBlt(hdcMe, nPixels, 0, GetWidth(), GetHeight(), hdcMe, 0, 0, SRCCOPY);
		CloseDC(hdcMe);
	}
	return nToRet;
}

//--------------------------------------------------------------------------
// Shift the canvas up
//--------------------------------------------------------------------------
INT FAST_CALL CGDICanvas::ShiftUp(
	CONST INT nPixels
		)
{
	INT nToRet = 0;
	if (this->usingDX())
	{
		CGDICanvas temp = (*this);
		//blt them using directX blt call
		RECT destRect;
		SetRect(&destRect, 0, 0, m_nWidth, m_nHeight-nPixels);

		RECT rect;
		SetRect(&rect, 0, nPixels, m_nWidth, m_nHeight);

		//I'm going to use a raster operation witht he blt
		//it will be SRCCOPY (straight copy!)
		DDBLTFX bltFx;
		memset(&bltFx, 0, sizeof(DDBLTFX));
		bltFx.dwSize = sizeof(DDBLTFX);
		bltFx.dwROP = SRCCOPY;

		HRESULT hr = GetDXSurface()->Blt(&destRect, temp.GetDXSurface(), &rect, DDBLT_WAIT | DDBLT_ROP, &bltFx);
		if (FAILED(hr))
		{
			nToRet = 0;
		}
		else
		{
			nToRet = 1;
		}
	}
	else
	{
		HDC hdcMe = OpenDC();
		nToRet = BitBlt(hdcMe, 0, -nPixels, GetWidth(), GetHeight(), hdcMe, 0, 0, SRCCOPY);
		CloseDC(hdcMe);
	}
	return nToRet;
}

//--------------------------------------------------------------------------
// Shift the canvas down
//--------------------------------------------------------------------------
INT FAST_CALL CGDICanvas::ShiftDown(
	CONST INT nPixels
		)
{
	INT nToRet = 0;
	if (this->usingDX())
	{
		CGDICanvas temp = (*this);
		//blt them using directX blt call
		RECT destRect;
		SetRect(&destRect, 0, nPixels, m_nWidth, m_nHeight);

		RECT rect;
		SetRect(&rect, 0, 0, m_nWidth, m_nHeight-nPixels);

		//I'm going to use a raster operation witht he blt
		//it will be SRCCOPY (straight copy!)
		DDBLTFX bltFx;
		memset(&bltFx, 0, sizeof(DDBLTFX));
		bltFx.dwSize = sizeof(DDBLTFX);
		bltFx.dwROP = SRCCOPY;

		HRESULT hr = GetDXSurface()->Blt(&destRect, temp.GetDXSurface(), &rect, DDBLT_WAIT | DDBLT_ROP, &bltFx);
		if (FAILED(hr))
		{
			nToRet = 0;
		}
		else
		{
			nToRet = 1;
		}
	}
	else
	{
		HDC hdcMe = OpenDC();
		nToRet = BitBlt(hdcMe, 0, nPixels, GetWidth(), GetHeight(), hdcMe, 0, 0, SRCCOPY);
		CloseDC(hdcMe);
	}
	return nToRet;
}


//--------------------------------------------------------------------------
// Convert a DirectX color to RGB
//--------------------------------------------------------------------------
INLINE LONG CGDICanvas::GetSurfaceColor(
	CONST LONG dxColor
		) CONST
{

	if (usingDX())
	{
		DDPIXELFORMAT ddpf;
		DD_INIT_STRUCT(ddpf);
		GetDXSurface()->GetPixelFormat(&ddpf);
		return ConvertDDColor(dxColor, &ddpf);
	}
	else
	{
		// GDI already uses RGB
		return dxColor;
	}

}

//--------------------------------------------------------------------------
// Convert a RGB color to a color reference
//--------------------------------------------------------------------------
INLINE LONG CGDICanvas::GetRGBColor(
	CONST LONG crColor
		) CONST
{

	if (usingDX())
	{
		DDPIXELFORMAT ddpf;
		DD_INIT_STRUCT(ddpf);
		GetDXSurface()->GetPixelFormat(&ddpf);
		return ConvertColorRef(crColor, &ddpf);
	}
	else
	{
		// GDI only uses RGB
		return crColor;
	}

}

//--------------------------------------------------------------------------
// Obtain width of the canvas
//--------------------------------------------------------------------------
INLINE INT CGDICanvas::GetWidth(VOID) CONST
{
	return m_nWidth;
}

//--------------------------------------------------------------------------
// Obtain height of the canvas
//--------------------------------------------------------------------------
INLINE INT CGDICanvas::GetHeight(VOID) CONST
{
	return m_nHeight;
}

//--------------------------------------------------------------------------
// Obtain the canvas' HDC
//--------------------------------------------------------------------------
INLINE HDC CGDICanvas::OpenDC(VOID) CONST
{
	if (m_hdcLocked)
	{
		// Surface is locked
		return m_hdcLocked;
	}
	if (m_bUseDX && m_lpddsSurface)
	{
		// Using DirectX
		HDC toRet = NULL;
		m_lpddsSurface->GetDC(&toRet);
		return toRet;
	}
	else
	{
		// Use GDI's DC
		return m_hdcMem;
	}
}

//--------------------------------------------------------------------------
// Close the canvas' HDC
//--------------------------------------------------------------------------
INLINE VOID CGDICanvas::CloseDC(
	CONST HDC hdc
		) CONST
{
	if (!m_hdcLocked && m_bUseDX && m_lpddsSurface && hdc)
	{
		// Release the DC
		m_lpddsSurface->ReleaseDC(hdc);
	}
}

//--------------------------------------------------------------------------
// Lock the canvas
//--------------------------------------------------------------------------
INLINE VOID CGDICanvas::Lock(VOID)
{
	m_hdcLocked = OpenDC();
}

//--------------------------------------------------------------------------
// Unlock the canvas
//--------------------------------------------------------------------------
INLINE VOID CGDICanvas::Unlock(VOID)
{
	CONST HDC hdc = m_hdcLocked;
	m_hdcLocked = NULL;
	CloseDC(hdc);
}

//--------------------------------------------------------------------------
// Convert a DX color to a RGB color
//--------------------------------------------------------------------------
INLINE COLORREF CGDICanvas::ConvertDDColor(
	CONST DWORD dwColor,
	CONST LPDDPIXELFORMAT pddpf
		)
{

	DWORD dwRed = dwColor & pddpf->dwRBitMask;
	DWORD dwGreen = dwColor & pddpf->dwGBitMask;
	DWORD dwBlue = dwColor & pddpf->dwBBitMask;

	dwRed *= 255;
	dwGreen *= 255;
	dwBlue *= 255;

	dwRed /= pddpf->dwRBitMask;
	dwGreen /= pddpf->dwGBitMask;
	dwBlue /= pddpf->dwBBitMask;

	return RGB(dwRed, dwGreen, dwBlue);

}

//--------------------------------------------------------------------------
// Convert a RGB color to a DX color
//--------------------------------------------------------------------------
INLINE DWORD CGDICanvas::ConvertColorRef(
	CONST COLORREF crColor,
	CONST LPDDPIXELFORMAT pddpf
		)
{

	DWORD dwRed = GetRValue(crColor);
	DWORD dwGreen = GetGValue(crColor);
	DWORD dwBlue = GetBValue(crColor);

	dwRed *= pddpf->dwRBitMask;
	dwGreen *= pddpf->dwGBitMask;
	dwBlue *= pddpf->dwBBitMask;
	dwRed /= 255;
	dwGreen /= 255;
	dwBlue /= 255;

	dwRed &= pddpf->dwRBitMask;
	dwGreen &= pddpf->dwGBitMask;
	dwBlue &= pddpf->dwBBitMask;

	return (dwRed | dwGreen | dwBlue);

}

//--------------------------------------------------------------------------
// Get the number of bits from a mask
//--------------------------------------------------------------------------
INLINE WORD CGDICanvas::GetNumberOfBits(
	DWORD dwMask
		)
{
    WORD wBits = 0;
    while (dwMask)
    {
        dwMask &= (dwMask - 1);
        wBits++;
    }
    return wBits;
}

//--------------------------------------------------------------------------
// Get a mask position
//--------------------------------------------------------------------------
INLINE WORD CGDICanvas::GetMaskPos(
	CONST DWORD dwMask
		)
{
    WORD wPos = 0;
    while (!(dwMask & (1 << wPos))) wPos++;
    return wPos;
}

//--------------------------------------------------------------------------
// Set a RGB pixel
//--------------------------------------------------------------------------
INLINE VOID CGDICanvas::SetRGBPixel(
	CONST LPDDSURFACEDESC2 destSurface,
	CONST LPDDPIXELFORMAT pddpf,
	CONST INT x,
	CONST INT y,
	CONST LONG rgb
		)
{
	CONST WORD wRBits = GetNumberOfBits(pddpf->dwRBitMask);
	CONST WORD wGBits = GetNumberOfBits(pddpf->dwGBitMask);
	CONST WORD wBBits = GetNumberOfBits(pddpf->dwBBitMask);
	CONST WORD wRPos = GetMaskPos(pddpf->dwRBitMask);
	CONST WORD wGPos = GetMaskPos(pddpf->dwGBitMask);
	CONST WORD wBPos = GetMaskPos(pddpf->dwBBitMask);
	CONST DWORD offset = y * destSurface->lPitch + x * (pddpf->dwRGBBitCount >> 3);
	*((LPDWORD)((DWORD)destSurface->lpSurface + offset)) =
		(((((((*((LPDWORD)(DWORD)destSurface->lpSurface + offset)) & ~pddpf->dwRBitMask) |
		((GetRValue(rgb) >> (8 - wRBits)) << wRPos)) & ~pddpf->dwGBitMask) |
		((GetGValue(rgb) >> (8 - wGBits)) << wGPos)) & ~pddpf->dwBBitMask) |
		((GetBValue(rgb) >> (8 - wBBits)) << wBPos));
}

//--------------------------------------------------------------------------
// Get a pixel on a locked surface
//--------------------------------------------------------------------------
INLINE LONG CGDICanvas::GetRGBPixel(
	CONST LPDDSURFACEDESC2 destSurface,
	CONST LPDDPIXELFORMAT pddpf,
	CONST INT x,
	CONST INT y
		)
{
	CONST WORD wRBits = GetNumberOfBits(pddpf->dwRBitMask);
	CONST WORD wGBits = GetNumberOfBits(pddpf->dwGBitMask);
	CONST WORD wBBits = GetNumberOfBits(pddpf->dwBBitMask);
	CONST WORD wRPos = GetMaskPos(pddpf->dwRBitMask);
	CONST WORD wGPos = GetMaskPos(pddpf->dwGBitMask);
	CONST WORD wBPos = GetMaskPos(pddpf->dwBBitMask);
	CONST DWORD offset = y * destSurface->lPitch + x * (pddpf->dwRGBBitCount >> 3);
	CONST DWORD pixel = *((LPDWORD)((DWORD)destSurface->lpSurface + offset));
	CONST BYTE r = (pixel & pddpf->dwRBitMask) << (8 - wRBits);
	CONST BYTE g = (pixel & pddpf->dwGBitMask) << (8 - wGBits);
	CONST BYTE b = (pixel & pddpf->dwBBitMask) << (8 - wBBits);
	return RGB(r, g, b);
}

//--------------------------------------------------------------------------
// Set a block of pixels
//--------------------------------------------------------------------------
VOID FAST_CALL CGDICanvas::SetPixels(
	CONST LPLONG p_crPixelArray,
	CONST INT x,
	CONST INT y,
	CONST INT width,
	CONST INT height
		)
{
	if (usingDX())
	{
		// Blt them using directX blt call
		SetPixelsDX(p_crPixelArray, x, y, width, height);
	}
	else
	{
		// Blt them using GDI
		SetPixelsGDI(p_crPixelArray, x, y, width, height);
	}
}

//--------------------------------------------------------------------------
// Set pixels using DirectX
//--------------------------------------------------------------------------
VOID FAST_CALL CGDICanvas::SetPixelsDX(
	CONST LPLONG p_crPixelArray,
	CONST INT x,
	CONST INT y,
	CONST INT width,
	CONST INT height
		)
{

	LPDIRECTDRAWSURFACE7 lpddsSurface = this->GetDXSurface();

	if (lpddsSurface && this->usingDX())
	{
		//do a quick blt on my own...
		RECT destRect;
		SetRect(&destRect, x, y, x+this->GetWidth(), y+this->GetHeight());

		RECT rect;
		SetRect(&rect, 0, 0, this->GetWidth(), this->GetHeight());

		//lock the destination surface...
		DDSURFACEDESC2 destSurface;
		memset(&destSurface, 0, sizeof(DDSURFACEDESC2));
		destSurface.dwSize = sizeof(DDSURFACEDESC2);
		HRESULT hr = lpddsSurface->Lock(NULL, &destSurface, DDLOCK_SURFACEMEMORYPTR | DDLOCK_NOSYSLOCK | DDLOCK_WAIT, NULL);
		if (!FAILED(hr))
		{
			DDPIXELFORMAT ddpfDest;
			memset(&ddpfDest, 0, sizeof(DDPIXELFORMAT));
			ddpfDest.dwSize = sizeof(DDPIXELFORMAT);
			lpddsSurface->GetPixelFormat(&ddpfDest);

			//I'm going to assume that the source and destination pixel formats are the same
			//maybe a bad assumption, I don't know :)
			switch (ddpfDest.dwRGBBitCount)
			{
				case 32:
				{
					INT nPixelsPerRow = destSurface.lPitch / (ddpfDest.dwRGBBitCount/8);
					DWORD* pSurfDest = (DWORD*)destSurface.lpSurface;

					//now blt!!!
					INT idx = 0;	//index into source array...
					for (INT yy = 0; yy < height; yy++)
					{
						INT idxd = (yy+y)*nPixelsPerRow + x;
						for (INT xx=0; xx < width; xx++)
						{
							//convert pixels to RGB...
							LONG srcRGB = p_crPixelArray[idx];

							//put the pixel down
							DWORD dClr = ConvertColorRef(srcRGB, &ddpfDest);
							pSurfDest[idxd] = dClr;

							idx++;
							idxd++;
						}
					}
				}
				break;

				case 16:
				{
					INT nPixelsPerRow = destSurface.lPitch / (ddpfDest.dwRGBBitCount/8);
					WORD* pSurfDest = (WORD*)destSurface.lpSurface;

					//now blt!!!
					INT idx = 0;	//index into source array...
					for (INT yy = 0; yy < height; yy++)
					{
						INT idxd = (yy+y)*nPixelsPerRow + x;
						for (INT xx=0; xx < width; xx++)
						{
							//convert pixels to RGB...
							LONG srcRGB = p_crPixelArray[idx];

							//put the pixel down
							WORD dClr = ConvertColorRef(srcRGB, &ddpfDest);
							pSurfDest[idxd] = dClr;

							idx++;
							idxd++;
						}
					}
				}
				break;

				default:
				{
					//for 24 bit, just do a regular set pixel thru GDI...
					SetPixelsGDI( p_crPixelArray, x, y, width, height );
				}
			}

			lpddsSurface->Unlock(NULL);
		}
		else
		{
			lpddsSurface->Unlock(NULL);
		}
	}
	else
	{
		SetPixelsGDI( p_crPixelArray, x, y, width, height );
	}

}

//--------------------------------------------------------------------------
// Set pixels using GDI
//--------------------------------------------------------------------------
INLINE VOID CGDICanvas::SetPixelsGDI(
	CONST LPLONG p_crPixelArray,
	CONST INT x,
	CONST INT y,
	CONST INT width,
	CONST INT height
		)
{

	// Lock the canvas
	Lock();

	// Position in the array
	INT arrayPos = 0;
	for (INT yy = y; yy < y + height; yy++)
	{
		for (INT xx = x; xx < x + width; xx++)
		{
			SetPixel(xx, yy, p_crPixelArray[arrayPos++]);
		}
	}

	// Unlock the canvas
	Unlock();

}
