echo off
setlocal enableextensions enabledelayedexpansion
set batchdir=%~dp0
call locatevc.bat %1 %2

cd %batchdir%\..\src

cls 

rem build %ringhttplib% for 64-bit and ring_httplib32_extension.cpp for 32-bit
set ringhttplib=ring_httplib_extension.cpp
set ringhttplibobj=ring_httplib_extension.obj
set ringhttpfilesfilter=httplib_extension\*.ring
if /I ["%ringbuildtarget%"]==["x86"] (
	set ringhttplib=ring_httplib32_extension.cpp
	set ringhttplibobj=ring_httplib32_extension.obj
	set ringhttpfilesfilter=httplib32_extension\*.ring
)

rem build dll - export symbols
 
cl /c %ringcflags% /D "RING_USEDLL=1" /D "RING_BUILDLIB=1" /D "CJSON_HIDE_SYMBOLS" ^
rstring.c rlist.c ritem.c ritems.c rhtable.c general.c state.c scanner.c parser.c hashlib.c vmgc.c ^
stmt.c expr.c codegen.c vm.c vmexpr.c vmvars.c ^
vmlists.c vmfuncs.c ringapi.c vmoop.c vmerror.c ^
vmtry.c vmstr.c vmjump.c vmrange.c vmeval.c vminfo_e.c  vmstack.c vmthread.c ^
vmperf.c vmexit.c vmstate.c genlib_e.c math_e.c file_e.c os_e.c list_e.c meta_e.c ^
ext.c dll_e.c objfile.c ring_openssl_extension.c ring_libui_extension.c ring_consolecolors_extension.c ring_fastpro_extension.c ring_libcurl_extension.c ring_internet_extension.c ring_cjson_extension.c ^
cjson_extension\cJSON.c cjson_extension\cJSON_Utils.c ring_sqlite_extension.c sqlite_extension\sqlite3.c ring_odbc_extension.c ring_winapi_extension.c ring_winlib_extension.c ring_libzip_extension.c ^
%ringhttplib% ring_wincreg_extension.cpp wincreg_extension\creg_registry.cpp ring_sockets_extension.c ring_stbimage_extension.c ring_libuv_extension.c ^
ring_murmurhash_extension.c murmurhash_extension\MurmurHash1.c murmurhash_extension\MurmurHash2.c murmurhash_extension\MurmurHash3.c ^
-I"..\include" -I"..\..\libdepwin\openssl\%ringbuildtarget%\include" -I"..\..\libdepwin\libui\%ringbuildtarget%" -I"..\..\libdepwin\libcurl\%ringbuildtarget%\include" ^
-I"..\..\libdepwin\libuv\%ringbuildtarget%\include"
  

link  %ringldflags% /OPT:REF /DLL /OUT:..\..\lib\ring.dll /SUBSYSTEM:CONSOLE,"%ringsubsystem%" rstring.obj ^
rlist.obj ritem.obj ritems.obj rhtable.obj general.obj state.obj scanner.obj parser.obj hashlib.obj vmgc.obj ^
stmt.obj expr.obj codegen.obj vm.obj vmexpr.obj vmvars.obj ^
vmlists.obj vmfuncs.obj ringapi.obj vmoop.obj vmerror.obj ^
vmtry.obj vmstr.obj vmjump.obj vmrange.obj vmeval.obj vminfo_e.obj  vmstack.obj vmthread.obj ^
vmperf.obj vmexit.obj vmstate.obj genlib_e.obj math_e.obj file_e.obj os_e.obj list_e.obj meta_e.obj ^
ext.obj dll_e.obj objfile.obj ring_openssl_extension.obj ring_libui_extension.obj ring_consolecolors_extension.obj ring_fastpro_extension.obj ring_libcurl_extension.obj ring_internet_extension.obj ring_cjson_extension.obj %ringhttplibobj% ^
cJSON.obj cJSON_Utils.obj ring_sqlite_extension.obj sqlite3.obj ring_odbc_extension.obj ring_winapi_extension.obj ring_winlib_extension.obj ring_wincreg_extension.obj creg_registry.obj ring_libzip_extension.obj ^
ring_sockets_extension.obj ring_stbimage_extension.obj ring_libuv_extension.obj ^
ring_murmurhash_extension.obj MurmurHash1.obj MurmurHash2.obj MurmurHash3.obj ^
/LIBPATH:..\..\libdepwin\openssl\%ringbuildtarget%\lib /LIBPATH:..\..\libdepwin\libui\%ringbuildtarget% /LIBPATH:..\..\libdepwin\libcurl\%ringbuildtarget%\lib /LIBPATH:..\..\libdepwin\libuv\%ringbuildtarget%\lib ^
libcrypto.lib libui.lib libcurl_a.lib uv_a.lib Advapi32.lib User32.lib Crypt32.lib Secur32.lib Ws2_32.lib shlwapi.lib shell32.lib odbc32.lib kernel32.lib gdi32.lib comctl32.lib uxtheme.lib msimg32.lib comdlg32.lib d2d1.lib dwrite.lib ole32.lib oleaut32.lib oleacc.lib uuid.lib windowscodecs.lib Wldap32.lib Normaliz.lib Iphlpapi.lib Userenv.lib

copy ..\..\lib\ring.dll ..\..\bin\

copy %ringhttpfilesfilter% ..\..\bin\load\

rem build static lib - we don't export symbols

cl /c %ringcflags% /D "RING_USEDLL=0" /D "RING_BUILDLIB=0" /D "CJSON_HIDE_SYMBOLS" ^
rstring.c rlist.c ritem.c ritems.c rhtable.c general.c state.c scanner.c parser.c hashlib.c vmgc.c ^
stmt.c expr.c codegen.c vm.c vmexpr.c vmvars.c ^
vmlists.c vmfuncs.c ringapi.c vmoop.c vmerror.c ^
vmtry.c vmstr.c vmjump.c vmrange.c vmeval.c vminfo_e.c  vmstack.c vmthread.c ^
vmperf.c vmexit.c vmstate.c genlib_e.c math_e.c file_e.c os_e.c list_e.c meta_e.c ^
ext.c dll_e.c objfile.c ring_openssl_extension.c ring_libui_extension.c ring_consolecolors_extension.c ring_fastpro_extension.c ring_libcurl_extension.c ring_internet_extension.c ring_cjson_extension.c ^
cjson_extension\cJSON.c cjson_extension\cJSON_Utils.c ring_sqlite_extension.c sqlite_extension\sqlite3.c ring_odbc_extension.c ring_winapi_extension.c ring_winlib_extension.c ring_libzip_extension.c ^
%ringhttplib% ring_wincreg_extension.cpp wincreg_extension\creg_registry.cpp ring_sockets_extension.c ring_stbimage_extension.c ring_libuv_extension.c ^
ring_murmurhash_extension.c murmurhash_extension\MurmurHash1.c murmurhash_extension\MurmurHash2.c murmurhash_extension\MurmurHash3.c ^
-I"..\include" -I"..\..\libdepwin\openssl\%ringbuildtarget%\include" -I"..\..\libdepwin\libui\%ringbuildtarget%" -I"..\..\libdepwin\libcurl\%ringbuildtarget%\include" ^
-I"..\..\libdepwin\libuv\%ringbuildtarget%\include"

lib  /OUT:..\..\lib\ringstatic.lib rstring.obj ^
rlist.obj ritem.obj ritems.obj rhtable.obj general.obj state.obj scanner.obj parser.obj hashlib.obj vmgc.obj ^
stmt.obj expr.obj codegen.obj vm.obj vmexpr.obj vmvars.obj ^
vmlists.obj vmfuncs.obj ringapi.obj vmoop.obj vmerror.obj ^
vmtry.obj vmstr.obj vmjump.obj vmrange.obj vmeval.obj vminfo_e.obj  vmstack.obj vmthread.obj ^
vmperf.obj vmexit.obj vmstate.obj genlib_e.obj math_e.obj file_e.obj os_e.obj list_e.obj meta_e.obj ^
ext.obj dll_e.obj objfile.obj ring_openssl_extension.obj ring_libui_extension.obj ring_consolecolors_extension.obj ring_fastpro_extension.obj ring_libcurl_extension.obj ring_internet_extension.obj ring_cjson_extension.obj %ringhttplibobj% ^
cJSON.obj cJSON_Utils.obj ring_sqlite_extension.obj sqlite3.obj ring_odbc_extension.obj ring_winapi_extension.obj ring_winlib_extension.obj ring_wincreg_extension.obj creg_registry.obj ring_libzip_extension.obj ^
ring_sockets_extension.obj ring_stbimage_extension.obj ring_libuv_extension.obj ^
ring_murmurhash_extension.obj MurmurHash1.obj MurmurHash2.obj MurmurHash3.obj ^
..\..\libdepwin\openssl\%ringbuildtarget%\lib\libcrypto.lib ..\..\libdepwin\libui\%ringbuildtarget%\libui.lib ..\..\libdepwin\libcurl\%ringbuildtarget%\lib\libcurl_a.lib ^
..\..\libdepwin\libuv\%ringbuildtarget%\lib\uv_a.lib

cl %ringcflags% ring.c ..\..\lib\ringstatic.lib -I"..\include" /link %ringldflags% /SUBSYSTEM:CONSOLE,"%ringsubsystem%" /STACK:8388608 /OPT:REF /OUT:..\..\bin\ring.exe ^
Advapi32.lib User32.lib Crypt32.lib Secur32.lib Ws2_32.lib shlwapi.lib shell32.lib odbc32.lib kernel32.lib gdi32.lib comctl32.lib uxtheme.lib msimg32.lib comdlg32.lib d2d1.lib dwrite.lib ole32.lib oleaut32.lib oleacc.lib uuid.lib windowscodecs.lib Wldap32.lib Normaliz.lib Iphlpapi.lib Userenv.lib

cl %ringcflags% ringw.c ..\..\lib\ringstatic.lib -I"..\include" /link %ringldflags% /SUBSYSTEM:WINDOWS,"%ringsubsystem%" /STACK:8388608 /OPT:REF /OUT:..\..\bin\ringw.exe ^
Advapi32.lib User32.lib Crypt32.lib Secur32.lib Ws2_32.lib shlwapi.lib shell32.lib odbc32.lib kernel32.lib gdi32.lib comctl32.lib uxtheme.lib msimg32.lib comdlg32.lib d2d1.lib dwrite.lib ole32.lib oleaut32.lib oleacc.lib uuid.lib windowscodecs.lib Wldap32.lib Normaliz.lib Iphlpapi.lib Userenv.lib

rem macro for testing errorlevel
rem https://stackoverflow.com/questions/10935693/foolproof-way-to-check-for-nonzero-error-return-code-in-windows-batch-file/10936093#10936093
set "ifErr=set foundErr=1&(if errorlevel 0 if not errorlevel 1 set foundErr=)&if defined foundErr"

rem wait 2 second to avoid issue with antivirus locking the created exe
timeout 2 /nobreak
rem embed the manifest for ui controls
mt.exe -manifest ..\..\bin\ring.exe.manifest -outputresource:..\..\bin\ring.exe;1
%ifErr% (
	timeout 5 /nobreak
	mt.exe -manifest ..\..\bin\ring.exe.manifest -outputresource:..\..\bin\ring.exe;1
)

mt.exe -manifest ..\..\bin\ringw.exe.manifest -outputresource:..\..\bin\ringw.exe;1
%ifErr% (
	timeout 5 /nobreak
	mt.exe -manifest ..\..\bin\ringw.exe.manifest -outputresource:..\..\bin\ringw.exe;1
)

del *.obj

del ..\..\bin\ring.exe.manifest
del ..\..\bin\ringw.exe.manifest

endlocal
