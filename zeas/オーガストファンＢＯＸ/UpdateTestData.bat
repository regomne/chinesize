@echo off
rem ↓请手动设置游戏目录
set "GAMEPATH=D:\Program Files\オーガスト\オーガストファンＢＯＸ"

set SELFPATH=%~dp0
set GAMEEXE=%GAMEPATH%\binary_chs.exe
set pypath="%~dp0python\python.exe"
if not exist %pypath% ( echo I can't find "%~dp0python\python.exe" !
pause
goto :EOF
)

if "%~1"=="" (
echo "Drag something to me ?"
pause
exit
)

%pypath% "%SELFPATH%\nstxt.py" "%SELFPATH%\nscript.txt.code" "%~1" "%GAMEPATH%\0.txt"
copy /y "%SELFPATH%\newfont\黑体.ttf" "%SELFPATH%\default.ttf"
pause