echo off
setlocal enableextensions enabledelayedexpansion
set path=%~dp0\bin;%path%
set rootdir=%~dp0
set RINGEXEPATH="%rootdir%\bin\ring.exe"
set RINGARCHPATH="%rootdir%\bin\buildarch.ring"

rem run buildarch.ring to get ring.exe architecture
rem we use trick documented at https://devblogs.microsoft.com/oldnewthing/20120731-00/?p=7003
for /f %%i in ('call %RINGEXEPATH% %RINGARCHPATH%') do set ringbuildtarget=%%i

cd %rootdir%\extensions\ringqt
call gencode_core.bat
call buildvc_core.bat
pause

call gencode_light.bat
call buildvc_light.bat
pause

call gencode_nobluetooth.bat
call buildvc_nobluetooth.bat
pause

cd binupdate
if /I ["%ringbuildtarget%"]==["x64"] (
    call installqt515_x64.bat
) else (
    call installqt515_x86.bat
)

cd %rootdir%\extensions\ringqt\binupdate
ring removedebugdlls.ring

endlocal
