Attribute VB_Name = "transMovement"
'=====================================================
'All contents copyright 2003, 2004, Christopher Matthews or Contributors
'All rights reserved.  YOU MAY NOT REMOVE THIS NOTICE.
'Read LICENSE.txt for licensing info
'=====================================================
'Movement info for players and items

Option Explicit

'Movement constants for pending movements
Public Const MV_IDLE = 0
Public Const MV_NORTH = 1
Public Const MV_SOUTH = 2
Public Const MV_EAST = 3
Public Const MV_WEST = 4
Public Const MV_NE = 5
Public Const MV_NW = 6
Public Const MV_SE = 7
Public Const MV_SW = 8

'General player movement code
Public Const LINK_NORTH = 1
Public Const LINK_SOUTH = 2
Public Const LINK_EAST = 3
Public Const LINK_WEST = 4
Public Const LINK_NE = 5
Public Const LINK_SE = 6
Public Const LINK_NW = 7
Public Const LINK_SW = 8

'====================================
'Tile type constants. Added by Delano
'Note: Stairs in the form "stairs & layer number"; i.e. layer = stairs - 10
'====================================
Public Const NORMAL = 0
Public Const SOLID = 1
Public Const UNDER = 2
Public Const NORTH_SOUTH = 3
Public Const EAST_WEST = 4
Public Const STAIRS1 = 11
Public Const STAIRS2 = 12
Public Const STAIRS3 = 13
Public Const STAIRS4 = 14
Public Const STAIRS5 = 15
Public Const STAIRS6 = 16
Public Const STAIRS7 = 17
Public Const STAIRS8 = 18

Public Type PLAYER_POSITION
    stance As String    'current stance
    frame As Long       'animation frame
    x As Double         'current board x positon
    y As Double         'y pos
    l As Long
End Type

Public ppos(4) As PLAYER_POSITION       'player positions of 5 players
Public selectedPlayer As Long           'number of player graphic

Public Type PENDING_MOVEMENT
    direction As Long       'MV_ direction code
    xOrig As Double         'original board co-ordinates
    yOrig As Double
    lOrig As Double

    xTarg As Double         'target board co-ordinates
    yTarg As Double
    lTarg As Double
End Type

Public pendingPlayerMovement(4) As PENDING_MOVEMENT   'pending player movements

Public pendingItemMovement() As PENDING_MOVEMENT 'pending movements for the items

Public facing As Long           'which direction are you facing? 1-s, 2-w, 3-n, 4-e

Private movedThisFrame As Double
Private mVarAnimationDelay As Double

Public itmPos() As PLAYER_POSITION    'positions of items on board

Public Property Get animationDelay() As Double
    animationDelay = mVarAnimationDelay
    If animationDelay = 0 Then animationDelay = 1
End Property

Public Property Let animationDelay(ByVal newVal As Double)
    mVarAnimationDelay = newVal
End Property

Public Property Get framesPerMove() As Double
    framesPerMove = 4 / slackTime
End Property

Public Function onlyDecimal(ByVal number As Double) As Double
    '==============================================
    'Returns the part of a number after the decimal
    '==============================================
    'Added by KSNiloc

    onlyDecimal = number - Int(number)

End Function

Public Function decimalToSteps(ByVal dec As Double) As Double
    '===============================================
    'Converts a decimal to the number of steps taken
    '===============================================
    'Added by KSNiloc

    decimalToSteps = dec / movementSize
    If decimalToSteps = 0 Then decimalToSteps = 1
    
End Function


Function checkAbove(ByVal x As Long, ByVal y As Long, ByVal layer As Long) As Long
    'Checks if there are tiles on any layer above x,y,layer
    '0- no, 1-yes
    On Error GoTo errorhandler
    
    If layer = boardList(activeBoardIndex).theData.Bsizel Then checkAbove = 0: Exit Function
    Dim lay As Long
    Dim uptile As String
    For lay = layer + 1 To boardList(activeBoardIndex).theData.Bsizel
        uptile$ = BoardGetTile(x, y, lay, boardList(activeBoardIndex).theData)
        If uptile$ <> "" Then checkAbove = 1: Exit Function
    Next lay
    checkAbove = 0

    Exit Function

'Begin error handling code:
errorhandler:
    Call HandleError
    Resume Next
End Function

Function CheckObstruction(ByVal x As Double, ByVal y As Double, ByVal l As Long) As Long
    '====================================
    'Checks if an item is blocking x,y,l
    'returns 0 for no, 1 for yes.
    '====================================
    'Called by EffectiveTileType, PushItem*, and ObtainTileType
    
    On Error GoTo errorhandler
    Dim atx As Long
    Dim aty As Long
    Dim atl As Long
    Dim returnVal As Long
    Dim xx As Long
    Dim runIt As Long
    Dim checkIt As Long
    Dim valueTest As Double
    Dim num As Double
    Dim lit As String
    Dim valueTes As String
    
    x = Int(x)
    y = Int(y)
    
    atx = 0: aty = 0: atl = 0
    returnVal = 0
    For xx = 0 To UBound(itmPos)
        If itmPos(xx).x = x And _
           itmPos(xx).y = y And _
           itmPos(xx).l = l Then
            'there's an item here, but is it active?
            If boardList(activeBoardIndex).theData.itmActivate(xx) = 1 Then
                'conditional activation
                runIt = 0
                checkIt = GetIndependentVariable(boardList(activeBoardIndex).theData.itmVarActivate$(xx), lit$, num)
                If checkIt = 0 Then
                    'it's a numerical variable
                    valueTest = num
                    If valueTest = val(boardList(activeBoardIndex).theData.itmActivateInitNum$(xx)) Then runIt = 1
                End If
                If checkIt = 1 Then
                    'it's a literal variable
                    valueTes$ = lit$
                    If valueTes$ = boardList(activeBoardIndex).theData.itmActivateInitNum$(xx) Then runIt = 1
                End If
                
                If runIt = 1 Then
                    'it's active!
                    atx = 1
                Else
                    'it's not active
                    atx = 0
                End If
            Else
                atx = 1
            End If
        End If
    Next xx
    'For yy = 0 To 10
    '    If itmy(yy) = y Then aty = 1
    'Next yy
    'For ll = 0 To 10
    '    If itmlayer(ll) = l Then atl = 1
    'Next ll
    'If atx = 1 And aty = 1 And atl = 1 Then returnval = 1
    If atx = 1 Then returnVal = 1
    CheckObstruction = returnVal

    Exit Function

'Begin error handling code:
errorhandler:
    Call HandleError
    Resume Next
End Function



Function PathFind(ByVal x1 As Integer, ByVal y1 As Integer, ByVal x2 As Integer, ByVal y2 As Integer, ByVal layer As Integer, ByVal bAllowDiagonal As Boolean, ByVal bFaster As Boolean) As String
    '============================================
    'EDITED: [Delano - 8/05/04]
    'Added commas to the letters as they are added to the return string, for compatibility with
    'the new syntax of the #Push/#PushItem commands.
    'Doesn't work very well on isometric boards and cannot yet generate diagonal paths.
    'Needs a bit more work!
    '============================================
    'Called by PlayerStepRPG, ItemStepRPG, PathFindRPG
    'Possibly not the most efficient routine to use for itemStep or PlayerStep?
    
    'find a path from x1,y1, layer to x2,y2,layer
    'return a string of directions if found, else nothing if no path found.
    
    Dim bChanged As Boolean
    
    Dim sx As Integer, sy As Integer
    Dim iX As Integer, iY As Integer
    
    Dim bestX As Integer, bestY As Integer
    Dim BestScore As Integer
    
    ReDim Score(boardList(activeBoardIndex).theData.Bsizex, boardList(activeBoardIndex).theData.Bsizey) As Integer
    
    'Initialise target square
    Score(x2, y2) = 1
    
    'Note - We are defining the score 0 as being
    'untravellable even though in the tutorial
    'it is the target. This is just so we don't need
    'to have another array
    
    Do
        DoEvents
        bChanged = False
        For sx = 1 To boardList(activeBoardIndex).theData.Bsizex
            For sy = 1 To boardList(activeBoardIndex).theData.Bsizey
                If Score(sx, sy) <> 0 Then
                    'This square has been travelled to
                    'Check to see if we can go one step
                    'further
                    For iX = sx - 1 To sx + 1
                        For iY = sy - 1 To sy + 1
                            'Cull some squares (those not on map
                            'or those that are already moved to)
                            If iX <> sx And iY <> sy And Not (bAllowDiagonal) Then
                                'ignore diagonals.
                            Else
                                If Not (iX = sx And iY = sy) Then
                                    If iX >= 0 And iX <= boardList(activeBoardIndex).theData.Bsizex Then
                                        If iY >= 0 And iY <= boardList(activeBoardIndex).theData.Bsizey Then
                                            'Now we check to see if
                                            'we can move a bit further
                                            'from (sX, sY) to (iX, iY)
                                            If EffectiveTileType(iX, iY, layer, bFaster) <> 1 Then
                                            'If boardlist(activeboardindex).thedata.tiletype(iX, iY, layer) <> 1 Then
                                                'It is walkable
                                                'If we are to move there
                                                'will the new score be an improvement?
                                                If Score(sx, sy) + 1 < Score(iX, iY) Or Score(iX, iY) = 0 Then
                                                    'Yes, so we'll put that score in and
                                                    'set bChanged to True
                                                    Score(iX, iY) = Score(sx, sy) + 1
                                                    bChanged = True
                                                End If
                                            End If
                                        End If
                                    End If
                                End If
                            End If
                        Next
                    Next
                End If
            Next
        Next
    Loop Until bChanged = False
    
    '************************************************
    'We now have a map of the distance from the Target
    'stored, now we just have to find a way through it
    'To do this, we can start at the first point
    'and work our way around, always moving to the
    'point with the closest distance.
    
    Dim toRet As String
    Dim lastX As Double
    Dim lastY As Double
    If Score(x1, y1) <> 0 Then
        'We found a path
                
        toRet$ = ""
            
        
        sx = x1
        sy = y1
        lastX = x1
        lastY = y1
        Do
            bestX = -1
            bestY = -1
            BestScore = 32767 'That's a big number, I hope that there aren't over 32767
            
            For iX = sx - 1 To sx + 1
                For iY = sy - 1 To sy + 1
                    'Cull some squares (those not on map
                    'or those that are already moved to)
                    If Not (iX = sx And iY = sy) Then
                        If iX >= 0 And iX <= boardList(activeBoardIndex).theData.Bsizex Then
                            If iY >= 0 And iY <= boardList(activeBoardIndex).theData.Bsizey Then
                                
                                If Score(iX, iY) < BestScore And Score(iX, iY) <> 0 Then
                                    BestScore = Score(iX, iY)
                                    bestX = iX
                                    bestY = iY
                                End If
                            
                            
                            End If
                        End If
                    End If
                Next
            Next
            
            If bestX = -1 Then
                'If somehow the next point is not
                'found then throw an error.
                
                'Lucky 19023 =)
                Error 19023
                PathFind = ""
                Exit Function
            End If
            
            If bestY <> lastY And bestX <> lastX Then
                'it's diagonal-- determine how the
                'diagonal will be navigated.
                If bestY > lastY And bestX > lastX Then
                    'SE
                    If EffectiveTileType(lastX, bestY, layer, bFaster) <> 1 Then
                    'If boardlist(activeboardindex).thedata.tiletype(lastX, bestY, layer) <> 1 Then
                        
                        'Edit!: Added commas for compatibility with the #Push/#PushItem commands.
                        toRet$ = toRet$ + "S,E,"
                    Else
                        toRet$ = toRet$ + "E,S,"
                    End If
                End If
                If bestY > lastY And bestX < lastX Then
                    'SW
                    If EffectiveTileType(lastX, bestY, layer, bFaster) <> 1 Then
                    'If boardlist(activeboardindex).thedata.tiletype(lastX, bestY, layer) <> 1 Then
                        toRet$ = toRet$ + "S,W,"
                    Else
                        toRet$ = toRet$ + "W,S,"
                    End If
                End If
                If bestY < lastY And bestX > lastX Then
                    'NE
                    If EffectiveTileType(lastX, bestY, layer, bFaster) <> 1 Then
                    'If boardlist(activeboardindex).thedata.tiletype(lastX, bestY, layer) <> 1 Then
                        toRet$ = toRet$ + "N,E,"
                    Else
                        toRet$ = toRet$ + "E,N,"
                    End If
                End If
                If bestY < lastY And bestX < lastX Then
                    'NW
                    If EffectiveTileType(lastX, bestY, layer, bFaster) <> 1 Then
                    'If boardlist(activeboardindex).thedata.tiletype(lastX, bestY, layer) <> 1 Then
                        toRet$ = toRet$ + "N,W,"
                    Else
                        toRet$ = toRet$ + "W,N,"
                    End If
                End If
            Else
                If bestY > lastY Then
                    toRet$ = toRet$ + "S,"
                Else
                    If bestY <> lastY Then
                        toRet$ = toRet$ + "N,"
                    End If
                End If
                
                If bestX > lastX Then
                    toRet$ = toRet$ + "E,"
                Else
                    If bestX <> lastX Then
                        toRet$ = toRet$ + "W,"
                    End If
                End If
            End If
            
            lastX = bestX
            lastY = bestY
            
            If bestX = x2 And bestY = y2 Then Exit Do
            
            sx = bestX
            sy = bestY
            
            DoEvents
        Loop
        
        'Edit: we now have a string with an extra comma on the end, so:
        toRet$ = Left$(toRet$, Len(toRet$) - 1)
       
        'Hopefully everything is done. Yay
        PathFind = toRet$

        
    Else
        'We didn't
        PathFind = ""
    End If
End Function


Function EffectiveTileType(ByVal x As Integer, ByVal y As Integer, ByVal l As Integer, ByVal bFast As Boolean) As Integer
    '===============================
    'return the effective tile type, checking for obstructions.
    '===============================
    'Called by PathFind only.
    On Error Resume Next
    
    If bFast Then
        EffectiveTileType = boardList(activeBoardIndex).theData.tiletype(x, y, l)
        Exit Function
    End If
    
    Dim testX As Long
    Dim testY As Long
    Dim testLayer As Long
    testX = x
    testY = y
    testLayer = l
       
    Dim typetile As Long
    typetile = boardList(activeBoardIndex).theData.tiletype(testX, testY, testLayer)
    
    'check if an item is blocking...
    Dim itemBlocking As Long
    itemBlocking = CheckObstruction(testX, testY, testLayer)
    
    Dim didItem As Boolean
    didItem = False
    If itemBlocking = 1 Then
        'Call programtest(testx, testy, testlayer, keycode, facing)
        didItem = True
        typetile = SOLID
        'Exit Sub
    End If
    
    
    'check for tiles above...
    Dim underneath As Long
    underneath = checkAbove(testX, testY, testLayer)
    
    'if we're sitting on stairs, forget about tiles above.
    If typetile >= STAIRS1 And typetile <= STAIRS8 Then
        typetile = NORMAL
        underneath = 0
    End If
    
    If underneath = 1 And typetile <> SOLID Then
        typetile = UNDER
    End If
    
    EffectiveTileType = typetile
End Function



Function TestLink(ByVal playerNum As Long, ByVal thelink As Long) As Boolean
    '=====================================
    'EDITED: [Isometrics - Delano 3/05/04]
    'Renamed variables: tx,ty >> topXTemp,topYTemp [Altered type also; Long >> Double]
    '                   xx,yy >> targetX,targetY
    '                   canwego >> targetTile
    'New variable:      targetBoard$ = boardList(activeBoardIndex).theData.dirLink$(thelink)
    '=====================================
    
    'If player walks off the edge, checks to see if a link is present and if it's
    'possible to go there. If so, then player is sent, and True returned, else False.
    'thelink is a number from 1-4  1-North, 2-South, 3-East, 4-West.
    'Code also present to check and run a program instead of a board.
    'Called by CheckEdges only.
    On Error Resume Next
    
    'Screen co-ords held in temporary varibles in case true variables altered.
    Dim topXtemp As Double 'Isometric fix. Were Longs, but topX could be decimal.
    Dim topYtemp As Double 'Not passed to any functions, so should be ok.
    topXtemp = topX
    topYtemp = topY
        
    Dim targetBoard As String
    targetBoard$ = boardList(activeBoardIndex).theData.dirLink$(thelink)
        
    If targetBoard$ = "" Then
        'no link exists...
        TestLink = False
        Exit Function
    End If
    
    'If link present, check to see if it's a program, and run if so, then exit.
    Dim ex As String
    ex$ = GetExt(targetBoard$)
    If UCase$(ex$) = "PRG" Then
        Call runProgram(projectPath$ + prgPath$ + targetBoard$)
        TestLink = True
        Exit Function
    End If
    
    Dim testX As Long
    Dim testY As Long
    Dim testLayer As Long
    testX = ppos(playerNum).x
    testY = ppos(playerNum).y
    testLayer = ppos(playerNum).l
    
    'Isometric addition: sprites jump when moving to new boards.
    'Y has to remain even or odd during transition, rather than just moving to the bottom row.
    'New function: linkIso, to check if the target board is iso. If so, sends to different co-ords.
    
    Dim targetX As Long 'Target board dimensions
    Dim targetY As Long
    
    If thelink = LINK_NORTH Then
        'Get dimensions of target board.
        Call boardSize(projectPath$ + brdPath$ + targetBoard$, targetX, targetY)

        testY = targetY 'The bottom row of the board
        
        'Only notice if you move from iso to normal boards
        'Trial with new function. If bad then use boardIso()
        If linkIso(projectPath$ + brdPath$ + targetBoard$) Then
            If ppos(playerNum).y Mod 2 <> targetY Mod 2 Then
                testY = testY - 1
            End If
        End If
        
    End If
    If thelink = LINK_SOUTH Then
        
        testY = 1
        
        'Trial with new function. If bad then use boardIso()
        If linkIso(projectPath$ + brdPath$ + targetBoard$) Then
            
            testY = 3 'This fixes sprites starting off top of screen also!
            
            If ppos(playerNum).y Mod 2 = 0 Then
                testY = testY - 1
            End If
        End If
        
    End If
    If thelink = LINK_EAST Then
    
        testX = 1
    
    End If
    If thelink = LINK_WEST Then
    
        'Get the dimensions of the target board.
        Call boardSize(projectPath$ + brdPath$ + targetBoard$, targetX, targetY)
        testX = targetX
        
    End If
    
        
    'now see if the space is ok...
    Dim targetTile As Long
    targetTile = TestBoard(projectPath$ + brdPath$ + targetBoard$, testX, testY, testLayer)
    
    If targetTile = -1 Or targetTile = SOLID Then
        'If board doesn't exist or board smaller than target location (-1) OR target tile is solid.
        'Stay at current position.
        topX = topXtemp
        topY = topYtemp
        
        TestLink = False
        Exit Function
    End If
    'Else targetTile is passable.

    'If we can go, then we will
    ppos(playerNum).x = testX
    ppos(playerNum).y = testY
    ppos(playerNum).l = testLayer
    
    ' ADDED BY KSNiloc...
    ClearNonPersistentThreads
    
    Call openboard(projectPath$ + brdPath$ + targetBoard$, boardList(activeBoardIndex).theData)
    lastRender.canvas = -1
    scTopX = -1000
    scTopY = -1000
    
    Call alignBoard(ppos(selectedPlayer).x, ppos(selectedPlayer).y)
    Call openItems
    Call renderNow
    Call CanvasGetScreen(cnvRPGCodeScreen)
    
    ' ! ADDED BY KSNiloc...
    launchBoardThreads boardList(activeBoardIndex).theData
    
    'Set the mainLoop movementCounter to the end of the move.
    'Goes straight into GS_DONEMOVE state, rather than finishing the last 3 frames (caused pause on moving to new board).
    movementCounter = framesPerMove
    
    TestLink = True
End Function



Function CheckEdges(ByRef pend As PENDING_MOVEMENT, ByVal playerNum As Long) As Boolean
    'check if the player has gone off an edge
    'if he has, we put him on the new board or in a new location and return true
    'else return false
    
    On Error Resume Next
    
    Dim bWentThere As Boolean
    
    If pend.yTarg < 1 Then
        'too far north
        bWentThere = TestLink(playerNum, LINK_NORTH)
        If bWentThere Then
            CheckEdges = True
            Exit Function
        Else
            CheckEdges = True
            Exit Function
        End If
    End If
    If pend.yTarg > boardList(activeBoardIndex).theData.Bsizey Then
        'too far south!
        bWentThere = TestLink(playerNum, LINK_SOUTH)
        If bWentThere Then
            CheckEdges = True
            Exit Function
        Else
            CheckEdges = True
            Exit Function
        End If
    End If
    If pend.xTarg < 1 Then
        'too far west!
        bWentThere = TestLink(playerNum, LINK_WEST)
        If bWentThere Then
            CheckEdges = True
            Exit Function
        Else
            CheckEdges = True
            Exit Function
        End If
    End If
    If pend.xTarg > boardList(activeBoardIndex).theData.Bsizex Then
        'too far east!
        bWentThere = TestLink(playerNum, LINK_EAST)
        If bWentThere Then
            CheckEdges = True
            Exit Function
        Else
            CheckEdges = True
            Exit Function
        End If
    End If
    CheckEdges = False
End Function



Sub pushItemNorth(ByVal itemNum As Long, ByVal moveFraction As Double)
    '==========================
    'EDITED: [Delano - 5/05/04]
    'Items can now move on "under" tiles!
    'Items and players can no longer cross paths (caused players to stop interacting with programs etc.
    'Substituted tile type constants.
    'Renamed variables: pnum >> itemNum
    '==========================
    
    'Push item itemNum North
    'Called by moveItems, each frame of the movement cycle (currently 4 times).
    
    On Error Resume Next

    If pendingItemMovement(itemNum).yTarg < 1 Then
        'If targetY is off the top of the board.
        Exit Sub
    End If
    
    If (pendingItemMovement(itemNum).yTarg = Int(ppos(selectedPlayer).y) Or _
        pendingItemMovement(itemNum).yTarg = pendingPlayerMovement(selectedPlayer).yTarg) And _
        (pendingItemMovement(itemNum).xTarg = Int(ppos(selectedPlayer).x) Or _
        pendingItemMovement(itemNum).xTarg = pendingPlayerMovement(selectedPlayer).xTarg) Then
        'If target is the player's location or their destination.
        Exit Sub
    End If
    
    'Check if there is another item obstructing us at the next position.
    If (CheckObstruction(pendingItemMovement(itemNum).xTarg, _
        pendingItemMovement(itemNum).yTarg, _
        pendingItemMovement(itemNum).lTarg) = 1) Then
        Exit Sub
    End If
        
    'tileType at the target.
    Dim tiletype As Long
    tiletype = boardList(activeBoardIndex).theData.tiletype( _
                pendingItemMovement(itemNum).xTarg, _
                pendingItemMovement(itemNum).yTarg, _
                pendingItemMovement(itemNum).lTarg)
    
    'Check to see if there are any tiles on any layers above.
    'Dim underneath As Long
    'underneath = checkAbove(pendingItemMovement(itemNum).xTarg, pendingItemMovement(itemNum).yTarg, pendingItemMovement(itemNum).lTarg)
    
    If tiletype >= STAIRS1 And tiletype <= STAIRS8 Then
        'If target tile is a stair to a level.
        
        itmPos(itemNum).l = tiletype - 10   'Put the item on this layer!
        tiletype = NORMAL                   'Can always been seen on stairs!
        'underneath = 0                      'Not underneath a tile.
        
    End If
    
    'If under a tile on another level, current tile is "under".
    'If underneath = 1 Then tileType = UNDER
    
    'If North-South normal tile, carry on as if it were normal.
    'If tileType = NORTH_SOUTH Then tileType = NORMAL
    
    Select Case tiletype
    
        Case NORMAL, UNDER, NORTH_SOUTH:
            itmPos(itemNum).stance = "walk_n"
            itmPos(itemNum).frame = itmPos(itemNum).frame + 1
            Call incrementPosition(itmPos(itemNum), pendingItemMovement(itemNum), moveFraction)
            
        Case SOLID:
            'Walk on the spot.
            itmPos(itemNum).stance = "walk_n"
            itmPos(itemNum).frame = itmPos(itemNum).frame + 1
            
    End Select
    
End Sub

Sub pushItemSouth(ByVal itemNum As Long, ByVal moveFraction As Double)
    '==========================
    'EDITED: [Delano - 5/05/04]
    'Items can now move on "under" tiles!
    'Items and players can no longer cross paths (caused players to stop interacting with programs etc.
    'Substituted tile type constants.
    'Renamed variables: pnum >> itemNum
    '==========================

    'Push item itemNum South.
    'Called by moveItems, each frame of the movement cycle (currently 4 times).
    
    On Error Resume Next

    If pendingItemMovement(itemNum).yTarg > boardList(activeBoardIndex).theData.Bsizey Then
        'If target is off the bottom of the board.
        Exit Sub
    End If
    
    If (pendingItemMovement(itemNum).yTarg = Int(ppos(selectedPlayer).y) Or _
        pendingItemMovement(itemNum).yTarg = pendingPlayerMovement(selectedPlayer).yTarg) And _
        (pendingItemMovement(itemNum).xTarg = Int(ppos(selectedPlayer).x) Or _
        pendingItemMovement(itemNum).xTarg = pendingPlayerMovement(selectedPlayer).xTarg) Then
        'If target is the player's location or their destination.
        Exit Sub
    End If
    
    'Check if there is another item obstructing us at the next position.
    If (CheckObstruction(pendingItemMovement(itemNum).xTarg, _
        pendingItemMovement(itemNum).yTarg, _
        pendingItemMovement(itemNum).lTarg) = 1) Then
        Exit Sub
    End If
        
    'tileType at the target.
    Dim tiletype As Long
    tiletype = boardList(activeBoardIndex).theData.tiletype( _
                pendingItemMovement(itemNum).xTarg, _
                pendingItemMovement(itemNum).yTarg, _
                pendingItemMovement(itemNum).lTarg)
    
    'Check to see if there are any tiles on any layers above.
    'Dim underneath As Long
    'underneath = checkAbove(pendingItemMovement(itemNum).xTarg, pendingItemMovement(itemNum).yTarg, pendingItemMovement(itemNum).lTarg)
    
    If tiletype >= STAIRS1 And tiletype <= STAIRS8 Then
        'If target tile is a stair to a level.
        
        itmPos(itemNum).l = tiletype - 10   'Put the item on this layer!
        tiletype = NORMAL                   'Can always been seen on stairs!
        'underneath = 0                     'Not underneath a tile.
        
    End If
    
    'If under a tile on another level, current tile is "under".
    'If underneath = 1 Then tileType = UNDER
    
    'If North-South normal tile, carry on as if it were normal.
    'If tileType = NORTH_SOUTH Then tileType = NORMAL
    
    Select Case tiletype
    
        Case NORMAL, UNDER, NORTH_SOUTH:
            itmPos(itemNum).stance = "walk_s"
            itmPos(itemNum).frame = itmPos(itemNum).frame + 1
            Call incrementPosition(itmPos(itemNum), pendingItemMovement(itemNum), moveFraction)
        
        Case SOLID:
            'Walk on the spot.
            itmPos(itemNum).stance = "walk_s"
            itmPos(itemNum).frame = itmPos(itemNum).frame + 1
            
    End Select
    
End Sub


Sub pushItemSouthEast(ByVal itemNum As Long, ByVal moveFraction As Double)
    '==========================
    'EDITED: [Delano - 5/05/04]
    'Items can now move on "under" tiles!
    'Items and players can no longer cross paths (caused players to stop interacting with programs etc.
    'Substituted tile type constants.
    'Renamed variables: pnum >> itemNum
    '==========================

    'Push item itemNum SouthEast.
    'Called by moveItems, each frame of the movement cycle (currently 4 times).
    
    On Error Resume Next

    If pendingItemMovement(itemNum).yTarg > boardList(activeBoardIndex).theData.Bsizey Or _
        pendingItemMovement(itemNum).xTarg > boardList(activeBoardIndex).theData.Bsizex Then
        'If target is off the board.
        Exit Sub
    End If
    
    If (pendingItemMovement(itemNum).yTarg = Int(ppos(selectedPlayer).y) Or _
        pendingItemMovement(itemNum).yTarg = pendingPlayerMovement(selectedPlayer).yTarg) And _
        (pendingItemMovement(itemNum).xTarg = Int(ppos(selectedPlayer).x) Or _
        pendingItemMovement(itemNum).xTarg = pendingPlayerMovement(selectedPlayer).xTarg) Then
        'If target is the player's location or their destination.
        Exit Sub
    End If
    
    'Check if there is another item obstructing us at the next position.
    If (CheckObstruction(pendingItemMovement(itemNum).xTarg, _
        pendingItemMovement(itemNum).yTarg, _
        pendingItemMovement(itemNum).lTarg) = 1) Then
        Exit Sub
    End If
        
    'tileType at the target.
    Dim tiletype As Long
    tiletype = boardList(activeBoardIndex).theData.tiletype( _
                pendingItemMovement(itemNum).xTarg, _
                pendingItemMovement(itemNum).yTarg, _
                pendingItemMovement(itemNum).lTarg)
    
    'Check to see if there are any tiles on any layers above.
    'Dim underneath As Long
    'underneath = checkAbove(pendingItemMovement(itemNum).xTarg, pendingItemMovement(itemNum).yTarg, pendingItemMovement(itemNum).lTarg)
    
    If tiletype >= STAIRS1 And tiletype <= STAIRS8 Then
        'If target tile is a stair to a level.
        
        itmPos(itemNum).l = tiletype - 10   'Put the item on this layer!
        tiletype = NORMAL                   'Can always been seen on stairs!
        'underneath = 0                     'Not underneath a tile.
        
    End If
    
    'If under a tile on another level, current tile is "under".
    'If underneath = 1 Then tileType = UNDER
    
    Select Case tiletype
    
        Case NORMAL, UNDER:
            itmPos(itemNum).stance = "walk_se"
            itmPos(itemNum).frame = itmPos(itemNum).frame + 1
            Call incrementPosition(itmPos(itemNum), pendingItemMovement(itemNum), moveFraction)
            
        Case SOLID:
            'Walk on the spot.
            itmPos(itemNum).stance = "walk_se"
            itmPos(itemNum).frame = itmPos(itemNum).frame + 1
            
    End Select
    
End Sub


Sub pushItemEast(ByVal itemNum As Long, ByVal moveFraction As Double)
    '==========================
    'EDITED: [Delano - 5/05/04]
    'Items can now move on "under" tiles!
    'Items and players can no longer cross paths (caused players to stop interacting with programs etc.
    'Substituted tile type constants.
    'Renamed variables: pnum >> itemNum
    '==========================

    'Push item itemNum East.
    'Called by moveItems, each frame of the movement cycle (currently 4 times).
    
    On Error Resume Next

    If pendingItemMovement(itemNum).xTarg > boardList(activeBoardIndex).theData.Bsizex Then
        'If targetX is off the board (Note: pushing east!)
        Exit Sub
    End If
    
    If (pendingItemMovement(itemNum).yTarg = Int(ppos(selectedPlayer).y) Or _
        pendingItemMovement(itemNum).yTarg = pendingPlayerMovement(selectedPlayer).yTarg) And _
        (pendingItemMovement(itemNum).xTarg = Int(ppos(selectedPlayer).x) Or _
        pendingItemMovement(itemNum).xTarg = pendingPlayerMovement(selectedPlayer).xTarg) Then
        'If target is the player's location or their destination.
        Exit Sub
    End If
    
    'Check if there is another item obstructing us at the next position.
    If (CheckObstruction(pendingItemMovement(itemNum).xTarg, _
        pendingItemMovement(itemNum).yTarg, _
        pendingItemMovement(itemNum).lTarg) = 1) Then
        Exit Sub
    End If
        
    'tileType at the target.
    Dim tiletype As Long
    tiletype = boardList(activeBoardIndex).theData.tiletype( _
                pendingItemMovement(itemNum).xTarg, _
                pendingItemMovement(itemNum).yTarg, _
                pendingItemMovement(itemNum).lTarg)
    
    'Check to see if there are any tiles on any layers above.
    'Doesn't make a difference! Only decided by putSpriteAt!
    'Dim underneath As Long
    'underneath = checkAbove(pendingItemMovement(itemNum).xTarg, pendingItemMovement(itemNum).yTarg, pendingItemMovement(itemNum).lTarg)
    
    If tiletype >= STAIRS1 And tiletype <= STAIRS8 Then
        'If target tile is a stair to a level.
        
        itmPos(itemNum).l = tiletype - 10   'Put the item on this layer!
        tiletype = NORMAL                   'Can always been seen on stairs!
        'underneath = 0                     'Not underneath a tile.
        
    End If
    
    'If under a tile on another level, current tile is "under".
    'But what if current tile is solid and there is something above?
    'If underneath = 1 Then tileType = UNDER
    
    'If East-West normal tile, carry on as if it were normal.
    'If tileType = EAST_WEST Then tileType = NORMAL
   
    Select Case tiletype
    
        Case NORMAL, UNDER, EAST_WEST:
            itmPos(itemNum).stance = "walk_e"
            itmPos(itemNum).frame = itmPos(itemNum).frame + 1
            Call incrementPosition(itmPos(itemNum), pendingItemMovement(itemNum), moveFraction)
            
        Case SOLID:
            'Walk on the spot. Might be better to be idle...?
            itmPos(itemNum).stance = "walk_e"
            itmPos(itemNum).frame = itmPos(itemNum).frame + 1
            
    End Select
    
End Sub



Sub pushItemNorthEast(ByVal itemNum As Long, ByVal moveFraction As Double)
    '==========================
    'EDITED: [Delano - 5/05/04]
    'Items can now move on "under" tiles!
    'Items and players can no longer cross paths (caused players to stop interacting with programs etc.
    'Substituted tile type constants.
    'Renamed variables: pnum >> itemNum
    '==========================
    
    'Push item itemNum NorthEast.
    'Called by moveItems, each frame of the movement cycle (currently 4 times).
    
    On Error Resume Next

    If pendingItemMovement(itemNum).xTarg > boardList(activeBoardIndex).theData.Bsizex Or _
        pendingItemMovement(itemNum).yTarg < 1 Then
        'If target is off the board.
        Exit Sub
    End If
    
    If (pendingItemMovement(itemNum).yTarg = Int(ppos(selectedPlayer).y) Or _
        pendingItemMovement(itemNum).yTarg = pendingPlayerMovement(selectedPlayer).yTarg) And _
        (pendingItemMovement(itemNum).xTarg = Int(ppos(selectedPlayer).x) Or _
        pendingItemMovement(itemNum).xTarg = pendingPlayerMovement(selectedPlayer).xTarg) Then
        'If target is the player's location or their destination.
        Exit Sub
    End If
    
    'Check if there is another item obstructing us at the next position.
    If (CheckObstruction(pendingItemMovement(itemNum).xTarg, _
        pendingItemMovement(itemNum).yTarg, _
        pendingItemMovement(itemNum).lTarg) = 1) Then
        Exit Sub
    End If
        
    'tileType at the target.
    Dim tiletype As Long
    tiletype = boardList(activeBoardIndex).theData.tiletype( _
                pendingItemMovement(itemNum).xTarg, _
                pendingItemMovement(itemNum).yTarg, _
                pendingItemMovement(itemNum).lTarg)
    
    'Check to see if there are any tiles on any layers above.
    'Dim underneath As Long
    'underneath = checkAbove(pendingItemMovement(itemNum).xTarg, pendingItemMovement(itemNum).yTarg, pendingItemMovement(itemNum).lTarg)
    
    If tiletype >= STAIRS1 And tiletype <= STAIRS8 Then
        'If target tile is a stair to a level.
        
        itmPos(itemNum).l = tiletype - 10   'Put the item on this layer!
        tiletype = NORMAL                   'Can always been seen on stairs!
        'underneath = 0                     'Not underneath a tile.
        
    End If
    
    'If under a tile on another level, current tile is "under".
    'If underneath = 1 Then tileType = UNDER
    
    Select Case tiletype
    
        Case NORMAL, UNDER:
            itmPos(itemNum).stance = "walk_ne"
            itmPos(itemNum).frame = itmPos(itemNum).frame + 1
            Call incrementPosition(itmPos(itemNum), pendingItemMovement(itemNum), moveFraction)
            
        Case SOLID:
            'Walk on the spot. Might be better to be idle...?
            itmPos(itemNum).stance = "walk_ne"
            itmPos(itemNum).frame = itmPos(itemNum).frame + 1
            
    End Select
    
End Sub




Sub pushItemWest(ByVal itemNum As Long, ByVal moveFraction As Double)
    '==========================
    'EDITED: [Delano - 5/05/04]
    'Items can now move on "under" tiles!
    'Items and players can no longer cross paths (caused players to stop interacting with programs etc.
    'Substituted tile type constants.
    'Renamed variables: pnum >> itemNum
    '==========================

    'Push item itemNum West.
    'Called by moveItems, each frame of the movement cycle (currently 4 times).
    
    On Error Resume Next

    If pendingItemMovement(itemNum).xTarg < 1 Then
        'If target is off the board.
        Exit Sub
    End If
    
    If (pendingItemMovement(itemNum).yTarg = Int(ppos(selectedPlayer).y) Or _
        pendingItemMovement(itemNum).yTarg = pendingPlayerMovement(selectedPlayer).yTarg) And _
        (pendingItemMovement(itemNum).xTarg = Int(ppos(selectedPlayer).x) Or _
        pendingItemMovement(itemNum).xTarg = pendingPlayerMovement(selectedPlayer).xTarg) Then
        'If target is the player's location or their destination.
        Exit Sub
    End If
    
    'Check if there is another item obstructing us at the next position.
    If (CheckObstruction(pendingItemMovement(itemNum).xTarg, _
        pendingItemMovement(itemNum).yTarg, _
        pendingItemMovement(itemNum).lTarg) = 1) Then
        Exit Sub
    End If
        
    'tileType at the target.
    Dim tiletype As Long
    tiletype = boardList(activeBoardIndex).theData.tiletype( _
                pendingItemMovement(itemNum).xTarg, _
                pendingItemMovement(itemNum).yTarg, _
                pendingItemMovement(itemNum).lTarg)
    
    'Check to see if there are any tiles on any layers above.
    'Dim underneath As Long
    'underneath = checkAbove(pendingItemMovement(itemNum).xTarg, pendingItemMovement(itemNum).yTarg, pendingItemMovement(itemNum).lTarg)
    
    If tiletype >= STAIRS1 And tiletype <= STAIRS8 Then
        'If target tile is a stair to a level.
        
        itmPos(itemNum).l = tiletype - 10   'Put the item on this layer!
        tiletype = NORMAL                   'Can always been seen on stairs!
        'underneath = 0                     'Not underneath a tile.
        
    End If
    
    'If under a tile on another level, current tile is "under".
    'If underneath = 1 Then tileType = UNDER
    
    'If East-West normal tile, carry on as if it were normal.
    'If tileType = EAST_WEST Then tileType = NORMAL
    
    Select Case tiletype
    
        Case NORMAL, UNDER, EAST_WEST:
            itmPos(itemNum).stance = "walk_w"
            itmPos(itemNum).frame = itmPos(itemNum).frame + 1
            Call incrementPosition(itmPos(itemNum), pendingItemMovement(itemNum), moveFraction)
            
        Case SOLID:
            'Walk on the spot.
            itmPos(itemNum).stance = "walk_w"
            itmPos(itemNum).frame = itmPos(itemNum).frame + 1
            
    End Select
    
End Sub


Sub pushItemSouthWest(ByVal itemNum As Long, ByVal moveFraction As Double)
    '==========================
    'EDITED: [Delano - 5/05/04]
    'Items can now move on "under" tiles!
    'Items and players can no longer cross paths (caused players to stop interacting with programs etc.
    'Substituted tile type constants.
    'Renamed variables: pnum >> itemNum
    '==========================

    'Push item itemNum SouthWest.
    'Called by moveItems, each frame of the movement cycle (currently 4 times).
    
    On Error Resume Next

    If pendingItemMovement(itemNum).xTarg < 1 Or _
        pendingItemMovement(itemNum).yTarg > boardList(activeBoardIndex).theData.Bsizey Then
        'If target is off the board.
        Exit Sub
    End If
    
    If (pendingItemMovement(itemNum).yTarg = Int(ppos(selectedPlayer).y) Or _
        pendingItemMovement(itemNum).yTarg = pendingPlayerMovement(selectedPlayer).yTarg) And _
        (pendingItemMovement(itemNum).xTarg = Int(ppos(selectedPlayer).x) Or _
        pendingItemMovement(itemNum).xTarg = pendingPlayerMovement(selectedPlayer).xTarg) Then
        'If target is the player's location or their destination.
        Exit Sub
    End If
    
    'Check if there is another item obstructing us at the next position.
    If (CheckObstruction(pendingItemMovement(itemNum).xTarg, _
        pendingItemMovement(itemNum).yTarg, _
        pendingItemMovement(itemNum).lTarg) = 1) Then
        Exit Sub
    End If
        
    'tileType at the target.
    Dim tiletype As Long
    tiletype = boardList(activeBoardIndex).theData.tiletype( _
                pendingItemMovement(itemNum).xTarg, _
                pendingItemMovement(itemNum).yTarg, _
                pendingItemMovement(itemNum).lTarg)
    
    'Check to see if there are any tiles on any layers above.
    'Dim underneath As Long
    'underneath = checkAbove(pendingItemMovement(itemNum).xTarg, pendingItemMovement(itemNum).yTarg, pendingItemMovement(itemNum).lTarg)
    
    If tiletype >= STAIRS1 And tiletype <= STAIRS8 Then
        'If target tile is a stair to a level.
        
        itmPos(itemNum).l = tiletype - 10   'Put the item on this layer!
        tiletype = NORMAL                   'Can always been seen on stairs!
        'underneath = 0                     'Not underneath a tile.
        
    End If
    
    'If under a tile on another level, current tile is "under".
    'If underneath = 1 Then tileType = UNDER
    
    Select Case tiletype
    
        Case NORMAL, UNDER:
            itmPos(itemNum).stance = "walk_sw"
            itmPos(itemNum).frame = itmPos(itemNum).frame + 1
            Call incrementPosition(itmPos(itemNum), pendingItemMovement(itemNum), moveFraction)
            
        Case SOLID:
            'Walk on the spot.
            itmPos(itemNum).stance = "walk_sw"
            itmPos(itemNum).frame = itmPos(itemNum).frame + 1
            
    End Select
    
End Sub


Sub pushItemNorthWest(ByVal itemNum As Long, ByVal moveFraction As Double)
    '==========================
    'EDITED: [Delano - 5/05/04]
    'Items can now move on "under" tiles!
    'Items and players can no longer cross paths (caused players to stop interacting with programs etc.
    'Substituted tile type constants.
    'Renamed variables: pnum >> itemNum
    '==========================

    'Push item itemNum NorthWest.
    'Called by moveItems, each frame of the movement cycle (currently 4 times).
    
    On Error Resume Next

    If pendingItemMovement(itemNum).xTarg < 1 Or _
        pendingItemMovement(itemNum).yTarg < 1 Then
        'If target is off the board.
        Exit Sub
    End If
    
    If (pendingItemMovement(itemNum).yTarg = Int(ppos(selectedPlayer).y) Or _
        pendingItemMovement(itemNum).yTarg = pendingPlayerMovement(selectedPlayer).yTarg) And _
        (pendingItemMovement(itemNum).xTarg = Int(ppos(selectedPlayer).x) Or _
        pendingItemMovement(itemNum).xTarg = pendingPlayerMovement(selectedPlayer).xTarg) Then
        'If target is the player's location or their destination.
        Exit Sub
    End If
    
    'Check if there is another item obstructing us at the next position.
    If (CheckObstruction(pendingItemMovement(itemNum).xTarg, _
        pendingItemMovement(itemNum).yTarg, _
        pendingItemMovement(itemNum).lTarg) = 1) Then
        Exit Sub
    End If
        
    'tileType at the target.
    Dim tiletype As Long
    tiletype = boardList(activeBoardIndex).theData.tiletype( _
                pendingItemMovement(itemNum).xTarg, _
                pendingItemMovement(itemNum).yTarg, _
                pendingItemMovement(itemNum).lTarg)
    
    'Check to see if there are any tiles on any layers above.
    'Dim underneath As Long
    'underneath = checkAbove(pendingItemMovement(itemNum).xTarg, pendingItemMovement(itemNum).yTarg, pendingItemMovement(itemNum).lTarg)
    
    If tiletype >= STAIRS1 And tiletype <= STAIRS8 Then
        'If target tile is a stair to a level.
        
        itmPos(itemNum).l = tiletype - 10   'Put the item on this layer!
        tiletype = NORMAL                   'Can always been seen on stairs!
        'underneath = 0                     'Not underneath a tile.
        
    End If
    
    'If under a tile on another level, current tile is "under".
    'If underneath = 1 Then tileType = UNDER
    
    Select Case tiletype
    
        Case NORMAL, UNDER:
            itmPos(itemNum).stance = "walk_nw"
            itmPos(itemNum).frame = itmPos(itemNum).frame + 1
            Call incrementPosition(itmPos(itemNum), pendingItemMovement(itemNum), moveFraction)
            
        Case SOLID:
            'Walk on spot.
            itmPos(itemNum).stance = "walk_nw"
            itmPos(itemNum).frame = itmPos(itemNum).frame + 1
            
    End Select
    
End Sub

Sub pushPlayerNorthEast(ByVal pnum As Long, ByVal moveFraction As Double)
    '======================================
    'EDITED: [Isometrics - Delano 11/05/04]
    'Substituted tile type constants.
    'Moved scroll checking to a new function: NORTH only... problem with East.
    '======================================
    
    'Push player pnum NorthEast by moveFraction.
    'Called by movePlayers, each frame of the movement cycle (currently 4 times).
    'Calls incrementPosition if movement is possible, and scrollNorthEast if scrolling required.
    
    On Error Resume Next
    
    fightInProgress = False
    stepsTaken = stepsTaken + 1
    
    'Before doing anything, let's see if we are going off the board.
    'Checks for links and will send to new board if a link is possible.
    Dim bWentOffEdge As Boolean
    bWentOffEdge = CheckEdges(pendingPlayerMovement(pnum), pnum)
    
    If bWentOffEdge Then
        pendingPlayerMovement(pnum).direction = MV_IDLE
        Exit Sub
    End If

    'obtain the tile type at the target...
    Dim didItem As Boolean
    Dim typetile As Long
    typetile = ObtainTileType(pendingPlayerMovement(pnum).xTarg, _
                              pendingPlayerMovement(pnum).yTarg, _
                              pendingPlayerMovement(pnum).lTarg, _
                              LINK_NE, _
                              ppos(pnum), _
                              didItem)
    
    'Advance the frame for all tile types (if SOLID, will walk on spot.)
    ppos(pnum).stance = "walk_ne"
    ' MODIFIED BY KSNiloc...
    incrementFrame ppos(pnum).frame
    'ppos(pNum).frame = ppos(pNum).frame + 1
    
    Select Case typetile
        Case NORMAL, UNDER:
            'see if we need to scroll...
            
            'Introducing new independent direction variables.
            Dim scrollEast As Boolean, scrollNorth As Boolean
            scrollEast = True
            
            'Scrolling north is handled by the new function checkScrollNorth, but for diagonal
            'isometrics the conditions are a little different from checkScrollEast. Not sure why...
            
            If boardIso() Then
                'PushEast code. SHOULD BE EXACTLY THE SAME AS FOR pushSouthEast! ANY CHANGES SHOULD BE COPIED!
                
                If (topX + isoTilesX + 0.5 >= boardList(activeBoardIndex).theData.Bsizex) Or _
                    (ppos(pnum).x < (isoTilesX / 2) And topX = 0) Or _
                    (ppos(pnum).x - topX + 0.5 < (isoTilesX / 2)) Or _
                    pnum <> selectedPlayer Then
                    scrollEast = False
                    
                End If
            Else
                'SAME AS pushSouthEast.
                'Swapping " + 1 >" for ">=" (boards do not scroll to edges)
                
                If (topX + tilesX >= boardList(activeBoardIndex).theData.Bsizex) Or _
                    (ppos(pnum).x < (tilesX / 2) And topX = 0) Or _
                    (ppos(pnum).x - topX < (tilesX / 2)) Or _
                    pnum <> selectedPlayer Then
                    scrollEast = False
                End If
            End If
            
            scrollNorth = checkScrollNorth(pnum) 'New function!
                      
            If scrollEast Or scrollNorth Then
                Call scrollDownLeft(moveFraction, scrollEast, scrollNorth)
            End If
            
            Call incrementPosition(ppos(pnum), pendingPlayerMovement(pnum), moveFraction)
            
        'Case SOLID:
            'Advance the frame but don't move.
    End Select
End Sub


Sub pushPlayerNorthWest(ByVal pnum As Long, ByVal moveFraction As Double)
    '======================================
    'EDITED: [Isometrics - Delano 11/05/04]
    'Substituted tile type constants.
    'Moved scroll checking to a new function. NORTH only... need to change West.
    '======================================
    
    'Push player pnum NorthWest by moveFraction.
    'Called by movePlayers, each frame of the movement cycle (currently 4 times).
    'Calls incrementPosition if movement is possible, and scrollNorthWest if scrolling required.
    
    On Error Resume Next
    
    fightInProgress = False
    stepsTaken = stepsTaken + 1
    
    'Before doing anything, let's see if we are going off the board.
    'Checks for links and will send to new board if a link is possible.
    Dim bWentOffEdge As Boolean
    bWentOffEdge = CheckEdges(pendingPlayerMovement(pnum), pnum)
    
    If bWentOffEdge Then
        pendingPlayerMovement(pnum).direction = MV_IDLE
        Exit Sub
    End If

    'obtain the tile type at the target...
    Dim didItem As Boolean
    Dim typetile As Long
    typetile = ObtainTileType(pendingPlayerMovement(pnum).xTarg, _
                              pendingPlayerMovement(pnum).yTarg, _
                              pendingPlayerMovement(pnum).lTarg, _
                              LINK_NW, _
                              ppos(pnum), _
                              didItem)
    
    
    'Advance the frame for all tile types (if SOLID, will walk on spot.)
    ppos(pnum).stance = "walk_nw"
    ' MODIFIED BY KSNiloc...
    incrementFrame ppos(pnum).frame
    'ppos(pNum).frame = ppos(pNum).frame + 1
    
    Select Case typetile
        Case NORMAL, UNDER:
            'See if we need to scroll...
            
            'Introducing new independent direction variables.
            Dim scrollWest As Boolean, scrollNorth As Boolean
            scrollWest = True
    
            'Scrolling north is handled by the new function checkScrollNorth, but for diagonal
            'isometrics the conditions need to be a little different from checkScrollWest. Not sure why...
            
            If boardIso() Then
                'pushWest code. SHOULD BE EXACTLY THE SAME AS FOR pushSouthWest! ANY CHANGES SHOULD BE COPIED
                If (ppos(pnum).x > boardList(activeBoardIndex).theData.Bsizex - (isoTilesX / 2) And _
                    topX + 1 = boardList(activeBoardIndex).theData.Bsizex - isoTilesX) Or _
                    (ppos(pnum).x - (topX + 1) > (isoTilesX / 2)) Or _
                    ((topX + 1) - 1 < 0) Or _
                    pnum <> selectedPlayer Then
                    
                    scrollWest = False
                End If
            Else
                'This is pushWest standard code.
                
                If (ppos(pnum).x > boardList(activeBoardIndex).theData.Bsizex - (tilesX / 2) And _
                    topX = boardList(activeBoardIndex).theData.Bsizex - tilesX) Or _
                    (ppos(pnum).x - topX > (tilesX / 2)) Or _
                    (topX <= 0) Or _
                    pnum <> selectedPlayer Then
                    
                    scrollWest = False
                End If
            End If
            
            scrollNorth = checkScrollNorth(pnum) 'New function!
            
            If scrollWest Or scrollNorth Then
                Call scrollDownRight(moveFraction, scrollWest, scrollNorth)
            End If
            
            Call incrementPosition(ppos(pnum), pendingPlayerMovement(pnum), moveFraction)
            
        'Case SOLID:
            'Walk on the spot.
    End Select
End Sub

Sub pushPlayerSouthEast(ByVal pnum As Long, ByVal moveFraction As Double)
    '=====================================
    'EDITED: [Isometrics - Delano 3/05/04]
    'Substituted tile type constants.
    '=====================================
    
    'Push player pnum SouthEast by moveFraction.
    'Called by movePlayers, each frame of the movement cycle (currently 4 times).
    'Calls incrementPosition if movement is possible, and scrollSouthEast if scrolling required.
    
    On Error Resume Next
    
    fightInProgress = False
    stepsTaken = stepsTaken + 1
    
    'Before doing anything, let's see if we are going off the board.
    'Checks for links and will send to new board if a link is possible.
    Dim bWentOffEdge As Boolean
    bWentOffEdge = CheckEdges(pendingPlayerMovement(pnum), pnum)
    
    If bWentOffEdge Then
        pendingPlayerMovement(pnum).direction = MV_IDLE
        Exit Sub
    End If

    'obtain the tile type at the target...
    Dim didItem As Boolean
    Dim typetile As Long
    typetile = ObtainTileType(pendingPlayerMovement(pnum).xTarg, _
                              pendingPlayerMovement(pnum).yTarg, _
                              pendingPlayerMovement(pnum).lTarg, _
                              LINK_SE, _
                              ppos(pnum), _
                              didItem)
    
    'Introducing new independent direction variables.
    Dim scrollEast As Boolean
    scrollEast = True
    Dim scrollsouth As Boolean
    scrollsouth = True
    
    Select Case typetile
        Case NORMAL, UNDER:
            'See if we need to scroll...
            
            'Accounting for isometrics:
            If boardIso() Then
                'This is the PushEast code. + 0.5 modif - does it work?
                If (topX + isoTilesX + 0.5 >= boardList(activeBoardIndex).theData.Bsizex) Or _
                    (ppos(pnum).x < (isoTilesX / 2) And topX = 0) Or _
                    (ppos(pnum).x - topX + 0.5 < (isoTilesX / 2)) Or _
                    pnum <> selectedPlayer Then
                    scrollEast = False
                End If
                'pushSouth code with topY modification. ANY CHANGES SHOULD BE COPIED TO pushSouthWest
                If ((topY * 2 + 1) + isoTilesY >= boardList(activeBoardIndex).theData.Bsizey) Or _
                    (ppos(pnum).y < (isoTilesY / 2) And topY = 0) Or _
                    (ppos(pnum).y - (topY) < (isoTilesY / 2)) Or _
                    pnum <> selectedPlayer Then '^Doesn't work with topy * 2...
                    scrollsouth = False
                End If
            Else
                'Original code was incomplete even for standard boards!! FIXED.
                'Swapping " + 1 >" for ">=" (boards do not scroll to edges)
                If (topX + tilesX >= boardList(activeBoardIndex).theData.Bsizex) Or _
                    (ppos(pnum).x < (tilesX / 2) And topX = 0) Or _
                    (ppos(pnum).x - topX < (tilesX / 2)) Or _
                    pnum <> selectedPlayer Then
                    scrollEast = False
                End If
                'TRIAL ADDITION: pushSouth code w/scrollSouth
                'Swapping " + 1 >" for ">=" (boards do not scroll to edges)
                If (topY + tilesY >= boardList(activeBoardIndex).theData.Bsizey) Or _
                    (ppos(pnum).y < (tilesY / 2) And topY = 0) Or _
                    (ppos(pnum).y - topY < (tilesY / 2)) Or _
                    pnum <> selectedPlayer Then
                    scrollsouth = False
                End If
            End If
            
            If scrollEast Or scrollsouth Then
                Call scrollUpLeft(moveFraction, scrollEast, scrollsouth)
            End If
            
            ppos(pnum).stance = "walk_se"
            ' MODIFIED BY KSNiloc...
            incrementFrame ppos(pnum).frame
            'ppos(pNum).frame = ppos(pNum).frame + 1
            Call incrementPosition(ppos(pnum), pendingPlayerMovement(pnum), moveFraction)
            
        Case SOLID:
            'Walk on the spot.
            ppos(pnum).stance = "walk_se"
            ' MODIFIED BY KSNiloc...
            incrementFrame ppos(pnum).frame
            'ppos(pNum).frame = ppos(pNum).frame + 1
    End Select
End Sub

Sub pushPlayerSouthWest(ByVal pnum As Long, ByVal moveFraction As Double)
    '=====================================
    'EDITED: [Isometrics - Delano 3/05/04]
    'Substituted tile type constants.
    '=====================================
    
    'Push player pNum SouthWest by moveFraction.
    'Called by movePlayers, each frame of the movement cycle (currently 4 times).
    'Calls incrementPosition if movement is possible, and scrollSouthWest if scrolling required.
    
    On Error Resume Next
    
    fightInProgress = False
    stepsTaken = stepsTaken + 1
    
    'Before doing anything, let's see if we are going off the board.
    'Checks for links and will send to new board if a link is possible.
    Dim bWentOffEdge As Boolean
    bWentOffEdge = CheckEdges(pendingPlayerMovement(pnum), pnum)
    
    If bWentOffEdge Then
        pendingPlayerMovement(pnum).direction = MV_IDLE
        Exit Sub
    End If

    'obtain the tile type at the target...
    Dim didItem As Boolean
    Dim typetile As Long
    typetile = ObtainTileType(pendingPlayerMovement(pnum).xTarg, _
                              pendingPlayerMovement(pnum).yTarg, _
                              pendingPlayerMovement(pnum).lTarg, _
                              LINK_SW, _
                              ppos(pnum), _
                              didItem)
    
    'Introducing new independent direction variables
    Dim scrollWest As Boolean
    scrollWest = True
    Dim scrollsouth As Boolean
    scrollsouth = True
    
    Select Case typetile
        Case NORMAL, UNDER:
            'See if we need to scroll...
            
            'Accounting for isometrics:
            If boardIso() Then
                'This is the pushWest code. MIGHT NEED TO CHANGE cf. SouthEast 0.5
                If (ppos(pnum).x > boardList(activeBoardIndex).theData.Bsizex - (isoTilesX / 2) And _
                    topX + 1 = boardList(activeBoardIndex).theData.Bsizex - isoTilesX) Or _
                    (ppos(pnum).x - (topX + 1) > (isoTilesX / 2)) Or _
                    ((topX + 1) - 1 < 0) Or _
                    pnum <> selectedPlayer Then
                    scrollWest = False
                End If
                'pushSouth code. SHOULD BE EXACTLY THE SAME AS FOR pushSouthEast! ANY CHANGES SHOULD BE COPIED
                If ((topY * 2 + 1) + isoTilesY >= boardList(activeBoardIndex).theData.Bsizey) Or _
                    (ppos(pnum).y < (isoTilesY / 2) And topY = 0) Or _
                    (ppos(pnum).y - (topY) < (isoTilesY / 2)) Or _
                    pnum <> selectedPlayer Then '^Doesn't work with topy * 2...
                    scrollsouth = False
                End If
            Else
                'Original code was incomplete! This is pushWest standard code.
                'Swapping " - 1 <" for "<=" (boards do not scroll to edges)
                If (ppos(pnum).x > boardList(activeBoardIndex).theData.Bsizex - (tilesX / 2) And _
                    topX = boardList(activeBoardIndex).theData.Bsizex - tilesX) Or _
                    (ppos(pnum).x - topX > (tilesX / 2)) Or _
                    (topX <= 0) Or _
                    pnum <> selectedPlayer Then
                    scrollWest = False
                End If
                'pushSouth standard board code with topY modification. ANY CHANGES SHOULD BE COPIED TO pushSouthEast
                'Swapping " + 1 >" for ">=" (boards do not scroll to edges)
                If (topY + tilesY >= boardList(activeBoardIndex).theData.Bsizey) Or _
                    (ppos(pnum).y < (tilesY / 2) And topY = 0) Or _
                    (ppos(pnum).y - topY < (tilesY / 2)) Or _
                    pnum <> selectedPlayer Then
                    scrollsouth = False
                End If
            End If
            
            If scrollWest Or scrollsouth Then
                Call scrollUpRight(moveFraction, scrollWest, scrollsouth)
            End If
            
            ppos(pnum).stance = "walk_sw"
            ' MODIFIED BY KSNiloc...
            incrementFrame ppos(pnum).frame
            'ppos(pNum).frame = ppos(pNum).frame + 1
            Call incrementPosition(ppos(pnum), pendingPlayerMovement(pnum), moveFraction)
            
        Case SOLID:
            'Walk on the spot.
            ppos(pnum).stance = "walk_sw"
            ' MODIFIED BY KSNiloc...
            incrementFrame ppos(pnum).frame
            'ppos(pNum).frame = ppos(pNum).frame + 1
    End Select
End Sub

Sub pushPlayerNorth(ByVal pnum As Long, ByVal moveFraction As Double)
    '======================================
    'EDITED: [Isometrics - Delano 11/05/04]
    'Substituted tile type constants.
    'Moved scroll checking to a new function.
    '======================================
    
    'Push player pnum North by moveFraction.
    'Called by movePlayers, each frame of the movement cycle (currently 4 times).
    'Calls incrementPosition if movement is possible, and scrollNorth if scrolling required.
    
    On Error Resume Next

    fightInProgress = False
    stepsTaken = stepsTaken + 1
    
    'Before doing anything, let's see if we are going off the board.
    'Checks for links and will send to new board if a link is possible.
    Dim bWentOffEdge As Boolean
    bWentOffEdge = CheckEdges(pendingPlayerMovement(pnum), pnum)
    
    If bWentOffEdge Then
        pendingPlayerMovement(pnum).direction = MV_IDLE
        Exit Sub
    End If

    'obtain the tile type at the target...
    Dim didItem As Boolean
    Dim typetile As Long
    typetile = ObtainTileType(pendingPlayerMovement(pnum).xTarg, _
                              pendingPlayerMovement(pnum).yTarg, _
                              pendingPlayerMovement(pnum).lTarg, _
                              LINK_NORTH, _
                              ppos(pnum), _
                              didItem)
    
    'Advance the frame for all tile types (if SOLID, will walk on spot.)
    ppos(pnum).stance = "walk_n"
    
    ' MODIFIED BY KSNiloc...
    incrementFrame ppos(pnum).frame
    'ppos(pNum).frame = ppos(pNum).frame + 1
    
    Select Case typetile
        Case NORMAL, UNDER:
            'See if we need to scroll...
    
            Dim scrollNorth As Boolean
           
            scrollNorth = checkScrollNorth(pnum) 'New function!
            
            If scrollNorth Then Call scrollDown(moveFraction)
            'shift the screen drawing co-ords down (topY).
            
            Call incrementPosition(ppos(pnum), pendingPlayerMovement(pnum), moveFraction)
            
        'Case SOLID:
            'Walk on the spot.
    End Select
    
End Sub

Sub pushPlayerSouth(ByVal pnum As Long, ByVal moveFraction As Double)
    '======================================
    'EDITED: [Isometrics - Delano 11/05/04]
    'Substituted tile type constants.
    'Moved scroll checking to a new function.
    '======================================
    
    'Push player pnum South by moveFraction.
    'Called by movePlayers, each frame of the movement cycle (currently 4 times).
    'Calls incrementPosition if movement is possible, and scrollSouth if scrolling required.
    
    On Error Resume Next

    fightInProgress = False
    stepsTaken = stepsTaken + 1
    
    'Before doing anything, let's see if we are going off the board.
    'Checks for links and will send to new board if a link is possible.
    Dim bWentOffEdge As Boolean
    bWentOffEdge = CheckEdges(pendingPlayerMovement(pnum), pnum)
    
    If bWentOffEdge Then
        pendingPlayerMovement(pnum).direction = MV_IDLE
        Exit Sub
    End If

    'obtain the 'tile type' at the target...
    Dim didItem As Boolean
    Dim typetile As Long
    typetile = ObtainTileType(pendingPlayerMovement(pnum).xTarg, _
                              pendingPlayerMovement(pnum).yTarg, _
                              pendingPlayerMovement(pnum).lTarg, _
                              LINK_SOUTH, _
                              ppos(pnum), _
                              didItem)
    
    'Advance the frame for all tile types (if SOLID, will walk on spot.)
    ppos(pnum).stance = "walk_s"
    ' MODIFIED BY KSNiloc...
    incrementFrame ppos(pnum).frame
    'ppos(pNum).frame = ppos(pNum).frame + 1
    
    Select Case typetile
        Case NORMAL, UNDER:
            'see if we need to scroll...
            
            Dim scrollsouth As Boolean 'determine if a scroll is required
                
            scrollsouth = checkScrollSouth(pnum) 'New function!
            
            If scrollsouth Then Call scrollUp(moveFraction)
            'shift the screen drawing co-ords up (topY).
            
            Call incrementPosition(ppos(pnum), pendingPlayerMovement(pnum), moveFraction)
            
        'Case SOLID:
            'Walk on the spot.
    End Select
End Sub


Sub pushPlayerEast(ByVal pnum As Long, ByVal moveFraction As Double)
    '======================================
    'EDITED: [Isometrics - Delano 11/05/04]
    'Substituted tile type constants.
    'Moved scroll checking to a new function.
    '======================================
    
    'Push player pnum East by moveFraction.
    'Called by movePlayers, each frame of the movement cycle (currently 4 times).
    'Calls incrementPosition if movement is possible, and scrollEast if scrolling required.
    
    On Error Resume Next

    fightInProgress = False
    stepsTaken = stepsTaken + 1
    
    'Before doing anything, let's see if we are going off the board.
    'Checks for links and will send to new board if a link is possible.
    Dim bWentOffEdge As Boolean
    bWentOffEdge = CheckEdges(pendingPlayerMovement(pnum), pnum)
    
    If bWentOffEdge Then
        pendingPlayerMovement(pnum).direction = MV_IDLE
        Exit Sub
    End If

    'Obtain the 'tile type' at the target...
    Dim didItem As Boolean
    Dim typetile As Long
    typetile = ObtainTileType(pendingPlayerMovement(pnum).xTarg, _
                              pendingPlayerMovement(pnum).yTarg, _
                              pendingPlayerMovement(pnum).lTarg, _
                              LINK_EAST, _
                              ppos(pnum), _
                              didItem)
    
    'Advance the frame for all tile types (if SOLID, will walk on spot.)
    ppos(pnum).stance = "walk_e"
    
    ' MODIFIED BY KSNiloc...
    incrementFrame ppos(pnum).frame
    'ppos(pNum).frame = ppos(pNum).frame + 1
    
    Select Case typetile
    
        Case NORMAL, UNDER:
            'See if we need to scroll...
            
            Dim scrollEast As Boolean

            scrollEast = checkScrollEast(pnum) 'New function! Cleaned up a bit.
            
            If scrollEast Then Call scrollLeft(moveFraction)
            'shift the screen drawing co-ords left (topX).
            
            Call incrementPosition(ppos(pnum), pendingPlayerMovement(pnum), moveFraction)
                        
        'Case SOLID:
            'Walk on the spot.
    End Select
End Sub

Sub pushPlayerWest(ByVal pnum As Long, ByVal moveFraction As Double)
    '======================================
    'EDITED: [Isometrics - Delano 11/05/04]
    'Substituted tile type constants.
    'Moved scroll checking to a new function.
    '======================================
    
    'Push player pNum NorthEast by moveFraction.
    'Called by movePlayers, each frame of the movement cycle (currently 4 times).
    'Calls incrementPosition if movement is possible, and scrollNorthEast if scrolling required.
    
    On Error Resume Next

    fightInProgress = False
    stepsTaken = stepsTaken + 1
    
    'Before doing anything, let's see if we are going off the board.
    'Checks for links and will send to new board if a link is possible.
    Dim bWentOffEdge As Boolean
    bWentOffEdge = CheckEdges(pendingPlayerMovement(pnum), pnum)
    
    If bWentOffEdge Then
        pendingPlayerMovement(pnum).direction = MV_IDLE
        Exit Sub
    End If

    'obtain the tile type at the target...
    Dim didItem As Boolean
    Dim typetile As Long
    typetile = ObtainTileType(pendingPlayerMovement(pnum).xTarg, _
                              pendingPlayerMovement(pnum).yTarg, _
                              pendingPlayerMovement(pnum).lTarg, _
                              LINK_WEST, _
                              ppos(pnum), _
                              didItem)
    
    'Advance the frame for all tile types (if SOLID, will walk on spot.)
    ppos(pnum).stance = "walk_w"
    ' MODIFIED BY KSNiloc...
    incrementFrame ppos(pnum).frame
    'ppos(pNum).frame = ppos(pNum).frame + 1
    
    Select Case typetile
        Case NORMAL, UNDER:
            'see if we need to scroll...
            
            Dim scrollWest As Boolean
            
            scrollWest = checkScrollWest(pnum) 'New function!
            
            If scrollWest Then Call scrollRight(moveFraction)
            'shift the screen drawing co-ords right (topX).
            
            Call incrementPosition(ppos(pnum), pendingPlayerMovement(pnum), moveFraction)
            
        'Case SOLID:
            'Walk on the spot.
    End Select
End Sub

Public Function roundUp(ByVal number As Double) As Double
    'ADDED BY KSNiloc...

    If Int(number) = number Then
        roundUp = number
        Exit Function
    End If

    If onlyDecimal(number) < 0.25 Then
        roundUp = Int(number)
        Exit Function
    End If
    
    roundUp = Int(number) + 1
    
End Function

Public Function roundDown(ByVal number As Double) As Double
    'ADDED BY KSNiloc...

    If Int(number) = number Then
        roundDown = number
        Exit Function
    End If

    If onlyDecimal(number) < 0.25 Then
        roundDown = Int(number)
        Exit Function
    End If
    
    roundDown = Int(number) - 1
    
End Function

Public Function roundCoords( _
                               ByRef passPos As PLAYER_POSITION, _
                               Optional ByVal linkDirection As Long = 0 _
                                                                          ) As PLAYER_POSITION

    '=============================================
    'Rounds player coordinates [KSNiloc]
    '=============================================
    
    'Called by programTest, passing in the target co-ordinates after (pixel) movement.
    
    Dim pos As PLAYER_POSITION
    pos = passPos
    

    If boardIso() Then
    
    Else
        'Non-isometric.
        Select Case linkDirection

            Case LINK_NORTH
                pos.x = Round(pos.x)
                pos.y = Int(pos.y)

            Case LINK_SOUTH
                pos.x = Round(pos.x)
                pos.y = -Int(-pos.y)

            Case LINK_EAST
                pos.x = -Int(-pos.x)
                pos.y = Round(pos.y)

            Case LINK_WEST
                pos.x = Int(pos.x)
                pos.y = Round(pos.y)

            'Case LINK_NE
            
            'Case LINK_NW
            
            'Case LINK_SE
            
            'Case LINK_SW

            Case Else
                pos.x = Round(pos.x)
                pos.y = Round(pos.y)

        End Select

    End If

    roundCoords = pos

End Function

Public Function ObtainTileType( _
                                  ByVal testX As Long, _
                                  ByVal testY As Long, _
                                  ByVal testLayer As Long, _
                                  ByVal thelink As Long, _
                                  ByRef passPos As PLAYER_POSITION, _
                                  ByRef didItem As Boolean _
                                                             ) As Integer
    
    '=====================================================================================
    'Modified by KSNiloc
    '
    'NOTES: Modified to work with pixel movement
    '
    'BUG FIX: You can no longer walk 'onto' NPCs.
    '=====================================================================================
    
    'look at testX, testY, testLayer
    'to find the tile type (also based upon the theLink direction to which we are moving)
    'if there's an item or something we return solid.
    'will run programs if necissary
    'will move to new layer if stairs encountered.
    'also return the diditem constant in the args passed in.
    '========================================================
    'Edited by Delano 28/06/04 for 3.0.4
    'Added code to prevent players walking across the corners
    'of solid tiles in isometrics.

    'thelink is the direction of movement, rather than the linking board.
    'Called by the pushPlayer subs.
    On Error Resume Next
    
    testLayer = Round(testLayer)

    Dim typetile As Byte
    'Tiletype at the target.
    'typetile = boardList(activeBoardIndex).theData.tiletype(testX, testY, testLayer)
    
    Dim first As Byte, second As Byte
    With boardList(activeBoardIndex).theData
        Select Case thelink
            Case LINK_NORTH:
                first = .tiletype(Int(testX), Int(testY), testLayer)
                second = .tiletype(-Int(-testX), Int(testY), testLayer)
            Case LINK_SOUTH:
                first = .tiletype(Int(testX), -Int(-testY), testLayer)
                second = .tiletype(-Int(-testX), -Int(-testY), testLayer)
            Case LINK_EAST:
                first = .tiletype(-Int(-testX), -Int(-testY), testLayer)
                second = .tiletype(-Int(-testX), Int(testY), testLayer)
            Case LINK_WEST:
                first = .tiletype(Int(testX), -Int(-testY), testLayer)
                second = .tiletype(Int(testX), Int(testY), testLayer)
            Case LINK_NE:
                typetile = .tiletype(-Int(-testX), Int(testY), testLayer)
            Case LINK_NW:
                typetile = .tiletype(Int(testX), Int(testY), testLayer)
            Case LINK_SE:
                typetile = .tiletype(-Int(-testX), -Int(-testY), testLayer)
            Case LINK_SW:
                typetile = .tiletype(Int(testX), -Int(-testY), testLayer)
        End Select
    End With
    
    Dim a As Byte
    For a = 1 To 18
        If first = a Or second = a Then
            typetile = a
            Exit For
        End If
    Next a

    'check if an item is blocking...
    Dim itemBlocking As Long
    itemBlocking = CheckObstruction(testX, testY, testLayer)
    
    didItem = False
    If itemBlocking = 1 Then
        'TODO
        'Call programtest(testX, testY, testLayer, keycode, facing)
        didItem = True
        typetile = SOLID
    End If
    
    Dim underneath As Long
    'check for tiles above...
    underneath = checkAbove(testX, testY, testLayer)
    
    'if we're sitting on stairs, forget about tiles above.
    If typetile >= STAIRS1 And typetile <= STAIRS8 Then
        passPos.l = typetile - 10
        typetile = NORMAL
        underneath = 0
    End If
    
    If typetile = EAST_WEST And (thelink = LINK_EAST Or thelink = LINK_WEST) Then
        typetile = NORMAL   'if ew normal, carry on as if it were normal
    End If
    
    If typetile = NORTH_SOUTH And (thelink = LINK_SOUTH Or thelink = LINK_NORTH) Then
        typetile = NORMAL   'if ns normal, carry on as if it were normal
    End If
    
    If underneath = 1 And typetile <> SOLID Then
        typetile = UNDER
    End If
    
    'TODO
    'test if we're on stairs...
    'Dim testIt As Long
    'testIt = boardList(activeBoardIndex).theData.tiletype(testX, testY, testLayer)
    'If testIt >= STAIRS1 And testIt <= STAIRS8 Then
    '    pos.l = testIt - 10
    'End If
    
    'Added: Prevent players from crossing corners of solid tiles on isometric boards:
    Dim leftTile As Byte, rightTile As Byte, aboveTile As Byte, belowTile As Byte
    leftTile = NORMAL: rightTile = NORMAL: aboveTile = NORMAL: belowTile = NORMAL
    
    If boardIso() Then
        'Check if the tiles above and below the movement are solid.
        'We get the location with respect to the *test* (target) co-ordinates.
        Select Case thelink
            Case LINK_NORTH:
                If testY Mod 2 = 0 Then
                    'Even y
                    leftTile = boardList(activeBoardIndex).theData.tiletype(testX - 1, testY + 1, testLayer)
                    rightTile = boardList(activeBoardIndex).theData.tiletype(testX, testY + 1, testLayer)
                Else
                    'Odd y
                    leftTile = boardList(activeBoardIndex).theData.tiletype(testX, testY + 1, testLayer)
                    rightTile = boardList(activeBoardIndex).theData.tiletype(testX + 1, testY + 1, testLayer)
                End If
            Case LINK_SOUTH:
                If testY Mod 2 = 0 Then
                    'Even y
                    leftTile = boardList(activeBoardIndex).theData.tiletype(testX - 1, testY - 1, testLayer)
                    rightTile = boardList(activeBoardIndex).theData.tiletype(testX, testY - 1, testLayer)
                Else
                    'Odd y
                    leftTile = boardList(activeBoardIndex).theData.tiletype(testX, testY - 1, testLayer)
                    rightTile = boardList(activeBoardIndex).theData.tiletype(testX + 1, testY - 1, testLayer)
                End If
            Case LINK_EAST:
                If testY Mod 2 = 0 Then
                    'Even y
                    aboveTile = boardList(activeBoardIndex).theData.tiletype(testX - 1, testY - 1, testLayer)
                    belowTile = boardList(activeBoardIndex).theData.tiletype(testX - 1, testY + 1, testLayer)
                Else
                    'Odd y
                    aboveTile = boardList(activeBoardIndex).theData.tiletype(testX, testY - 1, testLayer)
                    belowTile = boardList(activeBoardIndex).theData.tiletype(testX, testY + 1, testLayer)
                End If
             Case LINK_WEST:
                If testY Mod 2 = 0 Then
                    'Even y
                    aboveTile = boardList(activeBoardIndex).theData.tiletype(testX, testY - 1, testLayer)
                    belowTile = boardList(activeBoardIndex).theData.tiletype(testX, testY + 1, testLayer)
                Else
                    'Odd y
                    aboveTile = boardList(activeBoardIndex).theData.tiletype(testX + 1, testY - 1, testLayer)
                    belowTile = boardList(activeBoardIndex).theData.tiletype(testX + 1, testY + 1, testLayer)
                End If
        End Select

        If (leftTile = SOLID Xor rightTile = SOLID) Or (aboveTile = SOLID Xor belowTile = SOLID) Then
            'Block the movement if one adajecent tile is solid, but not both (Xor).
            'Two solid tiles suggests the player should be able to pass between the tiles.
            typetile = SOLID
        End If
    End If
    
    ObtainTileType = typetile
    
End Function


Sub moveItems()
    'move all pending items
    On Error Resume Next
    
    ' ! MODIFIED BY KSNiloc...
    If MAXITEM = -1 Then Exit Sub
    
    Dim maxP As Long
    maxP = UBound(pendingItemMovement)
       
    Dim t As Long
    
    Dim moveFraction As Double
    moveFraction = movementSize / framesPerMove
    
    For t = 0 To maxP
        Select Case pendingItemMovement(t).direction
            Case MV_IDLE:
                'this item isn't moving...
            Case MV_NORTH:
                Call pushItemNorth(t, moveFraction)
            Case MV_SOUTH:
                Call pushItemSouth(t, moveFraction)
            Case MV_EAST:
                Call pushItemEast(t, moveFraction)
            Case MV_WEST:
                Call pushItemWest(t, moveFraction)
            Case MV_NE:
                Call pushItemNorthEast(t, moveFraction)
            Case MV_NW:
                Call pushItemNorthWest(t, moveFraction)
            Case MV_SE:
                Call pushItemSouthEast(t, moveFraction)
            Case MV_SW:
                Call pushItemSouthWest(t, moveFraction)
        End Select
    Next t
End Sub


Public Sub movePlayers()

    '=====================================================================================
    'MODIFIED BY KSNiloc
    '
    'Modified to help prevent super fast character animation, especially now that we use
    'pixel movement.
    '=====================================================================================

    'move all pending players
    On Error Resume Next
    
    Dim maxP As Long
    maxP = UBound(pendingPlayerMovement)
    
    Dim t As Long
    
    Dim moveFraction As Double
    moveFraction = movementSize / framesPerMove

    ' ! ADDED BY KSNiloc...
    incrementFrame
    
    For t = 0 To maxP
        Select Case pendingPlayerMovement(t).direction
            Case MV_IDLE:
                'this player isn't moving...
            Case MV_NORTH:
                Call pushPlayerNorth(t, moveFraction)
            Case MV_SOUTH:
                Call pushPlayerSouth(t, moveFraction)
            Case MV_EAST:
                Call pushPlayerEast(t, moveFraction)
            Case MV_WEST:
                Call pushPlayerWest(t, moveFraction)
            Case MV_NE:
                Call pushPlayerNorthEast(t, moveFraction)
            Case MV_NW:
                Call pushPlayerNorthWest(t, moveFraction)
            Case MV_SE:
                Call pushPlayerSouthEast(t, moveFraction)
            Case MV_SW:
                Call pushPlayerSouthWest(t, moveFraction)
        End Select
    Next t
    
    ' ! ADDED BY KSNiloc...
    incrementFrame -2
    
End Sub

Public Sub incrementFrame(Optional ByRef frame As Long = -1)

    '=====================================================================================
    'Increments the frame a player walking animation is on [KSNiloc]
    '
    'PURPOSE:   Even with pixel movement, we want the animation to play four frames a
    '           tile in order to prevent 'super-fast' animation
    '
    'ARGUMENT:  Passed the .frame property of a player position (i.e. ppos(t).frame)
    '
    'NOTES:     Called only by the PushPlayer[Direction]() subs and movePlayers()
    '
    '=====================================================================================

    Dim fraction As Double
    fraction = 1 / framesPerMove * animationDelay

    If frame = -2 Then
        If movedThisFrame >= fraction Then movedThisFrame = 0
    ElseIf frame = -1 Then
        movedThisFrame = movedThisFrame + movementSize
    Else
        If movedThisFrame >= fraction Then
            'We HAVE moved a quarter of a tile!
            frame = frame + 1
        End If
    End If

End Sub

Sub runQueuedMovements()
    'run all player and item movements currently pending
    'without regard for gamestate (like if gamestate is GD_MOVEMENT or not)
    'won't run programs and stuff when player moves.
    On Error Resume Next

    'movement has occurred...
    Dim cnt As Long
    For cnt = 1 To framesPerMove
        Call moveItems
        Call movePlayers
        Call renderNow
    Next cnt

    For cnt = 0 To UBound(pendingPlayerMovement)
        pendingPlayerMovement(cnt).direction = MV_IDLE
    Next cnt

    For cnt = 0 To UBound(pendingItemMovement)
        pendingItemMovement(cnt).direction = MV_IDLE
    Next cnt

    If UCase$(ppos(selectedPlayer).stance) = "WALK_S" Then facing = 1
    If UCase$(ppos(selectedPlayer).stance) = "WALK_W" Then facing = 2
    If UCase$(ppos(selectedPlayer).stance) = "WALK_N" Then facing = 3
    If UCase$(ppos(selectedPlayer).stance) = "WALK_E" Then facing = 4
End Sub


Public Function checkScrollNorth(ByVal playerNum As Long) As Boolean
'======================================================
'NEW FUNCTION: [Delano - 9/05/04]
'Clearing up the pushPlayer subs by placing similar scrolling checks in functions.
'======================================================
On Error Resume Next

    checkScrollNorth = True

    If boardIso() Then
        'If at top of board
        'OR in lower half of screen AND at the bottom of the board
        'OR in lower half of screen
        
        If ((topY * 2 + 1) - 1 < 0) Or _
            (ppos(playerNum).y > boardList(activeBoardIndex).theData.Bsizey - (isoTilesY / 2) And _
            (topY * 2) + 1 = boardList(activeBoardIndex).theData.Bsizey - isoTilesY) Or _
            (ppos(playerNum).y - ((topY * 2) + 1) > (isoTilesY / 2)) Or _
            playerNum <> selectedPlayer Then
            
            checkScrollNorth = False
        End If
    Else
        'Swapping " - 1 <" for "<=" (boards do not scroll to edges)
        
        If (topY <= 0) Or _
            (ppos(playerNum).y > boardList(activeBoardIndex).theData.Bsizey - (tilesY / 2) And _
            topY = boardList(activeBoardIndex).theData.Bsizey - tilesY) Or _
            (ppos(playerNum).y - topY > (tilesY / 2)) Or _
            playerNum <> selectedPlayer Then
            
            checkScrollNorth = False
        End If
    End If

End Function

Public Function checkScrollSouth(ByVal playerNum As Long) As Boolean
'======================================================
'NEW FUNCTION: [Delano - 9/05/04]
'Clearing up the pushPlayer subs by placing similar scrolling checks in functions.
'======================================================
On Error Resume Next

    checkScrollSouth = True

    If boardIso() Then
        'If at south edge of board
        'OR at north edge and pos less than half screen height
        'OR in middle and pos less than half screen height
        'Trading + 1 for ">="
        
        If ((topY * 2 + 1) + isoTilesY >= boardList(activeBoardIndex).theData.Bsizey) Or _
            (ppos(playerNum).y < (isoTilesY / 2) And topY = 0) Or _
            (ppos(playerNum).y - (topY * 2) < (isoTilesY / 2)) Or _
            playerNum <> selectedPlayer Then '^Doesn't work with topy * 2 + 1
            
            checkScrollSouth = False
        End If
    Else
        'Swapping " + 1 >" for ">=" (boards do not scroll to edges)
        
        If (topY + tilesY >= boardList(activeBoardIndex).theData.Bsizey) Or _
            (ppos(playerNum).y < (tilesY / 2) And topY = 0) Or _
            (ppos(playerNum).y - topY < (tilesY / 2)) Or _
            playerNum <> selectedPlayer Then
            
            checkScrollSouth = False
        End If
    End If


End Function

Public Function checkScrollEast(ByVal playerNum As Long) As Boolean
'======================================================
'NEW FUNCTION: [Delano - 9/05/04]
'Clearing up the pushPlayer subs by placing similar scrolling checks in functions.
'======================================================
On Error Resume Next

    checkScrollEast = True

    If boardIso() Then
        'If at east edge of board
        'OR at west edge and pos less than half screen width
        'OR in middle and pos less than half screen width
        'Trading + 1 for ">=", adding + 0.5 because each tile has two columns
        
        If (topX + isoTilesX + 0.5 >= boardList(activeBoardIndex).theData.Bsizex) Or _
            (ppos(playerNum).x < (isoTilesX / 2) And topX = 0) Or _
            (ppos(playerNum).x - topX < (isoTilesX / 2)) Or _
            playerNum <> selectedPlayer Then
            
            checkScrollEast = False
        End If
    Else
        'Swapping " + 1 >" for ">=" (boards do not scroll to edges)
        
        If (topX + tilesX >= boardList(activeBoardIndex).theData.Bsizex) Or _
            (ppos(playerNum).x < (tilesX / 2) And topX = 0) Or _
            (ppos(playerNum).x - topX < (tilesX / 2)) Or _
            playerNum <> selectedPlayer Then
            
            checkScrollEast = False
        End If
    End If

End Function


Public Function checkScrollWest(ByVal playerNum As Long) As Boolean
'======================================================
'NEW FUNCTION: [Delano - 11/05/04]
'Clearing up the pushPlayer subs by placing similar scrolling checks in functions.
'======================================================
On Error Resume Next

    checkScrollWest = True
    
    If boardIso() Then
        'If at right edge of board AND on right side of screen
        'OR on right side of screen
        'OR at left edge of board.
        'isoTopX = topX + 1
        
        If (ppos(playerNum).x > boardList(activeBoardIndex).theData.Bsizex - (isoTilesX / 2) And _
            topX + 1 = boardList(activeBoardIndex).theData.Bsizex - isoTilesX) Or _
            (ppos(playerNum).x - (topX + 1) > (isoTilesX / 2)) Or _
            ((topX + 1) - 1 < 0) Or _
            playerNum <> selectedPlayer Then
            
            checkScrollWest = False
        End If
    Else
        'Swapping " - 1 <" for "<=" (boards do not scroll to edges)
        
        If (ppos(playerNum).x > boardList(activeBoardIndex).theData.Bsizex - (tilesX / 2) And _
            topX = boardList(activeBoardIndex).theData.Bsizex - tilesX) Or _
            (ppos(playerNum).x - topX > (tilesX / 2)) Or _
            (topX <= 0) Or _
            playerNum <> selectedPlayer Then
            
            checkScrollWest = False
        End If
    End If

End Function


Function TestBoard(ByVal file As String, ByVal testX As Long, ByVal testY As Long, ByVal testL As Long) As Long
'==========================
'EDITED: [Delano - 1/05/04]
'Fixed variant data types.
'Removed unused code.
'Renamed variables: XXX,YYY,layer >> testX,testY,testL
'MOVED from Mod commonBoard to transMovement; was unused in toolkit3.
'==========================
'Called by: TestLink and Send only (in trans3).

'Tests if we can go to x,x,layer on specified board.
'Returns -1 if we cannot, otherwise it returns the tiletype

    On Error Resume Next
    
    Dim test As Boolean
    test = fileExists(file$)
    
    If Not (pakFileRunning) Then
        If Not (test) Then
            TestBoard = -1
            Exit Function
        End If
    End If

    Dim aBoard As TKBoard
    Call openboard(file$, aBoard)
    lastRender.canvas = -1
    If testX > aBoard.Bsizex Or testY > aBoard.Bsizey Or testL > aBoard.Bsizel Then
        TestBoard = -1
        Exit Function
    End If
    TestBoard = aBoard.tiletype(testX, testY, testL)

End Function
