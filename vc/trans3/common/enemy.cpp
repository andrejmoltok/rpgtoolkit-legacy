/*
 * All contents copyright 2003, 2004, 2005 Christopher Matthews or Contributors.
 * Various port optimizations copyright by Colin James Fitzpatrick, 2005.
 * All rights reserved. You may not remove this notice
 * Read license.txt for licensing details.
 */

#include "CFile.h"
#include "paths.h"
#include "../misc/misc.h"
#include "../rpgcode/parser/parser.h"
#include "enemy.h"
#include "tilebitmap.h"
#include "animation.h"

void tagEnemy::open(const std::string strFile)
{
	CFile file(strFile);

	file.seek(14);
	char c;
	file >> c;
	file.seek(0);
	if (!c)
	{
		std::string header;
		file >> header;
		if (header != "RPGTLKIT ENEMY")
		{
			MessageBox(NULL, ("Unrecognised File Format! " + strFile).c_str(), "Open Enemy", 0);
			return;
		}
		short majorVer, minorVer;
		file >> majorVer >> minorVer;
		file >> strName;
		file >> iHp; iMaxHp = iHp;
		file >> iSmp; iMaxSmp = iSmp;
		file >> fp;
		file >> dp;
		file >> run;
		file >> takeCrit;
		file >> giveCrit;
		short count;
		file >> count;
		specials.clear();
		unsigned int i;
		for (i = 0; i <= count; i++)
		{
			std::string str;
			file >> str;
			specials.push_back(str);
		}
		file >> count;
		weaknesses.clear();
		for (i = 0; i <= count; i++)
		{
			std::string str;
			file >> str;
			weaknesses.push_back(str);
		}
		file >> count;
		strengths.clear();
		for (i = 0; i <= count; i++)
		{
			std::string str;
			file >> str;
			strengths.push_back(str);
		}
		file >> ai;
		file >> useCode;
		file >> prg;
		file >> exp;
		file >> gp;
		file >> winPrg;
		file >> runPrg;
		file >> count;
		gfx.clear();
		for (i = 0; i <= count; i++)
		{
			std::string str;
			file >> str;
			gfx.push_back(str);
		}
		file >> count;
		customAnims.clear();
		for (i = 0; i <= count; i++)
		{
			std::string str;
			file >> str;
			file >> customAnims[str];
		}
	}
	else
	{

		if (file.line() != "RPGTLKIT ENEMY")
		{
			MessageBox(NULL, ("Unrecognised File Format! " + strFile).c_str(), "Open Enemy", 0);
			return;
		}

		const short majorVer = atoi(file.line().c_str());
		const short minorVer = atoi(file.line().c_str());

		strName = file.line();
		iMaxHp = iHp = atoi(file.line().c_str());
		iMaxSmp = iSmp = atoi(file.line().c_str());
		fp = atoi(file.line().c_str());
		dp = atoi(file.line().c_str());
		run = atoi(file.line().c_str());
		takeCrit = atoi(file.line().c_str());
		giveCrit = atoi(file.line().c_str());

		const int width = atoi(file.line().c_str());
		const int height = atoi(file.line().c_str());

		TILE_BITMAP tbm;
		tbm.resize(width, height);
		unsigned int i, j;
		for (i = 0; i < width; i++)
		{
			for (j = 0; j < height; j++)
			{
				tbm.tiles[i][j] = file.line();
			}
		}

		extern std::string g_projectPath;

		const std::string tbmFile = replace(removePath(strFile), '.', '_') + "_rest.tbm";
		tbm.save(g_projectPath + BMP_PATH + tbmFile);

		ANIMATION anm;
		anm.animSizeX = width * 32;
		anm.animSizeY = height * 32;
		anm.animPause = 0.167;
		anm.animFrame.push_back(tbmFile);
		anm.animTransp.push_back(RGB(255, 255, 255));
		anm.animSound.push_back("");
		anm.animFrames = 1;
		const std::string anmFile = replace(removePath(strFile), '.', '_') + "_rest.anm";
		anm.save(g_projectPath + MISC_PATH + anmFile);

		gfx.clear();
		gfx.push_back(anmFile);

		specials.clear();
		weaknesses.clear();
		for (i = 0; i < 101; i++)
		{
			specials.push_back(file.line());
			weaknesses.push_back(file.line());
		}

		ai = atoi(file.line().c_str());
		useCode = atoi(file.line().c_str());
		prg = file.line();
		exp = atoi(file.line().c_str());
		gp = atoi(file.line().c_str());
		winPrg = file.line();
		runPrg = file.line();

		file.line();
		file.line();
		file.line();
		file.line();

		gfx.push_back(file.line());
		gfx.push_back(file.line());
		gfx.push_back(file.line());
		gfx.push_back(file.line());

		customAnims.clear();

	}

}

std::string tagEnemy::getStanceAnimation(const std::string anim)
{
	const std::string stance = anim.empty() ? "REST" : parser::uppercase(anim);
	if (stance == "FIGHT" || stance == "ATTACK")
	{
		return gfx[EN_FIGHT];
	}
	else if (stance == "DEFEND")
	{
		return gfx[EN_DEFEND];
	}
	else if (stance == "SPC" || stance == "SPECIAL MOVE")
	{
		return gfx[EN_SPECIAL];
	}
	else if (stance == "DIE")
	{
		return gfx[EN_DIE];
	}
	else if (stance == "REST")
	{
		return gfx[EN_REST];
	}
	if (customAnims.count(stance))
	{
		return customAnims[stance];
	}
	return "";
}
