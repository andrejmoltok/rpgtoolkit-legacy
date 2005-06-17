/*
 * All contents copyright 2003, 2004, 2005 Christopher Matthews or Contributors.
 * Various port optimizations copyright by Colin James Fitzpatrick, 2005.
 * All rights reserved. You may not remove this notice
 * Read license.txt for licensing details.
 */

/*
 * Inclusions.
 */
#include "board.h"
#include "paths.h"
#include "CFile.h"

/*
 * Open a board.
 *
 * fileName (in) - file to open
 */
void tagBoard::open(const std::string fileName)
{
	// Set up some test vectors.
	vectors.clear();

	vectors.push_back(new CVector(32, 32, 4, TT_UNDER));
	vectors.back()->push_back(192, 32);
	vectors.back()->push_back(192, 224);
	vectors.back()->push_back(32, 224);
	vectors.back()->close(4, true, 0);


	vectors.push_back(new CVector(96, 64, 4, TT_SOLID));
	vectors.back()->push_back(32, 128);
	vectors.back()->push_back(96, 192);
	vectors.back()->push_back(160, 128);
	vectors.back()->close(4, true, 0);

	vectors.push_back(new CVector(480, 192, 4, TT_SOLID));
	vectors.back()->push_back(352, 64);
	vectors.back()->push_back(512, 128);
	vectors.back()->push_back(480, 64);
	vectors.back()->close(4, false, 0);

	vectors.push_back(new CVector(576, 320, 6, TT_SOLID));
	vectors.back()->push_back(576, 416);
	vectors.back()->push_back(448, 416);
	vectors.back()->push_back(448, 352);
	vectors.back()->push_back(512, 352);
	vectors.back()->push_back(512, 320);
	vectors.back()->close(6, true, 0);


	vectors.push_back(new CVector(96, 320, 10, TT_SOLID));
	vectors.back()->push_back(192, 256);
	vectors.back()->push_back(288, 384);
	vectors.back()->push_back(320, 224);
	vectors.back()->push_back(416, 320);
	vectors.back()->push_back(320, 288);
	vectors.back()->push_back(288, 416);
	vectors.back()->push_back(128, 320);
	vectors.back()->push_back(160, 448);
	vectors.back()->push_back(32, 288);
	vectors.back()->close(10, true, 0);

	CFile file(fileName);

	bSizeX = 50;
	bSizeY = 50;
	bSizeL = 8;

	strFilename = removePath(fileName);

	file.seek(14);
	char cUnused;
	file >> cUnused;
	file.seek(0);
	if (!cUnused)
	{

		std::string fileHeader;
		file >> fileHeader;

		if (fileHeader != "RPGTLKIT BOARD")
		{
			file.seek(0);
			goto ver1;
		}

		short majorVer, minorVer;
		file >> majorVer;
		file >> minorVer;

		if (minorVer < 2)
		{
			file.seek(0);
			goto ver2;
		}

		int regYN;
		std::string regCode;
		file >> regYN;
		file >> regCode;

		file >> bSizeX;
		file >> bSizeY;
		file >> bSizeL;
		setSize(bSizeX, bSizeY, bSizeL);

		file >> playerX;
		file >> playerY;
		file >> playerLayer;
		file >> brdSavingYN;
    
		short lutSize;
		file >> lutSize;
		tileIndex.clear();

		unsigned int i;
		hasAnmTiles = false;
		for (i = 0; i <= lutSize; i++)
		{
			std::string entry;
			file >> entry;
			tileIndex.push_back(entry);
			if (!entry.empty())
			{
				const std::string ext = getExtension(entry);
				if (_stricmp(ext.c_str(), "TAN") == 0)
				{
					anmTileLUTIndices.push_back(i);
					hasAnmTiles = true;
				}
			}
		}

		unsigned int x, y, l;
		for (l = 1; l <= bSizeL; l++)
		{
			for (y = 1; y <= bSizeY; y++)
			{
				for (x = 1; x <= bSizeX; x++)
				{
					short test;
					file >> test;
					if (test < 0)
					{
						test = -test;
						short bb, rr, gg, bl;
						char tt;
						file >> bb;
						file >> rr;
						file >> gg;
						file >> bl;
						file >> tt;
						for (unsigned int cnt = 1; cnt <= test; cnt++)
						{
							board[x][y][l] = bb;
							ambientRed[x][y][l] = rr;
							ambientGreen[x][y][l] = gg;
							ambientBlue[x][y][l] = bl;
							tiletype[x][y][l] = tt;
							unsigned int tAnm;
							const int len = anmTileLUTIndices.size();
							for (tAnm = 0; tAnm < len; tAnm++)
							{
								if (board[x][y][l] == anmTileLUTIndices[tAnm])
								{
									addAnimTile(tileIndex[board[x][y][l]], x, y, l);
								}
							}
							if (++x > bSizeX)
							{
								x = 1;
								if (++y > bSizeY)
								{
									y = 1;
									if (++l > bSizeL)
									{
										goto lutEnd;
									}
								}
							}
						}
						x--;
					}
					else
					{
						board[x][y][l] = test;
						file >> ambientRed[x][y][l];
						file >> ambientGreen[x][y][l];
						file >> ambientBlue[x][y][l];
						file >> tiletype[x][y][l];
						unsigned int tAnm;
						const int len = anmTileLUTIndices.size();
						for (tAnm = 0; tAnm < len; tAnm++)
						{
							if (board[x][y][l] == anmTileLUTIndices[tAnm])
							{
								addAnimTile(tileIndex[board[x][y][l]], x, y, l);
							}
						}
					}
				}
			}
		}
lutEnd:

		file >> brdBack;
		file >> brdFore;
		file >> borderBack;
		file >> brdColor;
		file >> borderColor;
		file >> ambientEffect;

		dirLink.clear();
		for (i = 1; i <= 4; i++)
		{
			std::string link;
			file >> link;
			dirLink.push_back(link);
		}

		file >> boardSkill;
		file >> boardBackground;
		file >> fightingYN;
		file >> BoardDayNight;
		file >> BoardNightBattleOverride;
		file >> BoardSkillNight;
		file >> BoardBackgroundNight;

		brdConst.clear();
		for (i = 0; i <= 10; i++)
		{
			short sConst;
			file >> sConst;
			brdConst.push_back(sConst);
		}

		file >> boardMusic;

		boardTitle.clear();
		for (i = 0; i <= 8; i++)
		{
			std::string title;
			file >> title;
			boardTitle.push_back(title);
		}

		short numPrg;
		file >> numPrg;

		programName.clear();
		progX.clear();
		progY.clear();
		progLayer.clear();
		progGraphic.clear();
		progActivate.clear();
		progVarActivate.clear();
		progDoneVarActivate.clear();
		activateInitNum.clear();
		activateDoneNum.clear();
		activationType.clear();

		for (i = 0; i <= numPrg; i++)
		{
			std::string strUnused;
			short sUnused;
			//
			file >> strUnused;
			programName.push_back(strUnused);
			//
			file >> sUnused;
			progX.push_back(sUnused);
			//
			file >> sUnused;
			progY.push_back(sUnused);
			//
			file >> sUnused;
			progLayer.push_back(sUnused);
			//
			file >> strUnused;
			progGraphic.push_back(strUnused);
			//
			file >> sUnused;
			progActivate.push_back(sUnused);
			//
			file >> strUnused;
			progVarActivate.push_back(strUnused);
			//
			file >> strUnused;
			progDoneVarActivate.push_back(strUnused);
			//
			file >> strUnused;
			activateInitNum.push_back(strUnused);
			//
			file >> strUnused;
			activateDoneNum.push_back(strUnused);
			//
			file >> sUnused;
			activationType.push_back(sUnused);
		}

		file >> enterPrg;
		file >> bgPrg;

		itmName.clear();
		itmName.push_back("");
		itmX.clear();
		itmX.push_back(0);
		itmY.clear();
		itmY.push_back(0);
		itmLayer.clear();
		itmLayer.push_back(0);
		itmActivate.clear();
		itmActivate.push_back(0);
		itmVarActivate.clear();
		itmVarActivate.push_back("");
		itmDoneVarActivate.clear();
		itmDoneVarActivate.push_back("");
		itmActivateInitNum.clear();
		itmActivateInitNum.push_back("");
		itmActivateDoneNum.clear();
		itmActivateDoneNum.push_back("");
		itmActivationType.clear();
		itmActivationType.push_back(0);
		itemProgram.clear();
		itemProgram.push_back("");
		itemMulti.clear();
		itemMulti.push_back("");

		short numItm;
		file >> numItm;

		int pos = 0;
		for (i = 0; i <= numItm; i++)
		{
			file >> itmName[pos];
			file >> itmX[pos];
			file >> itmY[pos];
			file >> itmLayer[pos];
			file >> itmActivate[pos];
			file >> itmVarActivate[pos];
			file >> itmDoneVarActivate[pos];
			file >> itmActivateInitNum[pos];
			file >> itmActivateDoneNum[pos];
			file >> itmActivationType[pos];
			file >> itemProgram[pos];
			file >> itemMulti[pos];
			if (!itmName[pos].empty())
			{
				itmName.push_back("");
				itmX.push_back(0);
				itmY.push_back(0);
				itmLayer.push_back(0);
				itmActivate.push_back(0);
				itmVarActivate.push_back("");
				itmDoneVarActivate.push_back("");
				itmActivateInitNum.push_back("");
				itmActivateDoneNum.push_back("");
				itmActivationType.push_back(0);
				itemProgram.push_back("");
				itemMulti.push_back("");
				pos++;
			}
		}

		threads.clear();

		if (minorVer >= 3)
		{
			int tCount;
			file >> tCount;
			for (i = 0; i <= tCount; i++)
			{
				std::string thread;
				file >> thread;
				threads.push_back(thread);
			}
		}

		if (minorVer > 2)
		{
			file >> isIsometric;
		}

	}
	else
	{
ver2:

		if (file.line() != "RPGTLKIT BOARD")
		{
			file.seek(0);
			goto ver1;
		}

		const int majorVer = atoi(file.line().c_str());
		const int minorVer = atoi(file.line().c_str());

		file.line();
		file.line();

		if (minorVer == 1)
		{
			bSizeX = atoi(file.line().c_str());
			bSizeY =  atoi(file.line().c_str());
			setSize(bSizeX, bSizeY, bSizeL);
		}
		else if (minorVer == 0)
		{
			setSize(19, 11, 8);
		}

		unsigned int x, y, l;
		for (x = 1; x <= bSizeX; x++)
		{
			for (y = 1; y <= bSizeY; y++)
			{
				for (l = 1; l <= bSizeL; l++)
				{
					// setTile(x, y, l, file.line());
					ambientRed[x][y][l] = atoi(file.line().c_str());
					ambientGreen[x][y][l] = atoi(file.line().c_str());
					ambientBlue[x][y][l] = atoi(file.line().c_str());
					tiletype[x][y][l] = atoi(file.line().c_str());
				}
			}
		}

		brdBack = file.line();
		borderBack = file.line();
		brdColor = atoi(file.line().c_str());
		borderColor = atoi(file.line().c_str());
		ambientEffect = atoi(file.line().c_str());

		unsigned int i;

		dirLink.clear();
		for (i = 1; i <= 4; i++)
		{
			dirLink.push_back(file.line());
		}

		boardSkill = atoi(file.line().c_str());
		boardBackground = file.line();
		fightingYN = atoi(file.line().c_str());

		brdConst.clear();
		for (i = 1; i <= 10; i++)
		{
			brdConst.push_back(atoi(file.line().c_str()));
		}

		boardMusic = file.line();

		boardTitle.clear();
		for (i = 1; i <= 8; i++)
		{
			boardTitle.push_back(file.line());
		}

		for (i = 0; i <= 50; i++)
		{
			programName.push_back(file.line());
			progX.push_back(atoi(file.line().c_str()));
			progY.push_back(atoi(file.line().c_str()));
			progLayer.push_back(atoi(file.line().c_str()));
			progGraphic.push_back(file.line());
			progActivate.push_back(atoi(file.line().c_str()));
			progVarActivate.push_back(file.line());
			progDoneVarActivate.push_back(file.line());
			activateInitNum.push_back(file.line());
			activateDoneNum.push_back(file.line());
			activationType.push_back(atoi(file.line().c_str()));
		}

		itmName.clear();
		itmX.clear();
		itmY.clear();
		itmLayer.clear();
		itmActivate.clear();
		itmVarActivate.clear();
		itmDoneVarActivate.clear();
		itmActivateInitNum.clear();
		itmActivateDoneNum.clear();
		itmActivationType.clear();
		itemProgram.clear();
		itemMulti.clear();

		for (i = 0; i <= 10; i++)
		{
			itmName.push_back(file.line());
			itmX.push_back(atoi(file.line().c_str()));
			itmY.push_back(atoi(file.line().c_str()));
			itmLayer.push_back(atoi(file.line().c_str()));
			itmActivate.push_back(atoi(file.line().c_str()));
			itmVarActivate.push_back(file.line());
			itmDoneVarActivate.push_back(file.line());
			itmActivateInitNum.push_back(file.line());
			itmActivateDoneNum.push_back(file.line());
			itmActivationType.push_back(atoi(file.line().c_str()));
		}

		playerX = atoi(file.line().c_str());
		playerY = atoi(file.line().c_str());
		playerLayer = atoi(file.line().c_str());

		for (i = 0; i <= 10; i++)
		{
			itemProgram.push_back(file.line());
		}

		brdSavingYN = atoi(file.line().c_str());

		for (i = 0; i <= 10; i++)
		{
			itemMulti.push_back(file.line());
		}

		BoardDayNight = atoi(file.line().c_str());
		BoardNightBattleOverride = atoi(file.line().c_str());
		BoardSkillNight = atoi(file.line().c_str());
		BoardBackgroundNight = atoi(file.line().c_str());

	}

ver1:

	/*
	 * Do we need version one support?
	 */

	return;

}

/*
 * Add an animated tile to the board.
 *
 * fileName (in) - tile to add
 * x (in) - x position on board
 * y (in) - y position on board
 * z (in) - z position on board
 */
void tagBoard::addAnimTile(const std::string fileName, const int x, const int y, const int z)
{
	static std::string lastAnimFile;
	static TILEANIM lastAnim;
	if (_stricmp(lastAnimFile.c_str(), fileName.c_str()) != 0)
	{
		extern std::string g_projectPath;
		lastAnim.open(g_projectPath + TILE_PATH + fileName);
		lastAnimFile = fileName;
	}
	BOARD_TILEANIM anim;
	anim.tile = lastAnim;
	anim.x = x;
	anim.y = y;
	anim.z = z;
	animatedTile.push_back(anim);
}

/*
 * Set the board's size.
 *
 * width (in) - new width
 * height (in) - new height
 * depth (in) - new depth
 */
void tagBoard::setSize(const int width, const int height, const int depth)
{
	bSizeX = width;
	bSizeY = height;
	bSizeL = depth;
	//
	board.clear();
	ambientRed.clear();
	ambientBlue.clear();
	ambientGreen.clear();
	tiletype.clear();
	//
	for (unsigned int x = 0; x <= bSizeX; x++)
	{
		board.push_back(VECTOR_SHORT2D());
		VECTOR_SHORT2D &back2d = board.back();
		//
		ambientRed.push_back(VECTOR_SHORT2D());
		VECTOR_SHORT2D &back2dR = ambientRed.back();
		//
		ambientGreen.push_back(VECTOR_SHORT2D());
		VECTOR_SHORT2D &back2dG = ambientGreen.back();
		//
		ambientBlue.push_back(VECTOR_SHORT2D());
		VECTOR_SHORT2D &back2dB = ambientBlue.back();
		//
		tiletype.push_back(VECTOR_CHAR2D());
		VECTOR_CHAR2D &back2dT = tiletype.back();
		for (unsigned int y = 0; y <= bSizeY; y++)
		{
			back2d.push_back(VECTOR_SHORT());
			VECTOR_SHORT &back = back2d.back();
			//
			back2dR.push_back(VECTOR_SHORT());
			VECTOR_SHORT &backR = back2dR.back();
			//
			back2dG.push_back(VECTOR_SHORT());
			VECTOR_SHORT &backG = back2dG.back();
			//
			back2dB.push_back(VECTOR_SHORT());
			VECTOR_SHORT &backB = back2dB.back();
			//
			back2dT.push_back(VECTOR_CHAR());
			VECTOR_CHAR &backT = back2dT.back();
			//
			for (unsigned int l = 0; l <= bSizeL; l++)
			{
				back.push_back(short());
				backR.push_back(short());
				backG.push_back(short());
				backB.push_back(short());
				backT.push_back(char());
			}
		}
	}
}
