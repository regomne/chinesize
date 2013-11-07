@echo off
rem ↓请手动设置游戏目录
set GAMEPATH="D:\Program Files\美少女万華鏡"

set SELFPATH=%~dp0
set GAMEEXE=%GAMEPATH%\game.exe
set pypath="%~dp0python\python.exe"
if not exist %pypath% ( echo I can't find "%~dp0python\python.exe" !
pause
goto :EOF
)

if "%1"=="" goto :ASM_TEXT_DIR

:LOOP
%pypath% "%SELFPATH%join.py" "%SELFPATH%SplitData\%~1.splt" "%1" "%SELFPATH%Output\scenario\ks_01\%~n1.s"
shift
if %1=="" goto :PACK
goto :LOOP

:ASM_TEXT_DIR
for %%a in ("%SELFPATH%text\*.txt") do %pypath% "%SELFPATH%join.py" "%SELFPATH%SplitData\%%~na.splt" "%%a" "%SELFPATH%Output\scenario\ks_01\%%~na.s"

:PACK
call "%SELFPATH%packqliefp1.bat" "%SELFPATH%output" "%GAMEPATH%\GameData\data8.pack"

echo 操作完成
pause