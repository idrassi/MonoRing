/* Copyright (c) 2013-2022 Mahmoud Fayed <msfclipper@yahoo.com> */
#ifndef ringext_h
    #define ringext_h
    /* Constants */
    #define RING_VM_LISTFUNCS 1
    #define RING_VM_MATH 1
    #define RING_VM_FILE 1
    #define RING_VM_OS 1
    #define RING_VM_DLL 1
    #define RING_VM_REFMETA 1
	#ifndef RING_VM_NO_OPENSSL
	#define RING_VM_OPENSSL 1
	#endif
	#ifndef RING_VM_NO_LIBUI
	#define RING_VM_LIBUI 1
	#endif
	#ifndef RING_VM_NO_CC
	#define RING_VM_CC 1
	#endif
	#ifndef RING_VM_NO_LIBCURL
	#define RING_VM_LIBCURL 1
	#endif
	#ifndef RING_VM_NO_INTERNET
	#define RING_VM_INTERNET 1
	#endif
	#ifndef RING_VM_NO_CJSON
	#define RING_VM_CJSON 1
	#endif
	#ifndef RING_VM_NO_HTTPLIB
	#define RING_VM_HTTPLIB 1
	#endif
	#ifndef RING_VM_NO_SQLITE
	#define RING_VM_SQLITE 1
	#endif
	#ifndef RING_VM_NO_ODBC
	#define RING_VM_ODBC 1
	#endif
	#ifdef _WIN32
		#ifndef RING_VM_NO_WINAPI
		#define RING_VM_WINAPI 1
		#endif
		#ifndef RING_VM_NO_WINCREG
		#define RING_VM_WINCREG 1
		#endif
	#endif
	#ifndef RING_VM_NO_LIBZIP
	#define RING_VM_LIBZIP 1
	#endif
	#ifndef RING_VM_NO_SOCKETS
	#define RING_VM_SOCKETS 1
	#endif
	#ifndef RING_VM_NO_STBIMAGE
	#define RING_VM_STBIMAGE 1
	#endif
	#ifndef RING_VM_NO_MURMURHASH
	#define RING_VM_MURMURHASH 1
	#endif
	#ifndef RING_VM_NO_LIBUV
	#define RING_VM_LIBUV 1
	#endif
    /* Compiler - Scanner - Reading Files Functions */
    #define RING_FILE FILE *
    #define RING_OPENFILE(x,y) fopen(x,y)
    #define RING_CLOSEFILE(x) fclose(x)
    #define RING_READCHAR(x,y,z) y = getc(x)  /* z is unused here = number of characters - default = 1 */
#endif
