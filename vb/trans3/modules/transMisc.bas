Attribute VB_Name = "transMisc"
'=========================================================================
'All contents copyright 2003, 2004, Christopher Matthews or Contributors
'All rights reserved.  YOU MAY NOT REMOVE THIS NOTICE.
'Read LICENSE.txt for licensing info
'=========================================================================

'=========================================================================
' Misc trans supporting functions
' Status: B+
'=========================================================================

Option Explicit

'=========================================================================
' Integral variables
'=========================================================================

Public target As Long             'targeted player number
Public targetType As TARGET_TYPE  'targetted type
Public Source As Long             'source player number
Public sourceType As TARGET_TYPE  'source type

Public Enum TARGET_TYPE           'targetted type
    TYPE_PLAYER                   '  player
    TYPE_ITEM                     '  item
    TYPE_ENEMY                    '  enemy
End Enum

'=========================================================================
' Determine a player's special moves
'=========================================================================
Public Function determineSpecialMoves(ByVal handle As String, ByRef fileList() As String) As Long
    On Error Resume Next
    Dim cnum As Long, t As Long, theMove As Long, cnt As Long, a As Long, l As String, expl As Double
    cnum = -1
    'figure out which char it is:
    For t = 0 To UBound(playerListAr)
        If UCase$(playerListAr(t)) = UCase$(handle$) Then cnum = t
    Next t
    If cnum = -1 Then Exit Function
    'Now figure out which move it is:
    theMove = -1
    cnt = 0
    For t = 0 To UBound(playerMem(cnum).smlist)
        Dim ignore As Long
        ignore = 0
        If (LenB(playerMem(cnum).smlist$(t))) Then
            'Now check if the player can use it yet:
            'First check exp.
            a = getIndependentVariable(playerMem(cnum).experienceVar$, l$, expl)
            If expl >= playerMem(cnum).spcMinExp(t) Then
                theMove = theMove + 1
                ignore = 1
                fileList$(cnt) = playerMem(cnum).smlist$(t)
                cnt = cnt + 1
            End If
            
            Dim lev As Double, txt As String, nn As Double
            If ignore <> 1 Then
                'now check level
                a = getIndependentVariable(playerMem(cnum).leVar$, l$, lev)
                If lev >= playerMem(cnum).spcMinLevel(t) Then
                    theMove = theMove + 1
                    ignore = 1
                    fileList$(cnt) = playerMem(cnum).smlist$(t)
                    cnt = cnt + 1
                End If
            End If
            
            If ignore <> 1 Then
                'now check conditioned var
                If (LenB(playerMem(cnum).spcVar$(t))) Then
                    a = getIndependentVariable(playerMem(cnum).spcVar$(t), txt$, nn)
                    If a = 0 Then
                        'numerical
                        If nn = val(playerMem(cnum).spcEquals$(t)) Then
                            theMove = theMove + 1
                            fileList$(cnt) = playerMem(cnum).smlist$(t)
                            cnt = cnt + 1
                        End If
                    End If
                    If a = 1 Then
                        'literal
                        If txt$ = playerMem(cnum).spcEquals$(t) Then
                            theMove = theMove + 1
                            fileList$(cnt) = playerMem(cnum).smlist$(t)
                            cnt = cnt + 1
                        End If
                    End If
                End If
            End If
        End If
    Next t
    determineSpecialMoves = cnt
End Function

'=========================================================================
' Returns if a number is within low and high
'=========================================================================
Public Function within(ByVal num As Double, ByVal low As Double, ByVal high As Double) As Long
    If ((num <= high) And (num >= low)) Then
        ' It's within!
        within = 1
    End If
End Function

'=========================================================================
' Centers the board on playerX, playerY
'=========================================================================
Public Sub alignBoard(ByVal playerX As Double, ByVal playerY As Double)
    
    On Error Resume Next
    
    'Clear the current screen co-ordinates.
    topX = -100: topY = -100
    
    Dim effectiveSizeX As Long
    Dim effectiveSizeY As Long
    Dim effectiveTilesX As Long
    Dim effectiveTilesY As Long
    
    If (boardList(activeBoardIndex).theData.isIsometric = 1) Then
        effectiveSizeX = boardList(activeBoardIndex).theData.bSizeX
        effectiveSizeY = boardList(activeBoardIndex).theData.bSizeY
        effectiveTilesX = tilesX \ 2 '= isoTilesX
        effectiveTilesY = tilesY * 2
    Else
        effectiveSizeX = boardList(activeBoardIndex).theData.bSizeX
        effectiveSizeY = boardList(activeBoardIndex).theData.bSizeY
        effectiveTilesX = tilesX
        effectiveTilesY = tilesY
    End If
    
    If effectiveSizeX <= effectiveTilesX Then
        'If board is smaller or equal to the screen size horizontally
        topX = -((effectiveTilesX - effectiveSizeX) \ 2)
    End If
    
    If effectiveSizeY <= effectiveTilesY Then
        'If board smaller or equal to the screen size vertically
        
        'NOTE: Bug in iso board drawing where negative topYs are not properly placed!!
        topY = -((effectiveTilesY - effectiveSizeY) \ 2)
        If (boardList(activeBoardIndex).theData.isIsometric = 1) Then topY = 0 'Take this line out when bug fixed!!
    End If
    
    Dim tempx As Double
    Dim tempy As Double
    'Player position - half screen size
    'This will centre the screen on the player, unless this is off the edges.
    tempx = playerX - effectiveTilesX \ 2
    tempy = playerY - effectiveTilesY \ 2

    If tempx < 0 Then 'If player on left of board less than half a screen from the edge.
        tempx = 0
    End If
    '[board]sizeX - [screen]tilesX = amount board is wider than screen
    ' ">=" sign important for isometrics!
    If tempx >= effectiveSizeX - effectiveTilesX Then
        'If player is on right more than half a screen from the edge.
        tempx = effectiveSizeX - effectiveTilesX
        'Isometric fix:
        If (boardList(activeBoardIndex).theData.isIsometric = 1) Then tempx = tempx - 0.5 - (tilesX Mod 2) / 2
    End If
    
    If tempy < 0 Then 'If player at top of board less than half a screen from the edge.
        tempy = 0
    End If
    
    'If player is at bottom more than half a screen from the edge.
    'Isometric fix:
    If (boardList(activeBoardIndex).theData.isIsometric = 1) Then
        '[board]sizeY - [screen]tilesY = amount board is taller than screen
        If tempy + 1 >= effectiveSizeY - effectiveTilesY Then
            tempy = effectiveSizeY - effectiveTilesY - 1
        End If
        tempy = tempy / 2
    Else
        If tempy > effectiveSizeY - effectiveTilesY Then
            tempy = effectiveSizeY - effectiveTilesY
        End If
    End If
    
    'If board is larger than screen.
    If topX = -100 Then
        topX = tempx
    End If
    If topY = -100 Then
        topY = tempy
    End If
End Sub

'=========================================================================
' Destroy all item sprite canvases
'=========================================================================
Public Sub destroyItemSprites()

    On Error GoTo fin

    ' Destroy item canvases
    Dim i As Long
    For i = 0 To UBound(boardList(activeBoardIndex).theData.itmActivate)
        Call destroyCanvas(cnvSprites(i))
    Next i

fin:
End Sub

'=========================================================================
' Opens all the items on the board
'=========================================================================
Public Sub openItems()

    On Error Resume Next

    Dim itemNum As Long, itemCount As Long

    ' Get total number of items
    itemCount = UBound(boardList(activeBoardIndex).theData.itmActivate)

    ' Resize item arrays
    ReDim pendingItemMovement(itemCount)
    ReDim lastItemRender(itemCount)
    ReDim itmPos(itemCount)
    ReDim itemMem(itemCount)
    ReDim cnvSprites(itemCount)

    ' Create new item canvases
    For itemNum = 0 To itemCount
        cnvSprites(itemNum) = createCanvas(1, 1)
    Next itemNum

    ' Loop over each item
    For itemNum = 0 To itemCount

        ' With the active board
        With boardList(activeBoardIndex).theData

            ' Copy data over to global arrays
            itmPos(itemNum).frame = 0
            itmPos(itemNum).loopFrame = -1
            itmPos(itemNum).x = .itmX(itemNum)
            itmPos(itemNum).y = .itmY(itemNum)
            itmPos(itemNum).l = .itmLayer(itemNum)
            itmPos(itemNum).stance = "walk_s"

            ' Indicate that there was no last render
            lastItemRender(itemNum).canvas = -1

            ' Copy values to pending item movements
            With pendingItemMovement(itemNum)
                .xOrig = itmPos(itemNum).x
                .xTarg = .xOrig
                .yOrig = itmPos(itemNum).y
                .yTarg = .yOrig
                .lOrig = itmPos(itemNum).l
                .lTarg = .lOrig
                .queue.lngSize = 0
                ReDim .queue.lngMovements(16)
            End With

            ' Check if we should open this item
            Dim runIt As Boolean

            ' If item is conditionally active
            If (.itmActivate(itemNum) = 1) Then

                ' Default to not opening the item
                runIt = False

                ' Get the value of the activation variable
                Dim lit As String, num As Double, checkIt As RPGC_DT
                checkIt = getIndependentVariable(.itmVarActivate(itemNum), lit, num)

                If (checkIt = DT_NUM) Then

                    ' It's numerical! Cast value to double and compare.
                    runIt = (num = CDbl(.itmActivateInitNum(itemNum)))

                ElseIf (checkIt = DT_LIT) Then

                    ' It's literal! Compare right off.
                    runIt = (lit = .itmActivateInitNum(itemNum))

                End If

            Else

                ' Item is always active
                runIt = True

            End If ' (.itmActivate(itemNum) = 1)

            ' If we decided to open the item
            If ((runIt) And (LenB(.itmName(itemNum)) <> 0)) Then

                ' Open the item
                itemMem(itemNum) = openItem(projectPath & itmPath & .itmName(itemNum))

                ' Flag the item is open
                itemMem(itemNum).bIsActive = True

                ' Determine which thread to use
                Dim multiPrg As String

                ' If this specific item has its own thread
                If (LenB(.itemMulti(itemNum))) Then

                    ' Use its program
                    multiPrg = .itemMulti(itemNum)

                Else

                    ' Else, use the .itm file's program
                    multiPrg = itemMem(itemNum).itmPrgOnBoard

                End If

                ' Create the thread
                Call createThread(projectPath & prgPath & multiPrg, False, itemNum)

            Else

                ' Flag item is not active
                itemMem(itemNum).bIsActive = False

            End If ' (runIt) And (LenB(.itmName(itemNum)) <> 0)

        End With ' boardList(activeBoardIndex).theData

    Next itemNum ' 0 To itemCount

End Sub

'=========================================================================
' Delay for the number of seconds passed in
'=========================================================================
Public Sub delay(ByVal sec As Double)
    On Error Resume Next
    Dim millis As Long
    millis = sec * 1000
    Call Sleep(millis)
End Sub

'=========================================================================
' Load a character into the slot passed in
'=========================================================================
Public Sub createCharacter(ByVal file As String, ByVal number As Long)
    On Error Resume Next
    If (number < 0 Or number > 4) Then Exit Sub
    Call openChar(file, playerMem(number))
    With playerMem(number)
        playerListAr(number) = .charname
        playerFile(number) = file
        Call setIndependentVariable(.experienceVar, CStr(.initExperience))
        Call setIndependentVariable(.defenseVar, CStr(.initDefense))
        Call setIndependentVariable(.fightVar, CStr(.initFight))
        Call setIndependentVariable(.healthVar, CStr(.initHealth))
        Call setIndependentVariable(.maxHealthVar, CStr(.initMaxHealth))
        Call setIndependentVariable(.nameVar, .charname)
        Call setIndependentVariable(.smVar, CStr(.initSm))
        Call setIndependentVariable(.smMaxVar, CStr(.initSmMax))
        Call setIndependentVariable(.leVar, CStr(.initLevel))
        Call calcLevels(playerMem(number))
    End With
End Sub

'=========================================================================
' Calculate level up stuff
'=========================================================================
Public Sub calcLevels(ByRef player As TKPlayer, Optional ByVal setInit As Boolean = True)
    On Error Resume Next
    With player
        If (setInit) Then
            .nextLevel = .levelType
            .levelProgression = .levelType - .initExperience
        End If
        ReDim .levelStarts(.maxLevel)
        Dim levIdx As Long, exp As Long
        .levelStarts(.initLevel) = .initExperience
        exp = .levelType
        For levIdx = (.initLevel + 1) To (.maxLevel)
            .levelStarts(levIdx) = .levelStarts(levIdx - 1) + exp
            If (.charLevelUpType = 0) Then
                exp = exp + .experienceIncrease
            Else
                exp = exp * (1 + .experienceIncrease)
            End If
        Next levIdx
    End With
End Sub

'=========================================================================
' Dertermines if a player is able to use an item
'=========================================================================
Public Function canPlayerUse(ByVal num As Long, ByRef anItem As TKItem) As Boolean

    On Error Resume Next
   
    Dim okAll As Long
    okAll = 0
    If anItem.usedBy = 0 Then
        okAll = 1
    Else
        Dim ll As Long
        For ll = 0 To UBound(anItem.itmChars)
            If (LenB(anItem.itmChars$(ll))) Then
                If UCase$(anItem.itmChars$(ll)) = UCase$(playerListAr$(num)) Then
                    okAll = 1
                End If
            End If
        Next ll
    End If
    canPlayerUse = (okAll = 1)
End Function

'=========================================================================
' Remove an equippable item
'=========================================================================
Public Sub removeEquip(ByVal equipNum As Long, ByVal playerNum As Long)

    On Error Resume Next

    Dim anItem As TKItem

    If (LenB(playerEquip$(equipNum, playerNum)) = 0) Then Exit Sub

    anItem = openItem(projectPath & itmPath & playerEquip$(equipNum, playerNum))
    If (LenB(anItem.prgRemove$)) Then
        Call runProgram(projectPath & prgPath & anItem.prgRemove$)
    End If

    'Put the equipment back in the item list:
    Call inv.addItem(playerEquip(equipNum, playerNum))
    playerEquip$(equipNum, playerNum) = vbNullString
    equipList$(equipNum, playerNum) = vbNullString 'What is equipped on each player (handle)

    Dim HPa As Long
    Dim SMa As Long
    Dim DPa As Long
    Dim FPa As Long
    Dim lit As String
    Dim maxHP As Double
    Dim maxSM As Double
    Dim hp As Double
    Dim sm As Double
    Dim dp As Double
    Dim fp As Double
    
    HPa = anItem.equipHP     'amount of HP added because of equipment.
    SMa = anItem.equipSM     'amt of smp added by equipment.
    DPa = anItem.equipDP     'amt of dp added by equipment.
    FPa = anItem.equipFP     'amt of fp added by equipment.

    Call getIndependentVariable(playerMem(playerNum).maxHealthVar$, lit$, maxHP)
    Call getIndependentVariable(playerMem(playerNum).smMaxVar$, lit$, maxSM)
    Call getIndependentVariable(playerMem(playerNum).healthVar$, lit$, hp)
    Call getIndependentVariable(playerMem(playerNum).smVar$, lit$, sm)
    Call getIndependentVariable(playerMem(playerNum).defenseVar$, lit$, dp)
    Call getIndependentVariable(playerMem(playerNum).fightVar$, lit$, fp)
    
    maxHP = maxHP - HPa
    If hp > maxHP Then hp = maxHP
    maxSM = maxSM - SMa
    If sm > maxSM Then sm = maxSM
    dp = dp - DPa
    fp = fp - FPa

    Call setIndependentVariable(playerMem(playerNum).defenseVar$, CStr(dp))
    Call setIndependentVariable(playerMem(playerNum).fightVar$, CStr(fp))
    Call setIndependentVariable(playerMem(playerNum).healthVar$, CStr(hp))
    Call setIndependentVariable(playerMem(playerNum).maxHealthVar$, CStr(maxHP))
    Call setIndependentVariable(playerMem(playerNum).smVar$, CStr(sm))
    Call setIndependentVariable(playerMem(playerNum).smMaxVar$, CStr(maxSM))
End Sub

'=========================================================================
' Equip an item to a player
'=========================================================================
Public Sub addEquip(ByVal equipNum As Long, ByVal playerNum As Long, ByVal file As String)

    On Error Resume Next
    
    Dim aFile As String
    Dim anItem As TKItem
    
    aFile = projectPath & itmPath & file
    anItem = openItem(aFile)

    Call inv.removeItem(file)
    playerEquip(equipNum, playerNum) = file
    equipList(equipNum, playerNum) = anItem.itemName

    'Modify HP,DP,etc
    equipHPadd(playerNum) = anItem.equipHP     'amount of HP added because of equipment.
    equipSMadd(playerNum) = anItem.equipSM     'amt of smp added by equipment.
    equipDPadd(playerNum) = anItem.equipDP     'amt of dp added by equipment.
    equipFPadd(playerNum) = anItem.equipFP     'amt of fp added by equipment.

    Dim lit As String
    Dim maxHP As Double
    Dim maxSM As Double
    Dim hp As Double
    Dim sm As Double
    Dim dp As Double
    Dim fp As Double

    Call getIndependentVariable(playerMem(playerNum).defenseVar, lit, dp)
    Call getIndependentVariable(playerMem(playerNum).fightVar, lit, fp)
    Call getIndependentVariable(playerMem(playerNum).maxHealthVar, lit, maxHP)
    Call getIndependentVariable(playerMem(playerNum).smMaxVar, lit, maxSM)

    dp = dp + anItem.equipDP
    fp = fp + anItem.equipFP
    maxHP = maxHP + anItem.equipHP
    maxSM = maxSM + anItem.equipSM

    Call setIndependentVariable(playerMem(playerNum).defenseVar, CStr(dp))
    Call setIndependentVariable(playerMem(playerNum).fightVar, CStr(fp))
    Call setIndependentVariable(playerMem(playerNum).maxHealthVar, CStr(maxHP))
    Call setIndependentVariable(playerMem(playerNum).smMaxVar, CStr(maxSM))

    'Run equip program:
    targetType = TYPE_PLAYER
    target = playerNum
    If (LenB(anItem.prgEquip)) Then
        Call runProgram(projectPath & prgPath & anItem.prgEquip)
    End If

End Sub

'=========================================================================
' Set the game speed.
' Sets the walking speed, moderated by loopOffset.
' LoopOffset alters the player moves by a percentage of the fps per value.
' -ve loopOffset decreases speed proportionally, +ve increases.
'=========================================================================
Public Sub gameSpeed(ByVal speed As Long)

    'Bound the value.
    If speed > MAX_GAMESPEED Then speed = MAX_GAMESPEED
    If speed < -MAX_GAMESPEED Then speed = -MAX_GAMESPEED
    
    'Set the global walking offset.
    'Take the -ve because we want +ve speed to reduce the frames-per-move.
    loopOffset = -speed

End Sub

'=========================================================================
' Returns radians from degrees
'=========================================================================
Public Function radians(ByVal degrees As Double) As Double
    On Error Resume Next
    Const PI = 3.14159265358979
    radians = degrees / 180 * PI
End Function
