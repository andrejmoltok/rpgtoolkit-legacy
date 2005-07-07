/*
 * All contents copyright 2005, Colin James Fitzpatrick.
 * All rights reserved. You may not remove this notice.
 * Read license.txt for licensing details.
 */

#ifndef _FIGHT_H_
#define _FIGHT_H_

#include "../movement/CPlayer/CPlayer.h"
#include "../common/enemy.h"
#include "../common/status.h"
#include <map>

// Party definitions.
#define ENEMY_PARTY				0
#define PLAYER_PARTY			1

// Things that the plugin can be informed of.
#define INFORM_REMOVE_HP		0		// HP was removed
#define INFORM_REMOVE_SMP		1		// SMP was removed
#define INFORM_SOURCE_ATTACK	2		// Source attacks
#define INFORM_SOURCE_SMP		3		// Source does special move
#define INFORM_SOURCE_ITEM		4		// Source uses item
#define INFORM_SOURCE_CHARGED	5		// Source is charged
#define INFORM_SOURCE_DEAD		6		// Source *fighter* is dead
#define INFORM_SOURCE_DEFEATED	7		// Source *party* is all dead

// Possible fight outcomes.
#define FIGHT_RUN_AUTO			0		// Player party ran - have trans apply the running program for us
#define FIGHT_RUN_MANUAL		1		// Player party ran - tell trans that the plugin has already executed the run prg
#define FIGHT_WON_AUTO			2		// Player party won - have trans apply the rewards for us
#define FIGHT_WON_MANUAL		3		// Player party won - tell trans that the plugin has already given rewards
#define FIGHT_LOST				4		// Player party lost

// A fighter.
typedef struct tagFighter
{
	bool bPlayer;
	union
	{
		CPlayer *pPlayer;
		LPENEMY pEnemy;
	};
	IFighter *pFighter;
	int charge;
	int chargeMax;
	bool bFrozenCharge;
	unsigned int freezes;
	std::map<std::string, STATUS_EFFECT> statuses;
} FIGHTER, *LPFIGHTER;

// A plugin enemy.
typedef struct tagPluginEnemy
{
	ENEMY enemy;
	std::string fileName;
} PLUGIN_ENEMY;

// A party.
typedef std::vector<FIGHTER> VECTOR_FIGHTER;

// Run a fight.
void runFight(const std::vector<std::string> enemies, const std::string background);

// Get a fighter.
LPFIGHTER getFighter(const unsigned int party, const unsigned int idx);

// Advance the state of a fight.
void fightTick(void);

// Cause one fighter to attack another.
int performAttack(const int sourcePartyIdx, const int sourceFightIdx, const int targetPartyIdx, const int targetFightIdx, const int damage, const bool toSmp);

// Start a fight based on skill level.
void skillFight(const int skill, const std::string bkg);

// Test whether we need to begin a fight.
void fightTest(void);

#endif
