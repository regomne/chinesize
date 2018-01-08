@echo off
for %%i in (..\scene\*.cst) do (
    echo %%i
    cstTextProc -e -cst %%i -o ..\scene_txt\%%~ni.txt
)
pause