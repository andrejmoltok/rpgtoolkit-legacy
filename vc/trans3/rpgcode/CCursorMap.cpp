/*
 * All contents copyright 2005, Colin James Fitzpatrick.
 * All rights reserved. You may not remove this notice.
 * Read license.txt for licensing details.
 */

#include "CCursorMap.h"
#include "../../tkCommon/tkDirectX/platform.h"
#include "../input/input.h"

/*
 * Add a point to the cursor map.
 */
void CCursorMap::add(const int x, const int y)
{
	POINT pt = {x, y};
	m_points.push_back(pt);
}

/*
 * Run the cursor map and return the chose option,
 * with zero as the first index.
 */
int CCursorMap::run()
{
	extern int g_resX, g_resY;
	extern CDirectDraw *g_pDirectDraw;
	extern CCanvas *g_cnvCursor;
	extern void closeSystems();

	CCanvas cnv;
	cnv.CreateBlank(NULL, g_resX, g_resY, TRUE);
	g_pDirectDraw->CopyScreenToCanvas(&cnv);
	int toRet = 0, pos = -1;
	MSG message;

	while (true)
	{
		const DWORD time = GetTickCount();
		while ((GetTickCount() - time) < 100)
		{
			if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
			{
				if (message.message == WM_QUIT)
				{
					closeSystems();
					exit(message.wParam);
				}
				else
				{
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
			}
		}
		BYTE keys[256];
		extern IDirectInputDevice8A *g_lpdiKeyboard;
		if (FAILED(g_lpdiKeyboard->GetDeviceState(256, keys)))
		{
			continue;
		}
		if ((keys[DIK_UP] & 0x80) || (keys[DIK_LEFT] & 0x80))
		{
			if (toRet) --toRet;
			else toRet = m_points.size() - 1;
		}
		else if ((keys[DIK_DOWN] & 0x80) || (keys[DIK_RIGHT] & 0x80))
		{
			if (toRet != (m_points.size() - 1)) ++toRet;
			else toRet = 0;
		}
		else if ((keys[DIK_RETURN] & 0x80) || (keys[DIK_SPACE] & 0x80))
		{
			break;
		}
		if (toRet != pos)
		{
			g_pDirectDraw->DrawCanvas(&cnv, 0, 0);
			g_pDirectDraw->DrawCanvasTransparent(g_cnvCursor, m_points[toRet].x - 40, m_points[toRet].y - 10, RGB(255, 0, 0));
			g_pDirectDraw->Refresh();
		}
		pos = toRet;
	}
	g_pDirectDraw->DrawCanvas(&cnv, 0, 0);
	g_pDirectDraw->Refresh();
	return toRet;
}
