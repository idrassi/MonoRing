echo off
setlocal enableextensions enabledelayedexpansion
set path=%~dp0\bin;%path%
set rootdir=%~dp0

cd %rootdir%\language\src
call buildvc.bat %1 %2

cd  %rootdir%\tools\ring2exe
call buildring2exe.bat

cd %rootdir%\tools\ringrepl

call build.bat

del %rootdir%\lib\ring.exp

cd %rootdir%\tools\folder2qrc
call build.bat
del *.exe

cd %rootdir%\tools\ringnotepad\rnoteexe
call buildvc.bat
del *.exe
del *.res
del *.obj

endlocal
