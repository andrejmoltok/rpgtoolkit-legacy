Attribute VB_Name = "Global"
'All contents copyright 2003, 2004, Christopher Matthews or Contributors
'All rights reserved.  YOU MAY NOT REMOVE THIS NOTICE.
'Read LICENSE.txt for licensing info

'=======================================================
'Cleaned up a bit, 3.0.4 by KSNiloc
'
' ---What is done
' + Removed variants
' + Added Option Explicit
' + Removed unused variables
'
' ---What needs to be done
' + Examine usage of variables to nominalize boxing
'
'=======================================================

Option Explicit

Public currentDir As String          'Current directory
Public CurrentVersion As String      'Version "2.0"
Public Major As Integer              'Major version
Public Minor As Integer              'Minor version
Public compression As Integer        'compression used?
Public filename(30) As String        'Filename array
Public tilePath As String            'Tile dir path
Public brdPath As String             'board dir path
Public temPath As String             'character dir path
Public arcPath As String             'archive dir path
Public spcPath As String             'special move dir
Public bkgPath As String             'board background dir
Public mediaPath As String           'media files
Public prgPath As String             'prg files
Public fontPath As String            'font files
Public itmPath As String             'item path
Public enePath As String             'enemy path
Public gamPath As String             'mainForm file path
Public bmpPath As String             'bmp files
Public statusPath As String          'status effect path
Public miscPath As String            'miscellaneous path (ie. anims)
Public pluginPath As String          'plugin path
Public savPath As String             'saved games
Public projectPath As String         'project path
Public resourcePath As String        'resource path
Public nocodeYN As Boolean           'did it have nocode?

'Tile Editor
Public buftile(32, 32) As Long       'Tile buffer
Public lastTileset As String

'Board Editor
Public activeBoardIndex As Long      'index for active board

'Character Editor
Public playerMem(4) As TKPlayer
Public activePlayerIndex As Long

'Item Editor:
Public itemMem() As TKItem

Public activeItemIndex As Long

'mainForm File editor
Public mainMem As TKMain

'enemy editor
Public enemyMem(4) As TKEnemy
Public activeEnemyIndex As Long      'index of active enemy

'Special move editor
Public specialMoveMem As TKSpecialMove
Public activeSpecialMoveIndex As Long

'Background editor
Public bkgMem As TKBackground
Public activeBkgIndex As Long

'status effect editor
Public statusMem As TKStatusEffect
Public activeStatusEffectIndex As Long

'animation editor
Public animationMem As TKAnimation   'animation file
Public activeAnimationIndex As Long
Public activeTileAnmIndex As Long

'File manipulation
Public openFile() As String
Public openFullFile() As String
