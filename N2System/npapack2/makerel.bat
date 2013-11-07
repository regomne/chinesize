@echo off
: -------------------------------
: if resources exist, build them
: -------------------------------
if not exist rsrc.rc goto over1
\MASM32\BIN\Rc.exe /v rsrc.rc
\MASM32\BIN\Cvtres.exe /machine:ix86 rsrc.res
:over1

if exist %1.obj del npapack.obj
if exist %1.exe del npapack.exe
if exist npapack.pdb del npapack.pdb
if exist %1.ilk del npapack.ilk

: -----------------------------------------
: assemble npapack.asm into an OBJ file
: -----------------------------------------
"C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\bin\Ml.exe" /c /coff npapack.asm
:\MASM32\BIN\ml.exe /c /coff npapack.asm
if errorlevel 1 goto errasm

if not exist rsrc.obj goto nores

: --------------------------------------------------
: link the main OBJ file with the resource OBJ file
: --------------------------------------------------
set path=C:\Program Files (x86)\Microsoft Visual Studio 11.0\Common7\IDE
"C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\bin\Link.exe" /ltcg /SUBSYSTEM:WINDOWS npapack.obj rsrc.obj inffas32.obj match686.obj
:\MASM32\BIN\link.exe /SUBSYSTEM:WINDOWS npapack.obj rsrc.obj
if errorlevel 1 goto errlink
dir npapack.*
goto TheEnd

:nores
: -----------------------
: link the main OBJ file
: -----------------------
\MASM32\BIN\Link.exe /SUBSYSTEM:WINDOWS npapack.obj
if errorlevel 1 goto errlink
dir npapack.*
goto TheEnd

:errlink
: ----------------------------------------------------
: display message if there is an error during linking
: ----------------------------------------------------
echo.
echo There has been an error while linking this npapack.
echo.
goto TheEnd

:errasm
: -----------------------------------------------------
: display message if there is an error during assembly
: -----------------------------------------------------
echo.
echo There has been an error while assembling this npapack.
echo.
goto TheEnd

:TheEnd

pause
