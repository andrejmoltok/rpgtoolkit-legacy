Attribute VB_Name = "CommonAnimation"
'========================================================================
' The RPG Toolkit, Version 3
' This file copyright (C) 2007 Christopher Matthews & contributors
'
' Contributors:
'    - Colin James Fitzpatrick
'    - Jonathan D. Hughes
'========================================================================
'
' This program is free software; you can redistribute it and/or
' modify it under the terms of the GNU General Public License
' as published by the Free Software Foundation; either version 2
' of the License, or (at your option) any later version.
'
' This program is distributed in the hope that it will be useful,
' but WITHOUT ANY WARRANTY; without even the implied warranty of
' MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
' GNU General Public License for more details.
'
'========================================================================

'=========================================================================
' RPGToolkit animation file format (*.anm)
'=========================================================================

Option Explicit

'========================================================================
' Definition of a Tk Animation
'========================================================================
Type TKAnimation
    animSizeX As Integer            'width
    animSizeY As Integer            'height
    animFrames As Integer           'total number of frames
    animFrame(50) As String         'filenames of each image in animation
    animTransp(50) As Long          'Transparent color for frame
    animSound(50) As String         'sounds for each frame
    animPause As Double             'Pause length (sec) between each frame
    animCurrentFrame As Long        'current frame we are editing
    animGetTransp As Boolean        'currently getting transparent color?
    timerFrame As Long              'This number will be 0 to 29 to indicate how many
                                    'times the timer has clicked
    currentAnmFrame As Long         'currently animating frame
    animFile As String              'filename (no path)
    loop As Boolean                 'should this animation loop?
End Type

'========================================================================
' The stored data
'========================================================================
Type animationDoc
    animFile As String              'filename
    animNeedUpdate As Boolean       'Needs to be updated?
    theData As TKAnimation          'Stored Data
End Type

'========================================================================
' One frame of an animation
'========================================================================
Private Type AnimationFrame
    cnv As Long                     ' Canvas of frame
    file As String                  ' Animation filename
    frame As Long                   ' Frame number
    maxFrames As Long               ' Max frames in this anim
    strSound As String              ' Sound played on this frame
End Type

'========================================================================
' Other variables
'========================================================================

' Array of animations that can be created by a plugin
Public anmList() As animationDoc
Public anmListOccupied() As Boolean

' Cache of animation frames
Private anmCache() As AnimationFrame

' Next index for animation cache
Private nextAnmCacheIdx As Long

Declare Function TransparentBlt Lib "msimg32" (ByVal hdcDest As Long, _
                                               ByVal nXOriginDest As Long, _
                                               ByVal nYOriginDest As Long, _
                                               ByVal nWidthDest As Long, _
                                               ByVal nHeightDest As Long, _
                                               ByVal hdcSrc As Long, _
                                               ByVal nXOriginSrc As Long, _
                                               ByVal nYOriginSrc As Long, _
                                               ByVal nWidthSrc As Long, _
                                               ByVal nHeightSrc As Long, _
                                               ByVal crTransparent As Long) As Long

Private Const FILE_HEADER = "RPGTLKIT ANIM"

'========================================================================
' Open the animation
'========================================================================
Public Sub openAnimation(ByVal file As String, ByRef theAnim As TKAnimation)

    On Error Resume Next

    'Get ready to read the animation
    If Not fileExists(file) Then Exit Sub
    Call AnimationClear(theAnim)
    file = PakLocate(file)
#If isToolkit = 1 Then
    animationList(activeAnimationIndex).animNeedUpdate = False
#End If

    Dim num As Long         'file position
    Dim frameIdx As Long    'frame index
    Dim majorVer As Long    'major version of TK

    num = FreeFile()

    With theAnim

        Open file For Binary Access Read As num
            If (BinReadString(num) = FILE_HEADER) Then
                'Binary mode!
                Call BinReadInt(num)
                If (BinReadInt(num) = 3) Then
                    'New binary animation format
                    .animSizeX = BinReadLong(num)
                    .animSizeY = BinReadLong(num)
                    .animFrames = BinReadLong(num)
                    For frameIdx = 0 To (.animFrames)
                        .animFrame(frameIdx) = BinReadString(num)
                        .animTransp(frameIdx) = BinReadLong(num)
                        .animSound(frameIdx) = BinReadString(num)
                    Next frameIdx
                    .animPause = BinReadDouble(num)
                Else
                    Call MsgBox("This is not a valid animaton file. " & file)
                End If
            Else
                'Not in binary mode!
                Close num
                Open file For Input Access Read As num
                If fread(num) <> FILE_HEADER Then
                    Close num
                    Call MsgBox("This is not a valid animaton file. " & file)
                    Exit Sub
                End If
                majorVer = fread(num)
                Call fread(num)
                If majorVer <> major Then
                    Call MsgBox("This animation was created with an unrecognised" _
                                & " version of the Toolkit", , "Unable to open animation")
                    Close num
                    Exit Sub
                End If
                .animSizeX = fread(num)
                .animSizeY = fread(num)
                .animFrames = fread(num)
                For frameIdx = 0 To 50
                    .animFrame(frameIdx) = fread(num)
                    .animTransp(frameIdx) = fread(num)
                    .animSound(frameIdx) = fread(num)
                Next frameIdx
                .animPause = fread(num)
            End If
        Close num

        .animFile = RemovePath(file)

    End With

End Sub

'========================================================================
' Clear animation
'========================================================================
Public Sub AnimationClear(ByRef theAnim As TKAnimation)
    On Error Resume Next

    theAnim.animSizeX = 64                  'X-size
    theAnim.animSizeY = 64                  'Y-size
    theAnim.animFrames = 0                  'Total number of frames
    Dim t As Long
    For t = 0 To UBound(theAnim.animFrame)  'Clears the frames
        theAnim.animFrame(t) = vbNullString
        theAnim.animTransp(t) = 0
        theAnim.animSound(t) = vbNullString
    Next t
    theAnim.animPause = 0                   'Pause length (sec) between each frame

    theAnim.animCurrentFrame = 0            'Currentframe
    theAnim.animGetTransp = False           'Currently getting transparent color?
End Sub

'========================================================================
' Get the frame count of animation at idx
'========================================================================
Public Function AnimationIndexMaxFrames(ByVal idx As Long) As Long
    On Error Resume Next
    If anmListOccupied(idx) Then
        AnimationIndexMaxFrames = animGetMaxFrame(anmList(idx).theData)
    End If
End Function

'========================================================================
' Delays for x numbers of seconds
'========================================================================
Public Sub animDelay(ByVal seconds As Double): On Error Resume Next

    Dim startTime As Double
    startTime = timer()

    Do While timer() - startTime < seconds

        ' This loop is the "delay", during this loop nothing happens.
        ' Process user input for trans3.
#If isToolkit = 0 Then
        Call processEvent
#End If

    Loop

End Sub

'========================================================================
' Animate at x, y (Animation is presumed to be loaded)
' Called by: trans3 (none)
'            picPlay_Click (characterGraphics, itemGraphics)
'            playAnimation (AnimationHost)
'            animPlay      (AnimationEditor)
'========================================================================
Public Sub AnimateAt(ByRef theAnim As TKAnimation, _
                     ByVal x As Long, ByVal y As Long, _
                     ByVal pixelsMaxX As Long, ByVal pixelsMaxY As Long, _
                     ByRef pic As PictureBox)
    
    On Error Resume Next
    
    Dim i As Long
    
    For i = 0 To animGetMaxFrame(theAnim)
        pic.Cls
        Call AnimDrawFrame(theAnim, i, x, y, pic.hdc)
        pic.Refresh
        Call animDelay(theAnim.animPause)
    Next i

End Sub

Public Sub AnimDrawFrame(ByRef theAnim As TKAnimation, ByVal framenum As Long, ByVal x As Long, ByVal y As Long, ByVal hdc As Long, Optional ByVal playSound As Boolean = True)
'================================================
'draw the frame referenced by framenum
'loads a file into a picture box and resizes it.
'Called by: DrawAnimationIndex (unused), AnimateAt (toolkit3 only) - not used in trans3
'================================================

    On Error Resume Next

    Dim ex As String, tbm As TKTileBitmap
    
    ex = GetExt(theAnim.animFrame(framenum))
    
    If fileExists(projectPath & bmpPath & theAnim.animFrame(framenum)) Or Left$(UCase$(ex), 3) = "TST" Then
        
        If UCase$(ex) = "TBM" Then
        
            Call DrawSizedImage(projectPath & bmpPath & theAnim.animFrame(framenum), _
                                x, y, _
                                theAnim.animSizeX, _
                                theAnim.animSizeY, _
                                hdc)
        
        ElseIf Left$(UCase$(ex), 3) = "TST" Or UCase$(ex) = "GPH" Then
        
            Call TileBitmapSize(tbm, 1, 1)
            tbm.tiles(0, 0) = theAnim.animFrame(framenum)
            Call DrawSizedTileBitmap(tbm, x, y, theAnim.animSizeX, theAnim.animSizeY, hdc)
            
        Else
        
            Dim cnv As Long, anmHdc As Long
            cnv = createCanvas(theAnim.animSizeX, theAnim.animSizeY)
            Call canvasLoadSizedPicture(cnv, projectPath & bmpPath & theAnim.animFrame(framenum))

            'Blt it on
            anmHdc = canvasOpenHDC(cnv)
            'Drawing the frame without the transparent color causes confusion.
            'Also has a memory leak on Win9x.
            'Call TransparentBlt(hdc, _
                                x, _
                                y, _
                                theAnim.animSizeX - 1, _
                                theAnim.animSizeY - 1, _
                                anmHDC, _
                                0, _
                                0, _
                                theAnim.animSizeX - 1, _
                                theAnim.animSizeY - 1, _
                                theAnim.animTransp(framenum))
            Call StretchBlt(hdc, _
                            x, y, _
                            theAnim.animSizeX, _
                            theAnim.animSizeY, _
                            anmHdc, _
                            0, 0, _
                            theAnim.animSizeX, _
                            theAnim.animSizeY, _
                            SRCCOPY)
            
            Call canvasCloseHDC(cnv, anmHdc)
            Call destroyCanvas(cnv)
           
        End If 'UCase$(ex) = "TBM"
            
    End If 'fileExists

    If (playSound) And (LenB(theAnim.animSound(framenum)) <> 0) Then
        Call sndPlaySound(projectPath & mediaPath & theAnim.animSound(framenum), SND_ASYNC Or SND_NODEFAULT)
    End If

End Sub

Public Function animGetMaxFrame(ByRef theAnim As TKAnimation) As Long
    On Error Resume Next
    animGetMaxFrame = -1
    Dim frameIdx As Long
    For frameIdx = 0 To UBound(theAnim.animFrame)
        If LenB(theAnim.animFrame(frameIdx)) Then
            animGetMaxFrame = animGetMaxFrame + 1
        End If
    Next frameIdx
End Function

#If (isToolkit = 0) Then

'========================================================================
' Get the current frame of animation at idx
'========================================================================
Public Function AnimationIndexCurrentFrame(ByVal idx As Long) As Long
    On Error Resume Next
    If anmListOccupied(idx) Then
        AnimationIndexCurrentFrame = anmList(idx).theData.currentAnmFrame
    End If
End Function

'========================================================================
' Clear the animation cache
'========================================================================
Public Sub clearAnmCache(): On Error Resume Next

    ' Kill all cache entires
    Dim i As Long
    For i = 0 To UBound(anmCache)
        Call destroyCanvas(anmCache(i).cnv)
        anmCache(i).cnv = 0
        anmCache(i).file = vbNullString
        anmCache(i).frame = 0
        anmCache(i).maxFrames = 0
        anmCache(i).strSound = vbNullString
    Next i

    ' Flag to use first position again
    nextAnmCacheIdx = 0

End Sub

'========================================================================
' Shutdown animation system
'========================================================================
Public Sub AnimationShutdown()
    On Error Resume Next
    Dim t As Long
    For t = 0 To UBound(anmCache)
        Call destroyCanvas(anmCache(t).cnv)
    Next t
End Sub

'========================================================================
' Initialize the sprite system
'========================================================================
Public Sub initSprites()

    ' Make some space in the animation cache
    ReDim anmCache(250)

End Sub

'========================================================================
' Render an animation frame at canvas cnv, file if the animation filename,
' frame is the frame. Checks through the animation cache for previous
' renderings of this frame, if not found, it is rendered here and copied
' to the animation cache.
'========================================================================
Public Sub renderAnimationFrame(ByVal cnv As Long, ByRef file As String, ByVal frame As Long, ByVal x As Long, ByVal y As Long)

    '// Passing string(s) ByRef for preformance related reasons

    Dim anm As TKAnimation, maxF As Long

    'Whatever the case, clear the canvas in case the character has no graphics,
    'or the animation can't be loaded.
    Call canvasFill(cnv, TRANSP_COLOR)

    If (LenB(file) = 0) Then
        ' Bail if we were passed NULL
        Exit Sub
    End If

    ' Get canvas width and height
    Dim w As Long, h As Long
    w = getCanvasWidth(cnv)
    h = getCanvasHeight(cnv)
    
    ' Capitalize the file
    file = UCase$(file)     ' Safe because this is never passed important things

    ' First check sprite cache
    Dim t As Long
    Dim cacheFrameWidth As Long, cacheFrameHeight As Long
    For t = 0 To UBound(anmCache)

        If (anmCache(t).file = file) Then   ' All files in cache are already capital

            maxF = anmCache(t).maxFrames
            frame = frame Mod (maxF + 1)

            If (anmCache(t).frame = frame) Then

                ' Resize target canvas, if required
                cacheFrameWidth = getCanvasWidth(anmCache(t).cnv)
                cacheFrameHeight = getCanvasHeight(anmCache(t).cnv)
                
                If (w <> cacheFrameWidth Or h <> cacheFrameHeight) Then
                    Call setCanvasSize(cnv, cacheFrameWidth, cacheFrameHeight)
                End If

                ' Blt contents over
                Call canvas2CanvasBlt(anmCache(t).cnv, cnv, x, y, SRCCOPY)

                ' Play the frame's sound
                Call sndPlaySound(anmCache(t).strSound, SND_ASYNC Or SND_NODEFAULT)

                ' All done!
                Exit Sub

            End If

        End If

    Next t

    Call openAnimation(projectPath & miscPath & file, anm)
    maxF = animGetMaxFrame(anm)
    frame = frame Mod (maxF + 1)

    Dim frameFile As String
    frameFile = anm.animFrame(frame)

    ' Now we have the filename of the frame
    Dim ext As String, tbm As TKTileBitmap
    Dim cnvTbm As Long, cnvMaskTbm As Long

    If (LenB(frameFile) <> 0) Then
        ' We can draw the frame!

        ' Get the ambient level here. Must be done before opening the DC,
        ' otherwise trans3 *will* crash on Win9x
        Call getAmbientLevel(addOnR, addOnB, addOnG)

        ' Resize the canvas if needed.
        If (w <> anm.animSizeX Or h <> anm.animSizeY) Then
            Call setCanvasSize(cnv, anm.animSizeX, anm.animSizeY)
        End If

        Call canvasFill(cnv, TRANSP_COLOR)

        ext = UCase$(GetExt(frameFile))
        If ext = "TBM" Or LeftB$(ext, 6) = "TST" Or ext = "GPH" Then
        
            ' You *must* load a tile bitmap before opening an hdc
            ' because it'll lock up on windows 98 if you don't.
            
            If ext = "TBM" Then
                Call OpenTileBitmap(projectPath & bmpPath & frameFile, tbm)
            Else
                'Set up a 1x1 tile bitmap.
                Call TileBitmapClear(tbm)
                Call TileBitmapSize(tbm, 1, 1)
                tbm.tiles(0, 0) = frameFile
            End If

            'Draw the tilebitmap and mask to new canvases.
            cnvTbm = createCanvas(tbm.sizex * 32, tbm.sizey * 32)
            cnvMaskTbm = createCanvas(tbm.sizex * 32, tbm.sizey * 32)
            Call DrawTileBitmapCNV(cnvTbm, cnvMaskTbm, 0, 0, tbm)

            'Stretch the tbm canvas to the required size and draw it to the canvas.
            'hdc = canvasOpenHDC(cnv)
            Call canvasMaskBltStretchTransparent(cnvTbm, _
                                                 cnvMaskTbm, _
                                                 0, 0, _
                                                 anm.animSizeX, _
                                                 anm.animSizeY, _
                                                 cnv, _
                                                 anm.animTransp(frame))
            'Call canvasCloseHDC(cnv, hdc)

            Call destroyCanvas(cnvTbm)
            Call destroyCanvas(cnvMaskTbm)

            ' Done!
        
        Else

            ' Have to blt it across from an image
            Dim c2 As Long
            c2 = createCanvas(anm.animSizeX, anm.animSizeY)
            Call canvasLoadSizedPicture(c2, projectPath & bmpPath & frameFile)
            Call canvas2CanvasBltTransparent(c2, cnv, x, y, anm.animTransp(frame))
            Call destroyCanvas(c2)

        End If 'ext = TBM

        ' Play the frame's sound
        Dim strSound As String
        strSound = projectPath & mediaPath & anm.animSound(frame)
        Call sndPlaySound(strSound, SND_ASYNC Or SND_NODEFAULT)

        ' Now place this frame in the sprite cache
        t = nextAnmCacheIdx
        If (anm.animSizeX <> getCanvasWidth(anmCache(t).cnv) Or anm.animSizeY <> getCanvasHeight(anmCache(t).cnv)) Then

            If (anmCache(t).cnv) Then
                Call setCanvasSize(anmCache(t).cnv, anm.animSizeX, anm.animSizeY)
            Else
                ' Create the canvas
                anmCache(t).cnv = createCanvas(anm.animSizeX, anm.animSizeY)
            End If

        End If

        Call canvas2CanvasBlt(cnv, anmCache(nextAnmCacheIdx).cnv, 0, 0, SRCCOPY)
        anmCache(nextAnmCacheIdx).file = file
        anmCache(nextAnmCacheIdx).frame = frame
        anmCache(nextAnmCacheIdx).maxFrames = maxF
        anmCache(nextAnmCacheIdx).strSound = strSound
        nextAnmCacheIdx = nextAnmCacheIdx + 1

        Dim ub As Long
        ub = UBound(anmCache)
        If (nextAnmCacheIdx > ub) Then
            ' Enlarge the array
            ReDim Preserve anmCache(ub + 250)
        End If

    End If ' LenB(imageFile)

End Sub

'========================================================================
' Get the frame image filename of animation at idx
'========================================================================
Public Function AnimationIndexFrameImage(ByVal idx As Long, ByVal frame As Long) As String
    On Error Resume Next
    If anmListOccupied(idx) Then
        AnimationIndexFrameImage = anmList(idx).theData.animFrame(frame)
    End If
End Function

'========================================================================
' This routine assumes it will be called by a timer every 5 ms (that's
' 200 times per second (200 fps)) based upon the fps for the tile (the
' pause length), the current frame will be advanced or it won't be advanced.
' This will return true if it's time to draw this frame again. It will
' return false otherwise, but will advance the timer counter.
'========================================================================
Public Function AnimationShouldDrawFrame(ByRef theAnm As TKAnimation) As Boolean
    AnimationShouldDrawFrame = (theAnm.timerFrame Mod (80 * theAnm.animPause) = 0)
    theAnm.timerFrame = theAnm.timerFrame + 1
End Function

'========================================================================
'draw an animation from the anmList array
'call this every 5ms and it'll draw it accroding to the animation speed
'it will only advance the frame when required.  neato
' Called by: trans3 - nothing, toolkit3 - nothing
'========================================================================
Public Sub DrawAnimationIndex(ByVal idx As Long, ByVal x As Long, ByVal y As Long, ByVal hdc As Long)
    On Error Resume Next
    If anmListOccupied(idx) Then
        
        Dim theAnm As TKAnimation
        theAnm = anmList(idx).theData
        
        Dim playSound As Boolean
        
        If AnimationShouldDrawFrame(theAnm) Or anmList(idx).theData.currentAnmFrame = -1 Then
            'draw the next frame and make the sound...
            theAnm.currentAnmFrame = theAnm.currentAnmFrame + 1
            If theAnm.currentAnmFrame > animGetMaxFrame(theAnm) Then
                theAnm.currentAnmFrame = 0
            End If
            playSound = True
        Else
            'draw the current frame again...
            playSound = False
        End If
        Call AnimDrawFrame(theAnm, theAnm.currentAnmFrame, x, y, hdc, playSound)
    End If
End Sub

Public Sub AnimDrawFrameCanvas(ByRef theAnim As TKAnimation, ByVal framenum As Long, ByVal x As Long, ByVal y As Long, ByVal cnv As Long, Optional ByVal playSound As Boolean = True)
    'draw the frame referenced by framenum
    'loads a file into a canvas and resizes it.
    On Error Resume Next
    
    Dim cnvTemp As Long
    cnvTemp = createCanvas(32, 32)
    
    Call renderAnimationFrame(cnvTemp, theAnim.animFile, framenum, 0, 0)
    Call canvas2CanvasBltTransparent(cnvTemp, cnv, x, y, TRANSP_COLOR)
    Call destroyCanvas(cnvTemp)
    
    If playSound And ((LenB(theAnim.animSound(framenum)))) Then
        Const wFlags As Integer = SND_ASYNC Or SND_NODEFAULT
        If pakFileRunning Then
            Call sndPlaySound(PakLocate(mediaPath & theAnim.animSound(framenum)), wFlags%)
        Else
            Call sndPlaySound(projectPath$ & mediaPath$ & theAnim.animSound(framenum), wFlags%)
        End If
    End If
End Sub

Public Sub DrawAnimationIndexCanvas(ByVal idx As Long, ByVal x As Long, ByVal y As Long, ByVal cnv As Long, Optional ByVal forceDraw As Boolean = False, Optional ByVal forceTranspFill As Boolean = False)
    'draw an animation from the anmList array to a canvas
    'call this every 5ms and it'll draw it accroding to the animation speed
    'it will only advance the frame when required.  neato
    'if forceTraw is true, it will redraw the frame, even if it was lareayd drawn.  else it will not draw it again
    'if forceTranspFill is true, we'll fill the cnavas with the transparent color before drawing the frame
    On Error Resume Next
    If anmListOccupied(idx) Then
        
        Dim theAnm As TKAnimation
        
        Dim playSound As Boolean
        
        If AnimationShouldDrawFrame(anmList(idx).theData) Or anmList(idx).theData.currentAnmFrame = -1 Then
            'draw the next frame and make the sound...
            anmList(idx).theData.currentAnmFrame = anmList(idx).theData.currentAnmFrame + 1
            If anmList(idx).theData.currentAnmFrame > animGetMaxFrame(anmList(idx).theData) Then
                anmList(idx).theData.currentAnmFrame = 0
            End If
            playSound = True
            If forceTranspFill Then
                Call canvasFill(cnv, TRANSP_COLOR)
            End If
            Call AnimDrawFrameCanvas(anmList(idx).theData, anmList(idx).theData.currentAnmFrame, x, y, cnv, playSound)
        Else
            'draw the current frame again...
            playSound = False
            If forceDraw Then
                If forceTranspFill Then
                    Call canvasFill(cnv, TRANSP_COLOR)
                End If
                Call AnimDrawFrameCanvas(anmList(idx).theData, anmList(idx).theData.currentAnmFrame, x, y, cnv, playSound)
            End If
        End If
    End If
End Sub

Public Sub DrawAnimationIndexCanvasFrame(ByVal idx As Long, ByVal frame As Long, ByVal x As Long, ByVal y As Long, ByVal cnv As Long, Optional ByVal forceTranspFill As Boolean = False)
    'draw an animation from the anmList array to a canvas
    'if forceTranspFill is true, we'll fill the cnavas with the transparent color before drawing the frame
    On Error Resume Next
    If anmListOccupied(idx) Then
        
        Dim theAnm As TKAnimation
        
        Dim oldFrame As Long
        anmList(idx).theData.currentAnmFrame = frame
        If anmList(idx).theData.currentAnmFrame > animGetMaxFrame(anmList(idx).theData) Then
            anmList(idx).theData.currentAnmFrame = 0
        End If
        If forceTranspFill Then
            Call canvasFill(cnv, TRANSP_COLOR)
        End If
        Call AnimDrawFrameCanvas(anmList(idx).theData, anmList(idx).theData.currentAnmFrame, x, y, cnv, False)
        
    End If
End Sub

Public Sub DestroyAnimation(ByVal idx As Long)
    On Error Resume Next
    'free up memory in the ste list vector
    anmListOccupied(idx) = False
End Sub

Public Function CreateAnimation(ByVal file As String) As Long
    On Error GoTo vecterr
    'create an animation and return the index into the anmList array
       
    'test size of array
    Dim test As Long
    Dim oldSize As Long, newSize As Long, t As Long
    test = UBound(anmList)
    
    'find a new slot in the list of boards and return an index we can use
    For t = 0 To UBound(anmList)
        If anmListOccupied(t) = False Then
            anmListOccupied(t) = True
            Call openAnimation(file, anmList(t).theData)
            anmList(t).animFile = RemovePath(file)
            anmList(t).theData.currentAnmFrame = -1
            CreateAnimation = t
            Exit Function
        End If
    Next t
    
    'must resize the vector...
    oldSize = UBound(anmList)
    newSize = UBound(anmList) * 2
    ReDim Preserve anmList(newSize)
    ReDim Preserve anmListOccupied(newSize)
    
    anmListOccupied(oldSize + 1) = True
    Call openAnimation(file, anmList(oldSize + 1).theData)
    anmList(oldSize + 1).animFile = RemovePath(file)
    anmList(oldSize + 1).theData.currentAnmFrame = -1
    CreateAnimation = oldSize + 1
    
    Exit Function

vecterr:
    ReDim anmList(1)
    ReDim anmListOccupied(1)
    Resume Next
    
End Function
#End If

Public Sub saveAnimation(ByVal file As String, ByRef theAnim As TKAnimation)

    On Error Resume Next

    'Get ready to write the file
    Call Kill(file)
    #If isToolkit = 1 Then
        animationList(activeAnimationIndex).animNeedUpdate = False
    #End If

    Dim num As Long, frameIdx As Long

    num = FreeFile()

    With theAnim
        Open file For Binary Access Write As num
            Call BinWriteString(num, FILE_HEADER)
            Call BinWriteInt(num, major)
            Call BinWriteInt(num, 3)
            Call BinWriteLong(num, .animSizeX)
            Call BinWriteLong(num, .animSizeY)
            Call BinWriteLong(num, UBound(.animFrame))
            For frameIdx = 0 To (UBound(.animFrame))
                Call BinWriteString(num, .animFrame(frameIdx))
                Call BinWriteLong(num, .animTransp(frameIdx))
                Call BinWriteString(num, .animSound(frameIdx))
            Next frameIdx
            Call BinWriteDouble(num, .animPause)
        Close num
    End With

End Sub
