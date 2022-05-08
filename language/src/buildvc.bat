echo off
setlocal enableextensions enabledelayedexpansion
call locatevc.bat %1 %2

cls 

rem build dll - export symbols
 
cl /c %ringcflags% /D "RING_USEDLL=1" /D "RING_BUILDLIB=1" /D "CJSON_HIDE_SYMBOLS" ^
ring_string.c ring_list.c ring_item.c ring_items.c ring_hashtable.c ring_general.c ring_state.c ring_scanner.c ring_parser.c ring_hashlib.c ring_vmgc.c ^
ring_stmt.c ring_expr.c ring_codegen.c ring_vm.c ring_vmexpr.c ring_vmvars.c ^
ring_vmlists.c ring_vmfuncs.c ring_api.c ring_vmoop.c ring_vmcui.c ^
ring_vmtrycatch.c ring_vmstrindex.c ring_vmjump.c ring_vmduprange.c ^
ring_vmperformance.c ring_vmexit.c ring_vmstackvars.c ring_vmstate.c ring_generallib_extension.c ring_math_extension.c ring_file_extension.c ring_os_extension.c ring_list_extension.c ring_refmeta_extension.c ^
ring_ext.c ring_dll_extension.c ring_objfile.c ring_openssl_extension.c ring_libui_extension.c ring_consolecolors_extension.c ring_libcurl_extension.c ring_internet_extension.c ring_cjson_extension.c ^
cjson_extension\cJSON.c cjson_extension\cJSON_Utils.c ring_sqlite_extension.c sqlite_extension\sqlite3.c ring_odbc_extension.c ring_winapi_extension.c ring_threads_extension.c ring_libzip_extension.c ^
ring_httplib_extension.cpp ring_wincreg_extension.cpp wincreg_extension\creg_registry.cpp ring_sockets_extension.c ring_stbimage_extension.c ring_libuv_extension.c ^
ring_murmurhash_extension.c murmurhash_extension\MurmurHash1.c murmurhash_extension\MurmurHash2.c murmurhash_extension\MurmurHash3.c ^
-I"..\include" -I"..\..\libdepwin\openssl\%ringbuildtarget%\include" -I"..\..\libdepwin\libui\%ringbuildtarget%" -I"..\..\libdepwin\libcurl\%ringbuildtarget%\include" ^
-I"..\..\libdepwin\libuv\%ringbuildtarget%\include"
  

link  %ringldflags% /OPT:REF /DLL /OUT:..\..\lib\ring.dll /SUBSYSTEM:CONSOLE,"%ringsubsystem%" ring_string.obj ^
ring_list.obj ring_item.obj ring_items.obj ring_hashtable.obj ring_general.obj ring_state.obj ring_scanner.obj ring_parser.obj ring_hashlib.obj ring_vmgc.obj ^
ring_stmt.obj ring_expr.obj ring_codegen.obj ring_vm.obj ring_vmexpr.obj ring_vmvars.obj ^
ring_vmlists.obj ring_vmfuncs.obj ring_api.obj ring_vmoop.obj ring_vmcui.obj ^
ring_vmtrycatch.obj ring_vmstrindex.obj ring_vmjump.obj ring_vmduprange.obj ^
ring_vmperformance.obj ring_vmexit.obj ring_vmstackvars.obj ring_vmstate.obj ring_generallib_extension.obj ring_math_extension.obj ring_file_extension.obj ring_os_extension.obj ring_list_extension.obj ring_refmeta_extension.obj ^
ring_ext.obj ring_dll_extension.obj ring_objfile.obj ring_openssl_extension.obj ring_libui_extension.obj ring_consolecolors_extension.obj ring_libcurl_extension.obj ring_internet_extension.obj ring_cjson_extension.obj ring_httplib_extension.obj ^
cJSON.obj cJSON_Utils.obj ring_sqlite_extension.obj sqlite3.obj ring_odbc_extension.obj ring_winapi_extension.obj ring_wincreg_extension.obj creg_registry.obj ring_threads_extension.obj ring_libzip_extension.obj ^
ring_sockets_extension.obj ring_stbimage_extension.obj ring_libuv_extension.obj ^
ring_murmurhash_extension.obj MurmurHash1.obj MurmurHash2.obj MurmurHash3.obj ^
/LIBPATH:..\..\libdepwin\openssl\%ringbuildtarget%\lib /LIBPATH:..\..\libdepwin\libui\%ringbuildtarget% /LIBPATH:..\..\libdepwin\libcurl\%ringbuildtarget%\lib /LIBPATH:..\..\libdepwin\libuv\%ringbuildtarget%\lib ^
libcrypto.lib libui.lib libcurl_a.lib uv_a.lib Advapi32.lib User32.lib Crypt32.lib Ws2_32.lib shlwapi.lib shell32.lib odbc32.lib kernel32.lib gdi32.lib comctl32.lib uxtheme.lib msimg32.lib comdlg32.lib d2d1.lib dwrite.lib ole32.lib oleaut32.lib oleacc.lib uuid.lib windowscodecs.lib Wldap32.lib Normaliz.lib Iphlpapi.lib Userenv.lib

copy ..\..\lib\ring.dll ..\..\bin\

rem build static lib - we don't export symbols

cl /c %ringcflags% /D "RING_USEDLL=0" /D "RING_BUILDLIB=0" /D "CJSON_HIDE_SYMBOLS" ^
ring_string.c ring_list.c ring_item.c ring_items.c ring_hashtable.c ring_general.c ring_state.c ring_scanner.c ring_parser.c ring_hashlib.c ring_vmgc.c ^
ring_stmt.c ring_expr.c ring_codegen.c ring_vm.c ring_vmexpr.c ring_vmvars.c ^
ring_vmlists.c ring_vmfuncs.c ring_api.c ring_vmoop.c ring_vmcui.c ^
ring_vmtrycatch.c ring_vmstrindex.c ring_vmjump.c ring_vmduprange.c ^
ring_vmperformance.c ring_vmexit.c ring_vmstackvars.c ring_vmstate.c ring_generallib_extension.c ring_math_extension.c ring_file_extension.c ring_os_extension.c ring_list_extension.c ring_refmeta_extension.c ^
ring_ext.c ring_dll_extension.c ring_objfile.c ring_openssl_extension.c ring_libui_extension.c ring_consolecolors_extension.c ring_libcurl_extension.c ring_internet_extension.c ring_cjson_extension.c ^
cjson_extension\cJSON.c cjson_extension\cJSON_Utils.c ring_sqlite_extension.c sqlite_extension\sqlite3.c ring_odbc_extension.c ring_winapi_extension.c ring_threads_extension.c ring_libzip_extension.c ^
ring_httplib_extension.cpp ring_wincreg_extension.cpp wincreg_extension\creg_registry.cpp ring_sockets_extension.c ring_stbimage_extension.c ring_libuv_extension.c ^
ring_murmurhash_extension.c murmurhash_extension\MurmurHash1.c murmurhash_extension\MurmurHash2.c murmurhash_extension\MurmurHash3.c ^
-I"..\include" -I"..\..\libdepwin\openssl\%ringbuildtarget%\include" -I"..\..\libdepwin\libui\%ringbuildtarget%" -I"..\..\libdepwin\libcurl\%ringbuildtarget%\include" ^
-I"..\..\libdepwin\libuv\%ringbuildtarget%\include"

lib  /OUT:..\..\lib\ringstatic.lib ring_string.obj ^
ring_list.obj ring_item.obj ring_items.obj ring_hashtable.obj ring_general.obj ring_state.obj ring_scanner.obj ring_parser.obj ring_hashlib.obj ring_vmgc.obj ^
ring_stmt.obj ring_expr.obj ring_codegen.obj ring_vm.obj ring_vmexpr.obj ring_vmvars.obj ^
ring_vmlists.obj ring_vmfuncs.obj ring_api.obj ring_vmoop.obj ring_vmcui.obj ^
ring_vmtrycatch.obj ring_vmstrindex.obj ring_vmjump.obj ring_vmduprange.obj ^
ring_vmperformance.obj ring_vmexit.obj ring_vmstackvars.obj ring_vmstate.obj ring_generallib_extension.obj ring_math_extension.obj ring_file_extension.obj ring_os_extension.obj ring_list_extension.obj ring_refmeta_extension.obj ^
ring_ext.obj ring_dll_extension.obj ring_objfile.obj ring_openssl_extension.obj ring_libui_extension.obj ring_consolecolors_extension.obj ring_libcurl_extension.obj ring_internet_extension.obj ring_cjson_extension.obj ring_httplib_extension.obj ^
cJSON.obj cJSON_Utils.obj ring_sqlite_extension.obj sqlite3.obj ring_odbc_extension.obj ring_winapi_extension.obj ring_wincreg_extension.obj creg_registry.obj ring_threads_extension.obj ring_libzip_extension.obj ^
ring_sockets_extension.obj ring_stbimage_extension.obj ring_libuv_extension.obj ^
ring_murmurhash_extension.obj MurmurHash1.obj MurmurHash2.obj MurmurHash3.obj ^
..\..\libdepwin\openssl\%ringbuildtarget%\lib\libcrypto.lib ..\..\libdepwin\libui\%ringbuildtarget%\libui.lib ..\..\libdepwin\libcurl\%ringbuildtarget%\lib\libcurl_a.lib ^
..\..\libdepwin\libuv\%ringbuildtarget%\lib\uv_a.lib

cl %ringcflags% ring.c ..\..\lib\ringstatic.lib -I"..\include" /link %ringldflags% /SUBSYSTEM:CONSOLE,"%ringsubsystem%" /OPT:REF /OUT:..\..\bin\ring.exe ^
Advapi32.lib User32.lib Crypt32.lib Ws2_32.lib shlwapi.lib shell32.lib odbc32.lib kernel32.lib gdi32.lib comctl32.lib uxtheme.lib msimg32.lib comdlg32.lib d2d1.lib dwrite.lib ole32.lib oleaut32.lib oleacc.lib uuid.lib windowscodecs.lib Wldap32.lib Normaliz.lib Iphlpapi.lib Userenv.lib

cl %ringcflags% ringw.c ..\..\lib\ringstatic.lib -I"..\include" /link %ringldflags% /SUBSYSTEM:WINDOWS,"%ringsubsystem%" /OPT:REF /OUT:..\..\bin\ringw.exe ^
Advapi32.lib User32.lib Crypt32.lib Ws2_32.lib shlwapi.lib shell32.lib odbc32.lib kernel32.lib gdi32.lib comctl32.lib uxtheme.lib msimg32.lib comdlg32.lib d2d1.lib dwrite.lib ole32.lib oleaut32.lib oleacc.lib uuid.lib windowscodecs.lib Wldap32.lib Normaliz.lib Iphlpapi.lib Userenv.lib

rem wait 2 second to avoid issue with antivirus locking the created exe
timeout 2 /nobreak
rem embed the manifest for ui controls
mt.exe -manifest ..\..\bin\ring.exe.manifest -outputresource:..\..\bin\ring.exe;1
mt.exe -manifest ..\..\bin\ringw.exe.manifest -outputresource:..\..\bin\ringw.exe;1

del *.obj

del ..\..\bin\ring.exe.manifest
del ..\..\bin\ringw.exe.manifest

endlocal
