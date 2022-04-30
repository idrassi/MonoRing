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
	#define RING_VM_OPENSSL 1
	#define RING_VM_LIBUI 1
	#define RING_VM_CC 1
	#define RING_VM_LIBCURL 1
	#define RING_VM_INTERNET 1
	#define RING_VM_CJSON 1
	#define RING_VM_HTTPLIB 1
	#define RING_VM_SQLITE 1
	#define RING_VM_ODBC 1
	#define RING_VM_WINAPI 1
	#define RING_VM_WINCREG 1
	#define RING_VM_THREADS 1
	#define RING_VM_LIBZIP 1
	#define RING_VM_SOCKETS 1
	#define RING_VM_STBIMAGE 1
    /* Compiler - Scanner - Reading Files Functions */
    #define RING_FILE FILE *
    #define RING_OPENFILE(x,y) fopen(x,y)
    #define RING_CLOSEFILE(x) fclose(x)
    #define RING_READCHAR(x,y,z) y = getc(x)  /* z is unused here = number of characters - default = 1 */
#endif
