@echo off
: -------------------------------
: if resources exist, build them
: -------------------------------
if not exist rsrc.rc goto over1
\MASM32\BIN\Rc.exe /v rsrc.rc
\MASM32\BIN\Cvtres.exe /machine:ix86 rsrc.res
:over1

if exist %1.obj del yukapack.obj
if exist %1.exe del yukapack.exe
if exist yukapack.pdb del yukapack.pdb
if exist %1.ilk del yukapack.ilk

: -----------------------------------------
: assemble yukapack.asm into an OBJ file
: -----------------------------------------
\MASM32\BIN\Ml.exe /c /coff /Cp /Zi yukapack.asm
if errorlevel 1 goto errasm

if not exist rsrc.obj goto nores

: --------------------------------------------------
: link the main OBJ file with the resource OBJ file
: --------------------------------------------------
\MASM32\BIN\Link.exe /SUBSYSTEM:WINDOWS /DEBUG /DEBUGTYPE:CV yukapack.obj rsrc.obj
if errorlevel 1 goto errlink
dir yukapack.*
goto TheEnd

:nores
: -----------------------
: link the main OBJ file
: -----------------------
\MASM32\BIN\Link.exe /SUBSYSTEM:WINDOWS /DEBUG /DEBUGTYPE:CV yukapack.obj
if errorlevel 1 goto errlink
dir yukapack.*
goto TheEnd

:errlink
: ----------------------------------------------------
: display message if there is an error during linking
: ----------------------------------------------------
echo.
echo There has been an error while linking this yukapack.
echo.
goto TheEnd

:errasm
: -----------------------------------------------------
: display message if there is an error during assembly
: -----------------------------------------------------
echo.
echo There has been an error while assembling this yukapack.
echo.
goto TheEnd

:TheEnd

pause
