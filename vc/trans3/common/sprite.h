/*
 ********************************************************************
 * The RPG Toolkit, Version 3
 * This file copyright (C) 2006  Christopher Matthews & contributors
 *
 * Contributors:
 *    - Colin James Fitzpatrick
 *    - Jonathan D. Hughes
 ********************************************************************
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * Common sprite file attribute functions.
 */

#ifndef _SPRITE_H_
#define _SPRITE_H_

/*
 * Includes.
 */
#include <vector>
#include <map>
#include "../../tkCommon/strings.h"
#include "../movement/CVector/CVector.h"
#include "CAnimation.h"

/*
 * Definitions.
 */

// Ordered specifically for rotation permutations (when sliding).
typedef enum tagMovementCodes
{
	MV_IDLE,
	MV_E,
	MV_SE,
	MV_S,
	MV_SW,
	MV_W,
	MV_NW,
	MV_N,
	MV_NE
} MV_ENUM;

#define MV_MIN MV_E
#define MV_MAX MV_NE

// Prefix increment - rotate the movement "right".
inline MV_ENUM &operator++ (MV_ENUM &lhs)
{
	if (lhs == MV_MAX) { lhs = MV_MIN; }
	else if (lhs != MV_IDLE) { lhs = MV_ENUM(lhs + 1); }
	return lhs;
}

inline MV_ENUM &operator-- (MV_ENUM &lhs)
{
	if (lhs == MV_MIN) { lhs = MV_MAX; }
	else if (lhs != MV_IDLE) { lhs = MV_ENUM(lhs - 1); }
	return lhs;
}

inline MV_ENUM &operator+=(MV_ENUM &lhs, unsigned int inc)
{
	for (unsigned int i = 0; i < inc; ++i) ++lhs;
	return lhs;
}

/*
 * Movement vector values - increments for insertTarget().
 *		{ dx, dy }
 *		{ MV_IDLE, 
 *		 MV_E, MV_SE, MV_S, MV_SW, 
 *		 MV_W, MV_NW, MV_N, MV_NE }
 *
 *		g_directions[isIsometric][MV_CODE][x or y]
 */
const double g_directions[2][9][2] = 
			{
				// Non - isometric.
				{	{ 0, 0 }, 
					{ 1, 0 }, { 1, 1 }, { 0, 1 }, {-1, 1 },
					{-1, 0 }, {-1,-1 }, { 0,-1 }, { 1,-1 }
				},
				// Isometric.
				{	{ 0, 0 }, 
					{ 2, 0 }, { 1, 0.5 }, { 0, 1 }, {-1, 0.5 },
					{-2, 0 }, {-1,-0.5 }, { 0,-1 }, { 1,-0.5 }
				}
			};

// Vector indices for mapGfx.
#define GFX_MOVE 0
#define GFX_IDLE 1

// Custom map keys, mirrored in the battle plugin.
const STRING GFX_FIGHT		= _T("FIGHT");
const STRING GFX_DEFEND		= _T("DEFEND");
const STRING GFX_SPC		= _T("SPECIAL MOVE");
const STRING GFX_DIE		= _T("DIE");
const STRING GFX_REST		= _T("REST");

const STRING IDLE_E			= _T("idle e");
const STRING IDLE_SE		= _T("idle se");
const STRING IDLE_S			= _T("idle s");
const STRING IDLE_SW		= _T("idle sw");
const STRING IDLE_W			= _T("idle w");
const STRING IDLE_NW		= _T("idle nw");
const STRING IDLE_N			= _T("idle n");
const STRING IDLE_NE		= _T("idle ne");

/*
 * Common attributes of players and items:
 * animation sets, speed variables.
 */

// A sprite animation - store the file separately (or use a union?).
typedef struct tagGfxAnm
{
	STRING file;
	CSharedAnimation *pAnm;
	tagGfxAnm(): pAnm(NULL) {};

} GFX_ANM;

// A set of direction animations.
typedef std::map<MV_ENUM, GFX_ANM> GFX_MAP;
// A set of handle-indexed custom animations.
typedef std::map<STRING, GFX_ANM> GFX_CUSTOM_MAP;

typedef struct tagSpriteAttr
{
	// Provision for multiple sets of movement-indexed graphics.
	std::vector<GFX_MAP> mapGfx;

	// Map of custom animations, indexed by string handles.
	GFX_CUSTOM_MAP mapCustomGfx;

	double idleTime;							// Seconds to wait prior to switching to idle graphics.
	double speed;								// Seconds between each frame increase.
	CVector vActivate;							// Sprite's interaction area.
	CVector vBase;								// Sprite's contact area on board.

	tagSpriteAttr (void): 
		idleTime(3.0),
		speed(0.05), 
		vActivate (),
		vBase () {};

	~tagSpriteAttr() { freeAnimations(); }

	// Get the animation filename corresponding to stance.
	STRING getStanceAnm(STRING stance);

	// Fill out diagonal movement entries with axial entries.
	void completeStances(GFX_MAP &gfx);

	// Load animations in the GFX_MAPs.
	void loadAnimations(const bool bRenderFrames);	
	void freeAnimations(void);

	// Create some default vectors for old versions of players, items.
	void createVectors(const int activationType);

	// Get a default vector.
	static void defaultVector(
		DB_POINT *pts, 
		const bool bIsometric, 
		const bool bPixel, 
		const bool bActivate
	);

} SPRITE_ATTR;


/*
 * A board-set sprite.
 */

#define SPR_STEP		0				// Triggers once until player leaves area.
#define SPR_KEYPRESS	1				// Player must hit activation key.
#define SPR_REPEAT		2				// Triggers repeatedly after a certain distance or
										// can only be triggered after a certain distance.

#define SPR_ACTIVE		0				// Program is always active.
#define SPR_CONDITIONAL	1				// Program's running depends on RPGCode variables.

/*
 * A sprite path set in the board editor.
 */
typedef struct tagSpriteBoardPath
{
	CVector *pVector;					// Path in tagBoard::paths.
	int attributes;
	int cycles;
	int nextNode;						// Node currently travelling to.

	tagSpriteBoardPath(): pVector(NULL), attributes(0), cycles(0), nextNode(0) {}
	DB_POINT getNextNode()
	{
		DB_POINT pt = {0, 0};
		if (pVector)
		{
			pt = (*pVector)[nextNode];
			if (++nextNode >= pVector->size())
			{
				nextNode = 0;
				// Check if we need to finish.
				if (!--cycles) 
				{
					// Done. Null the pointer but do not delete the
					// path (since it belongs to the board).
					pVector = NULL;
				}
			}
		}
		return pt;
	}
	bool operator() (void) { return pVector; }

} SPR_BRDPATH, *LPSPR_BRDPATH;

typedef struct tagBoardSprite
{
	STRING fileName;					// Filename of item.
	short activate;						// SPR_ACTIVE - always active.
										// SPR_CONDITIONAL - conditional activation.
	STRING initialVar;					// Activation variable.
	STRING finalVar;					// Activation variable at end of sprite prg.
	STRING initialValue;				// Initial value of activation variable.
	STRING finalValue;					// Value of variable after sprite prg runs.
	STRING loadingVar;					// Variable checked before showing item on board.
	STRING loadingValue;				// Value loadingVar must be to show item.
	short activationType;				// Activation type: (flags)
										// SPR_STEP - walk in vector.
										// SPR_KEYPRESS - hit general activation key inside vector.
										// SPR_REPEAT - Whether player must leave vector before
										//				program can retrigger or not.
	STRING prgActivate;					// Program to run when sprite is activated.
	STRING prgMultitask;				// Multitask program for sprite.

	SPR_BRDPATH boardPath;				// Path from the board that the sprite is moving along.

	// The item will have its own vectors.
	tagBoardSprite(void): 
		activate(SPR_ACTIVE),
		activationType(SPR_STEP) {};

} BRD_SPRITE, *LPBRD_SPRITE;

#endif
