/*
 * All contents copyright 2005, Colin James Fitzpatrick.
 * All rights reserved. You may not remove this notice.
 * Read license.txt for licensing details.
 */

#ifndef _OLD_CALLBACKS_H_
#define _OLD_CALLBACKS_H_

void __stdcall CBRpgCode(BSTR rpgcodeCommand)
{
	g_pCallbacks->CBRpgCode(rpgcodeCommand);
}

BSTR __stdcall CBGetString(BSTR varname)
{
	BSTR toRet;
	g_pCallbacks->CBGetString(varname, &toRet);
	return toRet;
}

double __stdcall CBGetNumerical(BSTR varname)
{
	double toRet;
	g_pCallbacks->CBGetNumerical(varname, &toRet);
	return toRet;
}

void __stdcall CBSetString(BSTR varname, BSTR newValue)
{
	g_pCallbacks->CBSetString(varname, newValue);
}

void __stdcall CBSetNumerical(BSTR varname, double newValue)
{
	g_pCallbacks->CBSetNumerical(varname, newValue);
}

int __stdcall CBGetScreenDC(void)
{
	int toRet;
	g_pCallbacks->CBGetScreenDC(&toRet);
	return toRet;
}

int __stdcall CBGetScratch1DC(void)
{
	int toRet;
	g_pCallbacks->CBGetScratch1DC(&toRet);
	return toRet;
}

int __stdcall CBGetScratch2DC(void)
{
	int toRet;
	g_pCallbacks->CBGetScratch2DC(&toRet);
	return toRet;
}

int __stdcall CBGetMwinDC(void)
{
	int toRet;
	g_pCallbacks->CBGetMwinDC(&toRet);
	return toRet;
}

int __stdcall CBPopupMwin(void)
{
	int toRet;
	g_pCallbacks->CBPopupMwin(&toRet);
	return toRet;
}

int __stdcall CBHideMwin(void)
{
	int toRet;
	g_pCallbacks->CBHideMwin(&toRet);
	return toRet;
}

void __stdcall CBLoadEnemy(BSTR file, int eneSlot)
{
	g_pCallbacks->CBLoadEnemy(file, eneSlot);
}

int __stdcall CBGetEnemyNum(int infoCode, int eneSlot)
{
	int toRet;
	g_pCallbacks->CBGetEnemyNum(infoCode, eneSlot, &toRet);
	return toRet;
}

BSTR __stdcall CBGetEnemyString(int infoCode, int eneSlot)
{
	BSTR toRet;
	g_pCallbacks->CBGetEnemyString(infoCode, eneSlot, &toRet);
	return toRet;
}

void __stdcall CBSetEnemyNum(int infoCode, int newValue, int eneSlot)
{
	g_pCallbacks->CBSetEnemyNum(infoCode, newValue, eneSlot);
}

void __stdcall CBSetEnemyString(int infoCode, BSTR newValue, int eneSlot)
{
	g_pCallbacks->CBSetEnemyString(infoCode, newValue, eneSlot);
}

int __stdcall CBGetPlayerNum(int infoCode, int arrayPos, int playerSlot)
{
	int toRet;
	g_pCallbacks->CBGetPlayerNum(infoCode, arrayPos, playerSlot, &toRet);
	return toRet;
}

BSTR __stdcall CBGetPlayerString(int infoCode, int arrayPos, int playerSlot)
{
	BSTR toRet;
	g_pCallbacks->CBGetPlayerString(infoCode, arrayPos, playerSlot, &toRet);
	return toRet;
}

void __stdcall CBSetPlayerNum(int infoCode, int arrayPos, int newVal, int playerSlot)
{
	g_pCallbacks->CBSetPlayerNum(infoCode, arrayPos, newVal, playerSlot);
}

void __stdcall CBSetPlayerString(int infoCode, int arrayPos, BSTR newVal, int playerSlot)
{
	g_pCallbacks->CBSetPlayerString(infoCode, arrayPos, newVal, playerSlot);
}

BSTR __stdcall CBGetGeneralString(int infoCode, int arrayPos, int playerSlot)
{
	BSTR toRet;
	g_pCallbacks->CBGetGeneralString(infoCode, arrayPos, playerSlot, &toRet);
	return toRet;
}

int __stdcall CBGetGeneralNum(int infoCode, int arrayPos, int playerSlot)
{
	int toRet;
	g_pCallbacks->CBGetGeneralNum(infoCode, arrayPos, playerSlot, &toRet);
	return toRet;
}

void __stdcall CBSetGeneralString(int infoCode, int arrayPos, int playerSlot, BSTR newVal)
{
	g_pCallbacks->CBSetGeneralString(infoCode, arrayPos, playerSlot, newVal);
}

void __stdcall CBSetGeneralNum(int infoCode, int arrayPos, int playerSlot, int newVal)
{
	g_pCallbacks->CBSetGeneralNum(infoCode, arrayPos, playerSlot, newVal);
}

BSTR __stdcall CBGetCommandName(BSTR rpgcodeCommand)
{
	BSTR toRet;
	g_pCallbacks->CBGetCommandName(rpgcodeCommand, &toRet);
	return toRet;
}

BSTR __stdcall CBGetBrackets(BSTR rpgcodeCommand)
{
	BSTR toRet;
	g_pCallbacks->CBGetBrackets(rpgcodeCommand, &toRet);
	return toRet;
}

int __stdcall CBCountBracketElements(BSTR rpgcodeCommand)
{
	int toRet;
	g_pCallbacks->CBCountBracketElements(rpgcodeCommand, &toRet);
	return toRet;
}

BSTR __stdcall CBGetBracketElement(BSTR rpgcodeCommand, int elemNum)
{
	BSTR toRet;
	g_pCallbacks->CBGetBracketElement(rpgcodeCommand, elemNum, &toRet);
	return toRet;
}

BSTR __stdcall CBGetStringElementValue(BSTR rpgcodeCommand)
{
	BSTR toRet;
	g_pCallbacks->CBGetStringElementValue(rpgcodeCommand, &toRet);
	return toRet;
}

double __stdcall CBGetNumElementValue(BSTR rpgcodeCommand)
{
	double toRet;
	g_pCallbacks->CBGetNumElementValue(rpgcodeCommand, &toRet);
	return toRet;
}

int __stdcall CBGetElementType(BSTR rpgcodeCommand)
{
	int toRet;
	g_pCallbacks->CBGetElementType(rpgcodeCommand, &toRet);
	return toRet;
}

void __stdcall CBDebugMessage(BSTR message)
{
	g_pCallbacks->CBDebugMessage(message);
}

BSTR __stdcall CBGetPathString(int infoCode)
{
	BSTR toRet;
	g_pCallbacks->CBGetPathString(infoCode, &toRet);
	return toRet;
}

void __stdcall CBLoadSpecialMove(BSTR file)
{
	g_pCallbacks->CBLoadSpecialMove(file);
}

BSTR __stdcall CBGetSpecialMoveString(int infoCode)
{
	BSTR toRet;
	g_pCallbacks->CBGetSpecialMoveString(infoCode, &toRet);
	return toRet;
}

int __stdcall CBGetSpecialMoveNum(int infoCode)
{
	int toRet;
	g_pCallbacks->CBGetSpecialMoveNum(infoCode, &toRet);
	return toRet;
}

void __stdcall CBLoadItem(BSTR file, int itmSlot)
{
	g_pCallbacks->CBLoadItem(file, itmSlot);
}

BSTR __stdcall CBGetItemString(int infoCode, int arrayPos, int itmSlot)
{
	BSTR toRet;
	g_pCallbacks->CBGetItemString(infoCode, arrayPos, itmSlot, &toRet);
	return toRet;
}

int __stdcall CBGetItemNum(int infoCode, int arrayPos, int itmSlot)
{
	int toRet;
	g_pCallbacks->CBGetItemNum(infoCode, arrayPos, itmSlot, &toRet);
	return toRet;
}

int __stdcall CBGetBoardNum(int infoCode, int arrayPos1, int arrayPos2, int arrayPos3)
{
	int toRet;
	g_pCallbacks->CBGetBoardNum(infoCode, arrayPos1, arrayPos2, arrayPos3, &toRet);
	return toRet;
}

BSTR __stdcall CBGetBoardString(int infoCode, int arrayPos1, int arrayPos2, int arrayPos3)
{
	BSTR toRet;
	g_pCallbacks->CBGetBoardString(infoCode, arrayPos1, arrayPos2, arrayPos3, &toRet);
	return toRet;
}

void __stdcall CBSetBoardNum(int infoCode, int arrayPos1, int arrayPos2, int arrayPos3, int nValue)
{
	g_pCallbacks->CBSetBoardNum(infoCode, arrayPos1, arrayPos2, arrayPos3, nValue);
}

void __stdcall CBSetBoardString(int infoCode, int arrayPos1, int arrayPos2, int arrayPos3, BSTR newVal)
{
	g_pCallbacks->CBSetBoardString(infoCode, arrayPos1, arrayPos2, arrayPos3, newVal);
}

int __stdcall CBGetHwnd(void)
{
	int toRet;
	g_pCallbacks->CBGetHwnd(&toRet);
	return toRet;
}

int __stdcall CBRefreshScreen(void)
{
	int toRet;
	g_pCallbacks->CBRefreshScreen(&toRet);
	return toRet;
}

int __stdcall CBCreateCanvas(int width, int height)
{
	int toRet;
	g_pCallbacks->CBCreateCanvas(width, height, &toRet);
	return toRet;
}

int __stdcall CBDestroyCanvas(int canvasID)
{
	int toRet;
	g_pCallbacks->CBDestroyCanvas(canvasID, &toRet);
	return toRet;
}

int __stdcall CBDrawCanvas(int canvasID, int x, int y)
{
	int toRet;
	g_pCallbacks->CBDrawCanvas(canvasID, x, y, &toRet);
	return toRet;
}

int __stdcall CBDrawCanvasPartial(int canvasID, int xDest, int yDest, int xsrc, int ysrc, int width, int height)
{
	int toRet;
	g_pCallbacks->CBDrawCanvasPartial(canvasID, xDest, yDest, xsrc, ysrc, width, height, &toRet);
	return toRet;
}

int __stdcall CBDrawCanvasTransparent(int canvasID, int x, int y, int crTransparentColor)
{
	int toRet;
	g_pCallbacks->CBDrawCanvasTransparent(canvasID, x, y, crTransparentColor, &toRet);
	return toRet;
}

int __stdcall CBDrawCanvasTransparentPartial(int canvasID, int xDest, int yDest, int xsrc, int ysrc, int width, int height, int crTransparentColor)
{
	int toRet;
	g_pCallbacks->CBDrawCanvasTransparentPartial(canvasID, xDest, yDest, xsrc, ysrc, width, height, crTransparentColor, &toRet);
	return toRet;
}

int __stdcall CBDrawCanvasTranslucent(int canvasID, int x, int y, double dIntensity, int crUnaffectedColor, int crTransparentColor)
{
	int toRet;
	g_pCallbacks->CBDrawCanvasTranslucent(canvasID, x, y, dIntensity, crUnaffectedColor, crTransparentColor, &toRet);
	return toRet;
}

int __stdcall CBCanvasLoadImage(int canvasID, BSTR filename)
{
	int toRet;
	g_pCallbacks->CBCanvasLoadImage(canvasID, filename, &toRet);
	return toRet;
}

int __stdcall CBCanvasLoadSizedImage(int canvasID, BSTR filename)
{
	int toRet;
	g_pCallbacks->CBCanvasLoadSizedImage(canvasID, filename, &toRet);
	return toRet;
}

int __stdcall CBCanvasFill(int canvasID, int crColor)
{
	int toRet;
	g_pCallbacks->CBCanvasFill(canvasID, crColor, &toRet);
	return toRet;
}

int __stdcall CBCanvasResize(int canvasID, int width, int height)
{
	int toRet;
	g_pCallbacks->CBCanvasResize(canvasID, width, height, &toRet);
	return toRet;
}

int __stdcall CBCanvas2CanvasBlt(int cnvSrc, int cnvDest, int xDest, int yDest)
{
	int toRet;
	g_pCallbacks->CBCanvas2CanvasBlt(cnvSrc, cnvDest, xDest, yDest, &toRet);
	return toRet;
}

int __stdcall CBCanvas2CanvasBltPartial(int cnvSrc, int cnvDest, int xDest, int yDest, int xsrc, int ysrc, int width, int height)
{
	int toRet;
	g_pCallbacks->CBCanvas2CanvasBltPartial(cnvSrc, cnvDest, xDest, yDest, xsrc, ysrc, width, height, &toRet);
	return toRet;
}

int __stdcall CBCanvas2CanvasBltTransparent(int cnvSrc, int cnvDest, int xDest, int yDest, int crTransparentColor)
{
	int toRet;
	g_pCallbacks->CBCanvas2CanvasBltTransparent(cnvSrc, cnvDest, xDest, yDest, crTransparentColor, &toRet);
	return toRet;
}

int __stdcall CBCanvas2CanvasBltTransparentPartial(int cnvSrc, int cnvDest, int xDest, int yDest, int xsrc, int ysrc, int width, int height, int crTransparentColor)
{
	int toRet;
	g_pCallbacks->CBCanvas2CanvasBltTransparentPartial(cnvSrc, cnvDest, xDest, yDest, xsrc, ysrc, width, height, crTransparentColor, &toRet);
	return toRet;
}

int __stdcall CBCanvas2CanvasBltTranslucent(int cnvSrc, int cnvDest, int destX, int destY, double dIntensity, int crUnaffectedColor, int crTransparentColor)
{
	int toRet;
	g_pCallbacks->CBCanvas2CanvasBltTranslucent(cnvSrc, cnvDest, destX, destY, dIntensity, crUnaffectedColor, crTransparentColor, &toRet);
	return toRet;
}

int __stdcall CBCanvasGetScreen(int cnvDest)
{
	int toRet;
	g_pCallbacks->CBCanvasGetScreen(cnvDest, &toRet);
	return toRet;
}

BSTR __stdcall CBLoadString(int id, BSTR defaultString)
{
	BSTR toRet;
	g_pCallbacks->CBLoadString(id, defaultString, &toRet);
	return toRet;
}

int __stdcall CBCanvasDrawText(int canvasID, BSTR Text, BSTR font, int size, double x, double y, int crColor, int isBold, int isItalics, int isUnderline, int isCentred)
{
	int toRet;
	g_pCallbacks->CBCanvasDrawText(canvasID, Text, font, size, x, y, crColor, isBold, isItalics, isUnderline, isCentred, &toRet);
	return toRet;
}

int __stdcall CBCanvasPopup(int canvasID, int x, int y, int stepSize, int popupType)
{
	int toRet;
	g_pCallbacks->CBCanvasPopup(canvasID, x, y, stepSize, popupType, &toRet);
	return toRet;
}

int __stdcall CBCanvasWidth(int canvasID)
{
	int toRet;
	g_pCallbacks->CBCanvasWidth(canvasID, &toRet);
	return toRet;
}

int __stdcall CBCanvasHeight(int canvasID)
{
	int toRet;
	g_pCallbacks->CBCanvasHeight(canvasID, &toRet);
	return toRet;
}

int __stdcall CBCanvasDrawLine(int canvasID, int x1, int y1, int x2, int y2, int crColor)
{
	int toRet;
	g_pCallbacks->CBCanvasDrawLine(canvasID, x1, y1, x2, y2, crColor, &toRet);
	return toRet;
}

int __stdcall CBCanvasDrawRect(int canvasID, int x1, int y1, int x2, int y2, int crColor)
{
	int toRet;
	g_pCallbacks->CBCanvasDrawRect(canvasID, x1, y1, x2, y2, crColor, &toRet);
	return toRet;
}

int __stdcall CBCanvasFillRect(int canvasID, int x1, int y1, int x2, int y2, int crColor)
{
	int toRet;
	g_pCallbacks->CBCanvasFillRect(canvasID, x1, y1, x2, y2, crColor, &toRet);
	return toRet;
}

int __stdcall CBCanvasDrawHand(int canvasID, int pointx, int pointy)
{
	int toRet;
	g_pCallbacks->CBCanvasDrawHand(canvasID, pointx, pointy, &toRet);
	return toRet;
}

int __stdcall CBDrawHand(int pointx, int pointy)
{
	int toRet;
	g_pCallbacks->CBDrawHand(pointx, pointy, &toRet);
	return toRet;
}

int __stdcall CBCheckKey(BSTR keyPressed)
{
	int toRet;
	g_pCallbacks->CBCheckKey(keyPressed, &toRet);
	return toRet;
}

int __stdcall CBPlaySound(BSTR soundFile)
{
	int toRet;
	g_pCallbacks->CBPlaySound(soundFile, &toRet);
	return toRet;
}

int __stdcall CBMessageWindow(BSTR Text, int textColor, int bgColor, BSTR bgPic, int mbtype)
{
	int toRet;
	g_pCallbacks->CBMessageWindow(Text, textColor, bgColor, bgPic, mbtype, &toRet);
	return toRet;
}

BSTR __stdcall CBFileDialog(BSTR initialPath, BSTR fileFilter)
{
	BSTR toRet;
	g_pCallbacks->CBFileDialog(initialPath, fileFilter, &toRet);
	return toRet;
}

int __stdcall CBDetermineSpecialMoves(BSTR playerHandle)
{
	int toRet;
	g_pCallbacks->CBDetermineSpecialMoves(playerHandle, &toRet);
	return toRet;
}

BSTR __stdcall CBGetSpecialMoveListEntry(int idx)
{
	BSTR toRet;
	g_pCallbacks->CBGetSpecialMoveListEntry(idx, &toRet);
	return toRet;
}

void __stdcall CBRunProgram(BSTR prgFile)
{
	g_pCallbacks->CBRunProgram(prgFile);
}

void __stdcall CBSetTarget(int targetIdx, int ttype)
{
	g_pCallbacks->CBSetTarget(targetIdx, ttype);
}

void __stdcall CBSetSource(int sourceIdx, int sType)
{
	g_pCallbacks->CBSetSource(sourceIdx, sType);
}

double __stdcall CBGetPlayerHP(int playerIdx)
{
	double toRet;
	g_pCallbacks->CBGetPlayerHP(playerIdx, &toRet);
	return toRet;
}

double __stdcall CBGetPlayerMaxHP(int playerIdx)
{
	double toRet;
	g_pCallbacks->CBGetPlayerMaxHP(playerIdx, &toRet);
	return toRet;
}

double __stdcall CBGetPlayerSMP(int playerIdx)
{
	double toRet;
	g_pCallbacks->CBGetPlayerSMP(playerIdx, &toRet);
	return toRet;
}

double __stdcall CBGetPlayerMaxSMP(int playerIdx)
{
	double toRet;
	g_pCallbacks->CBGetPlayerMaxSMP(playerIdx, &toRet);
	return toRet;
}

double __stdcall CBGetPlayerFP(int playerIdx)
{
	double toRet;
	g_pCallbacks->CBGetPlayerFP(playerIdx, &toRet);
	return toRet;
}

double __stdcall CBGetPlayerDP(int playerIdx)
{
	double toRet;
	g_pCallbacks->CBGetPlayerDP(playerIdx, &toRet);
	return toRet;
}

BSTR __stdcall CBGetPlayerName(int playerIdx)
{
	BSTR toRet;
	g_pCallbacks->CBGetPlayerName(playerIdx, &toRet);
	return toRet;
}

void __stdcall CBAddPlayerHP(int amount, int playerIdx)
{
	g_pCallbacks->CBAddPlayerHP(amount, playerIdx);
}

void __stdcall CBAddPlayerSMP(int amount, int playerIdx)
{
	g_pCallbacks->CBAddPlayerSMP(amount, playerIdx);
}

void __stdcall CBSetPlayerHP(int amount, int playerIdx)
{
	g_pCallbacks->CBSetPlayerHP(amount, playerIdx);
}

void __stdcall CBSetPlayerSMP(int amount, int playerIdx)
{
	g_pCallbacks->CBSetPlayerSMP(amount, playerIdx);
}

void __stdcall CBSetPlayerFP(int amount, int playerIdx)
{
	g_pCallbacks->CBSetPlayerFP(amount, playerIdx);
}

void __stdcall CBSetPlayerDP(int amount, int playerIdx)
{
	g_pCallbacks->CBSetPlayerDP(amount, playerIdx);
}

int __stdcall CBGetEnemyHP(int eneIdx)
{
	int toRet;
	g_pCallbacks->CBGetEnemyHP(eneIdx, &toRet);
	return toRet;
}

int __stdcall CBGetEnemyMaxHP(int eneIdx)
{
	int toRet;
	g_pCallbacks->CBGetEnemyMaxHP(eneIdx, &toRet);
	return toRet;
}

int __stdcall CBGetEnemySMP(int eneIdx)
{
	int toRet;
	g_pCallbacks->CBGetEnemySMP(eneIdx, &toRet);
	return toRet;
}

int __stdcall CBGetEnemyMaxSMP(int eneIdx)
{
	int toRet;
	g_pCallbacks->CBGetEnemyMaxSMP(eneIdx, &toRet);
	return toRet;
}

int __stdcall CBGetEnemyFP(int eneIdx)
{
	int toRet;
	g_pCallbacks->CBGetEnemyFP(eneIdx, &toRet);
	return toRet;
}

int __stdcall CBGetEnemyDP(int eneIdx)
{
	int toRet;
	g_pCallbacks->CBGetEnemyDP(eneIdx, &toRet);
	return toRet;
}

void __stdcall CBAddEnemyHP(int amount, int eneIdx)
{
	g_pCallbacks->CBAddEnemyHP(amount, eneIdx);
}

void __stdcall CBAddEnemySMP(int amount, int eneIdx)
{
	g_pCallbacks->CBAddEnemySMP(amount, eneIdx);
}

void __stdcall CBSetEnemyHP(int amount, int eneIdx)
{
	g_pCallbacks->CBSetEnemyHP(amount, eneIdx);
}

void __stdcall CBSetEnemySMP(int amount, int eneIdx)
{
	g_pCallbacks->CBSetEnemySMP(amount, eneIdx);
}

void __stdcall CBCanvasDrawBackground(int canvasID, BSTR bkgFile, int x, int y, int width, int height)
{
	g_pCallbacks->CBCanvasDrawBackground(canvasID, bkgFile, x, y, width, height);
}

int __stdcall CBCreateAnimation(BSTR file)
{
	int toRet;
	g_pCallbacks->CBCreateAnimation(file, &toRet);
	return toRet;
}

void __stdcall CBDestroyAnimation(int idx)
{
	g_pCallbacks->CBDestroyAnimation(idx);
}

void __stdcall CBCanvasDrawAnimation(int canvasID, int idx, int x, int y, int forceDraw, int forceTransp)
{
	g_pCallbacks->CBCanvasDrawAnimation(canvasID, idx, x, y, forceDraw, forceTransp);
}

void __stdcall CBCanvasDrawAnimationFrame(int canvasID, int idx, int frame, int x, int y, int forceTranspFill)
{
	g_pCallbacks->CBCanvasDrawAnimationFrame(canvasID, idx, frame, x, y, forceTranspFill);
}

int __stdcall CBAnimationCurrentFrame(int idx)
{
	int toRet;
	g_pCallbacks->CBAnimationCurrentFrame(idx, &toRet);
	return toRet;
}

int __stdcall CBAnimationMaxFrames(int idx)
{
	int toRet;
	g_pCallbacks->CBAnimationMaxFrames(idx, &toRet);
	return toRet;
}

int __stdcall CBAnimationSizeX(int idx)
{
	int toRet;
	g_pCallbacks->CBAnimationSizeX(idx, &toRet);
	return toRet;
}

int __stdcall CBAnimationSizeY(int idx)
{
	int toRet;
	g_pCallbacks->CBAnimationSizeY(idx, &toRet);
	return toRet;
}

BSTR __stdcall CBAnimationFrameImage(int idx, int frame)
{
	BSTR toRet;
	g_pCallbacks->CBAnimationFrameImage(idx, frame, &toRet);
	return toRet;
}

int __stdcall CBGetPartySize(int partyIdx)
{
	int toRet;
	g_pCallbacks->CBGetPartySize(partyIdx, &toRet);
	return toRet;
}

int __stdcall CBGetFighterHP(int partyIdx, int fighterIdx)
{
	int toRet;
	g_pCallbacks->CBGetFighterHP(partyIdx, fighterIdx, &toRet);
	return toRet;
}

int __stdcall CBGetFighterMaxHP(int partyIdx, int fighterIdx)
{
	int toRet;
	g_pCallbacks->CBGetFighterMaxHP(partyIdx, fighterIdx, &toRet);
	return toRet;
}

int __stdcall CBGetFighterSMP(int partyIdx, int fighterIdx)
{
	int toRet;
	g_pCallbacks->CBGetFighterSMP(partyIdx, fighterIdx, &toRet);
	return toRet;
}

int __stdcall CBGetFighterMaxSMP(int partyIdx, int fighterIdx)
{
	int toRet;
	g_pCallbacks->CBGetFighterMaxSMP(partyIdx, fighterIdx, &toRet);
	return toRet;
}

int __stdcall CBGetFighterFP(int partyIdx, int fighterIdx)
{
	int toRet;
	g_pCallbacks->CBGetFighterFP(partyIdx, fighterIdx, &toRet);
	return toRet;
}

int __stdcall CBGetFighterDP(int partyIdx, int fighterIdx)
{
	int toRet;
	g_pCallbacks->CBGetFighterDP(partyIdx, fighterIdx, &toRet);
	return toRet;
}

BSTR __stdcall CBGetFighterName(int partyIdx, int fighterIdx)
{
	BSTR toRet;
	g_pCallbacks->CBGetFighterName(partyIdx, fighterIdx, &toRet);
	return toRet;
}

BSTR __stdcall CBGetFighterAnimation(int partyIdx, int fighterIdx, BSTR animationName)
{
	BSTR toRet;
	g_pCallbacks->CBGetFighterAnimation(partyIdx, fighterIdx, animationName, &toRet);
	return toRet;
}

int __stdcall CBGetFighterChargePercent(int partyIdx, int fighterIdx)
{
	int toRet;
	g_pCallbacks->CBGetFighterChargePercent(partyIdx, fighterIdx, &toRet);
	return toRet;
}

void __stdcall CBFightTick(void)
{
	g_pCallbacks->CBFightTick();
}

int __stdcall CBDrawTextAbsolute(BSTR text, BSTR font, int size, int x, int y, int crColor, int isBold, int isItalics, int isUnderline, int isCentred)
{
	int toRet;
	g_pCallbacks->CBDrawTextAbsolute(text, font, size, x, y, crColor, isBold, isItalics, isUnderline, isCentred, &toRet);
	return toRet;
}

void __stdcall CBReleaseFighterCharge(int partyIdx, int fighterIdx)
{
	g_pCallbacks->CBReleaseFighterCharge(partyIdx, fighterIdx);
}

int __stdcall CBFightDoAttack(int sourcePartyIdx, int sourceFightIdx, int targetPartyIdx, int targetFightIdx, int amount, int toSMP)
{
	int toRet;
	g_pCallbacks->CBFightDoAttack(sourcePartyIdx, sourceFightIdx, targetPartyIdx, targetFightIdx, amount, toSMP, &toRet);
	return toRet;
}

void __stdcall CBFightUseItem(int sourcePartyIdx, int sourceFightIdx, int targetPartyIdx, int targetFightIdx, BSTR itemFile)
{
	g_pCallbacks->CBFightUseItem(sourcePartyIdx, sourceFightIdx, targetPartyIdx, targetFightIdx, itemFile);
}

void __stdcall CBFightUseSpecialMove(int sourcePartyIdx, int sourceFightIdx, int targetPartyIdx, int targetFightIdx, BSTR moveFile)
{
	g_pCallbacks->CBFightUseSpecialMove(sourcePartyIdx, sourceFightIdx, targetPartyIdx, targetFightIdx, moveFile);
}

void __stdcall CBDoEvents(void)
{
	g_pCallbacks->CBDoEvents();
}

void __stdcall CBFighterAddStatusEffect(int partyIdx, int fightIdx, BSTR statusFile)
{
	g_pCallbacks->CBFighterAddStatusEffect(partyIdx, fightIdx, statusFile);
}

void __stdcall CBFighterRemoveStatusEffect(int partyIdx, int fightIdx, BSTR statusFile)
{
	g_pCallbacks->CBFighterRemoveStatusEffect(partyIdx, fightIdx, statusFile);
}

#endif
