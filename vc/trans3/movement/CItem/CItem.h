/*
 * All contents copyright 2005, Jonathan D. Hughes
 * All rights reserved. You may not remove this notice.
 * Read license.txt for licensing details.
 */

#ifndef _CITEM_H_
#define _CITEM_H_

#include "../../../tkCommon/strings.h"
#include "../../common/item.h"
#include "../../rpgcode/CProgram.h"
#include "../CSprite/CSprite.h"

class CInvalidItem { };

class CItemThread;

class CItem : public CSprite
{
public:

	// Empty constructor.
	CItem(): CSprite(false), m_pThread(NULL) {};

	// Default constructor.
	CItem(const STRING file, const bool show, const bool thread = true);

	// Board constructor.
	CItem(const STRING file, const BRD_SPRITE spr, short &version, const bool thread = true);

	// Open the item's file.
	short open(const STRING file, const bool thread = true) throw(CInvalidItem);

	// Extract the ITEM struct.
	LPITEM getItem() { return &m_itemMem; }

	// Get the thread for this item.
	CItemThread *getThread() const { return m_pThread; }

	// Attach a thread to the item.
	void attachThread(CItemThread *pThread);

	~CItem();

private:
	ITEM m_itemMem;
	CItemThread *m_pThread;
};

class CItemThread : public CThread
{
public:
	static CItemThread *create(const STRING str, CItem *pItem);
	bool execute(const unsigned int units);
private:
	CItemThread(const STRING str): CThread(str) { }
	CItem *m_pItem;
};

#endif
