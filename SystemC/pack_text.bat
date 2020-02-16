@echo off

echo cleaning working directory...

rmdir /s /q utils\new_txt

if exist utils\patchxcn (
  del utils\patchxcn\*.PTR utils\patchxcn\*.TXD
) else (
  mkdir utils\patchxcn
)

mkdir utils\new_txt

echo preprocessing texts...

utils\python\python.exe utils\pack_txt.py utils\ori_txt txt utils\new_txt

for %%i in (utils\new_txt\*.txt) do (
  echo packing %%~ni.txt ...
  utils\python\python.exe utils\pack_ptr.py %%i utils\patchxcn\%%~ni.PTR utils\patchxcn\%%~ni.TXD
)

echo.
echo all complete.

pause