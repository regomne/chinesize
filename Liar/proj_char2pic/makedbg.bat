@echo off
: -------------------------------
: if resources exist, build them
: -------------------------------
if not exist rsrc.rc goto over1
\MASM32\BIN\Rc.exe /v rsrc.rc
\MASM32\BIN\Cvtres.exe /machine:ix86 rsrc.res
:over1

if exist %1.obj del char2pic.obj
if exist %1.exe del char2pic.exe
if exist char2pic.pdb del char2pic.pdb
if exist %1.ilk del char2pic.ilk

: -----------------------------------------
: assemble char2pic.asm into an OBJ file
: -----------------------------------------
\MASM32\BIN\Ml.exe /c /coff /Cp /Zi char2pic.asm
if errorlevel 1 goto errasm

if not exist rsrc.obj goto nores

: --------------------------------------------------
: link the main OBJ file with the resource OBJ file
: --------------------------------------------------
\MASM32\BIN\Link.exe /SUBSYSTEM:WINDOWS /DEBUG /DEBUGTYPE:CV char2pic.obj rsrc.obj
if errorlevel 1 goto errlink
dir char2pic.*
goto TheEnd

:nores
: -----------------------
: link the main OBJ file
: -----------------------
\MASM32\BIN\Link.exe /SUBSYSTEM:WINDOWS /DEBUG /DEBUGTYPE:CV char2pic.obj
if errorlevel 1 goto errlink
dir char2pic.*
goto TheEnd

:errlink
: ----------------------------------------------------
: display message if there is an error during linking
: ----------------------------------------------------
echo.
echo There has been an error while linking this char2pic.
echo.
goto TheEnd

:errasm
: -----------------------------------------------------
: display message if there is an error during assembly
: -----------------------------------------------------
echo.
echo There has been an error while assembling this char2pic.
echo.
goto TheEnd

:TheEnd

pause
