/*
 * All contents copyright 2003, 2004, 2005 Christopher Matthews or Contributors.
 * Various port optimizations copyright by Colin James Fitzpatrick, 2005.
 * All rights reserved. You may not remove this notice
 * Read license.txt for licensing details.
 */

#ifndef _PLAYER_H_
#define _PLAYER_H_

/*
 * Inclusions.
 */
#include "sprite.h"

/*
 * Definitions.
 */
#define PRE_VECTOR_PLAYER	7					// Last version before vectors.		

#define UBOUND_GFX 13
#define UBOUND_STANDING_GFX 7

/*
 * A player.
 */
typedef struct tagPlayer
{
	std::string charname;						// Character name.
	std::string experienceVar;					// Experience variable.
	std::string defenseVar;						// DP variable.
	std::string fightVar;						// FP variable.
	std::string healthVar;						// HP variable.
	std::string maxHealthVar;					// Max HP var.
	std::string nameVar;						// Character name variable.
	std::string smVar;							// Special Move power variable.
	std::string smMaxVar;						// Special Move maximum variable.
	std::string leVar;							// Level variable.
	int experience;								// Initial Experience Level.
	int health;									// Initial health level.
	int maxHealth;								// Initial maximum health level.
	int defense;								// Initial DP.
	int fight;									// Initial FP.
	int sm;										// Initial SM power.
	int smMax;									// Initial Max SM power.
	int level;									// Initial level.
	std::string profilePic;						// Profile picture.
	std::vector<std::string> smlist;			// Special Move list (200 in total!).
	std::vector<int> spcMinExp;					// Minimum experience for each move.
	std::vector<int> spcMinLevel;				// Min level for each move.
	std::vector<std::string> spcVar;			// Conditional variable for each special move.
	std::vector<std::string> spcEquals;			// Condition of variable for each special move.
	std::string specialMoveName;				// Name of special move.
	char smYN;									// Does he do special moves? 0-Y, 1-N.
	std::vector<std::string> accessoryName;		// Names of 10 accessories.
	std::vector<char> armorType;				// Is ARMOURTYPE used (0-N,1-Y).  Armour types are: 1-head,2-neck,3-lh,4-rh,5-body,6-legs.
	int levelType;								// Initial Level progression.
	short experienceIncrease;					// Experience increase Factor.
	int maxLevel;								// Maximum level.
	short levelHp;								// HP incrase by % when level increaes.
	short levelDp;								// DP incrase by % when level increaes.
	short levelFp;								// FP incrase by % when level increaes.
	short levelSm;								// SMP incrase by % when level increaes.
	std::string charLevelUpRPGCode;				// Rpgcode program to run on level up.
	char charLevelUpType;						// Level up type 0- exponential, 1-linear.
	char charSizeType;							// Size type: 0- 32x32, 1 - 64x32.
	// std::vector<FIGHTER_STATUS> status;			// Status effects applied to player.
	short nextLevel;							// Exp value at which level up occurs.
	short levelProgression;						// Exp required until level up.
	std::vector<double> levelStarts;			// Exp values at which all levels start.

	// This member just keeps this structure the correct size.
	// (i.e., there is nothing in it.) It is a very crappy way
	// to do this, but do not remove it unless you like crashes.
	SPRITE_ATTR spriteAttributes;

	short open(const std::string fileName, SPRITE_ATTR &spriteAttr);
} PLAYER;

#endif
