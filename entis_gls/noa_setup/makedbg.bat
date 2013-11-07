@echo off
: -------------------------------
: if resources exist, build them
: -------------------------------
if not exist rsrc.rc goto over1
c:\MASM32\BIN\Rc.exe /v rsrc.rc
c:\MASM32\BIN\Cvtres.exe /machine:ix86 rsrc.res
:over1

if exist %1.obj del noasetup.obj
if exist %1.exe del noasetup.exe
if exist noasetup.pdb del noasetup.pdb
if exist %1.ilk del noasetup.ilk		

: -----------------------------------------
: assemble noasetup.asm into an OBJ file
: -----------------------------------------
c:\MASM32\BIN\Ml.exe /c /coff /Cp /Zi noasetup.asm
if errorlevel 1 goto errasm

if not exist rsrc.obj goto nores

: --------------------------------------------------
: link the main OBJ file with the resource OBJ file
: --------------------------------------------------
c:\MASM32\BIN\Link.exe /SUBSYSTEM:WINDOWS /DEBUG /DEBUGTYPE:CV noasetup.obj rsrc.obj
if errorlevel 1 goto errlink
dir noasetup.*
goto TheEnd

:nores
: -----------------------
: link the main OBJ file
: -----------------------
c:\MASM32\BIN\Link.exe /SUBSYSTEM:WINDOWS /DEBUG /DEBUGTYPE:CV noasetup.obj
if errorlevel 1 goto errlink
dir noasetup.*
goto TheEnd

:errlink
: ----------------------------------------------------
: display message if there is an error during linking
: ----------------------------------------------------
echo.
echo There has been an error while linking this noasetup.
echo.
goto TheEnd

:errasm
: -----------------------------------------------------
: display message if there is an error during assembly
: -----------------------------------------------------
echo.
echo There has been an error while assembling this noasetup.
echo.
goto TheEnd

:TheEnd

pause
