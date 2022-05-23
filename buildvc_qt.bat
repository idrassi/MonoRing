echo off
setlocal enableextensions enabledelayedexpansion
set path=%~dp0\bin;%path%
set rootdir=%~dp0

cd %rootdir%\extensions\ringqt
call gencodeqt515_nobluetooth.bat
call buildqt515_nobluetooth.bat

call gencodeqt515_light.bat
call buildqt515_light.bat

call gencodeqt515_core.bat
call buildqt515_core.bat

cd binupdate
call installqt515.bat

cd %rootdir%\extensions\ringqt\binupdate
ring removedebugdlls.ring

endlocal
