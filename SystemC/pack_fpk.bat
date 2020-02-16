@echo off

if not exist utils\patchxcn (
  echo no files to pack!
  goto _Exit
)

copy /y utils\CONTENT.DEF utils\patchxcn\CONTENT.DEF

set REPLACE_TXT=utils\patchxcn\replace.txt

echo !CHIP> %REPLACE_TXT%
for %%i in (utils\patchxcn\*.kg) do (
  echo %%~nxi>> %REPLACE_TXT%
)
for %%i in (utils\patchxcn\*.bmp) do (
  echo %%~nxi>> %REPLACE_TXT%
)
echo !DATA>> %REPLACE_TXT%
for %%i in (utils\patchxcn\ACT_*) do (
  echo %%~nxi>> %REPLACE_TXT%
)

utils\python\python.exe utils\packfpk.py utils\patchxcn patchxcn.fpk

echo.
echo all complete.

:_Exit
pause