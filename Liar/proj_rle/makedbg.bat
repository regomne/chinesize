@echo off
: -------------------------------
: if resources exist, build them
: -------------------------------
if not exist rsrc.rc goto over1
\MASM32\BIN\Rc.exe /v rsrc.rc
\MASM32\BIN\Cvtres.exe /machine:ix86 rsrc.res
:over1

if exist %1.obj del bmps2lwg.obj
if exist %1.exe del bmps2lwg.exe
if exist bmps2lwg.pdb del bmps2lwg.pdb
if exist %1.ilk del bmps2lwg.ilk

: -----------------------------------------
: assemble bmps2lwg.asm into an OBJ file
: -----------------------------------------
\MASM32\BIN\Ml.exe /c /coff /Cp /Zi bmps2lwg.asm
if errorlevel 1 goto errasm

if not exist rsrc.obj goto nores

: --------------------------------------------------
: link the main OBJ file with the resource OBJ file
: --------------------------------------------------
\MASM32\BIN\Link.exe /SUBSYSTEM:WINDOWS /DEBUG /DEBUGTYPE:CV bmps2lwg.obj rsrc.obj
if errorlevel 1 goto errlink
dir bmps2lwg.*
goto TheEnd

:nores
: -----------------------
: link the main OBJ file
: -----------------------
\MASM32\BIN\Link.exe /SUBSYSTEM:WINDOWS /DEBUG /DEBUGTYPE:CV bmps2lwg.obj
if errorlevel 1 goto errlink
dir bmps2lwg.*
goto TheEnd

:errlink
: ----------------------------------------------------
: display message if there is an error during linking
: ----------------------------------------------------
echo.
echo There has been an error while linking this bmps2lwg.
echo.
goto TheEnd

:errasm
: -----------------------------------------------------
: display message if there is an error during assembly
: -----------------------------------------------------
echo.
echo There has been an error while assembling this bmps2lwg.
echo.
goto TheEnd

:TheEnd

pause
