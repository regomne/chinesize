@echo off
: -------------------------------
: if resources exist, build them
: -------------------------------
if not exist rsrc.rc goto over1
\MASM32\BIN\Rc.exe /v rsrc.rc
\MASM32\BIN\Cvtres.exe /machine:ix86 rsrc.res
:over1

if exist %1.obj del comp.obj
if exist %1.exe del comp.exe
if exist comp.pdb del comp.pdb
if exist %1.ilk del comp.ilk

: -----------------------------------------
: assemble comp.asm into an OBJ file
: -----------------------------------------
\MASM32\BIN\Ml.exe /c /coff comp.asm
if errorlevel 1 goto errasm

if not exist rsrc.obj goto nores

: --------------------------------------------------
: link the main OBJ file with the resource OBJ file
: --------------------------------------------------
\MASM32\BIN\Link.exe /SUBSYSTEM:WINDOWS comp.obj rsrc.obj
if errorlevel 1 goto errlink
dir comp.*
goto TheEnd

:nores
: -----------------------
: link the main OBJ file
: -----------------------
\MASM32\BIN\Link.exe /SUBSYSTEM:WINDOWS comp.obj
if errorlevel 1 goto errlink
dir comp.*
goto TheEnd

:errlink
: ----------------------------------------------------
: display message if there is an error during linking
: ----------------------------------------------------
echo.
echo There has been an error while linking this comp.
echo.
goto TheEnd

:errasm
: -----------------------------------------------------
: display message if there is an error during assembly
: -----------------------------------------------------
echo.
echo There has been an error while assembling this comp.
echo.
goto TheEnd

:TheEnd

pause
