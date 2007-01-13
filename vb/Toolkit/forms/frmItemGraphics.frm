VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form frmItemGraphics 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Item Sprite List"
   ClientHeight    =   6570
   ClientLeft      =   45
   ClientTop       =   435
   ClientWidth     =   10260
   Icon            =   "frmItemGraphics.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   6600.845
   ScaleMode       =   0  'User
   ScaleWidth      =   10260
   StartUpPosition =   2  'CenterScreen
   Begin VB.Frame fra 
      Caption         =   "Vector base"
      Height          =   3375
      Index           =   2
      Left            =   4320
      TabIndex        =   26
      Top             =   3000
      Width           =   3375
      Begin VB.PictureBox picHolder 
         BorderStyle     =   0  'None
         HasDC           =   0   'False
         Height          =   735
         Index           =   5
         Left            =   1920
         ScaleHeight     =   735
         ScaleWidth      =   1095
         TabIndex        =   30
         Top             =   2160
         Width           =   1095
         Begin VB.OptionButton optCoord 
            Caption         =   "Isometric"
            Height          =   375
            Index           =   1
            Left            =   0
            TabIndex        =   32
            ToolTipText     =   "Display isometric base point (bottom-centre + 8 pixels)"
            Top             =   360
            Width           =   975
         End
         Begin VB.OptionButton optCoord 
            Caption         =   "Standard"
            Height          =   375
            Index           =   0
            Left            =   0
            TabIndex        =   31
            ToolTipText     =   "Display standard 2D base point (bottom-centre of sprite)"
            Top             =   0
            Value           =   -1  'True
            Width           =   1095
         End
      End
      Begin VB.PictureBox picHolder 
         BorderStyle     =   0  'None
         HasDC           =   0   'False
         Height          =   1095
         Index           =   3
         Left            =   360
         ScaleHeight     =   1095
         ScaleWidth      =   1575
         TabIndex        =   36
         Top             =   2160
         Width           =   1575
         Begin VB.CommandButton cmdVectorImport 
            Caption         =   "Import..."
            Height          =   375
            Left            =   0
            TabIndex        =   37
            ToolTipText     =   "Default collision vector / activation vector for automatic activation (""step-on"")"
            Top             =   720
            Width           =   1455
         End
         Begin VB.CommandButton cmdVectorDefault 
            Caption         =   "Default Collision"
            Height          =   375
            Index           =   0
            Left            =   0
            TabIndex        =   38
            ToolTipText     =   "Default collision vector / activation vector for automatic activation (""step-on"")"
            Top             =   0
            Width           =   1455
         End
         Begin VB.CommandButton cmdVectorDefault 
            Caption         =   "Default Interaction"
            Height          =   375
            Index           =   1
            Left            =   0
            TabIndex        =   39
            ToolTipText     =   "Default interaction vector for key-press activation"
            Top             =   360
            Width           =   1455
         End
      End
      Begin VB.CheckBox chkEdit 
         Height          =   375
         Index           =   1
         Left            =   360
         Picture         =   "frmItemGraphics.frx":0CCA
         Style           =   1  'Graphical
         TabIndex        =   35
         ToolTipText     =   "Edit vector points"
         Top             =   1200
         Width           =   375
      End
      Begin VB.CheckBox chkEdit 
         Height          =   375
         Index           =   0
         Left            =   360
         Picture         =   "frmItemGraphics.frx":1594
         Style           =   1  'Graphical
         TabIndex        =   34
         ToolTipText     =   "Draw vector points"
         Top             =   840
         Width           =   375
      End
      Begin VB.PictureBox picColor 
         AutoRedraw      =   -1  'True
         Height          =   375
         Left            =   360
         ScaleHeight     =   315
         ScaleWidth      =   315
         TabIndex        =   33
         ToolTipText     =   "Vector display color (for this window)"
         Top             =   1560
         Width           =   375
      End
      Begin VB.PictureBox picHolder 
         BorderStyle     =   0  'None
         HasDC           =   0   'False
         Height          =   255
         Index           =   2
         Left            =   360
         ScaleHeight     =   255
         ScaleWidth      =   2655
         TabIndex        =   27
         Top             =   360
         Width           =   2655
         Begin VB.OptionButton optType 
            Caption         =   "Collision"
            Height          =   255
            Index           =   0
            Left            =   120
            TabIndex        =   29
            ToolTipText     =   "View collision vector"
            Top             =   0
            Value           =   -1  'True
            Width           =   1095
         End
         Begin VB.OptionButton optType 
            Caption         =   "Activation"
            Height          =   255
            Index           =   1
            Left            =   1440
            TabIndex        =   28
            ToolTipText     =   "View sprite activation vector"
            Top             =   0
            Width           =   1095
         End
      End
      Begin MSComctlLib.ListView lvVector 
         Height          =   1335
         Left            =   1080
         TabIndex        =   40
         ToolTipText     =   "Current vector points"
         Top             =   720
         Width           =   1935
         _ExtentX        =   3413
         _ExtentY        =   2355
         View            =   3
         LabelEdit       =   1
         LabelWrap       =   -1  'True
         HideSelection   =   -1  'True
         HideColumnHeaders=   -1  'True
         FullRowSelect   =   -1  'True
         _Version        =   393217
         ForeColor       =   -2147483640
         BackColor       =   -2147483643
         Appearance      =   1
         NumItems        =   3
         BeginProperty ColumnHeader(1) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
            Text            =   "index"
            Object.Width           =   564
         EndProperty
         BeginProperty ColumnHeader(2) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
            SubItemIndex    =   1
            Text            =   "x-coord"
            Object.Width           =   1058
         EndProperty
         BeginProperty ColumnHeader(3) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
            SubItemIndex    =   2
            Text            =   "y-coord"
            Object.Width           =   1058
         EndProperty
      End
   End
   Begin VB.CommandButton cmdDefault 
      Caption         =   "OK"
      Height          =   375
      Left            =   8280
      TabIndex        =   0
      Top             =   5880
      Width           =   1455
   End
   Begin VB.PictureBox picHolder 
      BorderStyle     =   0  'None
      Height          =   6375
      Index           =   0
      Left            =   120
      ScaleHeight     =   6375
      ScaleWidth      =   9975
      TabIndex        =   1
      Top             =   120
      Width           =   9975
      Begin VB.Frame fra 
         Caption         =   "Animation"
         Height          =   2775
         Index           =   0
         Left            =   6240
         TabIndex        =   13
         Top             =   0
         Width           =   3735
         Begin VB.PictureBox picHolder 
            BorderStyle     =   0  'None
            Height          =   2340
            Index           =   1
            Left            =   120
            ScaleHeight     =   2340
            ScaleWidth      =   3495
            TabIndex        =   14
            Top             =   240
            Width           =   3495
            Begin VB.CommandButton cmdCustomNew 
               Caption         =   "New Custom..."
               Height          =   375
               Left            =   0
               TabIndex        =   21
               ToolTipText     =   "Add custom animation"
               Top             =   1920
               Width           =   1395
            End
            Begin VB.CommandButton cmdCustomDelete 
               Caption         =   "Delete Custom"
               Enabled         =   0   'False
               Height          =   375
               Left            =   1440
               TabIndex        =   20
               ToolTipText     =   "Delete selected custom animation"
               Top             =   1920
               Width           =   1395
            End
            Begin VB.TextBox txtFilename 
               Height          =   285
               Index           =   0
               Left            =   0
               TabIndex        =   19
               Top             =   240
               Width           =   2775
            End
            Begin VB.CommandButton cmdBrowse 
               Caption         =   "..."
               Height          =   255
               Index           =   0
               Left            =   2880
               TabIndex        =   18
               ToolTipText     =   "Browse for animation file"
               Top             =   240
               Width           =   495
            End
            Begin VB.TextBox txtFilename 
               Height          =   285
               Index           =   1
               Left            =   0
               TabIndex        =   17
               Top             =   840
               Width           =   2775
            End
            Begin VB.CommandButton cmdBrowse 
               Caption         =   "..."
               Height          =   255
               Index           =   1
               Left            =   2880
               TabIndex        =   16
               ToolTipText     =   "Browse for animation file"
               Top             =   840
               Width           =   495
            End
            Begin VB.TextBox txtCustomHandle 
               Height          =   285
               Left            =   0
               TabIndex        =   15
               Top             =   1560
               Width           =   2775
            End
            Begin VB.Label lbl 
               Caption         =   "Selected animation file"
               Height          =   255
               Index           =   3
               Left            =   0
               TabIndex        =   24
               Top             =   0
               Width           =   1815
            End
            Begin VB.Label lbl 
               Caption         =   "Idle animation file"
               Height          =   255
               Index           =   4
               Left            =   0
               TabIndex        =   23
               Top             =   600
               Width           =   1815
            End
            Begin VB.Label lbl 
               Caption         =   "Custom animation handle"
               Height          =   255
               Index           =   5
               Left            =   0
               TabIndex        =   22
               Top             =   1320
               Width           =   1815
            End
         End
      End
      Begin VB.Frame fra 
         Caption         =   "Preview"
         Height          =   3375
         Index           =   3
         Left            =   0
         TabIndex        =   8
         Top             =   2880
         Width           =   4095
         Begin VB.PictureBox picHolder 
            BorderStyle     =   0  'None
            Height          =   615
            Index           =   4
            Left            =   120
            ScaleHeight     =   615
            ScaleWidth      =   375
            TabIndex        =   11
            Top             =   240
            Width           =   375
            Begin VB.CommandButton cmdAnimate 
               Caption         =   ">"
               Height          =   375
               Left            =   0
               TabIndex        =   12
               ToolTipText     =   "Preview animation"
               Top             =   0
               Width           =   375
            End
         End
         Begin VB.PictureBox picPreviewHolder 
            AutoRedraw      =   -1  'True
            Height          =   2895
            Left            =   600
            ScaleHeight     =   189
            ScaleMode       =   3  'Pixel
            ScaleWidth      =   189
            TabIndex        =   9
            Top             =   240
            Width           =   2895
            Begin VB.PictureBox picPreview 
               AutoRedraw      =   -1  'True
               BorderStyle     =   0  'None
               Height          =   2775
               Left            =   120
               ScaleHeight     =   185
               ScaleMode       =   3  'Pixel
               ScaleWidth      =   177
               TabIndex        =   10
               Top             =   0
               Width           =   2655
            End
         End
      End
      Begin VB.Frame fra 
         Caption         =   "Miscellaneous"
         Height          =   2055
         Index           =   1
         Left            =   7680
         TabIndex        =   3
         Top             =   2880
         Width           =   2295
         Begin VB.TextBox txtFrameTime 
            Height          =   285
            Left            =   1440
            TabIndex        =   5
            Top             =   1320
            Width           =   615
         End
         Begin VB.TextBox txtIdleTime 
            Height          =   285
            Left            =   1440
            TabIndex        =   4
            Top             =   480
            Width           =   615
         End
         Begin VB.Label lbl 
            BackStyle       =   0  'Transparent
            Caption         =   "Seconds between each step"
            Height          =   495
            Index           =   1
            Left            =   120
            TabIndex        =   7
            Top             =   1320
            Width           =   1335
         End
         Begin VB.Label lbl 
            BackStyle       =   0  'Transparent
            Caption         =   "Seconds before player becomes idle"
            Height          =   615
            Index           =   0
            Left            =   120
            TabIndex        =   6
            Top             =   480
            Width           =   1335
         End
      End
      Begin VB.CommandButton cmdWizard 
         Caption         =   "Wizard..."
         Height          =   375
         Left            =   8160
         TabIndex        =   2
         Top             =   5280
         Width           =   1455
      End
      Begin MSComctlLib.ListView lvAnimations 
         Height          =   2655
         Left            =   0
         TabIndex        =   25
         Top             =   120
         Width           =   6015
         _ExtentX        =   10610
         _ExtentY        =   4683
         View            =   3
         LabelEdit       =   1
         LabelWrap       =   -1  'True
         HideSelection   =   0   'False
         FullRowSelect   =   -1  'True
         _Version        =   393217
         ForeColor       =   -2147483640
         BackColor       =   -2147483643
         BorderStyle     =   1
         Appearance      =   0
         NumItems        =   3
         BeginProperty ColumnHeader(1) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
            Text            =   "Animation"
            Object.Width           =   2716
         EndProperty
         BeginProperty ColumnHeader(2) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
            SubItemIndex    =   1
            Text            =   "Walking / default"
            Object.Width           =   3704
         EndProperty
         BeginProperty ColumnHeader(3) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
            SubItemIndex    =   2
            Text            =   "Idle (directional only)"
            Object.Width           =   3704
         EndProperty
      End
   End
End
Attribute VB_Name = "frmItemGraphics"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'========================================================================
'All contents copyright 2003 - 2006 Christopher Matthews or Contributors
'All rights reserved.  YOU MAY NOT REMOVE THIS NOTICE.
'Read LICENSE.txt for licensing info
'========================================================================

'This is essentially the same as frmCharacterGraphics, but unfortunately
'the PLAYER and ITEM objects are too different and too heavily ingrained
'to be able to easily make this a type-independent user control.

Option Explicit

Private m_viewIdles As Boolean
Private m_vector As CVector
Private m_vectorColor As Long
Private m_editing As Boolean
Private m_base As POINTAPI
Private m_drag As POINTAPI
Private m_filetype As ANM_TYPE

Private Const CUSTOM_OFFSET = 9                 'ListView is Base 1

Private Enum ANM_TYPE
    ANM_TK
    ANM_GIF
End Enum

'========================================================================
' Vector check buttons
'========================================================================
Private Sub chkEdit_MouseUp(Index As Integer, Button As Integer, Shift As Integer, x As Single, y As Single): On Error Resume Next
    If chkEdit(Index).value Then chkEdit(Abs(Index - 1)).value = 0
End Sub

'========================================================================
' The Play Button
'========================================================================
Private Sub cmdAnimate_Click(): On Error Resume Next

    'Open the animation
    Dim anmFile As String
    anmFile = getAnim()
    
    'No support to animate gif.
    If m_filetype = ANM_GIF Then
        MsgBox "Preview not available for animated gifs", vbInformation
        Exit Sub
    End If
    
    If LenB(anmFile) And fileExists(projectPath & miscPath & anmFile) Then
    
        'Open the animation
        Dim anm As TKAnimation
        Call openAnimation(projectPath & miscPath & anmFile, anm)
        
        'Change the animation's speed to the sprite's speed for the walking animations (only).
        If (Not m_viewIdles) And lvAnimations.SelectedItem.Index < CUSTOM_OFFSET Then
            anm.animPause = itemList(activeItemIndex).theData.speed
        End If
        
        If anm.animSizeX > picPreview.width Or anm.animSizeY > picPreview.Height Then
            
            'Too big for the animation screen - preview in animation host
            animationHost.file = projectPath & miscPath & anmFile
            animationHost.repeats = 3
            Call animationHost.Show(vbModal)
        Else
            Dim x As Long, y As Long
            x = (picPreview.width - anm.animSizeX) / 2
            y = (picPreview.Height - anm.animSizeY) / 2
            
            cmdAnimate.Enabled = False
            
            Dim i As Long, j As Long, ed As New CBoardEditor
            For i = 0 To 2
                For j = 0 To animGetMaxFrame(anm)
                    picPreview.Cls
                    Call AnimDrawFrame(anm, j, x, y, picPreview.hdc)
                    Call animDelay(anm.animPause)
                     
                    picPreview.DrawStyle = vbDot
                    picPreview.Line (m_base.x - 16, m_base.y)-(m_base.x + 16, m_base.y), m_vectorColor
                    picPreview.Line (m_base.x, m_base.y - 16)-(m_base.x, m_base.y + 16), m_vectorColor
                    picPreview.DrawStyle = vbSolid
                    
                    'Hijack some board editor code.
                    ed.topX = -m_base.x
                    ed.topY = -m_base.y
                    Call m_vector.draw(picPreview, ed, m_vectorColor, False)
                    
                    picPreview.Refresh
                    DoEvents
                Next j
            Next i
            
            cmdAnimate.Enabled = True
        End If

    End If

End Sub

'========================================================================
' Browse buttons
'========================================================================
Private Sub cmdBrowse_Click(Index As Integer): On Error Resume Next

    Dim returnPath As String

    If browseFileDialog( _
        Me.hwnd, _
        projectPath & miscPath, _
        "Select Animation", _
        "anm", _
        "Supported Formats|*.anm;*.gif|RPG Toolkit Animation (*.anm)|*.anm|Graphics Interchange Format (*.gif)|*.gif|All files(*.*)|*.*", _
        returnPath _
    ) Then
        'Setting the textbox calls the Change() event.
        txtFilename(Index).Text = returnPath
    End If
End Sub

'========================================================================
' Delete custom animation
'========================================================================
Private Sub cmdCustomDelete_Click(): On Error Resume Next
    
    'Exit sub if no animation is selected
    If lvAnimations.SelectedItem.Index < CUSTOM_OFFSET Then Exit Sub
    
    'See which animation the sprite wants to delete
    Dim i As Long
    i = itemGetCustomHandleIdx(itemList(activeItemIndex).theData, lvAnimations.SelectedItem.Index - CUSTOM_OFFSET)
    
    'Delete it
    itemList(activeItemIndex).theData.customGfx(i) = vbNullString
    itemList(activeItemIndex).theData.customGfxNames(i) = vbNullString
    
    'Update
    Call lvAnimations.ListItems.Remove(lvAnimations.SelectedItem.Index)
    Call lvApply

End Sub

'========================================================================
' New custom animation
'========================================================================
Private Sub cmdCustomNew_click(): On Error Resume Next
    
    'Ask what handle the user wants
    Dim newName As String
    newName = InputBox("Enter a handle for the custom animation")
    
    If LenB(newName) Then
    
        'Add it to the main data
        Call itemAddCustomGfx(itemList(activeItemIndex).theData, newName, vbNullString)
        
        'Update
        lvAnimations.SelectedItem = lvAnimations.ListItems.Add(, , newName)
        lvAnimations.SelectedItem.EnsureVisible
        Call lvApply
        
    End If
End Sub

'========================================================================
' The OK Button
'========================================================================
Private Sub cmdDefault_Click(): On Error Resume Next
    Unload Me
End Sub

'========================================================================
' Load a default vector base (isometric or 2D)
'========================================================================
Private Sub cmdVectorDefault_Click(Index As Integer): On Error Resume Next
    Call m_vector.defaultSpriteVector(Index = 0, Not optCoord(0).value)
    Call m_vector.lvPopulate(lvVector)
    Call setAnimation
End Sub

'========================================================================
' Load the vectors of another sprite
'========================================================================
Private Sub cmdVectorImport_Click(): On Error Resume Next
    
    Dim dlg As FileDialogInfo, ext As String
    With dlg
        .strDefaultFolder = projectPath
        .strTitle = "Import vectors from player or item"
        .strDefaultExt = ".itm"
        .strFileTypes = "Supported Formats|*.tem;*.itm|RPG Toolkit Character (*.tem)|*.tem|RPG Toolkit Item (*.itm)|*.itm|All files(*.*)|*.*"
    End With
    ChDir (currentDir)
    If Not OpenFileDialog(dlg, Me.hwnd) Then Exit Sub
    
    ext = LCase$(GetExt(dlg.strSelectedFileNoPath))
    If ext = "itm" Then
        
        Dim itm As TKItem
        itm = openItem(dlg.strSelectedFile)
        Call itm.vBase.copy(itemList(activeItemIndex).theData.vBase)
        Call itm.vActivate.copy(itemList(activeItemIndex).theData.vActivate)
    
    ElseIf ext = "tem" Then
    
        Dim tem As TKPlayer
        Call openChar(dlg.strSelectedFile, tem)
        Call tem.vBase.copy(itemList(activeItemIndex).theData.vBase)
        Call tem.vActivate.copy(itemList(activeItemIndex).theData.vActivate)
       
    Else
    
        MsgBox "Please select a character or an item", vbExclamation
    
    End If
    
    Call setAnimation
    
End Sub

'========================================================================
' Auto-complete the base graphics set
'========================================================================
Private Sub cmdWizard_Click(): On Error Resume Next
    
    With itemList(activeItemIndex).theData
        'Walking animations.
        Dim template As String
        template = InputBox( _
            "This wizard adds the default animations of the sprite on the basis they share common " & _
            "text in their filenames. A filename template containing an asterisk (*) is supplied by " & _
            "the user and the wizard substitutes directional characters for the asterisk " & _
            "(e.g., ""start_*.anm"" becomes ""start_s.anm"")." & vbCrLf & vbCrLf & _
            "Please enter the filename template (including any subfolders) common to all walking animations:", _
            "Sprite Animation Wizard" _
        )
        If LenB(template) = 0 Then Exit Sub
        
        .gfx(ITEM_WALK_S) = replace(template, "*", "s")
        .gfx(ITEM_WALK_N) = replace(template, "*", "n")
        .gfx(ITEM_WALK_E) = replace(template, "*", "e")
        .gfx(ITEM_WALK_W) = replace(template, "*", "w")
        .gfx(ITEM_WALK_NW) = replace(template, "*", "nw")
        .gfx(ITEM_WALK_NE) = replace(template, "*", "ne")
        .gfx(ITEM_WALK_SW) = replace(template, "*", "sw")
        .gfx(ITEM_WALK_SE) = replace(template, "*", "se")
        
        'Idle animations.
        template = InputBox( _
            "Please enter the filename template (and any subfolders) common to all idle animations:", _
            "Sprite Animation Wizard" _
        )
        If LenB(template) Then
        
            .standingGfx(ITEM_WALK_S) = replace(template, "*", "s")
            .standingGfx(ITEM_WALK_N) = replace(template, "*", "n")
            .standingGfx(ITEM_WALK_E) = replace(template, "*", "e")
            .standingGfx(ITEM_WALK_W) = replace(template, "*", "w")
            .standingGfx(ITEM_WALK_NW) = replace(template, "*", "nw")
            .standingGfx(ITEM_WALK_NE) = replace(template, "*", "ne")
            .standingGfx(ITEM_WALK_SW) = replace(template, "*", "sw")
            .standingGfx(ITEM_WALK_SE) = replace(template, "*", "se")
        
        End If
    End With
    
    Call lvUpdate
    Call setAnimation
End Sub

Public Sub disableAll(): On Error Resume Next
    Dim i As Control
    For Each i In Me
        i.Enabled = False
    Next i
    
    'Enable the picture holder and its containers.
    picHolder(0).Enabled = True
    fra(3).Enabled = True
    picPreviewHolder.Enabled = True
    picPreview.Enabled = True
End Sub
Private Sub enableAll(): On Error Resume Next
    Dim i As Control
    For Each i In Me
        i.Enabled = True
    Next i
    
    'Disable any controls that should remain disabled.
    Call lvApply
End Sub

'========================================================================
' Form_Load
'========================================================================
Private Sub Form_Load(): On Error Resume Next
    
    m_vectorColor = RGB(255, 255, 255)
    m_editing = False
    
    picColor.backColor = m_vectorColor
    Set m_vector = itemList(activeItemIndex).theData.vBase
    Call m_vector.lvPopulate(lvVector)
    
    Call lvUpdate
    Call lvApply
    
End Sub

'========================================================================
' Get the selected animation file
'========================================================================
Private Function getAnim() As String: On Error Resume Next

    If m_viewIdles And LenB(lvAnimations.SelectedItem.SubItems(2)) Then
        getAnim = lvAnimations.SelectedItem.SubItems(2)
    Else
        getAnim = lvAnimations.SelectedItem.SubItems(1)
    End If
    
    m_filetype = ANM_TK
    If LCase$(GetExt(getAnim)) = "gif" Then m_filetype = ANM_GIF

End Function

'========================================================================
' Show the first frame of the animation
'========================================================================
Private Sub setAnimation(): On Error Resume Next
    
    Dim anmFile As String, x As Long, y As Long, cnv As Long

    anmFile = getAnim()
    
    Call picPreview.Cls
    If LenB(anmFile) And fileExists(projectPath & miscPath & anmFile) Then
        
        'Open the animation
        Dim anm As TKAnimation
        
        Select Case m_filetype
            Case ANM_TK
                Call openAnimation(projectPath & miscPath & anmFile, anm)
            Case ANM_GIF
                cnv = createCanvas(1, 1)
                Call canvasLoadFullPicture(cnv, projectPath & miscPath & anmFile, -1, -1)
                anm.animSizeX = getCanvasWidth(cnv)
                anm.animSizeY = getCanvasHeight(cnv)
        End Select
        
        If anm.animSizeX > picPreview.width Or anm.animSizeY > picPreview.Height Then
            x = (picPreview.width - anm.animSizeX) / 2
            y = picPreview.Height - anm.animSizeY - 48
        Else
            x = (picPreview.width - anm.animSizeX) / 2
            y = (picPreview.Height - anm.animSizeY) / 2
        End If
        
        'Draw it
        Select Case m_filetype
            Case ANM_TK
                Call AnimDrawFrame(anm, 0, x, y, picPreview.hdc, False)
            Case ANM_GIF
                Call canvasBlt(cnv, x, y, picPreview.hdc)
                Call destroyCanvas(cnv)
        End Select
        
        Call picPreview.Refresh
    Else
        Exit Sub
    End If
    
    'Draw a cross to signify the base point.
    m_base.x = x + anm.animSizeX / 2
    m_base.y = y + anm.animSizeY
    
    'Isometric sprites are offset down by 8 pixels in trans3.
    If optCoord(1).value Then m_base.y = m_base.y - 8
    
    picPreview.DrawStyle = vbDot
    picPreview.Line (m_base.x - 16, m_base.y)-(m_base.x + 16, m_base.y), m_vectorColor
    picPreview.Line (m_base.x, m_base.y - 16)-(m_base.x, m_base.y + 16), m_vectorColor
    picPreview.DrawStyle = vbSolid
    
    'Hijack some board editor code.
    Dim ed As New CBoardEditor
    ed.topX = -m_base.x
    ed.topY = -m_base.y
    Call m_vector.draw(picPreview, ed, m_vectorColor, False)
        
End Sub

Private Sub optCoord_Click(Index As Integer): On Error Resume Next
    Call setAnimation
End Sub

'========================================================================
' Switch between the activation and collision vectors
'========================================================================
Private Sub optType_Click(Index As Integer): On Error Resume Next
    If Index = 0 Then
        Set m_vector = itemList(activeItemIndex).theData.vBase
    Else
        Set m_vector = itemList(activeItemIndex).theData.vActivate
    End If
    Call setAnimation
    Call m_vector.lvPopulate(lvVector)
End Sub

'========================================================================
' Alter the vector drawing colour
'========================================================================
Private Sub picColor_Click(): On Error Resume Next
    Dim color As Long
    color = ColorDialog
    If color >= 0 Then
        m_vectorColor = color
        picColor.backColor = color
        Call setAnimation
    End If
End Sub

'========================================================================
' Vector editing - draw or edit
'========================================================================
Private Sub picPreview_MouseDown(Button As Integer, Shift As Integer, x As Single, y As Single): On Error Resume Next
    If chkEdit(0).value Then
        'Drawing
        If Button = vbLeftButton Then
            If Not m_editing Then
                Call disableAll
                m_editing = True
                m_vector.deletePoints
                m_vector.tiletype = TT_SOLID
                m_vector.bClosed = False
            End If
            
            Call m_vector.setPoint(m_vector.getPoints, x - m_base.x, y - m_base.y, False)
            Call m_vector.addPoint(x - m_base.x + 1, y - m_base.y + 1)
        Else
            Call enableAll
            chkEdit(0).value = 0
            m_editing = False
            Call m_vector.deletePoints(m_vector.getPoints)
            Call m_vector.closeVector(0, 0)
            Call m_vector.lvPopulate(lvVector)
        End If
        Call setAnimation
        
    ElseIf chkEdit(1).value Then
        'Editing
        Dim pt As POINTAPI, distance As Long, sel As New CBoardSelection
        
        'Hijack some board code to assign selected node.
        Call m_vector.setSelection(sel)
            
        'Mouse-down = drag nearest point.
        Call m_vector.nearestPoint(x - m_base.x, y - m_base.y, pt.x, pt.y, distance)
        If distance >= 0 Then
            'A point was found.
            m_drag.x = x
            m_drag.y = y
            Call sel.assign(pt.x, pt.y, pt.x, pt.y)
            Call m_vector.setSelection(sel)
        End If
    End If
End Sub

'========================================================================
' Vector editing - draw or edit
'========================================================================
Private Sub picPreview_MouseMove(Button As Integer, Shift As Integer, x As Single, y As Single): On Error Resume Next
    If chkEdit(0).value And m_editing Then
        'Edit the last point.
        Call m_vector.setPoint(m_vector.getPoints, x - m_base.x, y - m_base.y, False)
        Call setAnimation
        
    ElseIf Button And chkEdit(1).value Then
        'Edit - drag nearest point around.
        Dim dx As Long, dy As Long
        dx = x - m_drag.x
        dy = y - m_drag.y
        m_drag.x = x
        m_drag.y = y
        Call m_vector.moveSelectionBy(dx, dy)
        Call m_vector.lvPopulate(lvVector)
        Call setAnimation
        
    End If
End Sub

Private Sub txtCustomHandle_Change(): On Error Resume Next
    
    'Exit sub if no animation is selected
    If lvAnimations.SelectedItem.Index < CUSTOM_OFFSET Then Exit Sub
    
    Dim i As Long
    i = itemGetCustomHandleIdx(itemList(activeItemIndex).theData, lvAnimations.SelectedItem.Index - CUSTOM_OFFSET)
    
    itemList(activeItemIndex).theData.customGfxNames(i) = txtCustomHandle.Text
    lvAnimations.SelectedItem.Text = txtCustomHandle.Text

End Sub

'========================================================================
' Filename edit
'========================================================================
Private Sub txtFilename_Change(Index As Integer): On Error Resume Next
    
    Dim i As Long
    i = lvAnimations.SelectedItem.Index
    
    With itemList(activeItemIndex).theData
        lvAnimations.SelectedItem.SubItems(Index + 1) = txtFilename(Index).Text
        
        If Index = 0 Then
            'Walking / custom graphics.
            If i < CUSTOM_OFFSET Then
                .gfx(i - 1) = lvAnimations.SelectedItem.SubItems(1)
            Else
                i = itemGetCustomHandleIdx(itemList(activeItemIndex).theData, i - CUSTOM_OFFSET)

                .customGfx(i) = lvAnimations.SelectedItem.SubItems(1)
                .customGfxNames(i) = lvAnimations.SelectedItem.Text
            End If
        Else
            'Idle graphics.
            If i < CUSTOM_OFFSET Then
                .standingGfx(i - 1) = lvAnimations.SelectedItem.SubItems(2)
            End If
        End If
    End With
    
    Call setAnimation
    
End Sub

'========================================================================
' Update the listview with the sprite's animations
'========================================================================
Private Sub lvUpdate(): On Error Resume Next

    With itemList(activeItemIndex).theData
    
        'Delay and idle time
        txtFrameTime.Text = CStr(.speed)
        txtIdleTime.Text = CStr(.idleTime)

        lvAnimations.ListItems.clear
        
        Dim li As ListItem
        Set li = lvAnimations.ListItems.Add(, , "South (Front View)")
        li.SubItems(1) = .gfx(ITEM_WALK_S)
        li.SubItems(2) = .standingGfx(ITEM_WALK_S)
        Set li = lvAnimations.ListItems.Add(, , "North (Back View)")
        li.SubItems(1) = .gfx(ITEM_WALK_N)
        li.SubItems(2) = .standingGfx(ITEM_WALK_N)
        Set li = lvAnimations.ListItems.Add(, , "East (Right View)")
        li.SubItems(1) = .gfx(ITEM_WALK_E)
        li.SubItems(2) = .standingGfx(ITEM_WALK_E)
        Set li = lvAnimations.ListItems.Add(, , "West (Left View)")
        li.SubItems(1) = .gfx(ITEM_WALK_W)
        li.SubItems(2) = .standingGfx(ITEM_WALK_W)
        
        Set li = lvAnimations.ListItems.Add(, , "North-West")
        li.SubItems(1) = .gfx(ITEM_WALK_NW)
        li.SubItems(2) = .standingGfx(ITEM_WALK_NW)
        Set li = lvAnimations.ListItems.Add(, , "North-East")
        li.SubItems(1) = .gfx(ITEM_WALK_NE)
        li.SubItems(2) = .standingGfx(ITEM_WALK_NE)
        Set li = lvAnimations.ListItems.Add(, , "South-West")
        li.SubItems(1) = .gfx(ITEM_WALK_SW)
        li.SubItems(2) = .standingGfx(ITEM_WALK_SW)
        Set li = lvAnimations.ListItems.Add(, , "South-East")
        li.SubItems(1) = .gfx(ITEM_WALK_SE)
        li.SubItems(2) = .standingGfx(ITEM_WALK_SE)
        
        Dim i As Long
        For i = 0 To UBound(.customGfxNames)
            If LenB(.customGfxNames(i)) Then
                Set li = lvAnimations.ListItems.Add(, , .customGfxNames(i))
                li.SubItems(1) = .customGfx(i)
            End If
        Next i
    End With

    'Set the selected animation
    lvAnimations.SelectedItem = lvAnimations.ListItems(0)

End Sub

'========================================================================
' Apply changes on the animaton listview
'========================================================================
Private Sub lvApply(): On Error Resume Next
    Dim i As Long, isCustomStance As Boolean
    i = lvAnimations.SelectedItem.Index
    isCustomStance = (i >= CUSTOM_OFFSET)
    
    txtFilename(1).Enabled = Not isCustomStance
    cmdBrowse(1).Enabled = Not isCustomStance
    lbl(4).Enabled = Not isCustomStance
    cmdCustomDelete.Enabled = isCustomStance
    txtCustomHandle.Enabled = isCustomStance
    lbl(5).Enabled = isCustomStance
            
    txtFilename(0).Text = lvAnimations.SelectedItem.SubItems(1)
    txtFilename(1).Text = lvAnimations.SelectedItem.SubItems(2)
    txtCustomHandle.Text = IIf(isCustomStance, lvAnimations.SelectedItem.Text, vbNullString)
    
    Call setAnimation
End Sub

'========================================================================
' Right click on lv views idle animations (second column)
' Left click default animations
'========================================================================
Private Sub lvAnimations_MouseUp(Button As Integer, Shift As Integer, x As Single, y As Single): On Error Resume Next
    m_viewIdles = (Button = vbRightButton)
    Call lvApply
End Sub

'========================================================================
' Edit the vector listview
'========================================================================
Private Sub lvVector_LostFocus(): On Error Resume Next
    Call m_vector.lvApply(lvVector, True)
    Call setAnimation
End Sub
Private Sub lvVector_MouseUp(Button As Integer, Shift As Integer, x As Single, y As Single): On Error Resume Next
    Call modBoard.vectorLvColumn(lvVector, x)
End Sub
Private Sub lvVector_KeyDown(keyCode As Integer, Shift As Integer): On Error Resume Next
    If modBoard.vectorLvKeyDown(lvVector, keyCode) Then
        Call m_vector.lvApply(lvVector, True)
        Call setAnimation
    End If
End Sub
Private Sub lvVector_Validate(Cancel As Boolean): On Error Resume Next
    Call m_vector.lvApply(lvVector, True)
    Call setAnimation
End Sub

'========================================================================
' Change idle time
'========================================================================
Private Sub txtIdleTime_Change(): On Error Resume Next
    itemList(activeItemIndex).theData.idleTime = CDbl(txtIdleTime.Text)
End Sub

'========================================================================
' Change speed
'========================================================================
Private Sub txtFrameTime_Change(): On Error Resume Next
    itemList(activeItemIndex).theData.speed = CDbl(txtFrameTime.Text)
End Sub

'========================================================================
' Type a key in the speed field
'========================================================================
Private Sub txtFrameTime_KeyPress(ByRef KeyAscii As Integer): On Error GoTo letter
    Dim ret As Double
    ret = CDbl(txtFrameTime.Text & chr(KeyAscii))
    Exit Sub
letter:
    If (KeyAscii <> 8) Then
        'Bad key
        KeyAscii = 0
    End If
End Sub

'========================================================================
' Type a key in the idle time field
'========================================================================
Private Sub txtIdleTime_KeyPress(ByRef KeyAscii As Integer): On Error GoTo letter
    Dim ret As Double
    ret = CDbl(txtIdleTime.Text & chr(KeyAscii))
    Exit Sub
letter:
    If (KeyAscii <> 8) Then
        'Bad key
        KeyAscii = 0
    End If
End Sub


