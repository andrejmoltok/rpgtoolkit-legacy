/*
 * All contents copyright 2005, Jonathan D. Hughes
 * All rights reserved. You may not remove this notice.
 * Read license.txt for licensing details.
 */

#ifndef _CSPRITE_H_
#define _CSPRITE_H_

/*
 * Includes 
 */
#include "../../../tkCommon/strings.h"
#include "../movement.h"
#include "../../common/sprite.h"
#include "../../render/render.h"
#include "../CVector/CVector.h"
#include "../CPathFind/CPathFind.h"

class CSprite  
{
public:

	// Constructor.
	CSprite(const bool show);

	// Destructor.
	virtual ~CSprite() { }

	// Evaluate the current movement state.
	bool move(const CSprite *selectedPlayer, const bool bRunningProgram);

	// Return the number of pixels for the whole move (e.g. 32, 1, 2).
	int moveSize() const
	{
		const int result = (!m_bPxMovement ? 32 : round(PX_FACTOR / m_pos.loopSpeed));
		return (result < 1 ? 1 : result);
	};

	// Determine if the sprite's base intersects a RECT.
	CVector getVectorBase()
	{
		const DB_POINT p = { m_pos.x, m_pos.y };
		return (m_attr.vBase + p);
	};

	// Create default vectors, overwriting any user-defined.
	// Called for PRE_VECTOR_ITEMs.
	void createVectors() { m_attr.createVectors(m_brdData.activationType); };

	// Parse a Push() string and pass to setQueuedMovement().
	void parseQueuedMovements(STRING str);

	// Place a movement in the sprite's queue.
	void setQueuedMovement(const int queue, const bool bClearQueue, int step = 0);
	
	// Run all the movements in the queue.
	void runQueuedMovements();

	// Clear the queue.
	void clearQueue();

	// Queue up a path-finding path.
	void setQueuedPath(PF_PATH &path);

	// Queue just one point to create a path.
	void setQueuedPoint(DB_POINT pt)
	{
		if (!m_pend.path.empty() || m_pos.loopFrame > LOOP_MOVE) return;
		m_pend.path.push_back(pt);
	}

	// Pathfind to pixel position x, y (same layer).
	PF_PATH pathFind(const int x, const int y, const int type);

	// Get the destination.
	void getDestination(DB_POINT &p) const;

	// Complete the selected player's move.
	void playerDoneMove();

	// Set the sprite's locations based on a co-ordinate system.
	void setPosition(int x, int y, const int l, const COORD_TYPE coord);

	// Evaluate sprites (players and items).
	TILE_TYPE spriteCollisions();

	// Unconditionally send the sprite to the active board.
	void send();

	// Test for program activations (by programs, items, players).
	bool programTest();

	// Override repeat values for programs the player is standing on.
	void deactivatePrograms();

	// Debug: draw the sprite's base vector.
	void drawVector(CCanvas *const cnv);

	// Draw the path this sprite is on.
	void drawPath(CCanvas *const cnv);

	// Calculate sprite location and place on destination canvas.
	bool putSpriteAt(const CCanvas *cnvTarget, 
					const int layer,
					RECT &rect);

	// Align a RECT to the sprite's location.
	void alignBoard(RECT &rect, const bool bAllowNegatives);

	// Using pixel/tile movement (whole engine).
	static bool m_bPxMovement;		

	bool isActive() const { return m_bActive; }
	void setActive(const bool bActive) { m_bActive = bActive; }
	static void setLoopOffset(const int offset) { m_loopOffset = offset; }
	void setSpeed(const double delay) { m_attr.speed = delay * MILLISECONDS; }

	void freePath() { m_pathFind.freeVectors(); }

	SPRITE_POSITION getPosition() const { return m_pos; }

	// Swap the graphics of this sprite for those of another.
	void swapGraphics(CSprite *rhs)
	{
		m_attr.mapGfx = rhs->m_attr.mapGfx;
		m_attr.mapCustomGfx = rhs->m_attr.mapCustomGfx;
		// Vector bases also?
	}

protected:
	SPRITE_ATTR m_attr;				// Sprite attributes (common file data).
	bool m_bActive;					// Is the sprite visible?
	BRD_SPRITE m_brdData;			// Board-set sprite data (activation variables).
	SPRITE_RENDER m_lastRender;		// Last render location / frame of the sprite.
	CCanvas m_canvas;			// Pointer to sprite's frame.
	SPRITE_POSITION m_pos;			// Current location and frame details.
	PENDING_MOVEMENT m_pend;		// Pending movements of the player, including queue.
	TILE_TYPE m_tileType;			// The tiletypes at the sprite's location (NOT the "tiletype" of the sprite).
	DB_POINT m_v;					// Position vector in movement direction
	CPathFind m_pathFind;			// Sprite-specific pathfinding information.

private:

	// Record whether we need to run playerDoneMove().
	static bool m_bDoneMove;

	// Global speed offset.
	static int m_loopOffset;

	// Complete a single frame's movement of the sprite.
	bool push(const bool bScroll);

	// Take the angle of movement and return a MV_ENUM direction.
	MV_ENUM getDirection();

	// Get the next position co-ordinates.
	DB_POINT getTarget();

	// Increment target co-ordinates based on a direction.
	void setTarget(MV_ENUM direction);

	// Insert target co-ordinates from the path.
	void setPathTarget();

	// Calculate the loopSpeed - the number of renders that equate to
	// the sprite's movement speed (and any offsets).
	inline int calcLoops() const
	{
		extern double g_renderCount, g_renderTime;

		// Frames per millisecond.
		const double fpms = g_renderCount / g_renderTime;
		const int result = round(m_attr.speed * fpms) + (m_loopOffset * round(fpms * 100.0));
		return (result < 1 ? 1 : result);
	};

	// Evaluate board vectors.
	TILE_TYPE boardCollisions(LPBOARD board, const bool recursing = false);

	// Tests for movement at the board edges.
	TILE_TYPE boardEdges(const bool bSend);

	// Render if the current frame requires updating.
	bool render();
};

/*
 * A z-ordered vector of players and items.
 */
typedef struct tagZOrderedSprites
{
	std::vector<CSprite *> v;

	// Form v into a z-ordered vector from g_players and g_items.
	void zOrder();

	// Free pathfinding vectors CPathFind::m_obstructions.
	void freePaths()
	{
		for (std::vector<CSprite *>::iterator i = v.begin(); i != v.end(); ++i)
			(*i)->freePath();
	};

	// Remove a pointer from the vector.
	void remove(CSprite *p)
	{
		for (std::vector<CSprite *>::iterator i = v.begin(); i != v.end(); ++i)
		{
			if (*i == p) 
			{
				v.erase(i);
				return;
			}
		}
	};

} ZO_VECTOR;

#endif
