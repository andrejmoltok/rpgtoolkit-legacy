/*
 * All contents copyright 2005, Colin James Fitzpatrick.
 * All rights reserved. You may not remove this notice.
 * Read license.txt for licensing details.
 */

/*
 * Inclusions.
 */
#include "misc.h"
#include "../common/player.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>

/*
 * Definitions.
 */
#define KEY_TRANS3 "Software\\VB and VBA Program Settings\\RPGToolkit3\\Trans3"

/*
 * Load a character.
 *
 * file (in) - file to load
 * slot (in) - slot to load into
 */
void createCharacter(const std::string file, const int slot)
{
	extern std::vector<PLAYER> g_playerMem;
	g_playerMem[slot].open(file);
	/* ... set rpgcode variables ... */
}

/*
 * Replace within a string.
 *
 * str (in) - string in question
 * find (in) - find this
 * replace (in) - replace with this
 * return (out) - result
 */
std::string replace(const std::string str, const char find, const char replace)
{
	std::string toRet = str;
	const int len = str.length();
	for (unsigned int i = 0; i < len; i++)
	{
		if (toRet[i] == find) toRet[i] = replace;
	}
	return toRet;
}

/*
 * Save a setting.
 *
 * strKey (in) - name of key to save to
 * dblValue (in) - value to save
 */
void saveSetting(const std::string strKey, const double dblValue)
{
	HKEY hKey;
	RegCreateKey(HKEY_CURRENT_USER, KEY_TRANS3, &hKey);
	RegSetValueEx(
		hKey,
		strKey.c_str(),
		0,
		REG_BINARY,
		LPBYTE(const_cast<double *>(&dblValue)),
		sizeof(dblValue)
	);
	RegCloseKey(hKey);
}

/*
 * Get a setting.
 *
 * strKey (in) - name of key to get
 * dblValue (out) - result
 */
void getSetting(const std::string strKey, double &dblValue)
{
	HKEY hKey;
	RegCreateKey(HKEY_CURRENT_USER, KEY_TRANS3, &hKey);
	DWORD dwSize = sizeof(dblValue);
	RegQueryValueEx(hKey, strKey.c_str(), 0, NULL, LPBYTE(&dblValue), &dwSize);
	RegCloseKey(hKey);
}
