/*
 * All contents copyright 2005, Colin James Fitzpatrick.
 * All rights reserved. You may not remove this notice.
 * Read license.txt for licensing details.
 */

#ifndef _RENDER_H_
#define _RENDER_H_

/*
 * Inclusions.
 */
#include "../../tkCommon/tkDirectX/platform.h"

/*
 * Typedefs.
 */
typedef struct tagSpriteRender
{
    CGDICanvas *canvas;		// Canvas used for this render.
	std::string stance;		// Stance player was rendered in.
    unsigned int frame;		// Frame of this stance.
    double x;				// X position the render occured in.
    double y;				// Y position the render occured in.
} SPRITE_RENDER, *LPSPRITE_RENDER;

/*
 * Constants.
 */
const long TRANSP_COLOR = 16711935;	// Transparent color (magic pink).

/*
 * Initialize the graphics engine.
 */
void initGraphics(void);

/*
 * Shut down the graphics engine.
 */
void closeGraphics(void);

/*
 * Render the RPGCode screen.
 */
void renderRpgCodeScreen(void);

/*
 * Render the scene now.
 *
 * cnv (in) - canvas to render to (NULL is screen)
 * bForce (in) - force the render?
 * return (out) - did a render occur?
 */
bool renderNow(CGDICanvas *cnv = NULL, const bool bForce = false);

 /*** These functions are looking for homes ***/

bool drawTile(const std::string fileName, 
			  const int x, const int y, 
			  const int r, const int g, const int b, 
			  CGDICanvas *cnv, 
			  const bool bIsometric, 
			  const int nIsoEvenOdd);

bool drawTileMask (const std::string fileName, 
				   const int x, const int y, 
				   const int r, const int g, const int b, 
				   CGDICanvas *cnv,
				   const int nDirectBlt,
				   const bool bIsometric,
				   const int nIsoEvenOdd);

bool drawTileCnv(CGDICanvas *cnv, 
				 const std::string file, 
				 const double x,
				 const double y, 
				 const int r, 
				 const int g,
				 const int b, 
				 const bool bMask, 
				 const bool bNonTransparentMask = true, 
				 const bool bIsometric = false, 
				 const bool isoEvenOdd = false);


#endif
