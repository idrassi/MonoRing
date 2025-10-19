/* Copyright (c) 2013-2025 Mahmoud Fayed <msfclipper@yahoo.com> */

#ifndef ringext_h
#define ringext_h
#define RING_VM_LISTFUNCS 1
#define RING_VM_MATH 1
#define RING_VM_FILE 1
#define RING_VM_OS 1
#define RING_VM_REFMETA 1
#define RING_VM_INFO 1
#if RING_NODLL
	#define RING_VM_DLL 0
#else
	#define RING_VM_DLL 1
#endif
	#ifndef RING_VM_NO_OPENSSL
	#define RING_VM_OPENSSL 1
	#endif
	#ifndef RING_VM_NO_LIBUI
	#define RING_VM_LIBUI 1
	#endif
	#ifndef RING_VM_NO_CC
	#define RING_VM_CC 1
	#endif
	#ifndef RING_VM_NO_FASTPRO
	#define RING_VM_FASTPRO 1
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
		#ifndef RING_VM_NO_WINLIB
		#define RING_VM_WINLIB 1
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

#define RING_FILE FILE *
#define RING_OPENFILE(cFile, cMode) fopen(cFile, cMode)
#define RING_CLOSEFILE(pFile) fclose(pFile)
#define RING_READCHAR(pFile, cOut, nCount)                                                                             \
	cOut = getc(pFile) /* nCount is unused here = number of characters - default = 1 */
#endif
