VERSION 5.00
Begin VB.Form frmCommonImages 
   BorderStyle     =   0  'None
   Caption         =   "Image Host"
   ClientHeight    =   735
   ClientLeft      =   0
   ClientTop       =   0
   ClientWidth     =   3180
   LinkTopic       =   "Form2"
   ScaleHeight     =   735
   ScaleWidth      =   3180
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.PictureBox picNewX 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      ForeColor       =   &H80000008&
      Height          =   375
      Left            =   2520
      Picture         =   "frmCommonImages.frx":0000
      ScaleHeight     =   345
      ScaleWidth      =   345
      TabIndex        =   3
      Top             =   120
      Width           =   375
   End
   Begin VB.PictureBox picNormal 
      Appearance      =   0  'Flat
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H80000008&
      Height          =   495
      Left            =   120
      MouseIcon       =   "frmCommonImages.frx":0F72
      MousePointer    =   99  'Custom
      Picture         =   "frmCommonImages.frx":127C
      ScaleHeight     =   465
      ScaleWidth      =   465
      TabIndex        =   2
      Top             =   120
      Width           =   495
   End
   Begin VB.PictureBox Corner 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BorderStyle     =   0  'None
      ForeColor       =   &H80000008&
      Height          =   450
      Left            =   1920
      Picture         =   "frmCommonImages.frx":17D4
      ScaleHeight     =   450
      ScaleWidth      =   450
      TabIndex        =   1
      Top             =   120
      Width           =   450
   End
   Begin VB.PictureBox picMouseOver 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      ForeColor       =   &H80000008&
      Height          =   495
      Left            =   720
      Picture         =   "frmCommonImages.frx":2265
      ScaleHeight     =   465
      ScaleWidth      =   465
      TabIndex        =   0
      Top             =   120
      Visible         =   0   'False
      Width           =   495
   End
   Begin VB.Image CloseX 
      Height          =   450
      Left            =   1320
      MousePointer    =   99  'Custom
      Picture         =   "frmCommonImages.frx":27BD
      ToolTipText     =   "Close"
      Top             =   120
      Width           =   450
   End
End
Attribute VB_Name = "frmCommonImages"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'All contents copyright 2004, Colin James Fitzpatrick
'All rights reserved.  YOU MAY NOT REMOVE THIS NOTICE.
'Read LICENSE.txt for licensing info

'Used to load common images
