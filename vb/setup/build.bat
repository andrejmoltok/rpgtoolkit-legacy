@echo off
echo RPGToolkit, Version 3 :: Installation Builder
echo.
echo Please wait...
start /wait helper setup.exe, zip.zip, setup.exe
start /wait helper setup.exe, tkzip.dll, setup.exe
echo.
echo Done!
pause > nul
exit