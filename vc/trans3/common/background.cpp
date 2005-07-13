/*
 * All contents copyright 2003, 2004, 2005 Christopher Matthews or Contributors.
 * Various port optimizations copyright by Colin James Fitzpatrick, 2005.
 * All rights reserved. You may not remove this notice
 * Read license.txt for licensing details.
 */

/*
 * Inclusions.
 */
#include "background.h"
#include "tilebitmap.h"
#include "paths.h"
#include "CFile.h"
#include "mbox.h"

/*
 * Open a background.
 *
 * fileName (in) - file to open
 */
void tagBackground::open(const std::string fileName)
{

	CFile file(fileName);

	char cUnused;
	file.seek(12);
	file >> cUnused;
	file.seek(0);
	if (!cUnused)
	{

		std::string fileHeader;
		file >> fileHeader;
		if (fileHeader != "RPGTLKIT BKG")
		{
			messageBox("Unrecognised File Format! " + fileName);
			return;
		}

		short majorVer, minorVer;
		file >> majorVer;
		file >> minorVer;

		file >> image;
		file >> bkgMusic;
		file >> bkgSelWav;
		file >> bkgChooseWav;
		file >> bkgReadyWav;
		file >> bkgCantDoWav;

	}
	else
	{

		if (file.line() != "RPGTLKIT BKG")
		{
			messageBox("Unrecognised File Format! " + fileName);
			return;
		}

		const short majorVer = atoi(file.line().c_str());
		const short minorVer = atoi(file.line().c_str());

		TILE_BITMAP tbm;
		tbm.width = 19;
		tbm.height = 19;

		unsigned int i, j;
		for (i = 0; i < 19; i++)
		{
			tbm.tiles.push_back(std::vector<std::string>());
			for (j = 0; j < 19; j++)
			{
				tbm.tiles.back().push_back(file.line());
			}
		}
 
		bkgMusic = file.line();
		bkgSelWav = file.line();
		bkgChooseWav = file.line();
		bkgReadyWav = file.line();
		bkgCantDoWav = file.line();

		file.line();

		for (i = 0; i < 19; i++)
		{
			tbm.red.push_back(std::vector<short>());
			tbm.green.push_back(std::vector<short>());
			tbm.blue.push_back(std::vector<short>());
			for (j = 0; j < 19; j++)
			{
				tbm.red.back().push_back(atoi(file.line().c_str()));
				tbm.green.back().push_back(atoi(file.line().c_str()));
				tbm.blue.back().push_back(atoi(file.line().c_str()));
			}
		}

		std::string noPath = removePath(fileName);
		const int len = noPath.length();
		for (i = 0; i < len; i++) if (noPath[i] == '.') noPath[i] = '_';
		const std::string tbmFile = noPath + ".tbm";
		image = tbmFile;
		extern std::string g_projectPath;
		tbm.save(g_projectPath + BMP_PATH + tbmFile);

	}

}
