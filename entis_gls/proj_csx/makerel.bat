@echo off
: -------------------------------
: if resources exist, build them
: -------------------------------
if not exist rsrc.rc goto over1
\MASM32\BIN\Rc.exe /v rsrc.rc
\MASM32\BIN\Cvtres.exe /machine:ix86 rsrc.res
:over1

if exist %1.obj del compcsx.obj
if exist %1.exe del compcsx.exe
if exist compcsx.pdb del compcsx.pdb
if exist %1.ilk del compcsx.ilk

: -----------------------------------------
: assemble compcsx.asm into an OBJ file
: -----------------------------------------
\MASM32\BIN\Ml.exe /c /coff compcsx.asm
if errorlevel 1 goto errasm

if not exist rsrc.obj goto nores

: --------------------------------------------------
: link the main OBJ file with the resource OBJ file
: --------------------------------------------------
\MASM32\BIN\Link.exe /SUBSYSTEM:WINDOWS compcsx.obj rsrc.obj
if errorlevel 1 goto errlink
dir compcsx.*
goto TheEnd

:nores
: -----------------------
: link the main OBJ file
: -----------------------
\MASM32\BIN\Link.exe /SUBSYSTEM:WINDOWS compcsx.obj
if errorlevel 1 goto errlink
dir compcsx.*
goto TheEnd

:errlink
: ----------------------------------------------------
: display message if there is an error during linking
: ----------------------------------------------------
echo.
echo There has been an error while linking this compcsx.
echo.
goto TheEnd

:errasm
: -----------------------------------------------------
: display message if there is an error during assembly
: -----------------------------------------------------
echo.
echo There has been an error while assembling this compcsx.
echo.
goto TheEnd

:TheEnd

pause
