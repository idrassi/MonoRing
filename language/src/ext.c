/* Copyright (c) 2013-2025 Mahmoud Fayed <msfclipper@yahoo.com> */

#include "ring.h"

void ring_vm_extension(RingState *pRingState) {
/* List */
#if RING_VM_LISTFUNCS
	ring_vm_list_loadfunctions(pRingState);
#endif
/* Math */
#if RING_VM_MATH
	ring_vm_math_loadfunctions(pRingState);
#endif
/* File */
#if RING_VM_FILE
	ring_vm_file_loadfunctions(pRingState);
#endif
/* OS */
#if RING_VM_OS
	ring_vm_os_loadfunctions(pRingState);
#endif
/* DLL */
#if RING_VM_DLL
	ring_vm_dll_loadfunctions(pRingState);
#endif
/* Reflection and Meta-programming */
#if RING_VM_REFMETA
	ring_vm_refmeta_loadfunctions(pRingState);
#endif
/* VM Information */
#if RING_VM_INFO
	ring_vm_info_loadfunctions(pRingState);
#endif
	#if RING_VM_OPENSSL
		ring_vm_openssl_loadfunctions(pRingState);
	#endif
	#if RING_VM_LIBUI
		ring_vm_libui_loadfunctions(pRingState);
	#endif
	#if RING_VM_CC
		ring_vm_cc_loadfunctions(pRingState);
	#endif
	#if RING_VM_FASTPRO
		ring_vm_fastpro_loadfunctions(pRingState);
	#endif
	#if RING_VM_LIBCURL
		ring_vm_libcurl_loadfunctions(pRingState);
	#endif
	#if RING_VM_INTERNET
		ring_vm_internet_loadfunctions(pRingState);
	#endif
	#if RING_VM_CJSON
		ring_vm_cjson_loadfunctions(pRingState);
	#endif
	#if RING_VM_HTTPLIB
		ring_vm_httplib_loadfunctions(pRingState);
	#endif
	#if RING_VM_SQLITE
		ring_vm_sqlite_loadfunctions(pRingState);
	#endif
	#if RING_VM_ODBC
		ring_vm_odbc_loadfunctions(pRingState);
	#endif
	#if RING_VM_WINAPI
		ring_vm_winapi_loadfunctions(pRingState);
	#endif
	#if RING_VM_WINCREG
		ring_vm_wincreg_loadfunctions(pRingState);
	#endif
	#if RING_VM_WINLIB
		ring_vm_winlib_loadfunctions(pRingState);
	#endif
	#if RING_VM_THREADS
		ring_vm_threads_loadfunctions(pRingState);
	#endif
	#if RING_VM_LIBZIP
		ring_vm_libzip_loadfunctions(pRingState);
	#endif
	#if RING_VM_SOCKETS
		ring_vm_sockets_loadfunctions(pRingState);
	#endif
	#if RING_VM_STBIMAGE
		ring_vm_stbimage_loadfunctions(pRingState);
	#endif
	#if RING_VM_MURMURHASH
		ring_vm_murmurhash_loadfunctions(pRingState);
	#endif
	#if RING_VM_LIBUV
		ring_vm_libuv_loadfunctions(pRingState);
	#endif
}
