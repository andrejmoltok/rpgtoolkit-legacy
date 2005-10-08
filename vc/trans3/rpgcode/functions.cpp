/*
 * All contents copyright 2005, Colin James Fitzpatrick.
 * All rights reserved. You may not remove this notice.
 * Read license.txt for licensing details.
 */

/*
 * RPGCode functions.
 */

/*
 * Inclusions.
 */
#include "../stdafx.h"
#include "CProgram.h"
#include "../../tkCommon/tkDirectX/platform.h"
#include "../../tkCommon/tkGfx/CTile.h"
#include "../input/input.h"
#include "../render/render.h"
#include "../audio/CAudioSegment.h"
#include "../common/animation.h"
#include "../common/board.h"
#include "../common/mainfile.h"
#include "../common/paths.h"
#include "../common/CAllocationHeap.h"
#include "../common/CInventory.h"
#include "../common/CFile.h"
#include "../movement/locate.h"
#include "../movement/CSprite/CSprite.h"
#include "../movement/CPlayer/CPlayer.h"
#include "../movement/CItem/CItem.h"
#include "../movement/CPathFind/CPathFind.h"
#include "../images/FreeImage.h"
#include "../fight/fight.h"
#include "../misc/misc.h"
#include "../plugins/plugins.h"
#include "CCursorMap.h"
#include <math.h>
#include <iostream>
#include <shellapi.h>
#include <objbase.h>

/*
 * Globals.
 */
extern CGDICanvas *g_cnvRpgCode;
std::string g_fontFace = "Arial";			// Font face.
int g_fontSize = 20;						// Font size.
COLORREF g_color = RGB(255, 255, 255);		// Current colour.
BOOL g_bold = FALSE;						// Bold enabled?
BOOL g_italic = FALSE;						// Italics enabled?
BOOL g_underline = FALSE;					// Underline enabled?
unsigned long g_mwinY = 0;					// MWin() y position.
std::string g_mwinBkg;						// MWin() background image.
COLORREF g_mwinColor = 0;					// Mwin() background colour.
CAllocationHeap<CGDICanvas> g_canvases;		// Allocated canvases.
CAllocationHeap<CCursorMap> g_cursorMaps;	// Cursor maps.
void *g_pTarget = NULL;						// Targetted entity.
TARGET_TYPE g_targetType = TI_EMPTY;		// Type of target entity.
void *g_pSource = NULL;						// Source entity.
TARGET_TYPE g_sourceType = TI_EMPTY;		// Type of source entity.
CInventory g_inv;							// Inventory.
unsigned long g_gp = 0;						// Amount of gold.
std::map<std::string, CFile> g_files;		// Files opened using RPGCode.

/*
 * Become ready to run a program.
 */
void programInit()
{
	extern CDirectDraw *g_pDirectDraw;
	g_pDirectDraw->CopyScreenToCanvas(g_cnvRpgCode);
	g_mwinY = 0;
	extern bool g_bShowMessageWindow;
	g_bShowMessageWindow = false;
}

/*
 * Finish running a program.
 */
void programFinish()
{
	extern bool g_bShowMessageWindow;
	g_bShowMessageWindow = false;
}

/*
 * Format direction string.
 */
std::string formatDirectionString(std::string str)
{
	str = parser::uppercase(str);
	const std::string delimiter = ",";

	// If the string contains any delimiters,
	// assume it's been properly formatted.
	if (str.find(delimiter, 0) != std::string::npos) return str;
	
	std::string s;

	for (std::string::iterator i = str.begin(); i != str.end(); ++i)
	{
		if (i[0] == ' ') continue;

		if (i[0] == 'N' || i[0] == 'W')
		{
			// Start of a direction.
			s += delimiter;
		}
		else if (i[0] == 'S')
		{
			// Look at the next letter.
			if (i >= str.end() - 1 || i[1] != 'T')
			{
				// Not part of west or east.
				s += delimiter;
			}
		}
		else if (i[0] == 'E')
		{
			// Look at the 2nd next letter.
			if (i >= str.end() - 2 || i[2] != 'T')
			{
				// Not part of west.
				s += delimiter;
			}
		}
		s += *i;

	} // for (i)

	return s;
}

/*
 * Get a player by name.
 */
IFighter *getFighter(const std::string name)
{
	// Check for "target".
	if (_strcmpi("target", name.c_str()) == 0)
	{
		if (g_targetType == TT_PLAYER || g_targetType == TT_ENEMY)
		{
			return (IFighter *)g_pTarget;
		}
		else
		{
			return NULL;
		}
	}
	// Check for "source".
	if (_strcmpi("source", name.c_str()) == 0)
	{
		if (g_sourceType == TT_PLAYER || g_sourceType == TT_ENEMY)
		{
			return (IFighter *)g_pSource;
		}
		else
		{
			return NULL;
		}
	}
	// Check names of party members.
	extern std::vector<CPlayer *> g_players;
	std::vector<CPlayer *>::iterator i = g_players.begin();
	for (; i != g_players.end(); ++i)
	{
		if (_strcmpi((*i)->name().c_str(), name.c_str()) == 0)
		{
			return *i;
		}
	}
	// Doesn't exist.
	return NULL;
}

/*
 * Get a player pointer from an index / literal target parameter.
 */
CPlayer *getPlayerPointer(STACK_FRAME &param)
{
	extern std::vector<CPlayer *> g_players;

	if (param.getType() & UDT_LIT)
	{
		// Handle, "target", "source".
		return (CPlayer *)getFighter(param.getLit());
	}

	// g_players index.
	const int i = int(param.getNum());
	if (i < g_players.size())
	{
		return g_players.at(i);
	}
	return NULL;
}

/*
 * Get an item pointer from a board index / literal target parameter.
 */
CItem *getItemPointer(STACK_FRAME &param)
{
	extern LPBOARD g_pBoard;

	if (param.getType() & UDT_NUM)
	{
		const unsigned int i = (unsigned int)param.getNum();
		if (g_pBoard->items.size() > i)
		{
			return g_pBoard->items[i];
		}
		else
		{
			char str[255]; itoa(i, str, 10);
			throw CError("Item does not exist at board index " + std::string(str) + ".");
		}
	}

	const std::string str = param.getLit();
	if (_strcmpi(str.c_str(), "target") == 0)
	{
		return (CItem *)g_pTarget;
	}
	else if (_strcmpi(str.c_str(), "source") == 0)
	{
		return (CItem *)g_pSource;
	}
	else
	{
		throw CError("Literal item target must be \"target\" or \"source\".");
	}

	return NULL;
}

/*
 * void mwin(string str)
 * 
 * Show the message window.
 */
void mwin(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("MWin() requires one parameter.");
	}
	extern CGDICanvas *g_cnvMessageWindow;
	// If this is the first line, draw the background.
	if (g_mwinY == 0)
	{
		if (!g_mwinBkg.empty())
		{
			drawImage(g_mwinBkg, g_cnvMessageWindow, 0, 0, 600, 100);
		}
		else
		{
			g_cnvMessageWindow->ClearScreen(g_mwinColor);
		}
		extern bool g_bShowMessageWindow;
		g_bShowMessageWindow = true;
	}
	// Write the text.
	g_cnvMessageWindow->DrawText(0, g_mwinY, params[0].getLit(), g_fontFace, g_fontSize, g_color, g_bold, g_italic, g_underline);
	g_mwinY += g_fontSize;
	// Draw the window.
	renderRpgCodeScreen();
}

/*
 * string wait([string &ret])
 * 
 * Wait for a key to be pressed, and return the key that was.
 */
void wait(CALL_DATA &params)
{
	params.ret().udt = UDT_LIT;
	params.ret().lit = waitForKey();
	if (params.params == 1)
	{
		*params.prg->getVar(params[0].lit) = params.ret();
	}
}

/*
 * void mwinCls()
 * 
 * Clear and hide the message window.
 */
void mwinCls(CALL_DATA &params)
{
	extern bool g_bShowMessageWindow;
	g_bShowMessageWindow = false;
	g_mwinY = 0;
	renderRpgCodeScreen();
}

/*
 * void send(string file, int x, int y, [int z = 1])
 * 
 * Send the player to a new board.
 */
void send(CALL_DATA &params)
{
	unsigned int layer = 1;
	if (params.params != 3)
	{
		if (params.params != 4)
		{
			throw CError("Send() requires three or four parameters.");
		}
		layer = params[3].getNum();
	}

	extern std::string g_projectPath;
	extern LPBOARD g_pBoard;
	g_pBoard->open(g_projectPath + BRD_PATH + params[0].getLit());

	unsigned int x = params[1].getNum(), y = params[2].getNum();

	/* Co-ordinate system stuff... */
	if (x > g_pBoard->bSizeX)
	{
		CProgram::debugger("Send() location exceeds target board x-dimension.");
		x = g_pBoard->bSizeX;
	}
	if (x < 1)
	{
		CProgram::debugger("Send() x location is less than one.");
		x = 1;
	}
	if (y > g_pBoard->bSizeY)
	{
		CProgram::debugger("Send() location exceeds target board y-dimension.");
		y = g_pBoard->bSizeY;
	}
	if (y < 1)
	{
		CProgram::debugger("Send() y location is less than one.");
		y = 1;
	}

	extern CSprite *g_pSelectedPlayer;

	g_pSelectedPlayer->setPosition(x, y, layer, g_pBoard->coordType);

	g_pSelectedPlayer->send();
}

/*
 * void text(double x, double y, string str, [canvas cnv])
 *
 * Displays text on the screen.
 */
void text(CALL_DATA &params)
{
	const int count = params.params;
	if (count != 3 && count != 4)
	{
		throw CError("Text() requires 3 or 4 parameters!");
	}
	CGDICanvas *cnv = (count == 3) ? g_cnvRpgCode : g_canvases.cast(int(params[3].getNum()));
	if (cnv)
	{
		cnv->DrawText(params[0].getNum() * g_fontSize - g_fontSize, params[1].getNum() * g_fontSize - g_fontSize, params[2].getLit(), g_fontFace, g_fontSize, g_color, g_bold, g_italic, g_underline);
	}
	if (count == 3)
	{
		renderRpgCodeScreen();
	}
}

/*
 * void pixelText(int x, int y, string str, [canvas cnv])
 *
 * Displays text on the screen using pixel coordinates.
 */
void pixelText(CALL_DATA &params)
{
	const int count = params.params;
	if (count != 3 && count != 4)
	{
		throw CError("PixelText() requires 3 or 4 parameters!");
	}
	CGDICanvas *cnv = (count == 3) ? g_cnvRpgCode : g_canvases.cast(int(params[3].getNum()));
	if (cnv)
	{
		cnv->DrawText(int(params[0].getNum()), int(params[1].getNum()), params[2].getLit(), g_fontFace, g_fontSize, g_color, g_bold, g_italic, g_underline);
	}
	if (count == 3)
	{
		renderRpgCodeScreen();
	}
}

/*
 * void branch(label lbl)
 * 
 * Jump to a label.
 */
void branch(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("Branch() requires one parameter.");
	}
	params.prg->jump((params[0].lit[0] == ':') ? params[0].lit : params[0].getLit());
}

/*
 * void change(string program)
 * 
 * Change this program so that next time it is triggered, a
 * different program runs instead. Change() is active only
 * while the player remains on the board (i.e., leaving
 * the board causes programs to return to normal).
 */
void change(CALL_DATA &params)
{

}

/*
 * void clear([canvas cnv])
 * 
 * Clear a surface.
 */
void clear(CALL_DATA &params)
{
	if (params.params != 0)
	{
		CGDICanvas *cnv = g_canvases.cast((int)params[0].getNum());
		if (cnv)
		{
			cnv->ClearScreen(0);
		}
	}
	else
	{
		g_cnvRpgCode->ClearScreen(0);
		renderRpgCodeScreen();
	}
}

/*
 * void done()
 * 
 * End the program.
 */
void done(CALL_DATA &params)
{
	params.prg->end();
}

/*
 * void windows()
 * 
 * Exit to windows.
 */
void windows(CALL_DATA &params)
{
	PostQuitMessage(0);
}

/*
 * void empty()
 * 
 * Clear all globals.
 */
void empty(CALL_DATA &params)
{
	params.prg->freeGlobals();
}

/*
 * void end()
 * 
 * End the program.
 */
void end(CALL_DATA &params)
{
	params.prg->end();
}

/*
 * void font(string font)
 * 
 * Load a font, either true type or TK2.
 */
void font(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("Font() requires one parameter.");
	}
	g_fontFace = params[0].getLit();
}

/*
 * void fontSize(int size)
 * 
 * Set the font size.
 */
void fontSize(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("FontSize() requires one parameter.");
	}
	g_fontSize = int(params[0].getNum());
}

/*
 * void fade(int type)
 * 
 * Perform a fade using the current colour. There are several
 * different types of fades.
 *
 * 0 - the screen is blotted out by a growing a shrinking box
 * 1 - blots out with vertical lines
 * 2 - fades from white to black
 * 3 - line sweeps across the screen
 * 4 - black circle swallows the player
 */
void fade(CALL_DATA &params)
{

}

/*
 * void fight(int skill, string background)
 * 
 * Start a skill level fight.
 */
void fight(CALL_DATA &params)
{
	if (params.params != 2)
	{
		throw CError("Fight() requires two parameters.");
	}
	skillFight(int(params[0].getNum()), params[1].getLit());
}

/*
 * string get([string &ret])
 * 
 * Get a key from the queue.
 */
void get(CALL_DATA &params)
{
	params.ret().udt = UDT_LIT;
	extern std::vector<char> g_keys;
	if (g_keys.size() == 0)
	{
		params.ret().lit = "";
		return;
	}
	const char chr = g_keys.front();
	g_keys.erase(g_keys.begin());
	std::string toRet;
	switch (chr)
	{
		case 13: toRet = "ENTER"; break;
		case 38: toRet = "UP"; break;
		case 40: toRet = "DOWN"; break;
		case 37: toRet = "RIGHT"; break;
		case 39: toRet = "LEFT"; break;
	}
	if (toRet.empty())
	{
		const char str[] = {chr, '\0'};
		toRet = str;
	}
	if (params.params == 1)
	{
		LPSTACK_FRAME var = params.prg->getVar(params[0].lit);
		var->udt = UDT_LIT;
		var->lit = toRet;
	}
	params.ret().lit = toRet;
}

/*
 * gone(...)
 * 
 * Description.
 */
void gone(CALL_DATA &params)
{

}

/*
 * void viewbrd(string filename [, int x, int y [, canvas cnv])
 * 
 * Draw a board to the screen or to a canvas,
 * starting at co-ordinates topX, topY.
 */
void viewbrd(CALL_DATA &params)
{
	extern CAllocationHeap<BOARD> g_boards;
	extern std::string g_projectPath;
	CGDICanvas *pCnv = g_cnvRpgCode;

	if (params.params == 4)
	{
		pCnv = g_canvases.cast(int(params[3].getNum()));
	}
	if (!pCnv) throw CError("ViewBrd(): canvas not found.");

	LPBOARD pBoard = NULL;
	if (params.params > 0)
	{
		pBoard = g_boards.allocate();
		if (!pBoard->open(g_projectPath + BRD_PATH + params[0].getLit()))
		{
			g_boards.free(pBoard);
			throw CError("ViewBrd(): unable to open board.");
		}
	}
	else throw CError("ViewBrd(): requires at least one parameter.");

	int x = 0, y = 0;
	if (params.params > 2)
	{
		x = params[1].getNum();
		y = params[2].getNum();
		pixelCoordinate(x, y, pBoard->coordType, false);
		if (pBoard->coordType == TILE_NORMAL)
		{
			// pixelCoordinate(,,,false) returns at top-left for 2D.
			x += 32;
			y += 32;
		}
	}

	pCnv->ClearScreen(pBoard->brdColor);
	pBoard->render(
		pCnv, 
		0, 0, 
		1, pBoard->bSizeL,
		x, y, 
		pCnv->GetWidth(), 
		pCnv->GetHeight(), 
		0, 0, 0); 
	g_boards.free(pBoard);

	if (params.params != 4) renderRpgCodeScreen();
}

/*
 * void bold(bool enable)
 * 
 * Toggle emboldening of text.
 */
void bold(CALL_DATA &params)
{
	if (params.params == 1)
	{
		g_bold = params[0].getBool();
	}
	else
	{
		throw CError("Bold() requires one parameter.");
	}
}

/*
 * void italics(bool enable)
 * 
 * Toggle italicizing of text.
 */
void italics(CALL_DATA &params)
{
	if (params.params == 1)
	{
		g_italic = params[0].getBool();
	}
	else
	{
		throw CError("Italics() requires one parameter.");
	}
}

/*
 * void underline(bool enable)
 * 
 * Toggle underlining of text.
 */
void underline(CALL_DATA &params)
{
	if (params.params == 1)
	{
		g_underline = params[0].getBool();
	}
	else
	{
		throw CError("Underline() requires one parameter.");
	}
}

/*
 * void winGraphic(string file)
 * 
 * Set the message window background image.
 */
void winGraphic(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("WinGraphic() requires one parameter.");
	}
	extern std::string g_projectPath;
	g_mwinBkg = g_projectPath + BMP_PATH + params[0].getLit();
	g_mwinColor = 0;
}

/*
 * void winColor(int dos)
 * 
 * Set the message window's colour using a DOS code.
 */
void winColor(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("WinColor() requires one parameter.");
	}
	g_mwinBkg = "";
	int color = int(params[0].getNum());
	if (color < 0) color = 0;
	else if (color > 255) color = 255;
	g_mwinColor = CTile::getDOSColor(color);
}

/*
 * void winColorRgb(int r, int g, int b)
 * 
 * Set the message window's colour.
 */
void winColorRgb(CALL_DATA &params)
{
	if (params.params != 3)
	{
		throw CError("WinColorRGB() requires three parameters.");
	}
	g_mwinBkg = "";
	g_mwinColor = RGB(int(params[0].getNum()), int(params[1].getNum()), int(params[2].getNum()));
}

/*
 * void color(int dos)
 * 
 * Change to a DOS colour.
 */
void color(CALL_DATA &params)
{
	if (params.params == 1)
	{
		int color = params[0].getNum();
		if (color < 0) color = 0;
		else if (color > 255) color = 255;
		g_color = CTile::getDOSColor(color);
	}
	else
	{
		throw CError("Color() requires one parameter.");
	}
}

/*
 * void colorRgb(int r, int g, int b)
 * 
 * Change the active colour to an RGB value.
 */
void colorRgb(CALL_DATA &params)
{
	if (params.params != 3)
	{
		throw CError("ColorRGB() requires three parameters.");
	}
	g_color = RGB(int(params[0].getNum()), int(params[1].getNum()), int(params[2].getNum()));
}

/*
 * void move(int x, int y, [int z = 1])
 * 
 * Move this board program to a new location on the board. The
 * effects last until the board has been left.
 */
void move(CALL_DATA &params)
{

}

/*
 * void over()
 * 
 * Displays a game over message and resets the game. Because
 * you can (and should) set a game over program, this function
 * is pointless.
 */
void over(CALL_DATA &params)
{

}

/*
 * void prg(string program. int x, int y, [int z = 1])
 * 
 * Move a program on the current board to a new location. This
 * stays in effect until the board is left.
 */
void prg(CALL_DATA &params)
{

}

/*
 * string prompt(string question, [string &ret])
 * 
 * Ask the player a question, and return the result.
 */
void prompt(CALL_DATA &params)
{

}

/*
 * void put(int x, int y, string tile)
 * 
 * Puts a tile at the specified location on the board.
 */
void put(CALL_DATA &params)
{
	if (params.params != 3)
	{
		throw CError("Put() requires three parameters.");
	}
	// TBD: getAmbientLevel();
	drawTileCnv(g_cnvRpgCode, params[2].getLit(), params[0].getNum(), params[1].getNum(), 0, 0, 0, false, true, false, false);
	renderRpgCodeScreen();
}

/*
 * void reset()
 * 
 * Reset the game.
 */
void reset(CALL_DATA &params)
{

}

/*
 * void run(string program)
 * 
 * Transfer control to a different program.
 */
void run(CALL_DATA &params)
{
	if (params.params == 1)
	{
		params.prg->end();
		extern std::string g_projectPath;
		CProgram(g_projectPath + PRG_PATH + params[0].getLit()).run();
	}
	else
	{
		throw CError("Run() requires one data element.");
	}
}

/*
 * void sound()
 * 
 * Obsolete.
 */
void sound(CALL_DATA &params)
{
	throw CError("Sound() is obsolete. Please use TK3's media functions.");
}

/*
 * void win()
 * 
 * Obsolete.
 */
void win(CALL_DATA &params)
{
	throw CError("Win() is obsolete.");
}

/*
 * void hp(string handle, int value)
 * 
 * Set a fighter's hp.
 */
void hp(CALL_DATA &params)
{
	if (params.params != 2)
	{
		throw CError("HP() requires two parameters.");
	}
	IFighter *p = getFighter(params[0].getLit());
	if (p)
	{
		p->health((params[0].getNum() <= p->maxHealth()) ? params[0].getNum() : p->maxHealth());
	}
}

/*
 * void giveHp(string handle, int add)
 * 
 * Increase a fighter's current hp.
 */
void giveHp(CALL_DATA &params)
{
	if (params.params != 2)
	{
		throw CError("GiveHP() requires two parameters.");
	}
	IFighter *p = getFighter(params[0].getLit());
	if (p)
	{
		const int hp = p->health() + params[0].getNum();
		p->health((hp <= p->maxHealth()) ? ((hp > 0) ? hp : 0) : p->maxHealth());
		// TBD: if (fighting)
		// {
		//		...
		// }
	}
}

/*
 * int getHp(string handle, [int &ret])
 * 
 * Get a fighter's hp.
 */
void getHp(CALL_DATA &params)
{
	if (params.params == 1)
	{
		IFighter *p = getFighter(params[0].getLit());
		if (p)
		{
			params.ret().udt = UDT_NUM;
			params.ret().num = p->health();
		}
	}
	else if (params.params == 2)
	{
		IFighter *p = getFighter(params[0].getLit());
		if (p)
		{
			LPSTACK_FRAME var = params.prg->getVar(params[1].lit);
			var->udt = UDT_NUM;
			var->num = p->health();
		}
	}
	else
	{
		throw CError("GetHP() requires one or two parameters.");
	}
}

/*
 * void maxHp(string handle, int value)
 * 
 * Set a fighter's max hp.
 */
void maxHp(CALL_DATA &params)
{
	if (params.params != 2)
	{
		throw CError("MaxHP() requires two parameters.");
	}
	IFighter *p = getFighter(params[0].getLit());
	if (p)
	{
		p->maxHealth(params[1].getNum());
	}
}

/*
 * int getMaxHp(string handle, [int &ret])
 * 
 * Get a fighter's max hp.
 */
void getMaxHp(CALL_DATA &params)
{
	if (params.params == 1)
	{
		IFighter *p = getFighter(params[0].getLit());
		if (p)
		{
			params.ret().udt = UDT_NUM;
			params.ret().num = p->maxHealth();
		}
	}
	else if (params.params == 2)
	{
		IFighter *p = getFighter(params[0].getLit());
		if (p)
		{
			LPSTACK_FRAME var = params.prg->getVar(params[1].lit);
			var->udt = UDT_NUM;
			var->num = p->maxHealth();
		}
	}
	else
	{
		throw CError("GetMaxHP() requires one or two parameters.");
	}
}

/*
 * void smp(string handle, int value)
 * 
 * Set a fighter's smp.
 */
void smp(CALL_DATA &params)
{
	if (params.params != 2)
	{
		throw CError("SMP() requires two parameters.");
	}
	IFighter *p = getFighter(params[0].getLit());
	if (p)
	{
		p->smp((params[0].getNum() <= p->maxSmp()) ? params[0].getNum() : p->maxSmp());
	}
}

/*
 * void giveSmp(string handle, int value)
 * 
 * Increase a fighter's smp.
 */
void giveSmp(CALL_DATA &params)
{
	if (params.params != 2)
	{
		throw CError("GiveSMP() requires two parameters.");
	}
	IFighter *p = getFighter(params[0].getLit());
	if (p)
	{
		const int hp = p->smp() + params[0].getNum();
		p->smp((hp <= p->maxSmp()) ? ((hp > 0) ? hp : 0) : p->maxSmp());
		// if (fighting)
		// {
		//		...
		// }
	}
}

/*
 * int getSmp(string handle, [int &ret])
 * 
 * Get a fighter's smp.
 */
void getSmp(CALL_DATA &params)
{
	if (params.params == 1)
	{
		IFighter *p = getFighter(params[0].getLit());
		if (p)
		{
			params.ret().udt = UDT_NUM;
			params.ret().num = p->smp();
		}
	}
	else if (params.params == 2)
	{
		IFighter *p = getFighter(params[0].getLit());
		if (p)
		{
			LPSTACK_FRAME var = params.prg->getVar(params[1].lit);
			var->udt = UDT_NUM;
			var->num = p->smp();
		}
	}
	else
	{
		throw CError("GetSMP() requires one or two parameters.");
	}
}

/*
 * void maxSmp(string handle, int value)
 * 
 * Set a fighter's max smp.
 */
void maxSmp(CALL_DATA &params)
{
	if (params.params != 2)
	{
		throw CError("MaxSMP() requires two parameters.");
	}
	IFighter *p = getFighter(params[0].getLit());
	if (p)
	{
		p->maxSmp(params[1].getNum());
	}
}

/*
 * int getMaxSmp(string handle, [int &ret])
 * 
 * Get a fighter's max smp.
 */
void getMaxSmp(CALL_DATA &params)
{
	if (params.params == 1)
	{
		IFighter *p = getFighter(params[0].getLit());
		if (p)
		{
			params.ret().udt = UDT_NUM;
			params.ret().num = p->maxSmp();
		}
	}
	else if (params.params == 2)
	{
		IFighter *p = getFighter(params[0].getLit());
		if (p)
		{
			LPSTACK_FRAME var = params.prg->getVar(params[1].lit);
			var->udt = UDT_NUM;
			var->num = p->maxSmp();
		}
	}
	else
	{
		throw CError("GetMaxSMP() requires one or two parameters.");
	}
}

/*
 * void start(string file)
 * 
 * Open a file with the shell.
 *
 * There is unfortunately no default directory for this
 * function and assuming one will only break backward
 * compatibility in some obscure manner.
 *
 * Note also that previous incarnations prevented the
 * launching of executables and shortcuts, but this is
 * a nonsensical security measure when plugins no longer
 * display a silly warning box, and this function can be
 * trivially modified by someone who intends to be malicious.
 */
void start(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("Start() requires one parameter.");
	}
	ShellExecute(NULL, "open", params[0].getLit().c_str(), NULL, NULL, 0);
}

/*
 * void giveItem(string itm)
 * 
 * Add an item to the inventory.
 */
void giveItem(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("GiveItem() requires one parameter.");
	}
	extern std::string g_projectPath;
	g_inv.give(g_projectPath + ITM_PATH + params[0].getLit());
}

/*
 * void takeItem(string itm)
 * 
 * Remove an item from the inventory.
 */
void takeItem(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("TakeItem() requires one parameter.");
	}
	extern std::string g_projectPath;
	g_inv.take(g_projectPath + ITM_PATH + params[0].getLit());
}

/*
 * void wav(string file)
 * 
 * Play a wave file (e.g. a sound effect).
 */
void wav(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("Wav() requires one parameter.");
	}
	CAudioSegment::playSoundEffect(params[0].getLit());
}

/*
 * void delay(double time)
 * 
 * Delay for a certain number of seconds.
 */
void delay(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("Delay() requires one data element.");
	}
	Sleep(DWORD(params[0].getNum() * 1000.0));
}

/*
 * int random(int max, [int &ret])
 * 
 * Generate a random number from one to the supplied maximum, inclusive.
 */
void random(CALL_DATA &params)
{
	if ((params.params != 1) && (params.params != 2))
	{
		throw CError("Random() requires one or two parameters.");
	}
	params.ret().udt = UDT_NUM;
	params.ret().num = (rand() % int(params[0].getNum())) + 1;
	if (params.params == 2)
	{
		*params.prg->getVar(params[1].lit) = params.ret();
	}
}

/*
 * void tileType(int x, int y, string type, [int z = 1])
 * 
 * Change a tile's type. Valid types for the string parameter
 * are "NORMAL", "SOLID", "UNDER", "NS", "EW", "STAIRS#".
 * Using vector collision, square vectors are added to the board with 
 * corresponding types. If an identical vector exists at the point,
 * alter its type rather than adding another.
 * Note: Overriding with "normal" may now not work as intended - users
 * should use vector tools instead.
 */
void tileType(CALL_DATA &params)
{
	/** TBD: unidirectionals **/

	extern LPBOARD g_pBoard;

	if (params.params != 3 && params.params != 4)
	{
		throw CError("TileType() requires three or four parameters.");
	}

	// Construct new vector. v.type defaults to TT_SOLID.
	BRD_VECTOR v;
	v.pV = new CVector();

	const std::string type = params[2].getLit();
	if (_strcmpi(type.c_str(), "NORMAL") == 0) v.type = TT_N_OVERRIDE;
	else if (_strcmpi(type.c_str(), "UNDER") == 0) v.type = TT_UNDER;
	else if (_strcmpi(type.substr(0, 6).c_str(), "STAIRS") == 0)
	{
		v.type = TT_STAIRS;
		v.attributes = atoi(type.substr(6).c_str());
	}
	else if (_strcmpi(type.c_str(), "NS") == 0) v.type = TT_UNIDIRECTIONAL;
	else if (_strcmpi(type.c_str(), "EW") == 0) v.type = TT_UNIDIRECTIONAL;

	// Transform to pixel co-ordinates.
	int x = params[0].getNum(), y = params[1].getNum();
	pixelCoordinate(x, y, g_pBoard->coordType, false);

	if (g_pBoard->isIsometric())
	{
		// Isometric diamond. See tagBoard::vectorize() for order.
		DB_POINT p[] = {{x, y - 16}, {x - 32, y}, {x, y + 16}, {x + 32, y}};
		v.pV->push_back(p, 4);
	}
	else
	{
		// 2D square. See tagBoard::vectorize() for order.
		DB_POINT p[] = {{x, y}, {x, y + 32}, {x + 32, y + 32}, {x + 32, y}};
		v.pV->push_back(p, 4);
	}
	v.pV->close(true);
	v.layer = (params.params == 4 ? params[3].getNum() : 1);

	std::vector<BRD_VECTOR>::iterator i = g_pBoard->vectors.begin();
	for (; i != g_pBoard->vectors.end(); ++i)
	{
		if (i->layer != v.layer) continue;
		if (i->pV->compare(*v.pV))
		{
			// Found a matching vector.
			if (v.type == TT_N_OVERRIDE)
			{
				// Delete this vector.
				delete i->pV;
				delete i->pCnv;
				g_pBoard->vectors.erase(i);
			}
			else
			{
				i->type = v.type;
				i->attributes = v.attributes;
				if (v.type == TT_UNDER) i->createCanvas(*g_pBoard);
			}
			return;
		}
	}

	// Didn't find a match.
	g_pBoard->vectors.push_back(v);
	if (v.type == TT_UNDER) g_pBoard->vectors.back().createCanvas(*g_pBoard);
}

/*
 * void mediaPlay(string file)
 * 
 * Play the specified file as the background music.
 */
void mediaPlay(CALL_DATA &params)
{
	if (params.params == 1)
	{
		extern CAudioSegment *g_bkgMusic;
		g_bkgMusic->open(params[0].getLit());
		g_bkgMusic->play(true);
	}
	else
	{
		throw CError("MediaPlay() requires one parameter.");
	}
}

/*
 * void mediaStop()
 * 
 * Stop the background music.
 */
void mediaStop(CALL_DATA &params)
{
	extern CAudioSegment *g_bkgMusic;
	g_bkgMusic->stop();
}

/*
 * void goDos(string command)
 * 
 * Obsolete.
 */
void goDos(CALL_DATA &params)
{
	throw CError("GoDos() is obsolete.");
}

/*
 * void setPixel(int x, int y, [canvas cnv])
 * 
 * Set a pixel in the current colour.
 */
void setPixel(CALL_DATA &params)
{
	if (params.params == 2)
	{
		g_cnvRpgCode->SetPixel(params[0].getNum(), params[1].getNum(), g_color);
		renderRpgCodeScreen();
	}
	else if (params.params == 3)
	{
		CGDICanvas *cnv = g_canvases.cast((int)params[2].getNum());
		if (cnv)
		{
			cnv->SetPixel(params[0].getNum(), params[1].getNum(), g_color);
		}
	}
	else
	{
		throw CError("SetPixel() requires two or three parameters.");
	}
}

/*
 * void drawLine(int x1, int y1, int x2, int y2, [canvas cnv])
 * 
 * Draw a line.
 */
void drawLine(CALL_DATA &params)
{
	if (params.params == 4)
	{
		g_cnvRpgCode->DrawLine(params[0].getNum(), params[1].getNum(), params[2].getNum(), params[3].getNum(), g_color);
		renderRpgCodeScreen();
	}
	else if (params.params == 5)
	{
		CGDICanvas *cnv = g_canvases.cast((int)params[4].getNum());
		if (cnv)
		{
			cnv->DrawLine(params[0].getNum(), params[1].getNum(), params[2].getNum(), params[3].getNum(), g_color);
		}
	}
	else
	{
		throw CError("DrawLine() requires four or five parameters.");
	}
}

/*
 * void drawRect(int x1, int y1, int x2, int y2, [canvas cnv])
 * 
 * Draw a rectangle.
 */
void drawRect(CALL_DATA &params)
{
	if (params.params == 4)
	{
		g_cnvRpgCode->DrawRect(params[0].getNum(), params[1].getNum(), params[2].getNum(), params[3].getNum(), g_color);
		renderRpgCodeScreen();
	}
	else if (params.params == 5)
	{
		CGDICanvas *cnv = g_canvases.cast((int)params[4].getNum());
		if (cnv)
		{
			cnv->DrawRect(params[0].getNum(), params[1].getNum(), params[2].getNum(), params[3].getNum(), g_color);
		}
	}
	else
	{
		throw CError("DrawRect() requires four or five parameters.");
	}
}

/*
 * void fillRect(int x1, int y1, int x2, int y2, [canvas cnv])
 * 
 * Draw a filled rectangle.
 */
void fillRect(CALL_DATA &params)
{
	if (params.params == 4)
	{
		g_cnvRpgCode->DrawFilledRect(params[0].getNum(), params[1].getNum(), params[2].getNum(), params[3].getNum(), g_color);
		renderRpgCodeScreen();
	}
	else if (params.params == 5)
	{
		CGDICanvas *cnv = g_canvases.cast((int)params[4].getNum());
		if (cnv)
		{
			cnv->DrawFilledRect(params[0].getNum(), params[1].getNum(), params[2].getNum(), params[3].getNum(), g_color);
		}
	}
	else
	{
		throw CError("FillRect() requires four or five parameters.");
	}
}

/*
 * void debug(bool enable)
 * 
 * Toggle debug mode.
 */
void debug(CALL_DATA &params)
{

}

/*
 * double castNum(variant x, [double &ret])
 * 
 * Cast the specified value to a number (double). There is really no need
 * to do this ever. RPGCode by itself will cast values passed to functions
 * to the correct types as required.
 */
void castNum(CALL_DATA &params)
{
	if (params.params == 1)
	{
		params.ret().udt = UDT_NUM;
		params.ret().num = params[0].getNum();
	}
	else if (params.params == 2)
	{
		LPSTACK_FRAME var = params.prg->getVar(params[1].lit);
		var->num = params[0].getNum();
		var->udt = UDT_NUM;
	}
	else
	{
		throw CError("CastNum() requires one or two parameters.");
	}
}

/*
 * string castLit(variant x, [string &ret])
 * 
 * Cast the specified value to a string. There is really no need
 * to do this ever. RPGCode by itself will cast values passed to
 * functions to the correct types as required.
 */
void castLit(CALL_DATA &params)
{
	if (params.params == 1)
	{
		params.ret().udt = UDT_LIT;
		params.ret().lit = params[0].getLit();
	}
	else if (params.params == 2)
	{
		LPSTACK_FRAME var = params.prg->getVar(params[1].lit);
		var->lit = params[0].getLit();
		var->udt = UDT_LIT;
	}
	else
	{
		throw CError("CastLit() requires one or two parameters.");
	}
}

/*
 * int castInt(variant x, [int &ret])
 * 
 * Cast the specified value to an integer (i.e., a number in the sequence
 * ...-2, -1, 0, 1, 2...). This is useful for removing fractional parts of
 * numbers. Note that this function does not round.
 */
void castInt(CALL_DATA &params)
{
	if (params.params == 1)
	{
		params.ret().udt = UDT_NUM;
		params.ret().num = double(int(params[0].getNum()));
	}
	else if (params.params == 2)
	{
		LPSTACK_FRAME var = params.prg->getVar(params[1].lit);
		var->num = double(int(params[0].getNum()));
		var->udt = UDT_NUM;
	}
	else
	{
		throw CError("CastInt() requires one or two parameters.");
	}
}

/*
 * bool pixelMovement([bool pixelMovement [, bool pixelPush]])
 * 
 * Toggles pixel movement and push() et al. in pixels.
 * Returns whether pixel movement is being used.
 * pixelPush is ineffective for tile movement.
 */
void pixelmovement(CALL_DATA &params)
{
	extern MAIN_FILE g_mainFile;

	if ((params.params == 1) || (params.params == 2))
	{
		if (params.params == 2)
		{
			g_mainFile.pixelMovement = params[1].getBool() ? MF_PUSH_PIXEL : MF_MOVE_PIXEL;
		}

		if (params[0].getBool())
		{
			CSprite::m_bPxMovement = true;
		}
		else
		{
			g_mainFile.pixelMovement = MF_MOVE_TILE;
			CSprite::m_bPxMovement = false;
		}
	}

	// Return the state.
	params.ret().udt = UDT_NUM;
	params.ret().num = double(CSprite::m_bPxMovement);
}

/*
 * string pathfind (int x1, int y1, int x2, int y2, string &ret [, int layer])
 *
 * Construct a directional string of the shortest tiled path from
 * one location to the other.
 */
void pathfind(CALL_DATA &params)
{
	extern CSprite *g_pSelectedPlayer;
	extern LPBOARD g_pBoard;

	if (params.params < 4 || params.params > 6)
	{
		throw CError("PathFind() requires four, five or six parameters."); 
	}

	const int layer = (params.params == 6) ?
					  int(params[5].getNum()) :
					  g_pSelectedPlayer->getPosition().l;

	int x1 = int(params[0].getNum()), y1 = int(params[1].getNum()),
		x2 = int(params[2].getNum()), y2 = int(params[3].getNum());

	// Transform the input co-ordinates based on the board co-ordinate system.
	pixelCoordinate(x1, y1, g_pBoard->coordType, true);
	pixelCoordinate(x2, y2, g_pBoard->coordType, true);

	// Parameters. r is unneeded for tile pathfinding.
	const DB_POINT start = {x1, y1}, goal = {x2, y2};
	const RECT r = {0, 0, 0, 0};
	std::string s;

	// Pre C++, PathFind() was implemented axially only.
	CPathFind path;
	path.pathFind(start, goal, layer, r, PF_AXIAL, NULL); 
	std::vector<MV_ENUM> p = path.directionalPath();

	for (std::vector<MV_ENUM>::reverse_iterator i = p.rbegin(); i != p.rend(); ++i)
	{
		switch (*i)
		{
			case MV_N: { s += "N"; } break;
			case MV_S: { s += "S"; } break;
			case MV_E: { s += "E"; } break;
			case MV_W: { s += "W"; } break;
			case MV_NE: { s += "NE"; } break;
			case MV_NW: { s += "NW"; } break;
			case MV_SE: { s += "SE"; } break;
			case MV_SW: { s += "SW"; }
		}
		if (i != p.rend() - 1) s += ",";
	}
	
	params.ret().udt = UDT_LIT;
	params.ret().lit = s;

	if (params.params >= 5)
	{
		*params.prg->getVar(params[4].lit) = params.ret();
	}
}

/*
 * void playerstep(string handle, int x, int y)
 * 
 * Causes the player to take one step in the direction of x, y
 * following a route determined by pathFind.
 */
void playerstep(CALL_DATA &params)
{
	extern LPBOARD g_pBoard;

	if (params.params != 3)
	{
		throw CError("PlayerStep() requires three parameters.");
	}

	CSprite *p = getPlayerPointer(params[0]);
	if (!p) throw CError("PlayerStep(): player not found");

	int x = int(params[1].getNum()), y = int(params[2].getNum());
	pixelCoordinate(x, y, g_pBoard->coordType, true);

	PF_PATH path = p->pathFind(x, y, PF_AXIAL);
	if (!path.empty())
	{
		// Prune to the last element.
		path.front() = path.back();		
		path.resize(1);
		p->setQueuedPath(path);

		if (!params.prg->isThread())
		{
			// If not a thread, move now.
			p->runQueuedMovements();
		}
	}
}

/*
 * void itemstep(variant handle, int x, int y)
 * 
 * Causes the item to take one step in the direction of x, y
 * following a route determined by pathFind.
 */
void itemstep(CALL_DATA &params)
{
	extern LPBOARD g_pBoard;

	if (params.params != 3)
	{
		throw CError("ItemStep() requires three parameters.");
	}

	CSprite *p = getItemPointer(params[0]);
	if (!p) throw CError("ItemStep(): item not found");

	int x = int(params[1].getNum()), y = int(params[2].getNum());
	pixelCoordinate(x, y, g_pBoard->coordType, true);

	PF_PATH path = p->pathFind(x, y, PF_AXIAL);
	if (!path.empty())
	{
		// Prune to the last element.
		path.front() = path.back();		
		path.resize(1);
		p->setQueuedPath(path);

		if (!params.prg->isThread())
		{
			// If not a thread, move now.
			p->runQueuedMovements();
		}
	}
}

/*
 * void push(string direction[, string handle])
 * 
 * Push the player with the specified handle, or the default player
 * if no handle is specified, along the given directions. The direction
 * should be a comma delimited, but if it is not, it will be delimited
 * for backward compatibility. These styles are accepted, and can be
 * mixed even within the same directonal string:
 *
 * - N, S, E, W, NE, NW, SE, SW
 * - NORTH, SOUTH, EAST, WEST, NORTHEAST, NORTHWEST, SOUTHEAST, SOUTHWEST
 * - 1, 2, 3, 4, 5, 6, 7, 8
 */
void push(CALL_DATA &params)
{
	extern CSprite *g_pSelectedPlayer;
	extern std::vector<CPlayer *> g_players;

	if ((params.params != 1) && (params.params != 2))
	{
		throw CError("Push() requires one or two parameters.");
	}

	CSprite *p = NULL;

	if (params.params == 2)
	{
		p = getPlayerPointer(params[1]);
	}
	else
	{
		p = g_pSelectedPlayer;
	}
	if (!p) throw CError("Push(): player not found");

	// Backwards compatibility.
	std::string str = formatDirectionString(params[0].getLit());

	// Parse and set queued movements.
	p->parseQueuedMovements(str);

	if (!params.prg->isThread())
	{
		// If not a thread, move now.
		p->runQueuedMovements();
	}
}

/*
 * void pushItem(variant item, string direction)
 * 
 * The first parameter accepts either a string that can be either
 * "target" or "source" direction or the number of an item. The
 * syntax of the directional string is the same as for [[push()]].
 */
void pushItem(CALL_DATA &params)
{
	extern CSprite *g_pSelectedPlayer;

	if (params.params != 2)
	{
		throw CError("PushItem() requires two parameters.");
	}

	CSprite *p = getItemPointer(params[0]);
	if (!p) throw CError("PushItem(): item not found");

	// Backwards compatibility.
	std::string str = formatDirectionString(params[1].getLit());

	// Parse and set queued movements.
	p->parseQueuedMovements(str);

	if (!params.prg->isThread())
	{
		// If not a thread, move now.
		p->runQueuedMovements();
	}
}

/*
 * void wander(variant target, [int restrict = 0])
 * 
 * The first parameter accepts either a string that can be either
 * "target" or "source" or the number of an item. The selected item
 * will take a step in a random direction, or as restricted by the
 * optional parameter. The allowed values for said parameter are:
 *
 * 0 - only north, south, east, and west on normal boards, only
 *     diagonals on isometric boards (default)
 * 1 - only north, south, east, and west
 * 2 - only diagonals
 * 3 - all directions
 */
void wander(CALL_DATA &params)
{
	extern LPBOARD g_pBoard;
	extern const double g_directions[2][9][2];

	if ((params.params != 1) && (params.params != 2))
	{
		throw CError("Wander() requires one or two parameters.");
	}

	CSprite *p = getItemPointer(params[0]);
	if (!p) throw CError("Wander(): item not found");

	const int isIso = int(g_pBoard->isIsometric());

	int restrict = 0;
	if (params.params == 2)
	{
		restrict = int(params[1].getNum());
		if (restrict < 0) restrict = 0;
		else if (restrict > 3) restrict = 3;
	}

	if (restrict-- == 0)
	{
		restrict = isIso;
	}

	int direction = 0;

	if (restrict == 0)
	{
		direction = (rand() % 4) * 2 + 1;
	}
	else if (restrict == 1)
	{
		direction = (rand() % 4) * 2 + 2;
	}
	else
	{
		direction = rand() % 8 + 1;
	}

	DB_POINT d;
	p->getDestination(d);

	DB_POINT pt = {d.x + g_directions[isIso][direction][0] * 32, d.y + g_directions[isIso][direction][1] * 32};
	p->setQueuedPoint(pt);

	if (!params.prg->isThread())
	{
		// If not a thread, move now.
		p->runQueuedMovements();
	}
}

/*
 * void addPlayer(string file)
 * 
 * Add a player to the party.
 */
void addPlayer(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("AddPlayer() requires one parameter.");
	}
	extern std::vector<CPlayer *> g_players;
	extern std::string g_projectPath;
	g_players.push_back(new CPlayer(g_projectPath + TEM_PATH + params[0].getLit(), false));
}

/*
 * void putplayer(string handle, int x, int y, int layer)
 * 
 * Place the player on the board at the given location.
 */
void putplayer(CALL_DATA &params)
{
	extern LPBOARD g_pBoard;
	extern RECT g_screen;
	extern ZO_VECTOR g_sprites;

	if (params.params != 4)
	{
		throw CError("PutPlayer() requires four parameters.");
	}

	CSprite *p = getPlayerPointer(params[0]);
	if (!p) throw CError("PutPlayer(): player not found");

	p->setActive(true);
    p->setPosition(int(params[1].getNum()), 
		int(params[2].getNum()), 
		int(params[3].getNum()), 
		g_pBoard->coordType);

	// Insert the pointer into the z-ordered vector.
	g_sprites.zOrder();
    
	/** TBD: do not "auto align" ?
	p->alignBoard(g_screen, true); **/
	renderNow(g_cnvRpgCode, true);
	renderRpgCodeScreen();
}

/*
 * void eraseplayer(string handle)
 *
 * Erase a party player from the screen.
 */
void eraseplayer(CALL_DATA &params)
{
	extern ZO_VECTOR g_sprites;

	if (params.params != 1)
	{
		throw CError("ErasePlayer() requires one parameter.");
	}

	CSprite *p = getPlayerPointer(params[0]);
	if (!p) throw CError("ErasePlayer(): player not found");

	// Remove the player from the z-ordered vector.
	g_sprites.remove(p);
	p->setActive(false);

	renderNow(g_cnvRpgCode, true);
	renderRpgCodeScreen();
}

/*
 * void removePlayer(string handle)
 * 
 * Remove a player from the party to an old player list.
 */
void removePlayer(CALL_DATA &params)
{

}

/*
 * void newPlyr(string file)
 * 
 * Change the graphics of the main player to that of the
 * file passed in. The file can be a character file (*.tem)
 * or that of a tile (*.gph, *.tstxxx, *.tbm).
 */
void newPlyr(CALL_DATA &params)
{
	extern CSprite *g_pSelectedPlayer;
	extern std::string g_projectPath;

	if (params.params != 1)
	{
		throw CError("newPlyr() requires one parameter.");
	}
	std::string ext = getExtension(params[0].getLit());

	if (_strcmpi(ext.c_str(), "TEM") == 0)
	{
		// Load new sprite graphics from this character.
		CPlayer p(g_projectPath + TEM_PATH + params[0].getLit(), false);
		g_pSelectedPlayer->swapGraphics(&p);
	}
	else if (_strcmpi(ext.c_str(), "GPH") == 0)
	{
		/** TBD: Construct anm... / depreciate **/
	}
	else if (_strcmpi(ext.substr(0, 3).c_str(), "TST") == 0)
	{
		/** TBD: Construct anm... / not in 3.0.6 **/
	}
	else if (_strcmpi(ext.c_str(), "TBM") == 0)
	{
		/** TBD: Construct anm... / not in 3.0.6 **/
	}
	else
	{
		throw CError("newPlyr() requires a tem, tst or tbm file.");
	}
}

/*
 * int onBoard(variant handle)
 *
 * Return whether a player is being shown on the board or not.
 */
void onboard(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("OnBoard() requires one parameter.");
	}
	
	CSprite *p = getPlayerPointer(params[0]);
	if (!p) throw CError("OnBoard(): player not found");

	params.ret().udt = UDT_NUM;
	params.ret().num = p->isActive() ? 1 : 0;
}

/*
 * int createItem(string filename[, int &pos])
 * 
 * Load an item and return the slot into which it was loaded.
 */
void createitem(CALL_DATA &params)
{
	/*
	 * In the beginning, CreateItem() loaded the item into
	 * the first available position and then returned the
	 * number of that position so that the user could use it.
	 *
	 * Because of poor documentation, people sometimes passed
	 * numbers into the second parameter, and then used only
	 * numbers to refer to the item. This generally worked because
	 * the coder would pick the first free number (thinking it
	 * would be loaded into it) and the interpreter would report
	 * no error when it failed to set the variable (which was
	 * not a variable but a number).
	 *
	 * In 3.0.4, I rewrote CreateItem() without a full understanding
	 * of its history and made it actually load the item into
	 * the slot that was passed in. This enabled code that had
	 * passed in numbers to continue to work but effectively
	 * broke code that was passing in a variable to be set.
	 *
	 * Thus, to preserve backwards compatibility with both systems,
	 * and provide predictable behaviour, 3.0.7 must support both
	 * methods in a way that does not break any code. The solution
	 * is to check whether a var has been passed for the second
	 * parameter, if a second parameter has been given. If the
	 * parameter is a var, it is safe to set its value to the
	 * first open slot and load the item into the first slot.
	 *
	 * There are two scenarios: either the user intended for that
	 * var to be set (historical code); or the variable contains
	 * the position that the user intends for the item to be loaded
	 * into. In the second case, it is highly likely that the
	 * user will proceed to use that var, rather than the constant
	 * number, to refer to the position; it is also unlikely that
	 * the user intends to overwrite an existing item in that spot.
	 * If, however, the user is attempting to do one of those two
	 * things, his code will break -- but it is a negligible worry.
	 *
	 * If the second parameter is a constant, we will simply load
	 * the item into that spot, as that is the only predictable action.
	 *
	 * Though this system is not fool proof, it shall suffice.
	 *
	 * --Colin
	 */

	extern std::string g_projectPath;
	extern LPBOARD g_pBoard;

	if ((params.params != 1) && (params.params != 2))
	{
		throw CError("CreateItem() requires one or two parameters.");
	}

	CItem *p = NULL;
	try
	{
		p = new CItem(g_projectPath + ITM_PATH + params[0].getLit(), false);
	}
	catch (CInvalidItem) { return; }

	// If two parameters were provided and the second parameter is
	// a cosntant, load the item into that position.
	if ((params.params == 2) && !(params[1].udt & UDT_ID))
	{
		const int i = params[1].getNum();

		// Is the board item array large enough?
		while (g_pBoard->items.size() <= i)
		{
			g_pBoard->items.push_back(NULL);
		}

		// Erase the item at this position.
		delete g_pBoard->items[i];
		g_pBoard->items[i] = p;

		return;
	}

	// Search for an open spot.
	std::vector<CItem *>::iterator i = g_pBoard->items.begin();
	for (; i != g_pBoard->items.end(); ++i)
	{
		if (*i == NULL)
		{
			// Insert the item here.
			*i = p;
			break;
		}
	}

	if (i == g_pBoard->items.end())
	{
		// Create a new spot for the item.
		g_pBoard->items.push_back(p);
		i = g_pBoard->items.end() - 1; // Not redundant.
	}

	params.ret().udt = UDT_NUM;
	params.ret().num = double(i - g_pBoard->items.begin());

	if (params.params == 2)
	{
		*params.prg->getVar(params[1].lit) = params.ret();
	}
}

/*
 * void putitem(variant handle, int x, int y, int layer)
 * 
 * Place the item on the board at the given location.
 */
void putitem(CALL_DATA &params)
{
	extern LPBOARD g_pBoard;
	extern RECT g_screen;
	extern ZO_VECTOR g_sprites;

	if (params.params != 4)
	{
		throw CError("PutItem() requires four parameters.");
	}

	CSprite *p = getItemPointer(params[0]);
	if (!p) throw CError("PutItem(): item not found");

	int x = int(params[1].getNum()), y = int(params[2].getNum());
	const int l = int(params[3].getNum());

	p->setActive(true);
    p->setPosition(int(params[1].getNum()), 
		int(params[2].getNum()), 
		int(params[3].getNum()), 
		g_pBoard->coordType);

	// Insert the pointer into the z-ordered vector.
	g_sprites.zOrder();
    
	renderNow(g_cnvRpgCode, true);
	renderRpgCodeScreen();
}

/*
 * void eraseitem(variant handle)
 *
 * Erase an item from the screen, but keep it in memory.
 */
void eraseitem(CALL_DATA &params)
{
	extern ZO_VECTOR g_sprites;

	if (params.params != 1)
	{
		throw CError("EraseItem() requires one parameter.");
	}

	CSprite *p = getItemPointer(params[0]);
	if (!p) throw CError("EraseItem(): item not found");

	// Remove the item from the z-ordered vector.
	g_sprites.remove(p);
	p->setActive(false);

	renderNow(g_cnvRpgCode, true);
	renderRpgCodeScreen();
}

/*
 * void destroyitem(int slot)
 * 
 * Remove an item from memory.
 */
void destroyitem(CALL_DATA &params)
{
	extern LPBOARD g_pBoard;
	extern ZO_VECTOR g_sprites;

	if (params.params != 1)
	{
		throw CError("DestroyItem() requires one parameter.");
	}

	unsigned int i = (unsigned int)params[0].getNum();

	// Only accept slot numbers, not source or target handles.
	if (i < g_pBoard->items.size())
	{
		g_sprites.remove(g_pBoard->items[i]);
		delete g_pBoard->items[i];
		g_pBoard->items[i] = NULL;
	}
}

/*
 * void gamespeed(int speed)
 * 
 * Set the overall walking speed. Changes the walking speed proportionally.
 * +ve values increase speed, -ve decrease, by a factor of 10% per increment.
 * Allowed values range from -MAX_GAMESPEED to +MAX_GAMESPEED.
 */
void gamespeed(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("GameSpeed() requires one parameter.");
	}
	CSprite::setLoopOffset(params[0].getNum());
}

/*
 * void playerspeed(string handle, int speed)
 * 
 * Set the delay in seconds between a player's steps.
 */
void playerspeed(CALL_DATA &params)
{
	if (params.params != 2)
	{
		throw CError("PlayerSpeed() requires two parameters.");
	}
	CSprite *p = getPlayerPointer(params[0]);
	if (!p) throw CError("PlayerSpeed(): player not found");

	p->setSpeed(params[1].getNum());
}

/*
 * void itemspeed(variant handle, int speed)
 * 
 * Set the delay in seconds between an item's steps.
 */
void itemspeed(CALL_DATA &params)
{
	if (params.params != 2)
	{
		throw CError("ItemSpeed() requires two parameters.");
	}
	CSprite *p = getItemPointer(params[0]);
	if (!p) throw CError("ItemSpeed(): item not found");

	p->setSpeed(params[1].getNum());
}

/*
 * void walkSpeed()
 * 
 * Obsolete.
 */
void walkSpeed(CALL_DATA &params)
{
	throw CError("WalkSpeed() is obsolete.");
}

/*
 * void itemWalkSpeed()
 * 
 * Obsolete.
 */
void itemWalkSpeed(CALL_DATA &params)
{
	throw CError("ItemWalkSpeed() is obsolete.");
}

/*
 * void characterSpeed()
 * 
 * Obsolete.
 */
void characterSpeed(CALL_DATA &params)
{
	CProgram::debugger("CharacterSpeed() has depreciated into GameSpeed().");
	gamespeed(params);
}

/*
 * void itemlocation(int slot, int &x, int &y, int &layer)
 * 
 * Get the location of an item. Take board slot numbers only;
 * use SourceLocation() and TargetLocation() otherwise.
 */
void itemlocation(CALL_DATA &params)
{
	extern LPBOARD g_pBoard;
	/** TBD: Multitasking considerations **/

	if (params.params != 4)
	{
		throw CError("ItemLocation() requires four parameters.");
	}
	
	LPSTACK_FRAME x = params.prg->getVar(params[1].lit),
				  y = params.prg->getVar(params[2].lit),
				  l = params.prg->getVar(params[3].lit);
	x->udt = UDT_NUM;
	y->udt = UDT_NUM;
	l->udt = UDT_NUM;

	const CSprite *p = getItemPointer(params[0]);
	if (!p) throw CError("ItemLocation(): item not found");

	const SPRITE_POSITION s = p->getPosition();

	// Transform from pixel to board type (e.g. tile).
	int dx = int(s.x), dy = int(s.y);
	tileCoordinate(dx, dy, g_pBoard->coordType);

	x->num = dx;
	y->num = dy;
	l->num = s.l;
}

/*
 * void sourcelocation(int &x, &int y)
 * 
 * Get the location of the source object.
 */
void sourcelocation(CALL_DATA &params)
{
	extern LPBOARD g_pBoard;
	/** TBD: Multitasking considerations **/

	if (params.params != 2)
	{
		throw CError("SourceLocation() requires two parameters.");
	}
	
	LPSTACK_FRAME x = params.prg->getVar(params[0].lit),
				  y = params.prg->getVar(params[1].lit);
	x->udt = UDT_NUM;
	y->udt = UDT_NUM;

	int dx = 0, dy = 0;

	if (g_sourceType == TT_ENEMY)
	{
		/** TBD: get enemy location. **/
	}
	else
	{
		// Player or item.
		/** TBD:
		if (fighting)
		{
			// Get location from plugin...
		}
		else
		**/
		{
			const CSprite *p = (CSprite *)g_pSource;
			const SPRITE_POSITION s = p->getPosition();
			dx = int(s.x);
			dy = int(s.y);
		}
	}
	// Transform from pixel to board type (e.g. tile).
	tileCoordinate(dx, dy, g_pBoard->coordType);
	x->num = dx;
	y->num = dy;
}

/*
 * void targetlocation(int &x, &int y)
 * 
 * Get the location of the target object.
 */
void targetlocation(CALL_DATA &params)
{
	extern LPBOARD g_pBoard;
	/** TBD: Multitasking considerations **/

	if (params.params != 2)
	{
		throw CError("TargetLocation() requires two parameters.");
	}
	
	LPSTACK_FRAME x = params.prg->getVar(params[0].lit),
				  y = params.prg->getVar(params[1].lit);
	x->udt = UDT_NUM;
	y->udt = UDT_NUM;

	int dx = 0, dy = 0;

	if (g_targetType == TT_ENEMY)
	{
		/** TBD: get enemy location. **/
	}
	else
	{
		// Player or item.
		/** TBD:
		if (fighting)
		{
			// Get location from plugin...
		}
		else
		**/
		{
			const CSprite *p = (CSprite *)g_pTarget;
			const SPRITE_POSITION s = p->getPosition();
			dx = int(s.x);
			dy = int(s.y);
		}
	}
	// Transform from pixel to board type (e.g. tile).
	tileCoordinate(dx, dy, g_pBoard->coordType);
	x->num = dx;
	y->num = dy;
}

/*
 * string sourcehandle([string &ret])
 * 
 * Get the handle of the source object.
 */
void sourcehandle(CALL_DATA &params)
{
	extern LPBOARD g_pBoard;

	std::string str;
	if (g_sourceType == TT_PLAYER)
	{
		CPlayer *p = (CPlayer *)g_pSource;
		str = p->name();
	}
	else if (g_sourceType == TT_ITEM)
	{
		// Return the item index...(!)
		int i = 0;
		str = "ITEM";
		CItem *p = (CItem *)g_pSource;
		std::vector<CItem *>::iterator j = g_pBoard->items.begin();
		for (; j != g_pBoard->items.end(); ++j)
		{
			if (p == *j) i = j - g_pBoard->items.begin();
		}
		char c[8];
		str += itoa(i, c, 8);
	}
	else if (g_sourceType == TT_ENEMY)
	{
		/** TBD: return enemy index... (?) 
		str = "ENEMY" + i; **/
	}

	params.ret().udt = UDT_LIT;
	params.ret().lit = str;
	if (params.params == 1)
	{
		*params.prg->getVar(params[0].lit) = params.ret();
	}
	else if (params.params != 0)
	{
		throw CError("SourceHandle() requires zero or one parameter(s).");
	}
}
/*
 * string targethandle([string &ret])
 * 
 * Get the handle of the target object.
 */
void targethandle(CALL_DATA &params)
{
	extern LPBOARD g_pBoard;

	std::string str;
	if (g_targetType == TT_PLAYER)
	{
		CPlayer *p = (CPlayer *)g_pTarget;
		str = p->name();
	}
	else if (g_targetType == TT_ITEM)
	{
		// Return the item index...(!)
		int i = 0;
		str = "ITEM";
		CItem *p = (CItem *)g_pTarget;
		std::vector<CItem *>::iterator j = g_pBoard->items.begin();
		for (; j != g_pBoard->items.end(); ++j)
		{
			if (p == *j) i = j - g_pBoard->items.begin();
		}
		char c[8];
		str += itoa(i, c, 8);
	}
	else if (g_targetType == TT_ENEMY)
	{
		/** TBD: return enemy index... (?) 
		str = "ENEMY" + i; **/
	}

	params.ret().udt = UDT_LIT;
	params.ret().lit = str;
	if (params.params == 1)
	{
		*params.prg->getVar(params[0].lit) = params.ret();
	}
	else if (params.params != 0)
	{
		throw CError("TargetHandle() requires zero or one parameter(s).");
	}
}

/*
 * void bitmap(string file, [canvas cnv])
 * 
 * Fill a surface with an image.
 */
void bitmap(CALL_DATA &params)
{
	CGDICanvas *cnv = NULL;
	if (params.params == 1)
	{
		cnv = g_cnvRpgCode;
	}
	else if (params.params == 2)
	{
		cnv = g_canvases.cast(int(params[1].getNum()));
	}
	else
	{
		throw CError("Bitmap() requires one or two parameters.");
	}
	if (cnv)
	{
		extern std::string g_projectPath;
		extern int g_resX, g_resY;
		drawImage(g_projectPath + BMP_PATH + params[0].getLit(), cnv, 0, 0, g_resX, g_resY);
		if (cnv == g_cnvRpgCode)
		{
			renderRpgCodeScreen();
		}
	}
}

/*
 * void mainFile(string gam)
 * 
 * Load a new main file.
 */
void mainFile(CALL_DATA &params)
{

}

/*
 * string dirSav([string &ret])
 * 
 * Allow the user to choose a *.sav file from the "Saved"
 * directory. For historical reasons, returns "CANCEL" if
 * no file is chosen, not "".
 */
void dirSav(CALL_DATA &params)
{

}

/*
 * void save(string file)
 * 
 * Save the current game state to a file.
 */
void save(CALL_DATA &params)
{

}

/*
 * void load(string file)
 * 
 * Load the game state from a file.
 */
void load(CALL_DATA &params)
{

}

/*
 * void scan(int x, int y, int pos)
 * 
 * Save the tile specified to a buffer identified by
 * pos. There is no particular number to pick for pos;
 * any will do.
 * ** Flag for depreciation **
 */
void scan(CALL_DATA &params)
{
	extern std::vector<CGDICanvas *> g_cnvRpgScans;
	extern LPBOARD g_pBoard;

	if (params.params != 3)
	{
		throw CError("Scan() requires three parameters.");
	}
	
	const int i = int(params[2].getNum());
	int x = int(params[0].getNum()), y = int(params[1].getNum());
	pixelCoordinate(x, y, g_pBoard->coordType, false);

	while (i >= g_cnvRpgScans.size())
	{
		g_cnvRpgScans.push_back(NULL);
	}
	
	if (!g_cnvRpgScans[i])
	{
		g_cnvRpgScans[i] = new CGDICanvas();
		g_cnvRpgScans[i]->CreateBlank(NULL, 32, 32, TRUE);
	}
	g_cnvRpgScans[i]->ClearScreen(TRANSP_COLOR);

	g_cnvRpgCode->BltPart(
		g_cnvRpgScans[i],
		0, 0,
		x, y,
		32, 32,
		SRCCOPY);
}

/*
 * void mem(int x, int y, int pos)
 * 
 * Lay a scanned tile on the screen at the specified position.
 * ** Flag for depreciation **
 */
void mem(CALL_DATA &params)
{
	extern std::vector<CGDICanvas *> g_cnvRpgScans;
	extern LPBOARD g_pBoard;

	if (params.params != 3)
	{
		throw CError("Mem() requires three parameters.");
	}

	const int i = int(params[2].getNum());
	int x = int(params[0].getNum()), y = int(params[1].getNum());
	pixelCoordinate(x, y, g_pBoard->coordType, false);

	if (i < g_cnvRpgScans.size() && g_cnvRpgScans[i])
	{
		// The canvas exists.
		g_cnvRpgScans[i]->BltPart(
			g_cnvRpgCode,
			x, y,
			0, 0,
			32, 32,
			SRCCOPY);
	}
	else throw CError("Mem(): canvas not found.");

	renderRpgCodeScreen();
}

/*
 * void print(string text)
 * 
 * Write the specified string one line down from the last call
 * to [[text()]].
 */
void print(CALL_DATA &params)
{

}

/*
 * void rpgCode(string line)
 * 
 * Independently run a line of RPGCode.
 */
void rpgCode(CALL_DATA &params)
{
	if (params.params == 1)
	{
		CProgramChild prg(*params.prg);
		prg.loadFromString(params[0].getLit());
		prg.run();
	}
	else
	{
		throw CError("RPGCode() requires one parameter.");
	}
}

/*
 * string charAt(string str, int pos, [string &ret])
 * 
 * Get a character from a string. The first character is one.
 */
void charAt(CALL_DATA &params)
{
	if ((params.params != 2) && (params.params != 3))
	{
		throw CError("CharAt() requires two or three parameters.");
	}
	params.ret().udt = UDT_LIT;
	params.ret().lit = params[0].getLit().substr(int(params[1].getNum()) - 1, 1);
	if (params.params == 3)
	{
		*params.prg->getVar(params[2].lit) = params.ret();
	}
}

/*
 * equip(...)
 * 
 * Description.
 */
void equip(CALL_DATA &params)
{

}

/*
 * remove(...)
 * 
 * Description.
 */
void remove(CALL_DATA &params)
{

}

/*
 * void kill(variant &var, ...)
 * 
 * Delete variables.
 */
void kill(CALL_DATA &params)
{
	for (unsigned int i = 0; i < params.params; ++i)
	{
		params.prg->freeVar(params[i].lit);
	}
}

/*
 * void giveGp(int gp)
 * 
 * Give gold pieces.
 */
void giveGp(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("GiveGP() requires one parameter.");
	}
	g_gp += params[0].getNum();
}

/*
 * void takeGp(int gp)
 * 
 * Take gold pieces.
 */
void takeGp(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("TakeGP() requires one parameter.");
	}
	g_gp -= params[0].getNum();
}

/*
 * int getGp([int &ret])
 * 
 * Return the amount of gold pieces held.
 */
void getGp(CALL_DATA &params)
{
	params.ret().udt = UDT_NUM;
	params.ret().num = double(g_gp);
	if (params.params == 1)
	{
		*params.prg->getVar(params[0].lit) = params.ret();
	}
}

/*
 * wavstop(...)
 * 
 * Description.
 */
void wavstop(CALL_DATA &params)
{

}

/*
 * void borderColor(int r, int g, int b)
 * 
 * Obsolete.
 */
void borderColor(CALL_DATA &params)
{
	throw CError("BorderColor() is obsolete.");
}

/*
 * void fightEnemy(string enemy, string enemy, ... string background)
 * 
 * Start a fight.
 */
void fightEnemy(CALL_DATA &params)
{
	if (params.params < 2)
	{
		throw CError("FightEnemy() requires at least two parameters.");
	}
	std::vector<std::string> enemies;
	for (unsigned int i = 0; i < (params.params - 1); ++i)
	{
		enemies.push_back(params[i].getLit());
	}
	runFight(enemies, params[params.params - 1].getLit());
}

/*
 * restoreplayer(...)
 * 
 * Description.
 */
void restoreplayer(CALL_DATA &params)
{

}

/*
 * callshop(...)
 * 
 * Description.
 */
void callshop(CALL_DATA &params)
{

}

/*
 * void clearBuffer()
 * 
 * Clear the keyboard buffer.
 */
void clearBuffer(CALL_DATA &params)
{
	extern std::vector<char> g_keys;
	g_keys.clear();
}

/*
 * attackall(...)
 * 
 * Description.
 */
void attackall(CALL_DATA &params)
{

}

/*
 * drainall(...)
 * 
 * Description.
 */
void drainall(CALL_DATA &params)
{

}

/*
 * inn()
 * 
 * Fully heal the player party.
 */
void inn(CALL_DATA &params)
{
	extern std::vector<CPlayer *> g_players;
	std::vector<CPlayer *>::const_iterator i = g_players.begin();
	for (; i != g_players.end(); ++i)
	{
		(*i)->health((*i)->maxHealth());
		(*i)->smp((*i)->maxSmp());
	}
}

/*
 * posture(...)
 * 
 * Description.
 */
void posture(CALL_DATA &params)
{

}

/*
 * setbutton(...)
 * 
 * Description.
 */
void setbutton(CALL_DATA &params)
{

}

/*
 * checkbutton(...)
 * 
 * Description.
 */
void checkbutton(CALL_DATA &params)
{

}

/*
 * clearbuttons(...)
 * 
 * Description.
 */
void clearbuttons(CALL_DATA &params)
{

}

/*
 * mouseclick(...)
 * 
 * Description.
 */
void mouseclick(CALL_DATA &params)
{

}

/*
 * mousemove(...)
 * 
 * Description.
 */
void mousemove(CALL_DATA &params)
{

}

/*
 * zoom(...)
 * 
 * Description.
 */
void zoom(CALL_DATA &params)
{

}

/*
 * earthquake(...)
 * 
 * Description.
 */
void earthquake(CALL_DATA &params)
{

}

/*
 * int itemCount(string fileName[, int &ret])
 * 
 * Count the number of a certain item in the inventory.
 */
void itemcount(CALL_DATA &params)
{
	extern std::string g_projectPath;

	if ((params.params != 1) && (params.params != 2))
	{
		throw CError("ItemCount() requires one or two parameters.");
	}

	const std::string file = g_projectPath + ITM_PATH + params[0].getLit();
	if (!CFile::fileExists(file)) return;

	params.ret().udt = UDT_NUM;
	params.ret().num = g_inv.getQuantity(file);

	if (params.params == 2)
	{
		*params.prg->getVar(params[1].lit) = params.ret();
	}
}

/*
 * destroyplayer(...)
 * 
 * Description.
 */
void destroyplayer(CALL_DATA &params)
{

}

/*
 * callplayerswap(...)
 * 
 * Description.
 */
void callplayerswap(CALL_DATA &params)
{

}

/*
 * playavi(...)
 * 
 * Description.
 */
void playavi(CALL_DATA &params)
{

}

/*
 * playavismall(...)
 * 
 * Description.
 */
void playavismall(CALL_DATA &params)
{

}

/*
 * void getCorner(int &topX, int &topY)
 * 
 * Get the corner of the currently shown portion of the board.
 */
void getCorner(CALL_DATA &params)
{
	extern RECT g_screen;

	if (params.params != 2)
	{
		throw CError("GetCorner() requires two parameters.");
	}
	{
		LPSTACK_FRAME var = params.prg->getVar(params[0].lit);
		var->udt = UDT_NUM;
		var->num = g_screen.left;
	}
	{
		LPSTACK_FRAME var = params.prg->getVar(params[1].lit);
		var->udt = UDT_NUM;
		var->num = g_screen.top;
	}
}

/*
 * void underArrow()
 * 
 * Toggle the under arrow.
 */
void underArrow(CALL_DATA &params)
{
	throw CError("UnderArrow() is obsolete.");
}

/*
 * getlevel(...)
 * 
 * Description.
 */
void getlevel(CALL_DATA &params)
{

}

/*
 * ai(...)
 * 
 * Description.
 */
void ai(CALL_DATA &params)
{

}

/*
 * void menuGraphic(string image)
 * 
 * Choose an image for the menu.
 */
void menugraphic(CALL_DATA &params)
{
	extern std::string g_menuGraphic;

	if (params.params != 1)
	{
		throw CError("MenuGraphic() requires one parameter.");
	}
	g_menuGraphic = params[0].getLit();
}

/*
 * void fightMenuGraphic(string image)
 * 
 * Choose an image for the fight menu graphic.
 */
void fightMenuGraphic(CALL_DATA &params)
{
	extern std::string g_fightMenuGraphic;

	if (params.params != 1)
	{
		throw CError("FightMenuGraphic() requires one parameter.");
	}
	g_fightMenuGraphic = params[0].getLit();
}

/*
 * void fightStyle()
 * 
 * Obsolete.
 */
void fightStyle(CALL_DATA &params)
{
	throw CError("FightStyle() is obsolete.");
}

/*
 * stance(...)
 * 
 * Description.
 */
void stance(CALL_DATA &params)
{

}

/*
 * void battleSpeed(int speed)
 * 
 * Obsolete.
 */
void battleSpeed(CALL_DATA &params)
{
	throw CError("BattleSpeed() is obsolete.");
}

/*
 * void textSpeed(int speed)
 * 
 * Obsolete.
 */
void textSpeed(CALL_DATA &params)
{
	throw CError("TextSpeed() is obsolete.");
}

/*
 * mwinsize(...)
 * 
 * Description.
 */
void mwinsize(CALL_DATA &params)
{

}

/*
 * int getDp(string handle, [int &ret])
 * 
 * Get a fighter's dp.
 */
void getDp(CALL_DATA &params)
{
	if (params.params == 1)
	{
		IFighter *p = getFighter(params[0].getLit());
		if (p)
		{
			params.ret().udt = UDT_NUM;
			params.ret().num = p->defence();
		}
	}
	else if (params.params == 2)
	{
		IFighter *p = getFighter(params[0].getLit());
		if (p)
		{
			LPSTACK_FRAME var = params.prg->getVar(params[1].lit);
			var->udt = UDT_NUM;
			var->num = p->defence();
		}
	}
	else
	{
		throw CError("GetDP() requires one or two parameters.");
	}
}

/*
 * int getFp(string handle, [int &ret])
 * 
 * Get a fighter's fp.
 */
void getFp(CALL_DATA &params)
{
	if (params.params == 1)
	{
		IFighter *p = getFighter(params[0].getLit());
		if (p)
		{
			params.ret().udt = UDT_NUM;
			params.ret().num = p->fight();
		}
	}
	else if (params.params == 2)
	{
		IFighter *p = getFighter(params[0].getLit());
		if (p)
		{
			LPSTACK_FRAME var = params.prg->getVar(params[1].lit);
			var->udt = UDT_NUM;
			var->num = p->fight();
		}
	}
	else
	{
		throw CError("GetFP() requires one or two parameters.");
	}
}

/*
 * internalmenu(...)
 * 
 * Description.
 */
void internalmenu(CALL_DATA &params)
{

}

/*
 * applystatus(...)
 * 
 * Description.
 */
void applystatus(CALL_DATA &params)
{

}

/*
 * removestatus(...)
 * 
 * Description.
 */
void removestatus(CALL_DATA &params)
{

}

/*
 * void setImage(string str, int x, int y, int width, int height, [canvas cnv])
 * 
 * Set an image.
 */
void setImage(CALL_DATA &params)
{
	CGDICanvas *cnv = NULL;
	if (params.params == 5)
	{
		cnv = g_cnvRpgCode;
	}
	else if (params.params == 6)
	{
		cnv = g_canvases.cast(int(params[5].getNum()));
	}
	else
	{
		throw CError("SetImage() requires five or six parameters.");
	}
	if (cnv)
	{
		extern std::string g_projectPath;
		drawImage(g_projectPath + BMP_PATH + params[0].getLit(), cnv, params[1].getNum(), params[2].getNum(), params[3].getNum(), params[4].getNum());
		if (cnv == g_cnvRpgCode)
		{
			renderRpgCodeScreen();
		}
	}

}

/*
 * drawcircle(...)
 * 
 * Description.
 */
void drawcircle(CALL_DATA &params)
{

}

/*
 * fillcircle(...)
 * 
 * Description.
 */
void fillcircle(CALL_DATA &params)
{

}

/*
 * void savescreen([int position = 0])
 * 
 * Save the current screen onto a canvas that can be restored at a
 * later time.
 */
void savescreen(CALL_DATA &params)
{
	extern std::vector<CGDICanvas *> g_cnvRpgScreens;
	extern CGDICanvas *g_cnvRpgCode;
	extern RECT g_screen;

	if (params.params != 0 && params.params != 1)
	{
		throw CError("RestoreScreen() requires zero or one parameter(s).");
	}

	const int i = (params.params == 0 ? 0 : int(params[0].getNum())),
			  width = g_screen.right - g_screen.left, 
			  height = g_screen.bottom - g_screen.top;

	while (i >= g_cnvRpgScreens.size())
	{
		g_cnvRpgScreens.push_back(NULL);
	}
	
	if (!g_cnvRpgScreens[i])
	{
		g_cnvRpgScreens[i] = new CGDICanvas();
		g_cnvRpgScreens[i]->CreateBlank(NULL, width, height, TRUE);
	}
	g_cnvRpgScreens[i]->ClearScreen(TRANSP_COLOR);

	g_cnvRpgCode->BltPart(
		g_cnvRpgScreens[i],
		0, 0,
		0, 0,
		width, height,
		SRCCOPY);
}

/*
 * void restorescreen([int x1, int y1, int x2, int y2, int xdest, int ydest])
 * 
 * Draw a buffered screen capture to the screen. The canvas
 * drawn is the first in the screen array. x1, x2 specify the bottom-
 * right corner of the screen, so that width = x2 - x1, etc.
 */
void restorescreen(CALL_DATA &params)
{
	extern std::vector<CGDICanvas *> g_cnvRpgScreens;
	extern CGDICanvas *g_cnvRpgCode;
	extern RECT g_screen;

	if (params.params != 0 && params.params != 6)
	{
		throw CError("RestoreScreen() requires zero or six parameters.");
	}

	int xSrc = 0, ySrc = 0, 
		width = g_screen.right - g_screen.left, 
		height = g_screen.bottom - g_screen.top, 
		xDest = 0, yDest = 0;

	if (params.params == 6)
	{
		xSrc = int(params[0].getNum());
		ySrc = int(params[1].getNum());
		width = int(params[2].getNum()) - xSrc;
		height = int(params[3].getNum()) - ySrc;
		xDest = int(params[4].getNum());
		yDest = int(params[5].getNum());
	}
	
	if (g_cnvRpgScreens.size() && g_cnvRpgScreens.front())
	{
		// The first canvas exists.
		g_cnvRpgScreens.front()->BltPart(
			g_cnvRpgCode,
			xDest, yDest,
			xSrc, ySrc,
			width, height,
			SRCCOPY);
	}
	else throw CError("RestoreScreen(): canvas not found.");

	renderRpgCodeScreen();
}

/*
 * void restorescreenarray(int pos [,int x1, int y1, int x2, int y2, int xdest, int ydest])
 * 
 * Draw a buffered screen capture to the screen. 
 * x1, x2 specify the bottom-right corner of the screen, 
 * so that width = x2 - x1, etc.
 */
void restorescreenarray(CALL_DATA &params)
{
	extern std::vector<CGDICanvas *> g_cnvRpgScreens;
	extern CGDICanvas *g_cnvRpgCode;

	if (params.params != 1 && params.params != 7)
	{
		throw CError("RestoreScreen() requires one or seven parameters.");
	}

	const int i = int(params[0].getNum());
	if (i < g_cnvRpgScreens.size() && g_cnvRpgScreens[i])
	{
		// Temporarily switch the pointers for
		// RestoreScreen() to use the first.
		CGDICanvas *pCnv = g_cnvRpgScreens[0];
		g_cnvRpgScreens[0] = g_cnvRpgScreens[i];

		// Send the last 6 parameters to RestoreScreen();
		--params.params;
		++params.p;
		restorescreen(params);

		g_cnvRpgScreens[0] = pCnv;
	}
	else throw CError("RestoreScreenArray(): canvas not found.");
}

/*
 * double sin(double x, [double &ret])
 * 
 * Calculate sine x.
 */
void sin(CALL_DATA &params)
{
	if (params.params == 1)
	{
		params.ret().udt = UDT_NUM;
		params.ret().num = sin(params[0].getNum() / 180 * PI);
	}
	else if (params.params == 2)
	{
		LPSTACK_FRAME var = params.prg->getVar(params[1].lit);
		var->udt = UDT_NUM;
		var->num = sin(params[0].getNum() / 180 * PI);
	}
}

/*
 * double cos(double x, [double &ret])
 * 
 * Calculate cosine x.
 */
void cos(CALL_DATA &params)
{
	if (params.params == 1)
	{
		params.ret().udt = UDT_NUM;
		params.ret().num = cos(params[0].getNum() / 180 * PI);
	}
	else if (params.params == 2)
	{
		LPSTACK_FRAME var = params.prg->getVar(params[1].lit);
		var->udt = UDT_NUM;
		var->num = cos(params[0].getNum() / 180 * PI);
	}
}

/*
 * double tan(double x, [double &ret])
 * 
 * Calculate tangent x.
 */
void tan(CALL_DATA &params)
{
	if (params.params == 1)
	{
		params.ret().udt = UDT_NUM;
		params.ret().num = tan(params[0].getNum() / 180 * PI);
	}
	else if (params.params == 2)
	{
		LPSTACK_FRAME var = params.prg->getVar(params[1].lit);
		var->udt = UDT_NUM;
		var->num = tan(params[0].getNum() / 180 * PI);
	}
}

/*
 * double sqrt(double x, [double &ret])
 * 
 * Calculate the square root of x.
 */
void sqrt(CALL_DATA &params)
{
	if (params.params == 1)
	{
		params.ret().udt = UDT_NUM;
		params.ret().num = sqrt(params[0].getNum());
	}
	else if (params.params == 2)
	{
		LPSTACK_FRAME var = params.prg->getVar(params[1].lit);
		var->udt = UDT_NUM;
		var->num = sqrt(params[0].getNum());
	}
}

/*
 * double log(double x, [double &ret])
 * 
 * Get the natural log of x.
 */
void log(CALL_DATA &params)
{
	if ((params.params != 1) && (params.params != 2))
	{
		throw CError("Log() requires one or two parameters.");
	}
	params.ret().udt = UDT_NUM;
	params.ret().num = log(params[0].getNum());
	if (params.params == 2)
	{
		*params.prg->getVar(params[1].lit) = params.ret();
	}
}

/*
 * void getPixel(int x, int y, int &r, int &g, int &b, [canvas cnv])
 * 
 * Get the colour of the pixel at x, y.
 */
void getPixel(CALL_DATA &params)
{
	COLORREF color = 0;
	if (params.params == 5)
	{
		color = g_cnvRpgCode->GetPixel(params[0].getNum(), params[1].getNum());
	}
	else if (params.params == 6)
	{
		CGDICanvas *cnv = g_canvases.cast(int(params[5].getNum()));
		if (cnv)
		{
			color = cnv->GetPixel(params[0].getNum(), params[1].getNum());
		}
	}
	else
	{
		throw CError("GetPixel() requires five or six parameters.");
	}
	{
		LPSTACK_FRAME var = params.prg->getVar(params[2].lit);
		var->udt = UDT_NUM;
		var->num = double(GetRValue(color));
	}
	{
		LPSTACK_FRAME var = params.prg->getVar(params[3].lit);
		var->udt = UDT_NUM;
		var->num = double(GetGValue(color));
	}
	{
		LPSTACK_FRAME var = params.prg->getVar(params[4].lit);
		var->udt = UDT_NUM;
		var->num = double(GetBValue(color));
	}
}

/*
 * void getColor(int &r, int &g, int &b)
 * 
 * Get the current colour.
 */
void getColor(CALL_DATA &params)
{
	if (params.params == 3)
	{
		{
			LPSTACK_FRAME var = params.prg->getVar(params[0].lit);
			var->udt = UDT_NUM;
			var->num = double(GetRValue(g_color));
		}
		{
			LPSTACK_FRAME var = params.prg->getVar(params[1].lit);
			var->udt = UDT_NUM;
			var->num = double(GetGValue(g_color));
		}
		{
			LPSTACK_FRAME var = params.prg->getVar(params[2].lit);
			var->udt = UDT_NUM;
			var->num = double(GetBValue(g_color));
		}
	}
	else
	{
		throw CError("GetColor() requires three parameters.");
	}
}

/*
 * int getFontSize([int &ret])
 * 
 * Get the current font size.
 */
void getFontSize(CALL_DATA &params)
{
	params.ret().udt = UDT_NUM;
	params.ret().num = double(g_fontSize);
	if (params.params == 1)
	{
		*params.prg->getVar(params[0].lit) = params.ret();
	}
}

/*
 * void setImageTransparent(string file, int x, int y, int width, int height, int r, int g, int b, [canvas cnv])
 * 
 * Set an image with a transparent colour.
 */
void setImageTransparent(CALL_DATA &params)
{
	CGDICanvas *cnv = NULL;
	if (params.params == 8)
	{
		cnv = g_cnvRpgCode;
	}
	else if (params.params == 9)
	{
		cnv = g_canvases.cast(int(params[8].getNum()));
	}
	else
	{
		throw CError("SetImageTransparent() requires eight or nine parameters.");
	}
	if (cnv)
	{
		extern std::string g_projectPath;
		CGDICanvas intermediate;
		intermediate.CreateBlank(NULL, params[3].getNum(), params[4].getNum(), TRUE);
		drawImage(g_projectPath + BMP_PATH + params[0].getLit(), &intermediate, 0, 0, params[3].getNum(), params[4].getNum());
		intermediate.BltTransparent(cnv, params[1].getNum(), params[2].getNum(), RGB(params[5].getNum(), params[6].getNum(), params[7].getNum()));
		if (cnv == g_cnvRpgCode)
		{
			renderRpgCodeScreen();
		}
	}
}

/*
 * void setImageTranslucent(string file, int x, int y, int width, int height, [canvas cnv])
 * 
 * Set an image translucently.
 */
void setImageTranslucent(CALL_DATA &params)
{
	CGDICanvas *cnv = NULL;
	if (params.params == 5)
	{
		cnv = g_cnvRpgCode;
	}
	else if (params.params == 6)
	{
		cnv = g_canvases.cast(int(params[5].getNum()));
	}
	else
	{
		throw CError("SetImageTransparent() requires five or six parameters.");
	}
	if (cnv)
	{
		extern std::string g_projectPath;
		CGDICanvas intermediate;
		intermediate.CreateBlank(NULL, params[3].getNum(), params[4].getNum(), TRUE);
		drawImage(g_projectPath + BMP_PATH + params[0].getLit(), &intermediate, 0, 0, params[3].getNum(), params[4].getNum());
		intermediate.BltTranslucent(cnv, params[1].getNum(), params[2].getNum(), 0.5, -1, -1);
		if (cnv == g_cnvRpgCode)
		{
			renderRpgCodeScreen();
		}
	}
}

/*
 * void drawEnemy(string file, int x, int y, [canvas cnv])
 * 
 * Draw an enemy.
 */
void drawEnemy(CALL_DATA &params)
{
	extern std::string g_projectPath;

	CGDICanvas *cnv = NULL;
	if (params.params == 3)
	{
		cnv = g_cnvRpgCode;
	}
	else if (params.params == 4)
	{
		if (!(cnv = g_canvases.cast(int(params[3].getNum()))))
		{
			return;
		}
	}
	else
	{
		throw CError("DrawEnemy() requires three or four parameters.");
	}
	ENEMY enemy;
	if (enemy.open(g_projectPath + ENE_PATH + params[0].getLit()))
	{
		CGDICanvas c;
		c.CreateBlank(NULL, 1, 1, TRUE);
		renderAnimationFrame(&c, enemy.gfx[EN_REST], 0, 0, 0);
		c.BltTransparent(cnv, int(params[1].getNum()), int(params[2].getNum()), TRANSP_COLOR);
		if (cnv == g_cnvRpgCode)
		{
			renderRpgCodeScreen();
		}
	}
}

/*
 * mp3pause(...)
 * 
 * Description.
 */
void mp3pause(CALL_DATA &params)
{

}

/*
 * void layerput(int x, int y, int layer, string tile)
 * 
 * Place a tile on the board for the duration the player is on the board.
 */
void layerput(CALL_DATA &params)
{
	extern LPBOARD g_pBoard;
	extern SCROLL_CACHE g_scrollCache;

	if (params.params != 4)
	{
		throw CError("LayerPut() requires four parameters.");
	}
	
	// Instead of drawing onto the scrollcache (which seems quite
	// useless), enter the tile into the tile lookup table.
	const int x = int(params[0].getNum()), 
			  y = int(params[1].getNum()),
			  l = int(params[2].getNum());
	const std::string tile = params[3].getLit();
	int index = 0;

	// Search the table for the tile.
	std::vector<std::string>::iterator i = g_pBoard->tileIndex.begin(),
									   j = i;
	for (; i != g_pBoard->tileIndex.end(); ++i)
	{
		if (_strcmpi(tile.c_str(), i->c_str()) == 0)
		{
			index = i - j;
			break;
		}
	}
	if (!index)
	{
		// Insert it onto the end.
		index = g_pBoard->tileIndex.size();
		g_pBoard->tileIndex.push_back(tile);
	}

	try
	{
		g_pBoard->board[x][y][l] = index;
		g_pBoard->bLayerOccupied[l] = true;
		g_pBoard->bLayerOccupied[0] = true;			// Any layer occupied.
	}
	catch (...)
	{
		throw CError("LayerPut(): tile co-ordinates out of bounds.");
	}

	// Redraw the scrollcache.
	g_scrollCache.render(true);
    renderNow(g_cnvRpgCode, true);
    renderRpgCodeScreen();
}

/*
 * string getBoardTile(int x, int y, int z, [string &ret])
 * 
 * Get the file name of the tile at x, y, z.
 */
void getBoardTile(CALL_DATA &params)
{
	extern LPBOARD g_pBoard;

	if ((params.params != 3) && (params.params != 4))
	{
		throw CError("GetBoardTile() requires three or four parameters.");
	}

	params.ret().udt = UDT_LIT;
	try
	{
		params.ret().lit = g_pBoard->tileIndex[g_pBoard->board[params[0].getNum()][params[1].getNum()][params[2].getNum()]];
	}
	catch (...)
	{
		throw CError("Out of bounds.");
	}

	if (params.params == 4)
	{
		*params.prg->getVar(params[3].lit) = params.ret();
	}
}

/*
 * string getBoardTileType(int x, int y, int z[, string &ret])
 * 
 * Get the type of a tile.
 */
void getBoardTileType(CALL_DATA &params)
{
	extern LPBOARD g_pBoard;

	if ((params.params != 3) && (params.params != 4))
	{
		throw CError("GetBoardTypeType() requires three or four parameters.");
	}

	// Get the vector that contains the tile.
	const BRD_VECTOR *p = g_pBoard->getVectorFromTile(
		int(params[0].getNum()),
		int(params[1].getNum()),
		int(params[2].getNum())
	);

	std::string type;
	if (!p)
	{
		type = "NORMAL";
	}
	else
	{
		// Convert the type to a string.
		switch (p->type)
		{
			case TT_NORMAL:
			case TT_N_OVERRIDE:
				type = "NORMAL";
				break;

			case TT_SOLID:
				type = "SOLID";
				break;

			case TT_UNDER:
				type = "UNDER";
				break;

			case TT_UNIDIRECTIONAL:
				// TBD: Differentiate between NW and NE?
				type = "NW";
				break;

			case TT_STAIRS:
				char str[255]; itoa(p->attributes, str, 10);
				type = std::string("STAIRS") + str;
				break;
		}
	}

	params.ret().udt = UDT_LIT;
	params.ret().lit = type;

	if (params.params == 4)
	{
		*params.prg->getVar(params[3].lit) = params.ret();
	}
}

/*
 * setimageadditive(...)
 * 
 * Description.
 */
void setimageadditive(CALL_DATA &params)
{

}

/*
 * animation(...)
 * 
 * Description.
 */
void animation(CALL_DATA &params)
{

}

/*
 * sizedanimation(...)
 * 
 * Description.
 */
void sizedanimation(CALL_DATA &params)
{

}

/*
 * void forceRedraw()
 * 
 * Force a redrawing of the screen.
 */
void forceRedraw(CALL_DATA &params)
{
	renderNow(g_cnvRpgCode, true);
	renderRpgCodeScreen();
}

/*
 * wipe(...)
 * 
 * Description.
 */
void wipe(CALL_DATA &params)
{

}

/*
 * void getRes(int &x, int &y)
 * 
 * Get the screen's resolution.
 */
void getRes(CALL_DATA &params)
{
	if (params.params != 2)
	{
		throw CError("GetRes() requires two parameters.");
	}
	extern RECT g_screen;
	{
		LPSTACK_FRAME var = params.prg->getVar(params[0].lit);
		var->udt = UDT_NUM;
		var->num = g_screen.right - g_screen.left;
	}
	{
		LPSTACK_FRAME var = params.prg->getVar(params[1].lit);
		var->udt = UDT_NUM;
		var->num = g_screen.bottom - g_screen.top;
	}
}

/*
 * void staticText()
 * 
 * Toggle antialiasing.
 */
void staticText(CALL_DATA &params)
{
	throw CError("StaticText() is obsolete.");
}

/*
 * redirect(...)
 * 
 * Description.
 */
void redirect(CALL_DATA &params)
{

}

/*
 * killredirect(...)
 * 
 * Description.
 */
void killredirect(CALL_DATA &params)
{

}

/*
 * killallredirects(...)
 * 
 * Description.
 */
void killallredirects(CALL_DATA &params)
{

}

/*
 * parallax(...)
 * 
 * Description.
 */
void parallax(CALL_DATA &params)
{

}

/*
 * giveexp(...)
 * 
 * Description.
 */
void giveexp(CALL_DATA &params)
{

}

/*
 * void animatedTiles()
 * 
 * Toggle animated tiles.
 */
void animatedTiles(CALL_DATA &params)
{
	throw CError("AnimatedTiles() is obsolete.");
}

/*
 * void smartStep()
 * 
 * Toggle "smart" stepping.
 */
void smartStep(CALL_DATA &params)
{
	throw CError("SmartStep() is obsolete.");
}

/*
 * thread thread(string file, bool presist, [thread &ret])
 * 
 * Start a thread.
 */
void thread(CALL_DATA &params)
{
	if ((params.params != 2) && (params.params != 3))
	{
		throw CError("Thread() requires two or three parameters.");
	}
	extern std::string g_projectPath;
	const std::string file = g_projectPath + PRG_PATH + params[0].getLit();
	if (!CFile::fileExists(file))
	{
		throw CError("Could not find " + params[0].getLit() + " for Thread().");
	}
	CThread *p = CThread::create(file);
	if (!params[1].getBool())
	{
		extern LPBOARD g_pBoard;
		g_pBoard->threads.push_back(p);
	}
	params.ret().udt = UDT_NUM;
	params.ret().num = double(int(p));
	if (params.params == 3)
	{
		*params.prg->getVar(params[2].lit) = params.ret();
	}
}

/*
 * void killThread(thread id)
 * 
 * Kill a thread.
 */
void killThread(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("KillThread() requires one parameter.");
	}
	CThread *p = (CThread *)int(params[0].getNum());
	if (!CThread::isThread(p))
	{
		throw CError("Invalid thread ID for KillThread().");
	}
	CThread::destroy(p); // Innocuous.
}

/*
 * thread getThreadId([thread &ret])
 * 
 * Get the ID of this thread.
 */
void getThreadId(CALL_DATA &params)
{
	if (!params.prg->isThread())
	{
		throw CError("GetThreadID() is invalid outside of threads.");
	}
	params.ret().udt = UDT_NUM;
	params.ret().num = double(int(params.prg));
	if (params.params == 1)
	{
		*params.prg->getVar(params[0].lit) = params.ret();
	}
	else if (params.params != 0)
	{
		throw CError("GetThreadID() requires zero or one parameters.");
	}
}

/*
 * void threadSleep(thread id, double seconds)
 * 
 * Put a thread to sleep.
 */
void threadSleep(CALL_DATA &params)
{
	if (params.params != 2)
	{
		throw CError("ThreadSleep() requires one parameter.");
	}
	CThread *p = (CThread *)int(params[0].getNum());
	if (!CThread::isThread(p))
	{
		throw CError("Invalid thread ID for ThreadSleep().");
	}
	p->sleep((unsigned long)(params[1].getNum() * 1000.0));
}

/*
 * tellthread(...)
 * 
 * Description.
 */
void tellthread(CALL_DATA &params)
{

}

/*
 * void threadWake(thread id)
 * 
 * Wake up a thread.
 */
void threadWake(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("ThreadWake() requires one parameter.");
	}
	CThread *p = (CThread *)int(params[0].getNum());
	if (!CThread::isThread(p))
	{
		throw CError("Invalid thread ID for ThreadWake().");
	}
	p->wakeUp();
}

/*
 * double threadSleepRemaining(thread id, [double &ret])
 * 
 * Check how much sleep remains for a thread.
 */
void threadSleepRemaining(CALL_DATA &params)
{
	if ((params.params != 1) && (params.params != 2))
	{
		throw CError("ThreadSleepRemaining() requires one or two parameters.");
	}
	CThread *p = (CThread *)int(params[0].getNum());
	if (!CThread::isThread(p))
	{
		throw CError("Invalid thread ID for ThreadSleepRemaining().");
	}
	params.ret().udt = UDT_NUM;
	params.ret().num = p->sleepRemaining() / 1000.0;
	if (params.params == 2)
	{
		*params.prg->getVar(params[1].lit) = params.ret();
	}
}

/*
 * variant &local(variant &var, [variant &ret])
 * 
 * Allocates a variable off the stack and returns a reference to it.
 */
void local(CALL_DATA &params)
{
	if ((params.params != 1) && (params.params != 2))
	{
		throw CError("Local() requires one or two parameters.");
	}
	params.prg->getLocal(params[0].lit); // Allocates a var if it does not exist.
	params.ret().udt = UDT_ID;
	params.ret().lit = params[0].lit;
	if (params.params == 2)
	{
		*params.prg->getVar(params[1].lit) = params.ret();
	}
}

/*
 * variant &global(variant &var, [variant &ret])
 * 
 * Allocates a variable on the heap and returns a reference to it.
 *
 * x = 1;
 * method func()
 * {
 *    local(x) = 2;
 *    mwin(global(x));
 *    mwin(x);
 * }
 *
 * Given the above code, the values of the message windows,
 * should func() be called, would be 1 and 2 respectively
 * because variables off the stack are preferred to ones
 * on the heap. The call to global() explictly requests
 * the variable 'x' from the heap.
 */
void global(CALL_DATA &params)
{
	if ((params.params != 1) && (params.params != 2))
	{
		throw CError("Global() requires one or two parameters.");
	}
	CProgram::getGlobal(params[0].lit); // Allocates a var if it does not exist.
	params.ret().udt = UDT_ID;
	params.ret().lit = ":" + params[0].lit;
	if (params.params == 2)
	{
		*params.prg->getVar(params[1].lit) = params.ret();
	}
}

/*
 * void autoCommand()
 * 
 * Obsolete.
 */
void autoCommand(CALL_DATA &params)
{
	throw CWarning("AutoCommand() is obsolete.");
}

/*
 * cursor_map createCursorMap([cursor_map &ret])
 * 
 * Create a cursor map.
 */
void createCursorMap(CALL_DATA &params)
{
	if (params.params == 0)
	{
		params.ret().udt = UDT_NUM;
		params.ret().num = double(int(g_cursorMaps.allocate()));
	}
	else if (params.params == 1)
	{
		LPSTACK_FRAME var = params.prg->getVar(params[0].lit);
		var->udt = UDT_NUM;
		var->num = double(int(g_cursorMaps.allocate()));
	}
	else
	{
		throw CError("CreateCursorMap() requires zero or one parameters.");
	}
}

/*
 * void killCursorMap(cursor_map map)
 * 
 * Kill a cursor map.
 */
void killCursorMap(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("KillCursorMap() requires one parameter.");
	}
	g_cursorMaps.free((CCursorMap *)(int)params[0].getNum());
}

/*
 * void cursorMapAdd(int x, int y, cursor_map map)
 * 
 * Add a point to a cursor map.
 */
void cursorMapAdd(CALL_DATA &params)
{
	if (params.params != 3)
	{
		throw CError("CursorMapAdd() requires three parameters.");
	}
	CCursorMap *p = g_cursorMaps.cast((int)params[2].getNum());
	if (p)
	{
		p->add(int(params[0].getNum()), int(params[1].getNum()));
	}
}

/*
 * int cursorMapRun(cursor_map map, [int &ret])
 * 
 * Run a cursor map.
 */
void cursorMapRun(CALL_DATA &params)
{
	if (params.params == 1)
	{
		CCursorMap *p = g_cursorMaps.cast((int)params[0].getNum());
		if (p)
		{
			params.ret().udt = UDT_NUM;
			params.ret().num = double(p->run());
		}
	}
	else if (params.params == 2)
	{
		CCursorMap *p = g_cursorMaps.cast((int)params[0].getNum());
		if (p)
		{
			LPSTACK_FRAME var = params.prg->getVar(params[1].lit);
			var->udt = UDT_NUM;
			var->num = double(p->run());
		}
	}
	else
	{
		throw CError("CursorMapRun() requires one or two parameters.");
	}
}

/*
 * canvas createCanvas(int width, int height, [canvas &cnv])
 * 
 * Create a canvas.
 */
void createCanvas(CALL_DATA &params)
{
	if (params.params != 2 && params.params != 3)
	{
		throw CError("CreateCanvas() requires two or three parameters.");
	}
	CGDICanvas *p = g_canvases.allocate();
	p->CreateBlank(NULL, params[0].getNum(), params[1].getNum(), TRUE);
	p->ClearScreen(0);
	params.ret().udt = UDT_NUM;
	params.ret().num = double(int(p));
	if (params.params == 3)
	{
		*params.prg->getVar(params[2].lit) = params.ret();
	}
}

/*
 * void killCanvas(canvas cnv)
 * 
 * Kill a canvas.
 */
void killCanvas(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("KillCanvas() requires one parameter.");
	}
	CGDICanvas *p = (CGDICanvas *)(int)params[0].getNum();
	g_canvases.free(p);
}

/*
 * void drawCanvas(canvas cnv, int x, int y, [int width, int height, [canvas dest]])
 * 
 * Blit a canvas forward.
 */
void drawCanvas(CALL_DATA &params)
{
	if (params.params == 3)
	{
		CGDICanvas *p = g_canvases.cast((int)params[0].getNum());
		if (p)
		{
			p->Blt(g_cnvRpgCode, params[1].getNum(), params[2].getNum());
			renderRpgCodeScreen();
		}
	}
	else if (params.params == 5)
	{
		CGDICanvas *p = g_canvases.cast((int)params[0].getNum());
		if (p)
		{
			p->BltStretch(g_cnvRpgCode, params[1].getNum(), params[2].getNum(), 0, 0, p->GetWidth(), p->GetHeight(), params[3].getNum(), params[4].getNum(), SRCCOPY);
			renderRpgCodeScreen();
		}
	}
	else if (params.params == 6)
	{
		CGDICanvas *p = g_canvases.cast((int)params[0].getNum());
		if (p)
		{
			CGDICanvas *pDest = g_canvases.cast((int)params[5].getNum());
			if (pDest)
			{
				p->BltStretch(pDest, params[1].getNum(), params[2].getNum(), 0, 0, p->GetWidth(), p->GetHeight(), params[3].getNum(), params[4].getNum(), SRCCOPY);
			}
		}
	}
	else
	{
		throw CError("DrawCanvas() requires three, five, or six parameters.");
	}
}

/*
 * void openFileInput(string file, string folder)
 * 
 * Open a file in input mode.
 */
void openFileInput(CALL_DATA &params)
{
	extern std::string g_projectPath;

	if (params.params != 2)
	{
		throw CError("OpenFileInput() requires two parameters.");
	}
	g_files[parser::uppercase(params[0].getLit())].open(g_projectPath + params[1].getLit() + '\\' + params[0].getLit(), OF_READ);
}

/*
 * void openFileOutput(string file, string folder)
 * 
 * Open a file in output mode.
 */
void openFileOutput(CALL_DATA &params)
{
	extern std::string g_projectPath;

	if (params.params != 2)
	{
		throw CError("OpenFileOutput() requires two parameters.");
	}
	g_files[parser::uppercase(params[0].getLit())].open(g_projectPath + params[1].getLit() + '\\' + params[0].getLit(), OF_CREATE | OF_WRITE);
}

/*
 * void openFileAppend(string file, string folder)
 * 
 * Open a file for appending.
 */
void openFileAppend(CALL_DATA &params)
{
	extern std::string g_projectPath;

	if (params.params != 2)
	{
		throw CError("OpenFileOutput() requires two parameters.");
	}
	CFile &file = g_files[parser::uppercase(params[0].getLit())];
	file.open(g_projectPath + params[1].getLit() + '\\' + params[0].getLit(), OF_WRITE);
	file.seek(file.size());
}

/*
 * void openFileBinary(string file, string folder)
 * 
 * Obsolete.
 */
void openFileBinary(CALL_DATA &params)
{
	CProgram::debugger("OpenFileBinary() is obsolete. Use OpenFileInput() instead.");
	openFileInput(params);
}

/*
 * void closeFile(string file)
 * 
 * Close a file.
 */
void closeFile(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("CloseFile() requires one parameter.");
	}
	std::map<std::string, CFile>::iterator i = g_files.find(parser::uppercase(params[0].getLit()));
	if (i != g_files.end())
	{
		g_files.erase(i);
	}
}

/*
 * string fileInput(string file, [string &ret])
 * 
 * Read a line from a line.
 */
void fileInput(CALL_DATA &params)
{
	if ((params.params != 1) && (params.params != 2))
	{
		throw CError("FileInput() requires one or two parameters.");
	}
	std::map<std::string, CFile>::iterator i = g_files.find(parser::uppercase(params[0].getLit()));
	if (!((i != g_files.end()) && i->second.isOpen())) return;
	params.ret().udt = UDT_LIT;
	params.ret().lit = i->second.line();
	params.ret().lit = params.ret().lit.substr(0, params.ret().lit.length() - 1);
	if (params.params == 2)
	{
		*params.prg->getVar(params[1].lit) = params.ret();
	}
}

/*
 * void filePrint(string file, string line)
 * 
 * Write a line to a file.
 */
void filePrint(CALL_DATA &params)
{
	if (params.params != 2)
	{
		throw CError("FilePrint() requires two parameters.");
	}
	std::map<std::string, CFile>::iterator i = g_files.find(parser::uppercase(params[0].getLit()));
	if (!((i != g_files.end()) && i->second.isOpen())) return;
	const std::string str = params[1].getLit();
	for (std::string::const_iterator j = str.begin(); j != str.end(); ++j)
	{
		i->second << *j;
	}
	i->second << '\r' << '\n';
}

/*
 * string fileGet(string file, [string &ret])
 * 
 * Get a byte from a file.
 */
void fileGet(CALL_DATA &params)
{
	if ((params.params != 1) && (params.params != 2))
	{
		throw CError("FileGet() requires one or two parameters.");
	}
	std::map<std::string, CFile>::iterator i = g_files.find(parser::uppercase(params[0].getLit()));
	if (!((i != g_files.end()) && i->second.isOpen())) return;
	params.ret().udt = UDT_LIT;
	char c;
	i->second >> c;
	params.ret().lit = c;
	if (params.params == 2)
	{
		*params.prg->getVar(params[1].lit) = params.ret();
	}
}

/*
 * void filePut(string file, string byte)
 * 
 * Write a byte to a file.
 */
void filePut(CALL_DATA &params)
{
	if (params.params != 2)
	{
		throw CError("FilePut() requires two parameters.");
	}
	std::map<std::string, CFile>::iterator i = g_files.find(parser::uppercase(params[0].getLit()));
	if (!((i != g_files.end()) && i->second.isOpen())) return;
	i->second << params[1].getLit()[0];
}

/*
 * bool fileEof(string file, [bool &ret])
 * 
 * Check whether the end of a file has been reached.
 */
void fileEof(CALL_DATA &params)
{
	if ((params.params != 1) && (params.params != 2))
	{
		throw CError("FileEOF() requires one or two parameters.");
	}
	std::map<std::string, CFile>::iterator i = g_files.find(parser::uppercase(params[0].getLit()));
	if (!((i != g_files.end()) && i->second.isOpen())) return;
	params.ret().udt = UDT_NUM;
	params.ret().num = double(i->second.isEof());
	if (params.params == 2)
	{
		*params.prg->getVar(params[1].lit) = params.ret();
	}
}

/*
 * int len(string str, [int &ret])
 * 
 * Get the length of a string.
 */
void len(CALL_DATA &params)
{
	if (params.params == 1)
	{
		params.ret().udt = UDT_NUM;
		params.ret().num = params[0].getLit().length();
	}
	else if (params.params == 2)
	{
		LPSTACK_FRAME var = params.prg->getVar(params[1].lit);
		var->udt = UDT_NUM;
		var->num = params[0].getLit().length();
	}
	else
	{
		throw CError("Len() requires one or two parameters.");
	}
}

/*
 * int inStr(string haystack, string needle[, int offset])
 * 
 * Returns the first occurence of needle within haystack,
 * optionally starting from an offset. Both the return
 * value and the offset are one-based.
 */
void instr(CALL_DATA &params)
{
	if ((params.params != 2) && (params.params != 3))
	{
		throw CError("InStr() requires two or three parameters.");
	}

	const std::string haystack = params[0].getLit();
	unsigned int offset = (params.params == 3) ? (int(params[2].getNum()) - 1) : 0;
	if (offset < 0) offset = 0;

	const unsigned int pos = haystack.find(params[1].getLit(), offset) + 1;

	params.ret().udt = UDT_NUM;
	params.ret().num = double(pos);
}

/*
 * string getItemName(string fileName[, string &ret])
 * 
 * Get an item's handle.
 */
void getitemname(CALL_DATA &params)
{
	extern std::string g_projectPath;

	if ((params.params != 1) && (params.params != 2))
	{
		throw CError("GetItemName() requires one or two parameters.");
	}

	const std::string file = g_projectPath + ITM_PATH + params[0].getLit();
	if (!CFile::fileExists(file)) return;

	ITEM itm; SPRITE_ATTR attr;
	itm.open(file, attr);

	params.ret().udt = UDT_LIT;
	params.ret().lit = itm.itemName;

	if (params.params == 2)
	{
		*params.prg->getVar(params[1].lit) = params.ret();
	}
}

/*
 * string getItemDesc(string fileName[, string &ret])
 * 
 * Get an item's description.
 */
void getitemdesc(CALL_DATA &params)
{
	extern std::string g_projectPath;

	if ((params.params != 1) && (params.params != 2))
	{
		throw CError("GetItemDesc() requires one or two parameters.");
	}

	const std::string file = g_projectPath + ITM_PATH + params[0].getLit();
	if (!CFile::fileExists(file)) return;

	ITEM itm; SPRITE_ATTR attr;
	itm.open(file, attr);

	params.ret().udt = UDT_LIT;
	params.ret().lit = itm.itmDescription;

	if (params.params == 2)
	{
		*params.prg->getVar(params[1].lit) = params.ret();
	}
}

/*
 * int getItemCost(string fileName[, int &ret])
 * 
 * Get an item's cost.
 */
void getitemcost(CALL_DATA &params)
{
	extern std::string g_projectPath;

	if ((params.params != 1) && (params.params != 2))
	{
		throw CError("GetItemDesc() requires one or two parameters.");
	}

	const std::string file = g_projectPath + ITM_PATH + params[0].getLit();
	if (!CFile::fileExists(file)) return;

	ITEM itm; SPRITE_ATTR attr;
	itm.open(file, attr);

	params.ret().udt = UDT_NUM;
	params.ret().num = itm.buyPrice;

	if (params.params == 2)
	{
		*params.prg->getVar(params[1].lit) = params.ret();
	}
}

/*
 * int getItemSellPrice(string fileName[, int &ret])
 * 
 * Get the price for which an item sells.
 */
void getitemsellprice(CALL_DATA &params)
{
	extern std::string g_projectPath;

	if ((params.params != 1) && (params.params != 2))
	{
		throw CError("GetItemDesc() requires one or two parameters.");
	}

	const std::string file = g_projectPath + ITM_PATH + params[0].getLit();
	if (!CFile::fileExists(file)) return;

	ITEM itm; SPRITE_ATTR attr;
	itm.open(file, attr);

	params.ret().udt = UDT_NUM;
	params.ret().num = itm.sellPrice;

	if (params.params == 2)
	{
		*params.prg->getVar(params[1].lit) = params.ret();
	}
}

/*
 * splicevariables(...)
 * 
 * Description.
 */
void splicevariables(CALL_DATA &params)
{

}

/*
 * int split(string str, string delimiter, array arr)
 * 
 * Splits a string at a delimiter. Returns the number of
 * upper bound of the array (i.e. the index of the last
 * set element).
 */
void split(CALL_DATA &params)
{
	if (params.params != 3)
	{
		throw CError("Split() requires three parameters.");
	}

	std::vector<std::string> parts;
	split(params[0].getLit(), params[1].getLit(), parts);

	// Strip '[]' off the array name.
	std::string array = params[2].lit;
	replace(array, "[]", "");

	if (params[2].udt & UDT_LIT)
	{
		// The array name was passed in quotes, so a $ or !
		// may have eluded the parser's stripping of them.
		char &c = array[array.length() - 1];
		if ((c == '$') || (c == '!'))
		{
			array = array.substr(0, array.length() - 1);
		}
	}

	std::vector<std::string>::const_iterator i = parts.begin();
	for (unsigned int j = 0; i != parts.end(); ++i, ++j)
	{
		char str[255]; itoa(j, str, 10);
		LPSTACK_FRAME var = params.prg->getVar(array + '[' + str + ']');
		var->udt = UDT_LIT;
		var->lit = *i;
	}

	params.ret().udt = UDT_NUM;
	params.ret().num = double(parts.size()) + 1.0;
}

/*
 * int asc(string chr, [int &ret])
 * 
 * Get the ASCII value of a character.
 */
void asc(CALL_DATA &params)
{
	if (params.params == 1)
	{
		params.ret().udt = UDT_NUM;
		params.ret().num = (double)params[0].getLit()[0];
	}
	else if (params.params == 2)
	{
		LPSTACK_FRAME var = params.prg->getVar(params[1].lit);
		var->udt = UDT_NUM;
		var->num = (double)params[0].getLit()[0];
	}
	else
	{
		throw CError("Asc() requires one or two parameters.");
	}
}

/*
 * string chr(int asc, [string &ret])
 * 
 * Get the character represented by the ASCII code passed in.
 */
void chr(CALL_DATA &params)
{
	if ((params.params != 1) && (params.params != 2))
	{
		throw CError("Chr() requires one or two parameters.");
	}
	params.ret().udt = UDT_LIT;
	params.ret().lit = (char)params[0].getNum();
	if (params.params == 2)
	{
		*params.prg->getVar(params[1].lit) = params.ret();
	}
}

/*
 * string trim(string str, [string &ret])
 * 
 * Trim whitespace from a string.
 */
void trim(CALL_DATA &params)
{
	if ((params.params != 1) && (params.params != 2))
	{
		throw CError("Trim() requires one or two parameters.");
	}
	params.ret().udt = UDT_LIT;
	params.ret().lit = parser::trim(params[0].getLit());
	if (params.params == 2)
	{
		*params.prg->getVar(params[1].lit) = params.ret();
	}
}

/*
 * string right(string str, int amount, [string &ret])
 * 
 * Get characters from the right of a string.
 */
void right(CALL_DATA &params)
{
	if ((params.params != 2) && (params.params != 3))
	{
		throw CError("Right() requires two or three parameters.");
	}
	params.ret().udt = UDT_LIT;
	try
	{
		params.ret().lit = params[0].getLit().substr(params[0].getLit().length() - int(params[1].getNum()), int(params[1].getNum()));
	}
	catch (...) { }
	if (params.params == 3)
	{
		*params.prg->getVar(params[2].lit) = params.ret();
	}
}

/*
 * string left(string str, int amount, [string &ret])
 * 
 * Get characters from the left of a string.
 */
void left(CALL_DATA &params)
{
	if ((params.params != 2) && (params.params != 3))
	{
		throw CError("Left() requires two or three parameters.");
	}
	params.ret().udt = UDT_LIT;
	try
	{
		params.ret().lit = params[0].getLit().substr(0, int(params[1].getNum()));
	}
	catch (...) { }
	if (params.params == 3)
	{
		*params.prg->getVar(params[2].lit) = params.ret();
	}
}

/*
 * cursormaphand(...)
 * 
 * Description.
 */
void cursormaphand(CALL_DATA &params)
{

}

/*
 * void debugger(string message)
 * 
 * Show a debug message.
 */
void debugger(CALL_DATA &params)
{
	if (params.params != 1)
	{
		throw CError("Debugger() requires one parameter.");
	}
	CProgram::debugger(params[0].getLit());
}

/*
 * onError(label lbl)
 * 
 * Obsolete.
 */
void onError(CALL_DATA &params)
{
	throw CError("OnError() is obsolete.");
}

/*
 * ResumeNext()
 * 
 * Obsolete.
 */
void resumeNext(CALL_DATA &params)
{
	throw CError("ResumeNext() is obsolete.");
}

/*
 * msgbox(...)
 * 
 * Description.
 */
void msgbox(CALL_DATA &params)
{

}

/*
 * setconstants(...)
 * 
 * Description.
 */
void setconstants(CALL_DATA &params)
{

}

/*
 * autolocal(...)
 * 
 * Description.
 */
void autolocal(CALL_DATA &params)
{

}

/*
 * string GetBoardName([string ret])
 * 
 * Get the current board's file name.
 */
void getBoardName(CALL_DATA &params)
{
	extern LPBOARD g_pBoard;
	params.ret().udt = UDT_LIT;
	params.ret().lit = g_pBoard->strFilename;
	if (params.params == 1)
	{
		*params.prg->getVar(params[0].lit) = params.ret();
	}
}

/*
 * string lcase(string str, [string &ret])
 * 
 * Convert a string to lowercase.
 */
void lcase(CALL_DATA &params)
{
	if ((params.params != 1) && (params.params != 2))
	{
		throw CError("LCase() requires one or two parameters.");
	}
	char *const str = _strlwr(_strdup(params[0].getLit().c_str()));
	params.ret().udt = UDT_LIT;
	params.ret().lit = str;
	free(str);
	if (params.params == 2)
	{
		*params.prg->getVar(params[1].lit) = params.ret();
	}
}

/*
 * string ucase(string str, [string &ret])
 * 
 * Convert a string to uppercase.
 */
void ucase(CALL_DATA &params)
{
	if ((params.params != 1) && (params.params != 2))
	{
		throw CError("UCase() requires one or two parameters.");
	}
	char *const str = _strupr(_strdup(params[0].getLit().c_str()));
	params.ret().udt = UDT_LIT;
	params.ret().lit = str;
	free(str);
	if (params.params == 2)
	{
		*params.prg->getVar(params[1].lit) = params.ret();
	}
}

/*
 * string appPath([string &dest])
 * 
 * Retrieve the path of trans3.exe, without the final backslash.
 */
void apppath(CALL_DATA &params)
{
	extern HINSTANCE g_hInstance;

	TCHAR path[MAX_PATH];
	GetModuleFileName(g_hInstance, path, MAX_PATH);

	// Path is fully qualified, but we want only the directory.
	std::string strPath = path;
	strPath = strPath.substr(0, strPath.find_last_of('\\'));

	params.ret().udt = UDT_LIT;
	params.ret().lit = strPath;

	if (params.params == 1)
	{
		*params.prg->getVar(params[0].lit) = params.ret();
	}
}

/*
 * string mid(string str, int start, int length[, string &dest])
 * 
 * Obtain a substring. The offset is one-based.
 */
void mid(CALL_DATA &params)
{
	if ((params.params != 3) && (params.params != 4))
	{
		throw CError("Mid() requires three or four parameters.");
	}

	unsigned int start = int(params[1].getNum()) - 1;
	unsigned int length = int(params[2].getNum());

	params.ret().udt = UDT_LIT;
	params.ret().lit = params[0].getLit().substr(start, length);

	if (params.params == 4)
	{
		*params.prg->getVar(params[3].lit) = params.ret();
	}
}

/*
 * string replace(string str, string find, string replace[, string &dest])
 * 
 * Replace within a string.
 */
void replace(CALL_DATA &params)
{
	if ((params.params != 3) && (params.params != 4))
	{
		throw CError("Replace() requires three or four parameters.");
	}

	std::string str = params[0].getLit();
	replace(str, params[1].getLit(), params[2].getLit());

	params.ret().udt = UDT_LIT;
	params.ret().lit = str;

	if (params.params == 4)
	{
		*params.prg->getVar(params[3].lit) = params.ret();
	}
}

/*
 * endanimation(...)
 * 
 * Description.
 */
void endanimation(CALL_DATA &params)
{

}

/*
 * rendernow(...)
 * 
 * Description.
 */
void rendernow(CALL_DATA &params)
{

}

/*
 * multirun(...)
 * 
 * Description.
 */
void multirun(CALL_DATA &params)
{

}

/*
 * shopcolors(...)
 * 
 * Description.
 */
void shopcolors(CALL_DATA &params)
{

}

/*
 * mousecursor(...)
 * 
 * Description.
 */
void mousecursor(CALL_DATA &params)
{

}

/*
 * gettextwidth(...)
 * 
 * Description.
 */
void gettextwidth(CALL_DATA &params)
{

}

/*
 * gettextheight(...)
 * 
 * Description.
 */
void gettextheight(CALL_DATA &params)
{

}

/*
 * iif(condition, true, false)
 * 
 * Obsolete.
 */
void iif(CALL_DATA &params)
{
	operators::tertiary(params);
	throw CWarning("IIf() is obsolete. Use the ?: operator.");
}

/*
 * itemstance(...)
 * 
 * Description.
 */
void itemstance(CALL_DATA &params)
{

}

/*
 * playerstance(...)
 * 
 * Description.
 */
void playerstance(CALL_DATA &params)
{

}

/*
 * drawcanvastransparent(...)
 * 
 * Description.
 */
void drawcanvastransparent(CALL_DATA &params)
{

}

/*
 * int getTickCount()
 * 
 * Get the number of milliseconds since Windows started.
 */
void getTickCount(CALL_DATA &params)
{
	params.ret().udt = UDT_NUM;
	params.ret().num = GetTickCount();
}

/*
 * setvolume(...)
 * 
 * Description.
 */
void setvolume(CALL_DATA &params)
{

}

/*
 * createtimer(...)
 * 
 * Description.
 */
void createtimer(CALL_DATA &params)
{

}

/*
 * killtimer(...)
 * 
 * Description.
 */
void killtimer(CALL_DATA &params)
{

}

/*
 * setmwintranslucency(...)
 * 
 * Description.
 */
void setmwintranslucency(CALL_DATA &params)
{

}

/*
 * string regExpReplace(string subject, string pattern, string replace)
 *
 * Replace using a regular expression.
 */
void regExpReplace(CALL_DATA &params)
{
	if (params.params != 3)
	{
		throw CError("RegExpReplace() requires three parameters.");
	}

	// Create a RegExp object.
	IDispatch *pRegExp = NULL;
	CLSID clsid = {0x3F4DACA4, 0x160D, 0x11D2, {0xA8, 0xE9, 0x00, 0x10, 0x4B, 0x36, 0x5C, 0x9F}};
	HRESULT res = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IDispatch, (void **)&pRegExp);

	if (FAILED(res))
	{
		throw CError("RegExpReplace(): Internet Explorer 4 or higher is required for regular expression support.");
	}

	CComDispatchDriver regexp = pRegExp;
	pRegExp->Release();

	// Set to replace all instances, not just the first.
	CComVariant global(true);
	regexp.PutPropertyByName(L"Global", &global);

	// Set the pattern to be used.
	CComVariant pattern(params[1].getLit().c_str());
	regexp.PutPropertyByName(L"Pattern", &pattern);

	// Execute the replace.
	CComVariant search(params[0].getLit().c_str());
	CComVariant replace(params[2].getLit().c_str());
	CComVariant ret;
	regexp.Invoke2(L"Replace", &search, &replace, &ret);

	// Return the result.
	params.ret().udt = UDT_LIT;
	params.ret().lit = getString(ret.bstrVal);
}

/*
 * Initialize RPGCode.
 */
void initRpgCode()
{
	// List of functions.
	CProgram::addFunction("mwin", mwin);
	CProgram::addFunction("wait", wait);
	CProgram::addFunction("mwincls", mwinCls);
	CProgram::addFunction("send", send);
	CProgram::addFunction("text", text);
	CProgram::addFunction("pixeltext", pixelText);
	CProgram::addFunction("mbox", mwin);
	CProgram::addFunction("branch", branch);
	CProgram::addFunction("change", change);
	CProgram::addFunction("clear", clear);
	CProgram::addFunction("done", done);
	CProgram::addFunction("dos", windows);
	CProgram::addFunction("windows", windows);
	CProgram::addFunction("empty", empty);
	CProgram::addFunction("end", end);
	CProgram::addFunction("font", font);
	CProgram::addFunction("fontsize", fontSize);
	CProgram::addFunction("fade", fade);
	CProgram::addFunction("fbranch", branch);
	CProgram::addFunction("fight", fight);
	CProgram::addFunction("get", get);
	CProgram::addFunction("gone", gone);
	CProgram::addFunction("viewbrd", viewbrd);
	CProgram::addFunction("bold", bold);
	CProgram::addFunction("italics", italics);
	CProgram::addFunction("underline", underline);
	CProgram::addFunction("wingraphic", winGraphic);
	CProgram::addFunction("wincolor", winColor);
	CProgram::addFunction("wincolorrgb", winColorRgb);
	CProgram::addFunction("color", color);
	CProgram::addFunction("colorrgb", colorRgb);
	CProgram::addFunction("move", move);
	CProgram::addFunction("newplyr", newPlyr);
	CProgram::addFunction("over", over);
	CProgram::addFunction("prg", prg);
	CProgram::addFunction("prompt", prompt);
	CProgram::addFunction("put", put);
	CProgram::addFunction("reset", reset);
	CProgram::addFunction("run", run);
	CProgram::addFunction("show", mwin);
	CProgram::addFunction("sound", sound);
	CProgram::addFunction("win", win);
	CProgram::addFunction("hp", hp);
	CProgram::addFunction("givehp", giveHp);
	CProgram::addFunction("gethp", getHp);
	CProgram::addFunction("maxhp", maxHp);
	CProgram::addFunction("getmaxhp", getMaxHp);
	CProgram::addFunction("smp", smp);
	CProgram::addFunction("givesmp", giveSmp);
	CProgram::addFunction("getsmp", getSmp);
	CProgram::addFunction("maxsmp", maxSmp);
	CProgram::addFunction("getmaxsmp", getMaxSmp);
	CProgram::addFunction("start", start);
	CProgram::addFunction("giveitem", giveItem);
	CProgram::addFunction("takeitem", takeItem);
	CProgram::addFunction("wav", wav);
	CProgram::addFunction("delay", delay);
	CProgram::addFunction("random", random);
	CProgram::addFunction("push", push);
	CProgram::addFunction("tiletype", tileType);
	CProgram::addFunction("midiplay", mediaPlay);
	CProgram::addFunction("playmidi", mediaPlay);
	CProgram::addFunction("mediaplay", mediaPlay);
	CProgram::addFunction("mediastop", mediaStop);
	CProgram::addFunction("mediarest", mediaStop);
	CProgram::addFunction("midirest", mediaStop);
	CProgram::addFunction("godos", goDos);
	CProgram::addFunction("addplayer", addPlayer);
	CProgram::addFunction("removeplayer", removePlayer);
	CProgram::addFunction("setpixel", setPixel);
	CProgram::addFunction("drawline", drawLine);
	CProgram::addFunction("drawrect", drawRect);
	CProgram::addFunction("fillrect", fillRect);
	CProgram::addFunction("debug", debug);
	CProgram::addFunction("castnum", castNum);
	CProgram::addFunction("castlit", castLit);
	CProgram::addFunction("castint", castInt);
	CProgram::addFunction("pushitem", pushItem);
	CProgram::addFunction("wander", wander);
	CProgram::addFunction("bitmap", bitmap);
	CProgram::addFunction("mainfile", mainFile);
	CProgram::addFunction("dirsav", dirSav);
	CProgram::addFunction("save", save);
	CProgram::addFunction("load", load);
	CProgram::addFunction("scan", scan);
	CProgram::addFunction("mem", mem);
	CProgram::addFunction("print", print);
	CProgram::addFunction("rpgcode", rpgCode);
	CProgram::addFunction("charat", charAt);
	CProgram::addFunction("equip", equip);
	CProgram::addFunction("remove", remove);
	CProgram::addFunction("putplayer", putplayer);
	CProgram::addFunction("eraseplayer", eraseplayer);
	CProgram::addFunction("kill", kill);
	CProgram::addFunction("givegp", giveGp);
	CProgram::addFunction("takegp", takeGp);
	CProgram::addFunction("getgp", getGp);
	CProgram::addFunction("wavstop", wavstop);
	CProgram::addFunction("bordercolor", borderColor);
	CProgram::addFunction("fightenemy", fightEnemy);
	CProgram::addFunction("restoreplayer", restoreplayer);
	CProgram::addFunction("callshop", callshop);
	CProgram::addFunction("clearbuffer", clearBuffer);
	CProgram::addFunction("attackall", attackall);
	CProgram::addFunction("drainall", drainall);
	CProgram::addFunction("inn", inn);
	CProgram::addFunction("targetlocation", targetlocation);
	CProgram::addFunction("eraseitem", eraseitem);
	CProgram::addFunction("putitem", putitem);
	CProgram::addFunction("createitem", createitem);
	CProgram::addFunction("destroyitem", destroyitem);
	CProgram::addFunction("walkspeed", walkSpeed);
	CProgram::addFunction("itemwalkspeed", itemWalkSpeed);
	CProgram::addFunction("posture", posture);
	CProgram::addFunction("setbutton", setbutton);
	CProgram::addFunction("checkbutton", checkbutton);
	CProgram::addFunction("clearbuttons", clearbuttons);
	CProgram::addFunction("mouseclick", mouseclick);
	CProgram::addFunction("mousemove", mousemove);
	CProgram::addFunction("zoom", zoom);
	CProgram::addFunction("earthquake", earthquake);
	CProgram::addFunction("itemcount", itemcount);
	CProgram::addFunction("destroyplayer", destroyplayer);
	CProgram::addFunction("callplayerswap", callplayerswap);
	CProgram::addFunction("playavi", playavi);
	CProgram::addFunction("playavismall", playavismall);
	CProgram::addFunction("getcorner", getCorner);
	CProgram::addFunction("underarrow", underArrow);
	CProgram::addFunction("getlevel", getlevel);
	CProgram::addFunction("ai", ai);
	CProgram::addFunction("menugraphic", menugraphic);
	CProgram::addFunction("fightmenugraphic", fightMenuGraphic);
	CProgram::addFunction("fightstyle", fightStyle);
	CProgram::addFunction("stance", stance);
	CProgram::addFunction("battlespeed", battleSpeed);
	CProgram::addFunction("textspeed", textSpeed);
	CProgram::addFunction("characterspeed", characterSpeed);
	CProgram::addFunction("mwinsize", mwinsize);
	CProgram::addFunction("getdp", getDp);
	CProgram::addFunction("getfp", getFp);
	CProgram::addFunction("internalmenu", internalmenu);
	CProgram::addFunction("applystatus", applystatus);
	CProgram::addFunction("removestatus", removestatus);
	CProgram::addFunction("setimage", setImage);
	CProgram::addFunction("drawcircle", drawcircle);
	CProgram::addFunction("fillcircle", fillcircle);
	CProgram::addFunction("savescreen", savescreen);
	CProgram::addFunction("restorescreen", restorescreen);
	CProgram::addFunction("sin", sin);
	CProgram::addFunction("cos", cos);
	CProgram::addFunction("tan", tan);
	CProgram::addFunction("getpixel", getPixel);
	CProgram::addFunction("getcolor", getColor);
	CProgram::addFunction("getfontsize", getFontSize);
	CProgram::addFunction("setimagetransparent", setImageTransparent);
	CProgram::addFunction("setimagetranslucent", setImageTranslucent);
	CProgram::addFunction("mp3", wav);
	CProgram::addFunction("sourcelocation", sourcelocation);
	CProgram::addFunction("targethandle", targethandle);
	CProgram::addFunction("sourcehandle", sourcehandle);
	CProgram::addFunction("drawenemy", drawEnemy);
	CProgram::addFunction("mp3pause", mp3pause);
	CProgram::addFunction("layerput", layerput);
	CProgram::addFunction("getboardtile", getBoardTile);
	CProgram::addFunction("boardgettile", getBoardTile);
	CProgram::addFunction("sqrt", sqrt);
	CProgram::addFunction("getboardtiletype", getBoardTileType);
	CProgram::addFunction("setimageadditive", setimageadditive);
	CProgram::addFunction("animation", animation);
	CProgram::addFunction("sizedanimation", sizedanimation);
	CProgram::addFunction("forceredraw", forceRedraw);
	CProgram::addFunction("itemlocation", itemlocation);
	CProgram::addFunction("wipe", wipe);
	CProgram::addFunction("getres", getRes);
	CProgram::addFunction("statictext", staticText);
	CProgram::addFunction("pathfind", pathfind);
	CProgram::addFunction("itemstep", itemstep);
	CProgram::addFunction("playerstep", playerstep);
	CProgram::addFunction("redirect", redirect);
	CProgram::addFunction("killredirect", killredirect);
	CProgram::addFunction("killallredirects", killallredirects);
	CProgram::addFunction("parallax", parallax);
	CProgram::addFunction("giveexp", giveexp);
	CProgram::addFunction("animatedtiles", animatedTiles);
	CProgram::addFunction("smartstep", smartStep);
	CProgram::addFunction("gamespeed", gamespeed);
	CProgram::addFunction("thread", thread);
	CProgram::addFunction("killthread", killThread);
	CProgram::addFunction("getthreadid", getThreadId);
	CProgram::addFunction("threadsleep", threadSleep);
	CProgram::addFunction("tellthread", tellthread);
	CProgram::addFunction("threadwake", threadWake);
	CProgram::addFunction("threadsleepremaining", threadSleepRemaining);
	CProgram::addFunction("local", local);
	CProgram::addFunction("global", global);
	CProgram::addFunction("autocommand", autoCommand);
	CProgram::addFunction("createcursormap", createCursorMap);
	CProgram::addFunction("killcursormap", killCursorMap);
	CProgram::addFunction("cursormapadd", cursorMapAdd);
	CProgram::addFunction("cursormaprun", cursorMapRun);
	CProgram::addFunction("createcanvas", createCanvas);
	CProgram::addFunction("killcanvas", killCanvas);
	CProgram::addFunction("drawcanvas", drawCanvas);
	CProgram::addFunction("openfileinput", openFileInput);
	CProgram::addFunction("openfileoutput", openFileOutput);
	CProgram::addFunction("openfileappend", openFileAppend);
	CProgram::addFunction("openfilebinary", openFileBinary);
	CProgram::addFunction("closefile", closeFile);
	CProgram::addFunction("fileinput", fileInput);
	CProgram::addFunction("fileprint", filePrint);
	CProgram::addFunction("fileget", fileGet);
	CProgram::addFunction("fileput", filePut);
	CProgram::addFunction("fileeof", fileEof);
	CProgram::addFunction("length", len);
	CProgram::addFunction("len", len);
	CProgram::addFunction("instr", instr);
	CProgram::addFunction("getitemname", getitemname);
	CProgram::addFunction("getitemdesc", getitemdesc);
	CProgram::addFunction("getitemcost", getitemcost);
	CProgram::addFunction("getitemsellprice", getitemsellprice);
	CProgram::addFunction("stop", end);
	CProgram::addFunction("restorescreenarray", restorescreenarray);
	CProgram::addFunction("restorearrayscreen", restorescreenarray);
	CProgram::addFunction("splicevariables", splicevariables);
	CProgram::addFunction("split", split);
	CProgram::addFunction("asc", asc);
	CProgram::addFunction("chr", chr);
	CProgram::addFunction("trim", trim);
	CProgram::addFunction("right", right);
	CProgram::addFunction("left", left);
	CProgram::addFunction("cursormaphand", cursormaphand);
	CProgram::addFunction("debugger", debugger);
	CProgram::addFunction("onerror", onError);
	CProgram::addFunction("resumenext", resumeNext);
	CProgram::addFunction("msgbox", msgbox);
	CProgram::addFunction("setconstants", setconstants);
	CProgram::addFunction("log", log);
	CProgram::addFunction("onboard", onboard);
	CProgram::addFunction("autolocal", autolocal);
	CProgram::addFunction("getboardname", getBoardName);
	CProgram::addFunction("pixelmovement", pixelmovement);
	CProgram::addFunction("lcase", lcase);
	CProgram::addFunction("ucase", ucase);
	CProgram::addFunction("apppath", apppath);
	CProgram::addFunction("mid", mid);
	CProgram::addFunction("replace", replace);
	CProgram::addFunction("endanimation", endanimation);
	CProgram::addFunction("rendernow", rendernow);
	CProgram::addFunction("multirun", multirun);
	CProgram::addFunction("shopcolors", shopcolors);
	CProgram::addFunction("itemspeed", itemspeed);
	CProgram::addFunction("playerspeed", playerspeed);
	CProgram::addFunction("mousecursor", mousecursor);
	CProgram::addFunction("gettextwidth", gettextwidth);
	CProgram::addFunction("gettextheight", gettextheight);
	CProgram::addFunction("iif", iif);
	CProgram::addFunction("itemstance", itemstance);
	CProgram::addFunction("playerstance", playerstance);
	CProgram::addFunction("drawcanvastransparent", drawcanvastransparent);
	CProgram::addFunction("gettickcount", getTickCount);
	CProgram::addFunction("setvolume", setvolume);
	CProgram::addFunction("createtimer", createtimer);
	CProgram::addFunction("killtimer", killtimer);
	CProgram::addFunction("setmwintranslucency", setmwintranslucency);
	CProgram::addFunction("regexpreplace", regExpReplace);
}
