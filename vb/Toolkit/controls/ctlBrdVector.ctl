VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.UserControl ctlBrdVector 
   ClientHeight    =   5835
   ClientLeft      =   0
   ClientTop       =   0
   ClientWidth     =   3510
   DefaultCancel   =   -1  'True
   ScaleHeight     =   5835
   ScaleWidth      =   3510
   Begin VB.ComboBox cmbVector 
      Height          =   315
      Left            =   0
      TabIndex        =   3
      Text            =   "cmbVector"
      ToolTipText     =   "Vector list - click to type new handle"
      Top             =   600
      Width           =   3375
   End
   Begin VB.CheckBox chkDraw 
      Caption         =   "Draw vectors"
      Height          =   375
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   1455
   End
   Begin VB.CommandButton cmdDefault 
      Caption         =   "Ok"
      Default         =   -1  'True
      Height          =   375
      Left            =   2160
      TabIndex        =   19
      Top             =   120
      Visible         =   0   'False
      Width           =   375
   End
   Begin VB.Frame fraProperties 
      Caption         =   "Properties"
      Height          =   4695
      Left            =   0
      TabIndex        =   1
      Top             =   960
      Width           =   3375
      Begin VB.HScrollBar hsbSlot 
         Height          =   255
         Left            =   1740
         Max             =   2
         TabIndex        =   14
         Top             =   2880
         Value           =   1
         Width           =   495
      End
      Begin MSComctlLib.ListView lvPoints 
         Height          =   1095
         Left            =   720
         TabIndex        =   15
         ToolTipText     =   "Vector's points. Click a number and press the Delete key to enter a new pixel coordinate"
         Top             =   3360
         Width           =   1935
         _ExtentX        =   3413
         _ExtentY        =   1931
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
      Begin VB.PictureBox picOpt 
         Appearance      =   0  'Flat
         BorderStyle     =   0  'None
         ForeColor       =   &H80000008&
         Height          =   735
         Left            =   360
         ScaleHeight     =   735
         ScaleWidth      =   2775
         TabIndex        =   16
         Top             =   240
         Width           =   2775
         Begin VB.OptionButton optType 
            Caption         =   "Waypoint"
            Height          =   375
            Index           =   16
            Left            =   1320
            TabIndex        =   7
            Top             =   360
            Width           =   1335
         End
         Begin VB.OptionButton optType 
            Caption         =   "Solid"
            Height          =   375
            Index           =   1
            Left            =   0
            TabIndex        =   4
            Top             =   0
            Value           =   -1  'True
            Width           =   855
         End
         Begin VB.OptionButton optType 
            Caption         =   "Under"
            Height          =   375
            Index           =   2
            Left            =   0
            TabIndex        =   6
            Top             =   360
            Width           =   855
         End
         Begin VB.OptionButton optType 
            Caption         =   "Stairs"
            Height          =   375
            Index           =   8
            Left            =   1320
            TabIndex        =   5
            Top             =   0
            Width           =   855
         End
      End
      Begin VB.CheckBox chkUnder 
         Caption         =   "Include background image"
         Height          =   255
         Index           =   0
         Left            =   600
         TabIndex        =   9
         ToolTipText     =   $"ctlBrdVector.ctx":0000
         Top             =   1320
         Width           =   2295
      End
      Begin VB.CheckBox chkUnder 
         Caption         =   "Include all layers below"
         Height          =   255
         Index           =   1
         Left            =   600
         TabIndex        =   10
         ToolTipText     =   $"ctlBrdVector.ctx":008B
         Top             =   1560
         Width           =   2295
      End
      Begin VB.CheckBox chkUnder 
         Caption         =   "Trigger on frame intersect"
         Height          =   255
         Index           =   2
         Left            =   600
         TabIndex        =   11
         ToolTipText     =   "Sprites appear behind the enclosed imagery if any part of their image intersects the vector"
         Top             =   1800
         Width           =   2295
      End
      Begin VB.TextBox txtStairs 
         Height          =   285
         Left            =   600
         TabIndex        =   13
         Text            =   "1"
         Top             =   2520
         Width           =   495
      End
      Begin VB.CheckBox chkClosed 
         Caption         =   "Closed vector"
         Height          =   255
         Left            =   600
         TabIndex        =   8
         ToolTipText     =   "Vector forms a closed polygon (requires three or more points)"
         Top             =   1080
         Width           =   1815
      End
      Begin VB.TextBox txtLayer 
         Height          =   285
         Left            =   600
         TabIndex        =   12
         Text            =   "1"
         Top             =   2205
         Width           =   495
      End
      Begin VB.Label lblSlot 
         Caption         =   "Slot index: 0"
         Height          =   255
         Left            =   600
         TabIndex        =   20
         ToolTipText     =   "Index for use with vector access RPGCode functions"
         Top             =   2940
         Width           =   1215
      End
      Begin VB.Label lblStairs 
         Caption         =   "Stairs to layer"
         Height          =   255
         Left            =   1200
         TabIndex        =   18
         ToolTipText     =   "Layer the player is transported to if it intersects a stairs-type vector"
         Top             =   2565
         Width           =   975
      End
      Begin VB.Label lblLayer 
         Caption         =   "Layer"
         Height          =   255
         Left            =   1200
         TabIndex        =   17
         ToolTipText     =   "Layer the vector is located on"
         Top             =   2265
         Width           =   975
      End
   End
   Begin VB.CommandButton cmdDelete 
      Caption         =   "Delete"
      Height          =   375
      Left            =   2520
      TabIndex        =   2
      Top             =   120
      Width           =   855
   End
End
Attribute VB_Name = "ctlBrdVector"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
'========================================================================
' The RPG Toolkit, Version 3
' This file copyright (C) 2007  Jonathan D. Hughes
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

Option Explicit

Private Sub apply(): On Error Resume Next
    Dim vector As CVector, i As Long, ctl As OptionButton
    Call activeBoard.setUndo
    Set vector = activeBoard.toolbarGetCurrent(BS_VECTOR)
    If Not vector Is Nothing Then
        With vector
            For Each ctl In optType
                If ctl.value Then .tiletype = ctl.Index
            Next ctl
            
            .bClosed = ((chkClosed.value <> 0) Or optType(TT_UNDER).value)

            Select Case .tiletype
                Case TT_UNDER
                    .bClosed = True
                    .attributes = 0
                    For i = 0 To 3 - 1
                        If chkUnder(i).value Then .attributes = .attributes Or (2 ^ i)
                    Next i
                Case TT_STAIRS
                    .attributes = val(txtStairs.Text)
                Case TT_UNIDIRECTIONAL
                    .bClosed = False
            End Select
            
            .layer = Abs(val(txtLayer.Text))
            Call vector.lvApply(lvPoints)
        End With
        Call activeBoard.drawAll
        Call populate(cmbVector.ListIndex, vector)
    End If
End Sub
Private Sub disableAll(): On Error Resume Next
    Dim i As Control
    For Each i In UserControl
        i.Enabled = False
        i.Text = vbNullString
    Next i
    cmbVector.Enabled = True
    Call activeBoard.toolbarSetCurrent(BTAB_VECTOR, -1)
    Call lvPoints.ListItems.clear
End Sub
Private Sub enableAll(): On Error Resume Next
    Dim i As Control
    For Each i In UserControl
        i.Enabled = True
    Next i
    chkDraw.value = Abs(activeBoard.toolbarDrawObject(BS_VECTOR))
End Sub

Public Sub populate(ByVal Index As Long, ByRef vector As CVector)  ':on error resume next
    Dim i As Long, j As Long
    
    tkMainForm.bTools_Tabs.Height = tkMainForm.pTools.Height - tkMainForm.bTools_Tabs.Top
    UserControl.Height = tkMainForm.bTools_Tabs.Height - tkMainForm.bTools_ctlVector.Top - 256
    fraProperties.Height = UserControl.Height - fraProperties.Top
    lvPoints.Height = fraProperties.Height - lvPoints.Top - 256
    
    If vector Is Nothing Then
        Call disableAll
        Exit Sub
    End If
    
    Call activeBoard.toolbarSetCurrent(BTAB_VECTOR, Index)
    Call enableAll
    
    If cmbVector.ListIndex <> Index Then cmbVector.ListIndex = Index
    cmbVector.List(Index) = CStr(Index) & ": " & IIf(LenB(vector.handle), vector.handle, BRD_VECTOR_HANDLE)
    
    'Option buttons have been assigned TT_ values as indices.
    If vector.tiletype <> TT_NULL Then optType(vector.tiletype).value = True
    chkClosed.value = Abs(vector.bClosed)
    txtLayer.Text = str(vector.layer)
    txtStairs.Enabled = (vector.tiletype = TT_STAIRS)
    lblStairs.Enabled = (vector.tiletype = TT_STAIRS)
    chkClosed.Enabled = (vector.tiletype <> TT_UNIDIRECTIONAL)
    txtLayer.Enabled = (vector.tiletype <> TT_WAYPOINT)
    lblLayer.Enabled = (vector.tiletype <> TT_WAYPOINT)
    lblSlot.Caption = "Slot index: " & CStr(Index)
    
    For i = 0 To chkUnder.count - 1
        chkUnder(i).Enabled = (vector.tiletype = TT_UNDER)
        chkUnder(i).value = 0
    Next i
    
    Select Case vector.tiletype
        Case TT_UNDER
            chkUnder(0).value = Abs((vector.attributes And TA_BRD_BACKGROUND) <> 0)
            chkUnder(1).value = Abs((vector.attributes And TA_ALL_LAYERS_BELOW) <> 0)
            chkUnder(2).value = Abs((vector.attributes And TA_FRAME_INTERSECT) <> 0)
        Case TT_STAIRS
            txtStairs.Text = str(vector.attributes)
        Case TT_UNIDIRECTIONAL
            chkClosed.value = 0
    End Select
    
    If vector.tiletype = TT_NULL Then Exit Sub
        
    Call vector.lvPopulate(lvPoints)
    
End Sub

Public Property Get ActiveControl() As Control: On Error Resume Next
    Set ActiveControl = UserControl.ActiveControl
End Property
Public Property Get getCombo() As ComboBox: On Error Resume Next
    Set getCombo = cmbVector
End Property

Private Sub chkDraw_MouseUp(Button As Integer, Shift As Integer, x As Single, y As Single)
    activeBoard.toolbarDrawObject(BS_VECTOR) = chkDraw.value
End Sub
Private Sub chkUnder_MouseUp(Index As Integer, Button As Integer, Shift As Integer, x As Single, y As Single): On Error Resume Next
    Call apply
End Sub
Private Sub chkClosed_MouseUp(Button As Integer, Shift As Integer, x As Single, y As Single): On Error Resume Next
    Call apply
End Sub
Private Sub cmbVector_Click(): On Error Resume Next
    If cmbVector.ListIndex <> -1 Then Call activeBoard.toolbarChange(cmbVector.ListIndex, BS_VECTOR)
End Sub
Private Sub cmbVector_LostFocus(): On Error Resume Next
        Call activeBoard.vectorSetHandle(cmbVector.Text)
End Sub
Private Sub cmdDefault_Click(): On Error Resume Next
    'Default button on form: hitting the Enter key calls this function.
    
    'Process the Enter Key for the handle ComboBox here, since vbKeyReturn is not captured
    'by _KeyDown() because there is a Default button on the control.
    If ActiveControl Is cmbVector Then
        Call activeBoard.vectorSetHandle(cmbVector.Text)
    Else
        Call apply
        Call activeBoard.drawAll
    End If
End Sub
Private Sub cmdDelete_Click(): On Error Resume Next
    Call activeBoard.setUndo
    Call activeBoard.vectorDeleteCurrent(BS_VECTOR)
    Call activeBoard.drawAll
End Sub
Private Sub hsbSlot_Change(): On Error Resume Next
    Dim i As Long
    If hsbSlot.value <> 1 Then
        i = cmbVector.ListIndex
        Call activeBoard.vectorSwapSlots(i, i + hsbSlot.value - 1)
    End If
    hsbSlot.value = 1
End Sub
Private Sub optType_MouseUp(Index As Integer, Button As Integer, Shift As Integer, x As Single, y As Single): On Error Resume Next
    Call apply
End Sub
Private Sub txtLayer_LostFocus(): On Error Resume Next
    Call apply
End Sub
Private Sub txtLayer_Validate(Cancel As Boolean): On Error Resume Next
    Call apply
End Sub
Private Sub txtStairs_LostFocus(): On Error Resume Next
    Call apply
End Sub
Private Sub txtStairs_Validate(Cancel As Boolean): On Error Resume Next
    Call apply
End Sub
Private Sub lvPoints_LostFocus(): On Error Resume Next
    Call apply
End Sub
Private Sub lvPoints_MouseUp(Button As Integer, Shift As Integer, x As Single, y As Single): On Error Resume Next
    Call modBoard.vectorLvColumn(lvPoints, x)
End Sub
Private Sub lvPoints_KeyDown(keyCode As Integer, Shift As Integer): On Error Resume Next
    If modBoard.vectorLvKeyDown(lvPoints, keyCode) Then Call apply
End Sub
Private Sub lvPoints_Validate(Cancel As Boolean): On Error Resume Next
    Call apply
End Sub
