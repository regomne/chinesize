@echo off
echo Packing pna...
mkdir ..\Ch
del /q ..\Ch\*.*

for /d %%i in (*) do (
echo Packing %%i
pnaUtil -p -folder %%i -pna ..\Ch\%%i.pna
)

echo Copying pna...
copy /y *.png ..\Ch\
copy /y *.pna ..\Ch\

echo Packing arc...
ahdprc arc -r ..\Ch -c

echo Complete!

pause