extArc\extArc.exe "%1" "%~dpn1"
FOR %%i IN ("%~dpn1\*.pna") DO pnaUtil\pnaUtil.exe -e -folder "%%~dpni" -pna "%%i"