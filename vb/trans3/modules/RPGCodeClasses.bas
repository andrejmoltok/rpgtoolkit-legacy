Attribute VB_Name = "RPGCodeClasses"
'=========================================================================
'All contents copyright 2004, Colin James Fitzpatrick (KSNiloc)
'All rights reserved. YOU MAY NOT REMOVE THIS NOTICE.
'Read LICENSE.txt for licensing info
'=========================================================================

'=========================================================================
' RPGCode classes
'=========================================================================

'=========================================================================
' Remaining to be done:
'   + Protect the base class' private methods and members from the
'     inheriting class' methods
'=========================================================================

Option Explicit

'=========================================================================
' All classes
'=========================================================================
Public classes() As RPGCODE_CLASS_INSTANCE  ' All classes

'=========================================================================
' Array of used handles
'=========================================================================
Public objHandleUsed() As Boolean           ' This handle used?

'=========================================================================
' An instance of a class
'=========================================================================
Private Type RPGCODE_CLASS_INSTANCE
    hClass As Long                          ' Handle to this class
    strInstancedFrom As String              ' It was instanced from this class
    objClass As Object                      ' For internal class use only
End Type

'=========================================================================
' A method
'=========================================================================
Private Type RPGCodeMethod
    name As String                          ' Name of the method
    line As Long                            ' Line method is defined on
    override As String                      ' Name used in inheritance situations
End Type

'=========================================================================
' A scope in a class
'=========================================================================
Private Type RPGCODE_CLASS_SCOPE
    strVars() As String                     ' Variables in this scope
    isDynamicArray() As Boolean             ' Are these vars dynamic arrays?
    methods() As RPGCodeMethod              ' Methods in this scope
End Type

'=========================================================================
' A class
'=========================================================================
Public Type RPGCODE_CLASS
    strName As String                       ' Name of this class
    scopePrivate As RPGCODE_CLASS_SCOPE     ' Private scope
    scopePublic As RPGCODE_CLASS_SCOPE      ' Public scope
    isInterface As Boolean                  ' Is an interface?
End Type

'=========================================================================
' Main data on classes (per program)
'=========================================================================
Private Type RPGCODE_CLASS_MAIN_DATA
    classes() As RPGCODE_CLASS              ' Classes this program can instance
    nestle() As Long                        ' Nestle of classes
    insideClass As Boolean                  ' Inside a class?
End Type

'=========================================================================
' An RPGCode program
'=========================================================================
Public Type RPGCodeProgram
    program() As String                     ' The program text
    methods() As RPGCodeMethod              ' Methods in this program
    programPos As Long                      ' Current position in program
    included(50) As String                  ' Included files
    Length As Long                          ' Length of program
    heapStack() As Long                     ' Stack of local heaps
    currentHeapFrame As Long                ' Current heap frame
    boardNum As Long                        ' The corresponding board index of the program (default to 0)
    threadID As Long                        ' The thread id (-1 if not a thread)
    compilerStack() As String               ' Stack used by 'compiled' programs
    currentCompileStackIdx As Long          ' Current index of compilerStack
    looping As Boolean                      ' Is a multitask program looping?
    autoLocal As Boolean                    ' Force implicitly created variables to the local scope?
    classes As RPGCODE_CLASS_MAIN_DATA      ' Class stuff
End Type

'=========================================================================
' Check if something is an internal class
'=========================================================================
Public Function isInternalClass(ByVal theClass As String, ByRef theObject As Object) As Boolean

    ' Capitalize theClass
    theClass = UCase$(theClass)

    ' Assume it is
    isInternalClass = True

    ' Switch on internal classes
    Select Case theClass

    '    Case "BOOL"
    '        ' It's a boolean
    '        Set theObject = New CRPGCodeBool

        Case Else
            ' It's not an internal class
            isInternalClass = False

    End Select

End Function

'=========================================================================
' Check a method override name
'=========================================================================
Public Function checkOverrideName(ByRef theClass As RPGCODE_CLASS, ByVal theMethod As String) As String

    ' Capitalize theMethod
    theMethod = UCase$(Trim$(theMethod))

    ' Loop variables
    Dim scopeIdx As Long, idx As Long

    ' A scope
    Dim scope As RPGCODE_CLASS_SCOPE

    ' Loop over each scope
    For scopeIdx = 0 To 1
        ' Get this scope
        If (scopeIdx = 0) Then
            scope = theClass.scopePublic
        Else
            scope = theClass.scopePrivate
        End If
        ' Loop over each of its methods
        For idx = 0 To UBound(scope.methods)
            ' Found the method
            If (scope.methods(idx).name = theMethod) Then
                ' Check for an override
                If (LenB(scope.methods(idx).override) <> 0) Then
                    ' Return this
                    checkOverrideName = scope.methods(idx).override
                End If
                ' Either way, bail
                Exit Function
            End If
        Next idx
    Next scopeIdx

End Function

'=========================================================================
' Determine if something is an object
'=========================================================================
Public Function isObject(ByVal hClass As Long, ByRef prg As RPGCodeProgram) As Boolean

    On Error Resume Next

    ' Return if it's an object
    isObject = (getClass(hClass, prg).strName <> "INVALID")

End Function

'=========================================================================
' Add a class to a program
'=========================================================================
Public Sub addClassToProgram(ByRef theClass As RPGCODE_CLASS, ByRef prg As RPGCodeProgram)

    On Error Resume Next

    Dim idx As Long         ' Loop var
    Dim pos As Long         ' Position to use

    ' Make pos void
    pos = -1

    ' Check all classes already in program
    For idx = 0 To UBound(prg.classes.classes)
        If (theClass.strName = prg.classes.classes(idx).strName) Then
            ' Already in program
            Exit Sub

        ElseIf (LenB(prg.classes.classes(idx).strName) = 0) Then
            ' Free space
            If (pos = -1) Then
                pos = idx
            End If

        End If
    Next idx

    If (pos = -1) Then
        ' No free spaces
        ReDim Preserve prg.classes.classes(UBound(prg.classes.classes) + 1)
        pos = UBound(prg.classes.classes)
    End If

    ' Write in the data
    prg.classes.classes(pos) = theClass

End Sub

'=========================================================================
' Read all data on classes from a program
'=========================================================================
Public Sub spliceUpClasses(ByRef prg As RPGCodeProgram)

    On Error Resume Next

    Dim lineIdx As Long         ' Current line
    Dim inClass As Boolean      ' Inside a class?
    Dim scope As String         ' Current scope (public or private)
    Dim cmd As String           ' The command
    Dim opening As Boolean      ' Looking for { bracket?
    Dim depth As Long           ' Depth in class
    Dim classIdx As Long        ' Current class
    Dim inStruct As Boolean     ' In a structure?
    Dim methodHere As Boolean   ' Method declared on this line?
    Dim ignoreCheck As Long     ' Ignore a check of some kind

    ' Some vars for splitting up the line
    Dim parts() As String, delimiters() As String, chars(1) As String

    ' Make classIdx void
    classIdx = -1

    ' Loop over each line
    For lineIdx = 0 To UBound(prg.program)

        If (LeftB$(LCase$(prg.program(lineIdx)), 12) = "global") Then

            ' Define a var
            Dim constParts() As String, constDelimiters(0) As String, ud() As String
            constDelimiters(0) = " "
            constParts = multiSplit(prg.program(lineIdx), constDelimiters, ud, True, True)

            ' Check for correct number of parts
            If (UBound(constParts) = 2) Then

                ' Only define if it's not existent
                If ( _
                        (Not numVarExists(constParts(1), globalHeap) And _
                        (Not numVarExists(constParts(1), prg.heapStack(prg.currentHeapFrame))))) _
                            Then

                    ' Create the var
                    Dim retval As RPGCODE_RETURN
                    Call DoSingleCommand(constParts(1) & "=" & constParts(2), prg, retval)
                    ' constParts(2) = Mid$(ParseRPGCodeCommand(spliceForObjects("x=" & constParts(2), prg), prg), 3)
                    ' Call SetVariable(constParts(1), constParts(2), prg)

                End If

                ' Don't run this line
                cmd = vbNullString

            End If

        Else

            ' Get the command on this line
            cmd = UCase$(GetCommandName(prg.program(lineIdx)))

        End If

        ignoreCheck = ignoreCheck - 1

        If (opening And inClass And (cmd = "OPENBLOCK")) Then
            ' Found first { bracket
            opening = False
            depth = depth + 1

        ElseIf (inClass And (Not opening) And (cmd = "OPENBLOCK")) Then
            ' Getting deeper
            depth = depth + 1

        ElseIf (inClass And (Not opening) And (cmd = "CLOSEBLOCK")) Then
            ' Coming out
            depth = depth - 1
            ' Check if we're completely out
            If (depth = 0) Then
                If (UBound(parts) >= 1) Then
                    ' Inheritance here
                    Dim inheritIdx As Long
                    For inheritIdx = 1 To UBound(parts)
                        If (inheritIdx <> 1) Then
                            If (delimiters(inheritIdx - 1) = ":") Then
                                ' Syntax error
                                Call debugger("Syntax error: " & prg.program(lineIdx))
                            End If
                        End If
                        Dim toInherit As String
                        toInherit = Trim$(parts(inheritIdx))
                        If (Not canInstanceClass(toInherit, prg)) Then
                            Call debugger("Base class " & toInherit & " not found-- " & prg.program(lineIdx))
                        Else
                            ' Make toInherit caps
                            toInherit = UCase$(toInherit)
                            ' Loop over every class it could be
                            Dim idx As Long, theClass As RPGCODE_CLASS
                            For idx = 0 To UBound(prg.classes.classes)
                                If (prg.classes.classes(idx).strName = toInherit) Then
                                    ' Found it!
                                    theClass = prg.classes.classes(idx)
                                    ' Make sure it's an interface if this class is an interface
                                    If ((Not theClass.isInterface) And (prg.classes.classes(classIdx).isInterface)) Then
                                        ' Not an interface
                                        Call debugger("Interfaces can only inherit other interfaces! -- " & prg.program(lineIdx))
                                        Exit Sub
                                    End If
                                    ' Break
                                    Exit For
                                End If
                            Next idx
                            ' For each scope
                            Dim scopeIdx As Long
                            For scopeIdx = 0 To 1
                                ' Get this scope
                                Dim theScope As RPGCODE_CLASS_SCOPE
                                If (scopeIdx = 0) Then
                                    theScope = theClass.scopePublic
                                Else
                                    theScope = theClass.scopePrivate
                                End If
                                ' Loop over each method
                                If (Not theClass.isInterface) Then
                                    For idx = 0 To UBound(theScope.methods)
                                        If (scopeIdx = 0) Then
                                            Call addMethodToScope(prg.classes.classes(classIdx).strName, theScope.methods(idx).name, prg, prg.classes.classes(classIdx).scopePublic, toInherit, , , True)
                                        Else
                                            Call addMethodToScope(prg.classes.classes(classIdx).strName, theScope.methods(idx).name, prg, prg.classes.classes(classIdx).scopePrivate, toInherit, , , True)
                                        End If
                                    Next idx
                                Else
                                    ' Inheriting class should implement an interface's methods
                                    For idx = 0 To UBound(theScope.methods)
                                        If (scopeIdx = 0) Then
                                            Call addMethodToScope(prg.classes.classes(classIdx).strName, theScope.methods(idx).name, prg, prg.classes.classes(classIdx).scopePublic)
                                        Else
                                            Call addMethodToScope(prg.classes.classes(classIdx).strName, theScope.methods(idx).name, prg, prg.classes.classes(classIdx).scopePrivate)
                                        End If
                                    Next idx
                                End If
                                ' Loop over each var
                                For idx = 0 To UBound(theScope.strVars)
                                    If (InStr(1, prg.program(lineIdx), "[")) Then
                                        ' It's an array
                                        If (scopeIdx = 1) Then
                                            Call addArrayToScope(theScope.strVars(idx), prg.classes.classes(classIdx).scopePrivate)
                                        Else
                                            Call addArrayToScope(theScope.strVars(idx), prg.classes.classes(classIdx).scopePublic)
                                        End If
                                    Else
                                        If (scopeIdx = 1) Then
                                            Call addVarToScope(theScope.strVars(idx), prg.classes.classes(classIdx).scopePrivate)
                                        Else
                                            Call addVarToScope(theScope.strVars(idx), prg.classes.classes(classIdx).scopePublic)
                                        End If
                                    End If
                                Next idx
                            Next scopeIdx
                        End If
                    Next inheritIdx
                End If
                inClass = False
                inStruct = False
                scope = vbNullString
            End If

        ElseIf (cmd = "CLASS" Or cmd = "STRUCT" Or cmd = "INTERFACE") Then
            ' Found a class
            depth = 0
            ignoreCheck = 0
            methodHere = False
            inClass = True
            opening = True
            classIdx = classIdx + 1
            ReDim Preserve prg.classes.classes(classIdx)
            ReDim prg.classes.classes(classIdx).scopePrivate.methods(0)
            ReDim prg.classes.classes(classIdx).scopePrivate.strVars(0)
            ReDim prg.classes.classes(classIdx).scopePrivate.isDynamicArray(0)
            ReDim prg.classes.classes(classIdx).scopePublic.methods(0)
            ReDim prg.classes.classes(classIdx).scopePublic.strVars(0)
            ReDim prg.classes.classes(classIdx).scopePublic.isDynamicArray(0)
            prg.classes.classes(classIdx).isInterface = (cmd = "INTERFACE")

            ' Split up the line
            chars(0) = ":"
            chars(1) = ","
            parts = multiSplit(UCase$(prg.program(lineIdx)), chars, delimiters, False)
            prg.classes.classes(classIdx).strName = GetMethodName(Trim$(parts(0)))

            If (cmd = "STRUCT") Then
                ' It's a structure, default to public visibility
                scope = "public"
                inStruct = True
            ElseIf (cmd = "INTERFACE") Then
                ' Default to public in interfaces
                scope = "public"
                inStruct = False
            Else
                ' Default to private in classes
                scope = "private"
                inStruct = False
            End If

        ElseIf (inClass And (LenB(scope) <> 0) And (LenB(prg.program(lineIdx)) <> 0) And (Right$(prg.program(lineIdx), 1) <> ":") And (depth = 1)) Then
            If (InStrB(1, prg.program(lineIdx), "(")) Then
                ' Found a method
                If (Not inStruct) Then
                    ' Check if the method is right here
                    Dim methodCheckIdx As Long
                    methodCheckIdx = lineIdx
                    Do
                        methodCheckIdx = methodCheckIdx + 1
                        If (LenB(prg.program(methodCheckIdx)) <> 0) Then
                            ' Check if the method is here
                            methodHere = (prg.program(methodCheckIdx) = "{")
                            ignoreCheck = methodCheckIdx - lineIdx + 2 ' + 2 to compensate for
                                                                       ' block opening and closing
                            ' Leave this loop
                            Exit Do
                        End If
                    Loop
                    If (scope = "private") Then
                        Call addMethodToScope(prg.classes.classes(classIdx).strName, prg.program(lineIdx), prg, prg.classes.classes(classIdx).scopePrivate, , (prg.classes.classes(classIdx).isInterface))
                    Else
                        Call addMethodToScope(prg.classes.classes(classIdx).strName, prg.program(lineIdx), prg, prg.classes.classes(classIdx).scopePublic, , (prg.classes.classes(classIdx).isInterface))
                    End If
                Else
                    Call debugger("Methods are not valid in structures-- " & prg.program(lineIdx))
                End If
            Else
                ' Found a variable
                If (InStrB(1, prg.program(lineIdx), "[")) Then
                    ' It's an array
                    If (scope = "private") Then
                        Call addArrayToScope(prg.program(lineIdx), prg.classes.classes(classIdx).scopePrivate)
                    Else
                        Call addArrayToScope(prg.program(lineIdx), prg.classes.classes(classIdx).scopePublic)
                    End If
                Else
                    If (scope = "private") Then
                        Call addVarToScope(prg.program(lineIdx), prg.classes.classes(classIdx).scopePrivate)
                    Else
                        Call addVarToScope(prg.program(lineIdx), prg.classes.classes(classIdx).scopePublic)
                    End If
                End If
            End If

        End If

        If ((inClass) And ((depth = 1) Or (depth = 0))) Then

            Select Case LCase$(Trim$(prg.program(lineIdx)))

                Case "private:"
                    ' Found start of private scope
                    scope = "public"
                    methodHere = False
                    If (inStruct) Then
                        scope = "error"
                    Else
                        scope = "private"
                    End If

                Case "public:"
                    ' Found start of public scope
                    scope = "public"
                    methodHere = False
                    If (inStruct) Then
                        scope = "error"
                    Else
                        scope = "public"
                    End If

            End Select

            If (scope = "error") Then
                ' No scope in structures
                Call debugger("Scope is not valid in structures-- " & prg.program(lineIdx))
                scope = "public"
            End If

            ' Make sure this line isn't run
            If (Not methodHere) Then
                prg.program(lineIdx) = vbNullString
            End If

        End If

        If ((depth = 1) And (ignoreCheck = 0)) Then
            methodHere = False
        End If

    Next lineIdx

End Sub

'=========================================================================
' Add an array to a scope
'=========================================================================
Private Sub addArrayToScope(ByVal theVar As String, ByRef scope As RPGCODE_CLASS_SCOPE)

    On Error Resume Next

    Dim toParse As String           ' Text to parse
    Dim variableType As String      ' Type of var
    Dim start As Long               ' First [
    Dim tEnd As Long                ' Last ]
    Dim variableName As String      ' Name of var
    Dim parseArrayD() As String     ' Dimensions
    Dim idx As Long                 ' Loop var

    ' Set toParse to the text passed in
    toParse = Trim$(theVar)

    ' Grab the variable's type (! or $)
    variableType = Right$(toParse, 1)
    If (variableType <> "!" And variableType <> "$") Then
        ' It's an object
        variableType = vbNullString
    End If

    ' See where the first [ is
    start = InStr(1, toParse, "[")

    ' Grab the variable's name
    variableName = Mid$(toParse, 1, start - 1)

    ' Find the last ]
    tEnd = InStr(1, StrReverse(toParse), "]")
    tEnd = Len(toParse) - tEnd + 1

    ' Just keep what's inbetween the two
    toParse = Trim$(Mid$(toParse, start + 1, tEnd - start - 1))

    ' Check if it's a dynamic array
    If (LenB(toParse) = 0) Then

        ' Add var to the scope as is
        Call addVarToScope(variableName & variableType, scope, True)
        Exit Sub

    End If

    ' Split it at '][' (bewteen elements)
    parseArrayD() = Split(toParse, "][")

    ' Add the vars
    ReDim x(UBound(parseArrayD)) As Long
    ReDim size(UBound(parseArrayD)) As Long
    For idx = 0 To UBound(size)
        size(idx) = CLng(parseArrayD(idx))
    Next idx
    Call getVarsFromArray(0, size, x, scope, variableName, variableType)

End Sub

'=========================================================================
' Add a variable to a scope
'=========================================================================
Private Sub addVarToScope(ByVal theVar As String, ByRef scope As RPGCODE_CLASS_SCOPE, Optional ByVal isDynamicArray As Boolean)

    On Error Resume Next

    Dim origName As String  ' Name in original case
    Dim idx As Long         ' Loop var
    Dim pos As Long         ' Position we're using

    ' Make theVar all caps
    origName = Trim$(theVar)
    theVar = Trim$(UCase$(theVar))

    ' Default to ! if no type def character
    If (Right$(theVar, 1) <> "!" And Right$(theVar, 1) <> "$") Then
        ' Add the !
        theVar = theVar & "!"
    End If

    ' Make pos void
    pos = -1

    ' Loop over all vars in this scope
    For idx = 0 To UBound(scope.strVars)
        If (scope.strVars(idx) = theVar) Then
            Call debugger("Illegal redefinition of variable " & origName)
            Exit Sub

        ElseIf (LenB(scope.strVars(idx)) = 0) Then
            If (pos = -1) Then
                ' Free position!
                pos = idx
            End If

        End If
    Next idx

    If (pos = -1) Then
        ' Didn't find a position
        ReDim Preserve scope.strVars(UBound(scope.strVars) + 1)
        ReDim Preserve scope.isDynamicArray(UBound(scope.isDynamicArray) + 1)
        pos = UBound(scope.strVars)
    End If

    ' Write in the data
    scope.strVars(pos) = theVar
    scope.isDynamicArray(pos) = isDynamicArray

End Sub

'=========================================================================
' Add a method to a scope
'=========================================================================
Public Sub addMethodToScope(ByVal theClass As String, ByVal Text As String, ByRef prg As RPGCodeProgram, ByRef scope As RPGCODE_CLASS_SCOPE, Optional ByVal overrideName As String = vbNullString, Optional ByVal needNotExist As Boolean, Optional ByVal internalClass As Boolean, Optional ByVal noErrorOnRedefine As Boolean)

    On Error Resume Next

    Dim theLine As Long         ' Line method starts on
    Dim methodName As String    ' Name of method
    Dim origName As String      ' Name of method in orig case
    Dim idx As Long             ' Loop variable
    Dim pos As Long             ' Pos we're using

    If (LenB(Text) = 0) Then
        ' No text, no method, no wasted time
        Exit Sub
    End If

    If (Not internalClass) Then

        ' Get the method's name
        origName = GetMethodName(removeClassName(Text))
        If (LenB(overrideName) = 0) Then
            methodName = UCase$(theClass) & "::" & UCase$(origName)
        Else
            methodName = overrideName & "::" & UCase$(origName)
        End If

        ' Get line method starts on
        theLine = getMethodLine(methodName, prg)

        ' Check if we errored out
        If ((theLine = -1) And (Not needNotExist)) Then
            Call debugger("Could not find method " & origName & " -- " & Text)
            Exit Sub
        ElseIf ((theLine <> -1) And (needNotExist)) Then
            Call debugger("Interfaces should not implement methods -- will be implemented by the inheriting class -- " & Text)
            Exit Sub
        End If

    Else

        ' Use the text passed in
        origName = Text

    End If

    ' Make pos void
    pos = -1

    ' Find an open position
    For idx = 0 To UBound(scope.methods)
        If (scope.methods(idx).name = UCase$(origName)) Then
            ' Illegal redifinition
            If (Not noErrorOnRedefine) Then
                Call debugger("Illegal redefinition of method " & origName & " -- " & Text)
            End If
            Exit Sub

        ElseIf (LenB(scope.methods(idx).name) = 0) Then
            If (pos = -1) Then
                ' Found a spot
                pos = idx
            End If
        End If
    Next idx

    ' Check if we found a spot
    If (pos = -1) Then
        ' Didn't find one
        ReDim Preserve scope.methods(UBound(scope.methods) + 1)
        pos = UBound(scope.methods)
    End If

    ' Add in the data
    scope.methods(pos).line = theLine
    scope.methods(pos).name = UCase$(origName)
    scope.methods(pos).override = overrideName

End Sub

'=========================================================================
' Remove class name from a function
'=========================================================================
Private Function removeClassName(ByVal Text As String) As String

    On Error Resume Next

    Dim idx As Long         ' For loop var
    Dim char As String * 2  ' Characters

    For idx = 1 To Len(Text)
        ' Get a character
        char = Mid$(Text, idx, 2)
        ' Check if it's the scope operator
        If (char = "::") Then
            ' Found it
            removeClassName = Mid$(Text, idx + 2)
            Exit Function
        End If
    Next idx

    ' Didn't find it
    removeClassName = Text

End Function

'=========================================================================
' Initiate the class system
'=========================================================================
Public Sub initRPGCodeClasses()
    ReDim objHandleUsed(0)
    ReDim classes(0)
    Call newHandle
End Sub

'=========================================================================
' Kill a handle number
'=========================================================================
Private Sub killHandle(ByVal hClass As Long)

    On Error Resume Next

    If (Not UBound(objHandleUsed) < hClass) Then
        ' Write in the data
        objHandleUsed(hClass) = False
    End If

End Sub

'=========================================================================
' Get a new handle number
'=========================================================================
Private Function newHandle() As Long

    On Error Resume Next

    Dim idx As Long     ' Loop var
    Dim pos As Long     ' Position to use

    ' Make pos void
    pos = -1

    ' Loop over each handle
    For idx = 0 To UBound(objHandleUsed)
        If (Not objHandleUsed(idx)) Then
            ' Free position
            pos = idx
            Exit For
        End If
    Next idx

    If (pos = -1) Then
        ' Didn't find a spot
        ReDim Preserve objHandleUsed(UBound(objHandleUsed) + 1)
        pos = UBound(objHandleUsed)
    End If

    ' Write in the data
    objHandleUsed(pos) = True
    newHandle = pos

End Function

'=========================================================================
' Check if a program can instance a class
'=========================================================================
Private Function canInstanceClass(ByVal theClass As String, ByRef prg As RPGCodeProgram) As Boolean

    On Error Resume Next

    Dim idx As Long     ' Loop var

    ' Loop over each class we can instance
    For idx = 0 To UBound(prg.classes.classes)
        If (prg.classes.classes(idx).strName = UCase$(theClass)) Then
            ' Yes, we can
            canInstanceClass = True
            Exit Function
        End If
    Next idx

    ' If we get here, we can't instance this class

End Function

'=========================================================================
' Determine if a variable is a member of a class
'=========================================================================
Public Function isVarMember(ByVal var As String, ByVal hClass As Long, ByRef prg As RPGCodeProgram, Optional ByVal outside As Boolean) As Boolean

    On Error Resume Next

    Dim idx As Long, scopeIdx As Long   ' Loop var
    Dim theClass As RPGCODE_CLASS       ' The class
    Dim scope As RPGCODE_CLASS_SCOPE    ' A scope

    ' Get the class
    theClass = getClass(hClass, prg)

    If (theClass.strName = "INVALID") Then
        ' Class doesn't exist!
        Exit Function
    End If

    ' Make the var all caps
    var = Trim$(UCase$(var))

    ' For each scope
    For scopeIdx = 0 To 1

        ' Get the scope
        If (scopeIdx = 1) Then
            ' Private scope
            scope = theClass.scopePrivate
        Else
            ' Public scope
            scope = theClass.scopePublic
        End If

        ' For each var within that scope
        For idx = 0 To UBound(scope.strVars)

            If (scope.isDynamicArray(idx)) Then
                ' Check this dynamic array
                Dim istr As Long
                istr = InStr(1, var, "[")
                If (istr) Then
                    If (scope.strVars(idx) = (Left$(var, istr - 1) & Right$(var, 1))) Then
                        ' It is a member
                        isVarMember = True
                        Exit Function
                    End If
                End If

            ElseIf (scope.strVars(idx) = var) Then
                ' Found it
                isVarMember = True
                Exit Function

            End If

        Next idx

        If (outside) Then
            ' Don't check private scope
            Exit Function
        End If

    Next scopeIdx

    ' It we get here, then this variable is not a member of the class

End Function

'=========================================================================
' Determine if a method is a member of a class
'=========================================================================
Public Function isMethodMember(ByVal methodName As String, ByVal hClass As Long, ByRef prg As RPGCodeProgram, Optional ByVal outside As Boolean) As Boolean

    On Error Resume Next

    Dim idx As Long, scopeIdx As Long   ' Loop var
    Dim theClass As RPGCODE_CLASS       ' The class
    Dim scope As RPGCODE_CLASS_SCOPE    ' A scope

    ' Get the class
    theClass = getClass(hClass, prg)

    If (theClass.strName = "INVALID") Then
        ' Class doesn't exist!
        Exit Function
    End If

    ' Make the method name all caps
    methodName = Trim$(UCase$(methodName))

    ' For each scope
    For scopeIdx = 0 To 1
        ' Get the scope
        If (scopeIdx = 1) Then
            ' Private scope
            scope = theClass.scopePrivate
        Else
            ' Public scope
            scope = theClass.scopePublic
        End If
        ' For each method within that scope
        For idx = 0 To UBound(scope.methods)
            If (scope.methods(idx).name = methodName) Then
                ' Found it
                isMethodMember = True
                Exit Function
            End If
        Next idx
        If (outside) Then
            ' Don't check private scope
            Exit Function
        End If
    Next scopeIdx

    ' It we get here, then this method is not a member of the class

End Function

'=========================================================================
' Get the *real* name of a variable
'=========================================================================
Public Function getObjectVarName(ByVal theVar As String, ByVal hClass As Long) As String

    On Error Resume Next

    ' Return the new name
    getObjectVarName = CStr(hClass) & "::" & theVar

End Function

'=========================================================================
' Decrease the nestle
'=========================================================================
Public Sub decreaseNestle(ByRef prg As RPGCodeProgram)

    On Error Resume Next

    ' Shrink the nestle array
    ReDim Preserve prg.classes.nestle(UBound(prg.classes.nestle) - 1)

    If (UBound(prg.classes.nestle) = 0) Then
        ' Flag we're out of all classes
        prg.classes.insideClass = False
    End If

End Sub

'=========================================================================
' Get value on top of nestle stack
'=========================================================================
Public Function topNestle(ByRef prg As RPGCodeProgram) As Long

    On Error Resume Next

    ' Return the value
    topNestle = prg.classes.nestle(UBound(prg.classes.nestle))

End Function

'=========================================================================
' Increase nestle
'=========================================================================
Public Sub increaseNestle(ByVal push As Long, ByRef prg As RPGCodeProgram)

    On Error Resume Next

    ' Enlarge the nestle array
    ReDim Preserve prg.classes.nestle(UBound(prg.classes.nestle) + 1)

    ' Push on the value
    prg.classes.nestle(UBound(prg.classes.nestle)) = push

    ' Flag we're inside a class
    prg.classes.insideClass = True

End Sub

'=========================================================================
' Get a class from an instance of it
'=========================================================================
Public Function getClass(ByVal hClass As Long, ByRef prg As RPGCodeProgram) As RPGCODE_CLASS

    On Error Resume Next

    Dim strClass As String  ' The class' name
    Dim idx As Long         ' Loop var

    ' Get the class' name
    strClass = classes(hClass).strInstancedFrom

    ' Loop over every class it could be
    For idx = 0 To UBound(prg.classes.classes)
        If (prg.classes.classes(idx).strName = strClass) Then
            ' Found it!
            getClass = prg.classes.classes(idx)
            Exit Function
        End If
    Next idx

    ' If we get here, it wasn't a valid class
    getClass.strName = "INVALID"

End Function

'=========================================================================
' Clear an object
'=========================================================================
Private Sub clearObject(ByRef object As RPGCODE_CLASS_INSTANCE, ByRef prg As RPGCodeProgram)

    On Error Resume Next

    Dim idx As Long, scopeIdx As Long           ' Loop var
    Dim theClass As RPGCODE_CLASS               ' The class
    Dim scope As RPGCODE_CLASS_SCOPE            ' A scope
    Dim oldDebug As Long, oldError As String    ' Old stuff

    ' Get the class
    theClass = getClass(object.hClass, prg)

    If (theClass.strName = "INVALID") Then
        ' Class doesn't exist!
        Exit Sub
    End If

    ' Get old values
    oldDebug = debugYN
    oldError = errorBranch

    ' Clear values
    debugYN = 0
    oldError = vbNullString

    ' For each scope
    For scopeIdx = 0 To 1
        ' Get the scope
        If (scopeIdx = 0) Then
            ' Private scope
            scope = theClass.scopePrivate
        Else
            ' Public scope
            scope = theClass.scopePublic
        End If
        ' For each var within that scope
        For idx = 0 To UBound(scope.strVars)
            ' Kill the variable
            Call KillRPG("Kill(" & getObjectVarName(scope.strVars(idx), object.hClass) & ")", prg)
        Next idx
    Next scopeIdx

    ' Restore values
    debugYN = oldDebug
    errorBranch = oldError

End Sub

'=========================================================================
' Create a new instance of a class
'=========================================================================
Public Function createRPGCodeObject(ByVal theClass As String, ByRef prg As RPGCodeProgram, ByRef constructParams() As String, ByVal noParams As Boolean) As Long

    On Error Resume Next

    Dim hClass As Long              ' Handle to use
    Dim retval As RPGCODE_RETURN    ' Return value
    Dim obj As Object               ' An object

    ' Check if we can instance this class
    If ((canInstanceClass(theClass, prg)) Or (isInternalClass(theClass, obj))) Then
        ' Create a new handle
        hClass = newHandle()
        ' Make sure we have enough room in the instances array
        If (UBound(classes) < hClass) Then
            ' Enlarge the array
            ReDim Preserve classes(hClass)
        End If
        ' Write in the data
        classes(hClass).strInstancedFrom = UCase$(theClass)
        classes(hClass).hClass = hClass
        ' If (obj Is Nothing) Then
            ' Clear the object
            Call clearObject(classes(hClass), prg)
            ' Call the constructor
            Call callObjectMethod(hClass, theClass & createParams(constructParams, noParams), prg, retval, theClass)
        ' Else
            ' Make a class structure for this class
            ' Dim theClass As RPGCODE_CLASS
            ' Call obj.CreateClassStruct(theClass, prg)
            ' Call addClassToProgram(theClass, prg)
            ' Save the object
            ' Set classes(hClass).objClass = obj
            ' Call the constructor
            ' Call obj.Construct(hClass, constructParams, prg)
        ' End If
    End If

    ' Return a handle to the class
    createRPGCodeObject = hClass

End Function

'=========================================================================
' Grab vars from an array
'=========================================================================
Private Sub getVarsFromArray(ByVal depth As Long, ByRef size() As Long, ByRef x() As Long, ByRef scope As RPGCODE_CLASS_SCOPE, ByVal prefix As String, ByVal postFix As String)

    On Error Resume Next

    Dim dimIdx As Long      ' Dimension index
    Dim theVar As String    ' The variable

    For x(depth) = 0 To size(depth)
        If (depth <= UBound(size)) Then
            Call getVarsFromArray(depth + 1, size(), x(), scope, prefix, postFix)
        Else
            theVar = vbNullString
            For dimIdx = 0 To UBound(size)
                theVar = theVar & "[" & CStr(x(dimIdx)) & "]"
            Next dimIdx
            Call addVarToScope(prefix & theVar & postFix, scope)
        End If
    Next x(depth)

End Sub

'=========================================================================
' Create a string for params from an array
'=========================================================================
Private Function createParams(ByRef params() As String, ByVal noParams As Boolean) As String

    On Error Resume Next

    Dim idx As Long     ' Loop var

    ' Begin the return string
    createParams = "("

    If (Not noParams) Then
        ' Loop over each param
        For idx = 0 To UBound(params)
            createParams = createParams & params(idx) & ","
        Next idx
        createParams = Left$(createParams, Len(createParams) - 1)
    End If

    ' Finish the return string
    createParams = createParams & ")"

End Function

'=========================================================================
' Splice up a line for object things
'=========================================================================
Public Function spliceForObjects(ByVal Text As String, ByRef prg As RPGCodeProgram) As String

    On Error Resume Next

    Dim arrayDepth As Long          ' Depth in arrays
    Dim value As String             ' Value of function
    Dim retval As RPGCODE_RETURN    ' Return value
    Dim begin As Long               ' Char to begin at
    Dim char As String              ' Character(s)
    Dim spacesOK As Boolean         ' Spaces are okay?
    Dim cLine As String             ' Command line
    Dim object As String            ' Object name
    Dim depth As Long               ' Depth
    Dim ignore As Boolean           ' In quotes?
    Dim lngEnd As Long              ' End of text
    Dim start As Long               ' Start of object manipulation
    Dim hClassDbl As Double         ' Handle to a class (double)
    Dim hClass As Long              ' Handle to a class
    Dim var As Boolean              ' Variable?
    Dim outside As Boolean          ' Calling from outside class?
    Dim cmdName As String           ' Command's name
    Dim Length As Long              ' Length of the text
    Dim a As Long                   ' Loop var

    ' Get location of first ->
    begin = inStrOutsideQuotes(1, Text, "->")

    If (begin = 0) Then
        ' Contains no object manipulation
        spliceForObjects = Text
        Exit Function
    End If

    ' Get the length of the text
    Length = Len(Text)

    ' Loop over each charater, forwards
    For a = (begin + 2) To Length
        ' Get a character
        char = Mid$(Text, a, 1)
        Select Case char

            Case "!", "$", "-"
                ' Could be a public var
                If (depth = 0 And (Not ignore) And (arrayDepth = 0)) Then
                    If (char <> "-") Then
                        lngEnd = a
                    Else
                        lngEnd = a - 1
                    End If
                    var = True
                    Exit For
                End If

            Case "("
                If (Not ignore) Then
                    ' Increase depth
                    depth = depth + 1
                End If

            Case ")"
                If (Not ignore) Then
                    ' Decrease depth
                    depth = depth - 1
                    If (depth = 0) Then
                        lngEnd = a
                        Exit For
                    End If
                End If

            Case "["
                ' Entering array
                arrayDepth = arrayDepth + 1

            Case "]"
                ' Leaving array
                arrayDepth = arrayDepth - 1

            Case """"
                ' Found a quote
                ignore = (Not ignore)

        End Select
    Next a

    ' Record the method's command line
    cLine = ParseRPGCodeCommand(Trim$(Mid$(Text, begin + 2, lngEnd - begin - 1)), prg)
    If (Not var) Then
        cmdName = UCase$(GetCommandName(cLine))
    Else
        ' Parse the var
        cLine = parseArray(cLine, prg)
        If (Right$(cLine, 1) <> "!" And Right$(cLine, 1) <> "$") Then
            ' Assume object
            cLine = cLine & "!"
        End If
    End If

    ' Flag we're not in quotes
    ignore = False

    ' Flag that spaces are okay
    spacesOK = True

    ' Make sure start has a value
    start = 1

    ' Set array depth at 0
    arrayDepth = 0

    ' Loop over each charater, backwards
    For a = (begin - 1) To 1 Step -1
        ' Get a character
        char = Mid$(Text, a, 1)
        If ((spacesOK) And (char = " ")) Then
            ' Alter char
            char = vbNullString
            ' Flag spaces are no longer okay
            spacesOK = False
        End If
        Select Case char

            Case " ", ",", "#", "=", "<", ">", "+", "-", ";", "*", "\", "/", "^", "(", ")", _
            "%", "`", "|", "&", "~"
                ' It's a divider
                If ((Not ignore) And (arrayDepth = 0)) Then
                    start = a + 1
                    Exit For
                End If

            Case """"
                ' Found a quote
                ignore = (Not ignore)
                spacesOK = False

            Case "["
                ' Entering array
                arrayDepth = arrayDepth + 1

            Case "]"
                ' Leaving array
                arrayDepth = arrayDepth - 1

            Case Else
                ' Not a space, so they aren't okay anymore
                spacesOK = False

        End Select
    Next a

    ' Record the object
    object = parseArray(UCase$(Trim$(Mid$(Text, start, begin - start))), prg)
    If (LenB(object) = 0) Then object = GetWithPrefix()

    ' Get its handle
    If ((Right$(object, 1) <> "!") And (Right$(object, 1) <> "$")) Then
        ' Append an "!"
        Call getValue(object & "!", object, hClassDbl, prg)
    Else
        ' "!" already found
        Call getValue(object, object, hClassDbl, prg)
    End If

    ' Convert the handle to long
    hClass = CLng(hClassDbl)

    ' Check if we're calling from outside
    outside = (topNestle(prg) <> hClass)

    If (Not var) Then

        ' Check if we're to release
        If (cmdName = "RELEASE") Then

            If (classes(hClass).objClass Is Nothing) Then
                Call callObjectMethod(hClass, "~" & classes(hClass).strInstancedFrom, prg, retval, "~" & classes(hClass).strInstancedFrom)
                Call clearObject(classes(hClass), prg)
            Else
                Call classes(hClass).objClass.Deconstruct
            End If

            Call killHandle(hClass)
            classes(hClass).hClass = 0
            classes(hClass).strInstancedFrom = vbNullString
            Set classes(hClass).objClass = Nothing

        ElseIf (cmdName = "GETTYPE") Then

            ' Return type of object
            value = classes(hClass).strInstancedFrom

        Else

            If (isMethodMember(cmdName, hClass, prg, outside)) Then

                ' Execute the method
                Call callObjectMethod(hClass, cLine, prg, retval, cmdName)

                ' Replace text with value the method returned
                If (retval.dataType = DT_NUM) Then
                    value = " " & CStr(retval.num)
                ElseIf (retval.dataType = DT_LIT) Then
                    value = " """ & retval.lit & """"
                ElseIf (retval.dataType = DT_REFERENCE) Then
                    value = " " & retval.ref
                End If

            Else

                Call debugger("Error: Could not call method-- " & cLine)

            End If

        End If

    Else
        ' It's a variable
        If (isVarMember(cLine, hClass, prg, outside)) Then
            ' It's a member
            value = getObjectVarName(cLine, hClass)
        Else
            Call debugger("Error: Could not get/set " & cLine & " -- " & Text)
        End If
    End If

    If ((lngEnd = Length) And (start = 1)) Then
        ' Return NULL
        spliceForObjects = vbNullString
    Else
        ' Complete the return string
        spliceForObjects = Mid$(Text, 1, start - 1) & value & Mid$(Text, lngEnd + 1)
        ' Recurse, passing in the running text
        spliceForObjects = spliceForObjects(spliceForObjects, prg)
    End If

End Function
