/* Copyright (c) 2013-2019 Mahmoud Fayed <msfclipper@yahoo.com> */
#ifndef ring_vminternet_h
#define ring_vminternet_h
/* Functions */

void ring_vm_internet_loadfunctions ( RingState *pRingState ) ;

size_t ring_getcurldata ( void *buffer, size_t size, size_t nmemb, void *pString ) ;

void ring_vm_curl_download ( void *pPointer ) ;

size_t ring_payload_source ( void *ptr, size_t size, size_t nmemb, void *userp ) ;

void ring_vm_curl_sendemail ( void *pPointer ) ;
/* Constants */
#define RING_VM_POINTER_CURL "curl"
#endif
