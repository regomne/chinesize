@echo off
: -------------------------------
: if resources exist, build them
: -------------------------------
if not exist rsrc.rc goto over1
\MASM32\BIN\Rc.exe /v rsrc.rc
\MASM32\BIN\Cvtres.exe /machine:ix86 rsrc.res
:over1

if exist %1.obj del d3d9.obj
if exist %1.dll del d3d9.dll

: -----------------------------------------
: assemble d3d9.asm into an OBJ file
: -----------------------------------------
\MASM32\BIN\Ml.exe /c /coff d3d9.asm
if errorlevel 1 goto errasm

if not exist rsrc.obj goto nores

: --------------------------------------------------
: link the main OBJ file with the resource OBJ file
: --------------------------------------------------
\MASM32\BIN\Link.exe /SUBSYSTEM:WINDOWS /Dll /Def:d3d9.def /section:.bss,S d3d9.obj rsrc.obj
if errorlevel 1 goto errlink
ren d3d9.dll sgsc.dll
dir d3d9.*
goto TheEnd

:nores
: -----------------------
: link the main OBJ file
: -----------------------
\MASM32\BIN\Link.exe /SUBSYSTEM:WINDOWS /Dll /Def:d3d9.def /section:.bss,S d3d9.obj
if errorlevel 1 goto errlink
ren d3d9.dll sgsc.dll
dir d3d9.*
goto TheEnd

:errlink
: ----------------------------------------------------
: display message if there is an error during linking
: ----------------------------------------------------
echo.
echo There has been an error while linking this d3d9.
echo.
goto TheEnd

:errasm
: -----------------------------------------------------
: display message if there is an error during assembly
: -----------------------------------------------------
echo.
echo There has been an error while assembling this d3d9.
echo.
goto TheEnd

:TheEnd

pause
