Attribute VB_Name = "transLocate"
'=========================================================================
'All contents copyright 2003, 2004, Christopher Matthews or Contributors
'All rights reserved.  YOU MAY NOT REMOVE THIS NOTICE.
'Read LICENSE.txt for licensing info
'=========================================================================

'=========================================================================
' Manages board coordiantes
' Status: B+
'=========================================================================

Option Explicit

'=========================================================================
' Integral variables
'=========================================================================
Public movementSize As Double    'movement size (in tiles)

Public Function isoCoordTransform(ByVal oldX As Double, ByVal oldY As Double, _
                                  ByRef newX As Double, ByRef newY As Double)
'======================================================
'Transform old-type isometric co-ordinates to new-type.
'======================================================
'By Delano for 3.0.5

    If boardIso() Then
        newX = oldX + Int((oldY - 1) / 2)
        newY = Int(oldY / 2) + 1 - Int(oldX) + (oldY - Int(oldY))
        
        newY = newY + boardList(activeBoardIndex).theData.bSizeX
    Else
        newX = oldX
        newY = oldY
    End If
                                
End Function

Public Function invIsoCoordTransform(ByVal newX As Double, ByVal newY As Double, _
                                     ByRef oldX As Double, ByRef oldY As Double)
'==============================================================
'Inverse transform old-type isometric co-ordinates to new-type.
'==============================================================
'By Delano for 3.0.5

    If boardIso() Then
    
        newY = newY - boardList(activeBoardIndex).theData.bSizeX
    
        If Int(newX) Mod 2 = 0 Then
            oldX = Int(newX / 2) - Int((newY - 1) / 2) + (newX - Int(newX))
            oldY = Int(newX) + newY
        Else
            oldX = Int((newX + 1) / 2) - Int(newY / 2) + (newX - Int(newX))
            oldY = Int(newX) + newY
        End If
        
    Else
        oldX = newX
        oldY = newY
    End If
                                
End Function

'=========================================================================
' Return if we are using pixel movement
'=========================================================================
Public Property Get usingPixelMovement() As Boolean
    usingPixelMovement = (movementSize <> 1)
End Property

'=========================================================================
' Return if we're on an isometric board
'=========================================================================
Public Property Get boardIso() As Boolean
    boardIso = (boardList(activeBoardIndex).theData.isIsometric = 1)
End Property

'=========================================================================
' Return if the board passed in is isometric
'=========================================================================
Public Function linkIso(ByVal linkBoard As String) As Boolean
    On Error Resume Next
    If Not fileExists(linkBoard) Then
        Exit Function
    End If
    lastRender.canvas = -1
    Dim brd As TKBoard
    Call openBoard(linkBoard, brd)
    linkIso = (brd.isIsometric = 1)
End Function

'=========================================================================
' Get the x coord at the bottom center of a board
' Called by putSpriteAt, checkScrollEast, checkScrollWest
'=========================================================================
Public Function getBottomCentreX(ByVal boardX As Double, ByVal boardY As Double) As Long
                                 
    On Error Resume Next
    
    'Co-ordinate transforms for isometrics from 3.0.5!
    
    If boardIso() Then
    
        Call isoCoordTransform(boardX, boardY, boardX, boardY)
        getBottomCentreX = Int((boardX - (boardY - boardList(activeBoardIndex).theData.bSizeX) - topX * 2) * 32)

    Else

        '2D board - easy!
        getBottomCentreX = Int((boardX - topX) * 32 - 16)

    End If

End Function

'=========================================================================
' Get the y coord at the bottom center of a board
' Called by putSpriteAt, checkScrollNorth, checkScrollSouth
'=========================================================================
Public Function getBottomCentreY(ByVal boardX As Double, ByVal boardY As Double) As Long

    On Error Resume Next
    
    'Co-ordinate transforms for isometrics from 3.0.5!
    
    If boardIso() Then
    
        Call isoCoordTransform(boardX, boardY, boardX, boardY)
        getBottomCentreY = Int((boardX + (boardY - boardList(activeBoardIndex).theData.bSizeX) - (topY * 2 + 1)) * 16)
    
    Else
    
        getBottomCentreY = Int((boardY - topY) * 32)
        
    End If
    
End Function

'=========================================================================
' Increment a player's position on the board
'=========================================================================
Public Sub incrementPosition( _
                                ByRef pos As PLAYER_POSITION, _
                                ByRef pend As PENDING_MOVEMENT, _
                                ByVal moveFraction As Double _
                                                               )
    On Error Resume Next

    With pos

        If boardIso() Then
        
            'Co-ordinate transform!
            Call isoCoordTransform(.x, .y, .x, .y)
            
            Select Case pend.direction

                Case MV_NE
                    .x = .x
                    .y = .y - moveFraction

                Case MV_NW
                    .x = .x - moveFraction
                    .y = .y

                Case MV_SE
                    .x = .x + moveFraction
                    .y = .y

                Case MV_SW
                    .x = .x
                    .y = .y + moveFraction

                Case MV_NORTH
                    .x = .x - moveFraction
                    .y = .y - moveFraction

                Case MV_SOUTH
                    .x = .x + moveFraction
                    .y = .y + moveFraction

                Case MV_EAST
                    .x = .x + moveFraction
                    .y = .y - moveFraction

                Case MV_WEST
                    .x = .x - moveFraction
                    .y = .y + moveFraction

            End Select

            'Invert!
            Call invIsoCoordTransform(.x, .y, .x, .y)

        Else

            Select Case pend.direction

                Case MV_NE
                    .x = .x + moveFraction
                    .y = .y - moveFraction

                Case MV_NW
                    .x = .x - moveFraction
                    .y = .y - moveFraction

                Case MV_SE
                    .x = .x + moveFraction
                    .y = .y + moveFraction

                Case MV_SW
                    .x = .x - moveFraction
                    .y = .y + moveFraction

                Case MV_NORTH
                    .y = .y - moveFraction
                    
                    If .y < pend.yTarg Then .y = pend.yTarg
            
                Case MV_SOUTH
                    .y = .y + moveFraction
                    
                    If .y > pend.yTarg Then .y = pend.yTarg
            
                Case MV_EAST
                    .x = .x + moveFraction
                    
                    If .x > pend.xTarg Then .x = pend.xTarg
                
                Case MV_WEST
                    .x = .x - moveFraction
                    
                    If .x < pend.xTarg Then .x = pend.xTarg
                    
            End Select

        End If 'boardIso

    End With 'pos

End Sub

'=========================================================================
' Fill in tile target coordinates from pending movement
'=========================================================================
Public Sub insertTarget(ByRef pend As PENDING_MOVEMENT)

    'Called by moveItems and movePlayers only.
    'Called once in a movement cycle.

    On Error Resume Next

    'Catch the movementSize property (speed reasons)
    Dim stepSize As Double
    stepSize = movementSize

    With pend

        If boardIso() Then
        
            'Co-ordinate transform!!
            '============================================================
            Call isoCoordTransform(.xOrig, .yOrig, .xOrig, .yOrig)
            
            Select Case .direction

                Case MV_NE
                    .xTarg = .xOrig
                    .yTarg = .yOrig - stepSize
                
                Case MV_NW
                    .xTarg = .xOrig - stepSize
                    .yTarg = .yOrig
                
                Case MV_SE
                    .xTarg = .xOrig + stepSize
                    .yTarg = .yOrig

                Case MV_SW
                    .xTarg = .xOrig
                    .yTarg = .yOrig + stepSize
                
                Case MV_NORTH
                    .xTarg = .xOrig - stepSize
                    .yTarg = .yOrig - stepSize
                
                Case MV_SOUTH
                    .xTarg = .xOrig + stepSize
                    .yTarg = .yOrig + stepSize

                Case MV_EAST
                    .xTarg = .xOrig + stepSize
                    .yTarg = .yOrig - stepSize

                Case MV_WEST
                    .xTarg = .xOrig - stepSize
                    .yTarg = .yOrig + stepSize

                Case Else
                    .xTarg = .xOrig
                    .yTarg = .yOrig

            End Select
            
            Call invIsoCoordTransform(.xTarg, .yTarg, .xTarg, .yTarg)
            Call invIsoCoordTransform(.xOrig, .yOrig, .xOrig, .yOrig)       'Don't forget these!
            '========================================================
        
        Else
            '2D.
            Select Case .direction

                Case MV_NE
                    .xTarg = .xOrig + stepSize
                    .yTarg = .yOrig - stepSize

                Case MV_NW
                    .xTarg = .xOrig - stepSize
                    .yTarg = .yOrig - stepSize

                Case MV_SE
                    .xTarg = .xOrig + stepSize
                    .yTarg = .yOrig + stepSize

                Case MV_SW
                    .xTarg = .xOrig - stepSize
                    .yTarg = .yOrig + stepSize

                Case MV_NORTH
                    .xTarg = .xOrig
                    .yTarg = .yOrig - stepSize

                Case MV_SOUTH
                    .xTarg = .xOrig
                    .yTarg = .yOrig + stepSize

                Case MV_EAST
                    .xTarg = .xOrig + stepSize
                    .yTarg = .yOrig

                Case MV_WEST
                    .xTarg = .xOrig - stepSize
                    .yTarg = .yOrig

                Case Else
                    .xTarg = .xOrig
                    .yTarg = .yOrig

            End Select

        End If 'boardIso
        
       .lTarg = .lOrig
    
    End With 'pend

End Sub

Public Function roundCoords(ByRef passpos As PLAYER_POSITION, _
                            ByVal direction As Long) As PLAYER_POSITION
    '==================================================================
    'Rounds player coordinates [KSNiloc/Delano]
    '==================================================================
    
    'Called by programTest, passing in the target co-ordinates after (pixel) movement.
    
    'We want programs to trigger when it *appears* that the sprite is in far enough onto the
    'tile to trigger it.
    'Sprite size will vary widely, but we assume 32px wide, with "feet" at the very base of
    'the sprite.
    'Triggering will act differently for horizontal and vertical movement.
    '   Horizontally, the position of the base is well-defined, and it can be clearly seen
    '   when a player is aligned with the tile (y = 0.25 -> 1.00 == 4 quarters.)
    '   Vertically, it depends on the width of the player. Assuming 32px, the player will
    '   straddle the trigger tile from x = -0.25 -> 0.75 == 7 quarters, but if we disregard
    '   the first on either side, that leaves x = -0.75 -> 0.5 == 4 quarters which is better.
    '
    '   This does however lead to inconsistencies when walking onto tiles from different
    '   directions: walking up the side of a tile @ x = -0.25 or x = 0.75 won't trigger,
    '   whilst walking on the same spots horizontally will trigger.
    '
    '   There are also problems with diagonals: the corner sectors won't trigger because
    '   their co-ords correspond to other trigger spots.
    '
    '   Trigger programs run only once per tile by only running when first entering the tile.
    '   Decimal checks on the co-ords ensure this.

    Dim rx As Double, ry As Double, pos As PLAYER_POSITION
    
    If Not usingPixelMovement() Then
        roundCoords = passpos
        Exit Function
    End If
    
    pos = passpos                                           'Copy across to a local.
    With pos
        
        If boardIso() Then
            'The conditions are slightly different because the sprite's base is a different
            'shape. Also, directions have rotated.
            
            Call isoCoordTransform(.x, .y, .x, .y)
            
            Select Case direction
            
                'First, check technical East-West. Directions have rotated, so North is now
                'NorthEast, SouthEast is now South etc.
                Case MV_EAST, MV_SE, MV_SOUTH
                
                    If .x - Int(.x) = 1 - movementSize Then
                        rx = -Int(-.x)
                        If Abs(.y - Round(.y)) <= movementSize Then    '<= 1/4 [sprite width / 2]
                            ry = Round(.y)
                        End If
                    End If
                    
                Case MV_NORTH, MV_NW, MV_WEST
                
                    If .x - Int(.x) = movementSize Then
                        rx = Int(.x)
                        If Abs(.y - Round(.y)) <= movementSize Then    '<= 1/4
                            ry = Round(.y)
                        End If
                    End If
                    
            End Select
    
            Select Case direction
    
                'Now check technical North-South. Overwrite rx for diagonals if found.
                Case MV_NORTH, MV_NE, MV_EAST
    
                    If .y - Int(.y) = movementSize Then
                        ry = Int(.y)
                        If Abs(.x - Round(.x)) <= movementSize Then    '<= 1/4
                            rx = Round(.x)
                        End If
                    End If
    
                Case MV_WEST, MV_SW, MV_SOUTH
    
                    If .y - Int(.y) = 1 - movementSize Then
                        ry = -Int(-.y)
                        If Abs(.x - Round(.x)) <= movementSize Then    '<= 1/4
                            rx = Round(.x)
                        End If
                    End If
                    
                Case MV_SE, MV_NW
                Case Else
                
                    rx = Round(.x)
                    ry = Round(.y)
    
            End Select
            
            'All cases, assign what we've calculated.
            'Most of the time these will be zero, and no prg will trigger, which prevents
            'multiple runnings whilst walking over a tile.
            .x = rx
            .y = ry
            
'Call traceString("passPos.x=" & passPos.x & " passpos.y=" & passPos.y & " .x=" & .x & " .y=" & .y)
        
            Call invIsoCoordTransform(.x, .y, .x, .y)
    
        Else
    
            'Standard.
            Select Case direction
            
                'First, check East-West.
                Case MV_EAST, MV_NE, MV_SE
                
                    If .x - Int(.x) = movementSize Then
                        rx = -Int(-.x)
                    End If
                    
                Case MV_WEST, MV_NW, MV_SW
                
                    If .x - Int(.x) = 1 - movementSize Then
                        rx = Int(.x)
                    End If
                    
            End Select
    
            Select Case direction
    
                'Now check North-South. Overwrite rx for diagonals if found.
                Case MV_NORTH, MV_NE, MV_NW
    
                    If Int(.y) = .y Then
                        rx = Round(.x)
                    End If
    
                Case MV_SOUTH, MV_SE, MV_SW
    
                    If .y - Int(.y) = movementSize Then
                        rx = Round(.x)
                    End If
                    
                Case MV_EAST, MV_WEST
                Case Else
                
                    rx = Round(.x)
                    ry = Round(.y)
    
            End Select
            
            'All cases, assign what we've calculated.
            .x = rx
            .y = -Int(-.y)
    
        End If 'boardIso
        
    End With 'pos

    roundCoords = pos

End Function

Public Function activationCoords(ByRef passpos As PLAYER_POSITION, _
                                 ByRef roundPos As PLAYER_POSITION) As PLAYER_POSITION
'=====================================================================================
'Increment the player co-ords one tile from the direction they are facing, to test
'if items or programs lie directly in front of them.
'Called by programTest only.
'=====================================================================================
'By Delano for 3.0.5

Dim passX As Double, passY As Double
Dim ret As PLAYER_POSITION

    Call isoCoordTransform(passpos.x, passpos.y, passX, passY)
    
    If boardIso() Then
    
        'For iso px/tile we can't get closer than a tile (if solid).
        'If .y not integer (px}, it won't trigger, which is good because
        'we don't want it to unless we're right next to it.
        Select Case LCase$(passpos.stance)
            Case "walk_n", "stand_n"
            
                If passX = Int(passX) Then 'Pushing against a right-hand edge.
                    ret.x = passX - 1
                Else
                    ret.x = Round(passX)
                End If
                If passY = Int(passY) Then 'Pushing against a left-hand edge.
                    ret.y = passY - 1
                Else
                    ret.y = Round(passY)
                End If
            
            Case "walk_s", "stand_s"
            
                If passX = Int(passX) Then 'Pushing against a right-hand edge.
                    ret.x = passX + 1
                Else
                    ret.x = Round(passX)
                End If
                If passY = Int(passY) Then 'Pushing against a left-hand edge.
                    ret.y = passY + 1
                Else
                    ret.y = Round(passY)
                End If
                
            Case "walk_e", "stand_e"
            
                If passX = Int(passX) Then 'Pushing against an upper edge.
                    ret.x = passX + 1
                Else
                    ret.x = Round(passX)
                End If
                If passY = Int(passY) Then 'Pushing against a lower edge.
                    ret.y = passY - 1
                Else
                    ret.y = Round(passY)
                End If
            
            Case "walk_w", "stand_w"
            
                If passX = Int(passX) Then 'Pushing against an upper edge.
                    ret.x = passX - 1
                Else
                    ret.x = Round(passX)
                End If
                If passY = Int(passY) Then 'Pushing against a lower edge.
                    ret.y = passY + 1
                Else
                    ret.y = Round(passY)
                End If
                
            Case "walk_ne", "stand_ne"
                ret.y = passY - 1
                ret.x = Round(passX)
            Case "walk_nw", "stand_nw"
                ret.x = passX - 1
                ret.y = Round(passY)
            Case "walk_se", "stand_se"
                ret.x = passX + 1
                ret.y = Round(passY)
            Case "walk_sw", "stand_sw"
                ret.y = passY + 1
                ret.x = Round(passX)
        End Select
        
    Else
    
        'Using .stance because pend.direction could be mv_idle.
        Select Case LCase$(passpos.stance)
            Case "walk_n", "stand_n"
            
                ret.x = roundPos.x
                If usingPixelMovement Then
                    ret.y = Round(passpos.y)
                Else
                    ret.y = passpos.y - 1
                End If
                
            Case "walk_s", "stand_s"
            
                ret.x = roundPos.x
                ret.y = Int(passpos.y) + 1
                
            Case "walk_e", "stand_e"
            
                ret.x = Int(passpos.x) + 1
                ret.y = -Int(-passpos.y)
                
            Case "walk_w", "stand_w"
            
                ret.x = -Int(-passpos.x) - 1
                ret.y = -Int(-passpos.y)
                
            Case "walk_ne", "stand_ne"
            
                ret.x = Int(passpos.x) + 1
                If usingPixelMovement Then
                    ret.y = Round(passpos.y) - 1
                Else
                    ret.y = passpos.y - 1
                End If
                
            Case "walk_nw", "stand_nw"
            
                ret.x = -Int(-passpos.x) - 1
                If usingPixelMovement Then
                    ret.y = Round(passpos.y) - 1
                Else
                    ret.y = passpos.y - 1
                End If
                
            Case "walk_se", "stand_se"
            
                ret.x = Int(passpos.x) + 1
                ret.y = Int(passpos.y) - 1
                
            Case "walk_sw", "stand_sw"
            
                ret.x = -Int(-passpos.x) - 1
                ret.y = Int(passpos.y) + 1
                
       End Select
        
    End If 'boardIso
    
    activationCoords = ret

End Function
