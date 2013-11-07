@echo off
: -------------------------------
: if resources exist, build them
: -------------------------------
if not exist rsrc.rc goto over1
\MASM32\BIN\Rc.exe /v rsrc.rc
\MASM32\BIN\Cvtres.exe /machine:ix86 rsrc.res
:over1

if exist %1.obj del lnedit.obj
if exist %1.exe del lnedit.exe
if exist lnedit.pdb del lnedit.pdb
if exist %1.ilk del lnedit.ilk

: -----------------------------------------
: assemble lnedit.asm into an OBJ file
: -----------------------------------------
\MASM32\BIN\Ml.exe /c /coff lnedit.asm
if errorlevel 1 goto errasm

if not exist rsrc.obj goto nores

: --------------------------------------------------
: link the main OBJ file with the resource OBJ file
: --------------------------------------------------
\MASM32\BIN\Link.exe /SUBSYSTEM:WINDOWS lnedit.obj rsrc.obj
if errorlevel 1 goto errlink
dir lnedit.*
goto TheEnd

:nores
: -----------------------
: link the main OBJ file
: -----------------------
\MASM32\BIN\Link.exe /SUBSYSTEM:WINDOWS lnedit.obj
if errorlevel 1 goto errlink
dir lnedit.*
goto TheEnd

:errlink
: ----------------------------------------------------
: display message if there is an error during linking
: ----------------------------------------------------
echo.
echo There has been an error while linking this lnedit.
echo.
goto TheEnd

:errasm
: -----------------------------------------------------
: display message if there is an error during assembly
: -----------------------------------------------------
echo.
echo There has been an error while assembling this lnedit.
echo.
goto TheEnd

:TheEnd

pause
