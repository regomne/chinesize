@echo off

if not exist utils\patchxcn mkdir utils\patchxcn

for %%i in (pic\*.png) do (
  echo converting %%~ni.png ...
  utils\packkg.exe %%i utils\patchxcn\%%~ni.kg
)

copy /y pic\*.bmp utils\patchxcn\

echo.
echo all complete.

pause