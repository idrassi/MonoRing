/* Version OpenSSL 0.9.8 and Later */

/* implementation of RSA functions
 * Author: Mounir IDRASSI (mounir@idrix.fr)
 * First version: April 23rd 2022
 */
 
static void ring_vm_openssl_pkeyfree( void *pRingState,void *pPointer )
{
    EVP_PKEY *pKey  ;
    pKey = (EVP_PKEY *) pPointer ;
    EVP_PKEY_free( pKey ) ;
}

void ring_vm_openssl_rsa_generate ( void *pPointer )
{
	EVP_PKEY *pKey ;
	BIGNUM *e ;
	#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	EVP_PKEY_CTX *ctx;
	#else
	RSA *pRsa  ;
	#endif
	int nBits, nPubExp=65537;/* we use 65537 as default exponent */
	if ( RING_API_PARACOUNT != 1 && RING_API_PARACOUNT != 2) {
		RING_API_ERROR(RING_API_BADPARACOUNT);
		return ;
	}
	if ( ! RING_API_ISNUMBER(1) ) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	
	nBits = (int) RING_API_GETNUMBER(1) ;
	
	if (nBits < 512 || nBits > OPENSSL_RSA_MAX_MODULUS_BITS) {
		RING_API_ERROR(RING_API_BADPARAVALUE);
		return ;
	}
	
	if ( RING_API_PARACOUNT == 2 ) {
		if ( ! RING_API_ISNUMBER(2) ) {
			RING_API_ERROR(RING_API_BADPARATYPE);
			return ;
		}
		
		nPubExp = (int) RING_API_GETNUMBER(2) ;
		
		if (nPubExp < 3) {
			RING_API_ERROR(RING_API_BADPARAVALUE);
			return ;
		}
	}
	
	e = BN_new();
	BN_set_word (e,nPubExp);
	pKey = NULL;
	#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
	if (ctx) {
		if ( (EVP_PKEY_keygen_init(ctx) > 0) 
			&& (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, nBits) > 0) 
			&& (EVP_PKEY_CTX_set_rsa_keygen_pubexp(ctx, e) > 0) ) {
            /* e belongs now to ctx so we should not free it */
            e = NULL;
			/* perform RSA key generation */
			if (EVP_PKEY_keygen(ctx, &pKey) <= 0) {
				/* failure */
				pKey = NULL;
			}
		}
		
		EVP_PKEY_CTX_free(ctx);
	}
	#else
	pRsa = RSA_new() ;
	if (RSA_generate_key_ex(pRsa,nBits,e,NULL)) {
		pKey = EVP_PKEY_new();
		EVP_PKEY_assign_RSA(pKey, pRsa); /* pRsa is now owned by pKey */
	}
	else {
		RSA_free(pRsa);
	}
	#endif
	
	if (pKey) {
		/* success case: return the object */
		RING_API_RETMANAGEDCPOINTER(pKey,"OSSL_PKEY",ring_vm_openssl_pkeyfree);
	}
	else {
		RING_API_ERROR(RING_API_INTERNALFAILURE);
	}
	if (e) {
        BN_free(e);
    }
}

void ring_vm_openssl_rsa_is_privatekey ( void *pPointer )
{
	EVP_PKEY *pKey = NULL;
	RSA *pRsa  ;
	const BIGNUM *p,*q,*dmp1,*dmq1,*iqmp,*n,*e,*d;
	List *pList  ;
	int nSize, nBits;
	unsigned char* pBuf;
	if ( RING_API_PARACOUNT != 1 ) {
		RING_API_ERROR(RING_API_BADPARACOUNT);
		return ;
	}
	if ( ! RING_API_ISCPOINTER(1) ) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	
	pKey = (EVP_PKEY*) RING_API_GETCPOINTER(1,"OSSL_PKEY");
	if ( pKey == NULL) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	
	/* check that this is indeed an RSA key */
	#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	if (EVP_PKEY_base_id(pKey) != EVP_PKEY_RSA) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	#else
	if (EVP_PKEY_type(pKey->type) != EVP_PKEY_RSA) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	#endif
	
	pRsa = EVP_PKEY_get1_RSA(pKey);
	
	/* export all parameters */
	#if OPENSSL_VERSION_NUMBER >= 0x10100000L
	n = RSA_get0_n(pRsa);
	e = RSA_get0_e(pRsa);
	d = RSA_get0_d(pRsa);
	p = RSA_get0_p(pRsa);
	q = RSA_get0_q(pRsa);
	dmp1 = RSA_get0_dmp1(pRsa);
	dmq1 = RSA_get0_dmq1(pRsa);
	iqmp = RSA_get0_iqmp(pRsa);
	#else
	n = pRsa->n;
	e = pRsa->e;
	d = pRsa->d;
	p = pRsa->p;
	q = pRsa->q;
	dmp1 = pRsa->dmp1;
	dmq1 = pRsa->dmq1;
	iqmp = pRsa->iqmp;
	#endif
	
	if (!n || !e) {
		/* modulus and public exponent must always be present */
		RING_API_ERROR(RING_API_BADPARATYPE);
	}
	else if (d || (p && q && dmp1 && dmq1 && iqmp)) {
		/* this is private key */
		RING_API_RETNUMBER(1);
	}
	else {
		/* this is a public key */
		RING_API_RETNUMBER(0);
	}

	RSA_free(pRsa);
}

static void ring_vm_openssl_add_bignum_to_list( void *pPointer, const char* cComponentName, const BIGNUM* pComponent, List* pList, unsigned char* pBuf )
{
	char* cStr;
	List* pSubList;
	
	pSubList = ring_list_newlist_gc(((VM *) pPointer)->pRingState,pList);
	/* first add component name, then add component value */
	ring_list_addstring_gc(((VM *) pPointer)->pRingState,pSubList,cComponentName);
	if (pComponent) {
		cStr = BN_bn2hex(pComponent);
		ring_list_addstring_gc(((VM *) pPointer)->pRingState,pSubList,cStr);
	}
	else {
		/* add empty string */
		ring_list_addstring_gc(((VM *) pPointer)->pRingState,pSubList,"");
	}
}

static void ring_vm_openssl_add_bits_to_list( void *pPointer, int nBits, List* pList )
{
	int nSize;
	List* pSubList;
	
	pSubList = ring_list_newlist_gc(((VM *) pPointer)->pRingState,pList);
	/* first add component name, then add component value */
	ring_list_addstring_gc(((VM *) pPointer)->pRingState,pSubList,"bits");
	ring_list_adddouble_gc(((VM *) pPointer)->pRingState,pSubList, (double) nBits);
}

static void ring_vm_openssl_add_keytype_to_list( void *pPointer, const char* cKeyType, List* pList )
{
	int nSize;
	List* pSubList;
	
	pSubList = ring_list_newlist_gc(((VM *) pPointer)->pRingState,pList);
	/* first add component name, then add component value */
	ring_list_addstring_gc(((VM *) pPointer)->pRingState,pSubList,"type");
	ring_list_addstring_gc(((VM *) pPointer)->pRingState,pSubList,cKeyType);
}

void ring_vm_openssl_rsa_export_params ( void *pPointer )
{
	EVP_PKEY *pKey = NULL;
	RSA *pRsa  ;
	const BIGNUM *p,*q,*dmp1,*dmq1,*iqmp,*n,*e,*d;
	List *pList  ;
	int nSize, nBits;
	unsigned char* pBuf;
	if ( RING_API_PARACOUNT != 1 ) {
		RING_API_ERROR(RING_API_BADPARACOUNT);
		return ;
	}
	if ( ! RING_API_ISCPOINTER(1) ) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	
	pKey = (EVP_PKEY*) RING_API_GETCPOINTER(1,"OSSL_PKEY");
	if ( pKey == NULL) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	
	/* check that this is indeed an RSA key */
	#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	if (EVP_PKEY_base_id(pKey) != EVP_PKEY_RSA) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	#else
	if (EVP_PKEY_type(pKey->type) != EVP_PKEY_RSA) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	#endif
	
	pRsa = EVP_PKEY_get1_RSA(pKey);
	
	/* export all parameters */
	#if OPENSSL_VERSION_NUMBER >= 0x10100000L
	n = RSA_get0_n(pRsa);
	e = RSA_get0_e(pRsa);
	d = RSA_get0_d(pRsa);
	p = RSA_get0_p(pRsa);
	q = RSA_get0_q(pRsa);
	dmp1 = RSA_get0_dmp1(pRsa);
	dmq1 = RSA_get0_dmq1(pRsa);
	iqmp = RSA_get0_iqmp(pRsa);
	#else
	n = pRsa->n;
	e = pRsa->e;
	d = pRsa->d;
	p = pRsa->p;
	q = pRsa->q;
	dmp1 = pRsa->dmp1;
	dmq1 = pRsa->dmq1;
	iqmp = pRsa->iqmp;
	#endif
	
	nBits = BN_num_bits(n);
	
	/* allocate a buffer that is enough to encode all components */
	pBuf = (unsigned char*) RING_API_MALLOC(BN_num_bytes(n));
	if (pBuf) {
		pList = RING_API_NEWLIST;

		ring_vm_openssl_add_bits_to_list(pPointer, nBits, pList);
		ring_vm_openssl_add_keytype_to_list(pPointer, "RSA", pList);
		ring_vm_openssl_add_bignum_to_list(pPointer, "n", n, pList, pBuf);
		ring_vm_openssl_add_bignum_to_list(pPointer, "e", e, pList, pBuf);
		ring_vm_openssl_add_bignum_to_list(pPointer, "d", d, pList, pBuf);
		ring_vm_openssl_add_bignum_to_list(pPointer, "p", p, pList, pBuf);
		ring_vm_openssl_add_bignum_to_list(pPointer, "q", q, pList, pBuf);
		ring_vm_openssl_add_bignum_to_list(pPointer, "dmp1", dmp1, pList, pBuf);
		ring_vm_openssl_add_bignum_to_list(pPointer, "dmq1", dmq1, pList, pBuf);
		ring_vm_openssl_add_bignum_to_list(pPointer, "iqmp", iqmp, pList, pBuf);

		RING_API_FREE(pBuf);
		RING_API_RETLIST(pList);		
	}
	else {
		RING_API_ERROR(RING_OOM);
	}
	
	RSA_free(pRsa);
}

void ring_vm_openssl_rsa_export_pem ( void *pPointer )
{
	EVP_PKEY *pKey = NULL;
	RSA *pRsa  ;
	const BIGNUM *p,*q,*dmp1,*dmq1,*iqmp,*n,*e,*d;
	BIO *bio;
	int nSize,nStatus;
	char* cStr;
	if ( RING_API_PARACOUNT != 1 ) {
		RING_API_ERROR(RING_API_BADPARACOUNT);
		return ;
	}
	if ( ! RING_API_ISCPOINTER(1) ) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	
	pKey = (EVP_PKEY*) RING_API_GETCPOINTER(1,"OSSL_PKEY");
	if ( pKey == NULL) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	
	/* check that this is indeed an RSA key */
	#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	if (EVP_PKEY_base_id(pKey) != EVP_PKEY_RSA) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	#else
	if (EVP_PKEY_type(pKey->type) != EVP_PKEY_RSA) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	#endif
	
	pRsa = EVP_PKEY_get1_RSA(pKey);
	
	/* export all parameters */
	#if OPENSSL_VERSION_NUMBER >= 0x10100000L
	n = RSA_get0_n(pRsa);
	e = RSA_get0_e(pRsa);
	d = RSA_get0_d(pRsa);
	p = RSA_get0_p(pRsa);
	q = RSA_get0_q(pRsa);
	dmp1 = RSA_get0_dmp1(pRsa);
	dmq1 = RSA_get0_dmq1(pRsa);
	iqmp = RSA_get0_iqmp(pRsa);
	#else
	n = pRsa->n;
	e = pRsa->e;
	d = pRsa->d;
	p = pRsa->p;
	q = pRsa->q;
	dmp1 = pRsa->dmp1;
	dmq1 = pRsa->dmq1;
	iqmp = pRsa->iqmp;
	#endif
	
	/* n and e must not be NULL */
	if (!n || !e) {
		RSA_free(pRsa);
		RING_API_ERROR(RING_API_BADPARAVALUE);
		return;
	}
	
	bio = BIO_new(BIO_s_mem());
	
	if (d || (p && q && dmp1 && dmq1 && iqmp)) {
		/* this is a private key */
		nStatus = PEM_write_bio_PrivateKey(bio, pKey, NULL, NULL, 0, 0, NULL);
	}
	else {
		/* this is a public key */
		nStatus = PEM_write_bio_PUBKEY(bio, pKey);
	}
	
	if (  nStatus > 0 ) {
		nSize = (int) BIO_pending(bio);
        /* Pre-allocated the return value on the stack */
        RING_API_RETSTRINGSIZE(nSize);
        cStr = ring_string_get(RING_API_GETSTRINGRAW);
		BIO_read(bio, cStr, nSize);
		BIO_free_all(bio);
	}
	else {
		/* OpenSSL error */
		RING_API_ERROR( ERR_reason_error_string(ERR_get_error()));
	}
	
	RSA_free(pRsa);
}

void ring_vm_openssl_rsa_import_params ( void *pPointer )
{
	EVP_PKEY *pKey = NULL;
	RSA *pRsa  ;
	BIGNUM *p=NULL,*q=NULL,*dmp1=NULL,*dmq1=NULL,*iqmp=NULL,*n=NULL,*e=NULL,*d=NULL;
	BIGNUM **targetComponent;
	List *pList,*pSubList  ;
	int x,nSize,nBits,nFailed,nBufSize;
	char* cStrIndex;
	unsigned char* pBuf;
	if ( RING_API_PARACOUNT != 1 ) {
		RING_API_ERROR(RING_API_BADPARACOUNT);
		return ;
	}
	if ( ! RING_API_ISLIST(1) ) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	
	pList = RING_API_GETLIST(1);
	nSize = ring_list_getsize(pList);
	/* the minimal list is the one container the modulus and public exponent */
	if ( nSize < 2) {
		RING_API_ERROR(RING_API_BADPARAVALUE);
		return ;
	}
	
	/* check that each element of the list is a list that has two string elements */
	nFailed = 0;
	for ( x = 1; x <= nSize ; x++ ) {
		if ( !ring_list_islist(pList,x) ) {
			nFailed = 1;
			break ;
		}
		
		pSubList = ring_list_getlist(pList,x);
		if ( ring_list_getsize(pSubList) != 2 ) {
			nFailed = 1;
			break ;
		}
		if ( !ring_list_isstring(pSubList,1)) {
			nFailed = 1;
			break;
		}
		if ( !ring_list_isstring(pSubList,2)) {
			continue;
		}

		cStrIndex = (char*) ring_list_getstring(pSubList,1);
		pBuf = (unsigned char*) ring_list_getstring(pSubList,2);
		nBufSize = (int) ring_list_getstringsize(pSubList,2);
		
		/* skip if the entry is empty */
		if ( (*cStrIndex == 0) || (nBufSize == 0) ) {
			continue;
		}
		
		/* look for RSA components by name */
		if ( strcmp(cStrIndex,"n") == 0) {
			targetComponent = &n;
		}
		else if (strcmp(cStrIndex,"e") == 0) {
			targetComponent = &e;
		}
		else if (strcmp(cStrIndex,"d") == 0) {
			targetComponent = &d;
		}
		else if (strcmp(cStrIndex,"p") == 0) {
			targetComponent = &p;
		}
		else if (strcmp(cStrIndex,"q") == 0) {
			targetComponent = &q;
		}
		else if (strcmp(cStrIndex,"dmp1") == 0) {
			targetComponent = &dmp1;
		}
		else if (strcmp(cStrIndex,"dmq1") == 0) {
			targetComponent = &dmq1;
		}
		else if (strcmp(cStrIndex,"iqmp") == 0) {
			targetComponent = &iqmp;
		}
		else {
			targetComponent = NULL;
		}
		
		if (targetComponent) {
			/* if we have been already allocated, then return error since it means there is 
			 * a duplicated entry in the list
			 */
			if ( *targetComponent )
			{
				nFailed = 1;
				break;				
			}

			if ( !BN_hex2bn(targetComponent, (const char*) (pBuf)) ) {
				nFailed = 1;
				break;
			}
		}
	}
	
	/* n and e must be set */
	if ( (nFailed == 0) && (!n || !e) )
	{
		nFailed = 1;
	}
	
	if ( nFailed == 0) {
		/* create the key */
		pRsa = RSA_new() ;
		#if OPENSSL_VERSION_NUMBER >= 0x10100000L
		RSA_set0_key(pRsa,n,e,d);
		RSA_set0_factors(pRsa,p,q);
		RSA_set0_crt_params(pRsa,dmp1,dmq1,iqmp);
		#else
		pRsa->n = n;
		pRsa->e = e;
		pRsa->d = d;
		pRsa->p = p;
		pRsa->q = q;
		pRsa->dmp1 = dmp1;
		pRsa->dmq1 = dmq1;
		pRsa->iqmp = iqmp;
		#endif
		/* memory management transfered to RSA object */
		n = NULL;
		e = NULL;
		d = NULL;
		p = NULL;
		q = NULL;
		dmp1 = NULL;
		dmq1 = NULL;
		iqmp = NULL;
		
		pKey = EVP_PKEY_new();
		EVP_PKEY_assign_RSA(pKey, pRsa); /* pRsa is now owned by pKey */
		/* success case: return the object */
		RING_API_RETMANAGEDCPOINTER(pKey,"OSSL_PKEY",ring_vm_openssl_pkeyfree);
	}
	else
	{
		RING_API_ERROR(RING_API_BADPARAVALUE);
	}
	
	if (n) BN_free(n);
	if (e) BN_free(e);
	if (d) BN_free(d);
	if (p) BN_free(p);
	if (q) BN_free(q);
	if (dmp1) BN_free(dmp1);
	if (dmq1) BN_free(dmq1);
	if (iqmp) BN_free(iqmp);
}

void ring_vm_openssl_rsa_import_pem ( void *pPointer )
{
	EVP_PKEY *pKey = NULL;
	char* cStr;
	int nSize,nStatus;
	BIO *bio;
	if ( RING_API_PARACOUNT != 1 ) {
		RING_API_ERROR(RING_API_BADPARACOUNT);
		return ;
	}
	if ( ! RING_API_ISSTRING(1) ) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	
	cStr = RING_API_GETSTRING(1);
	nSize = RING_API_GETSTRINGSIZE(1);
	if ( nSize < 2) {
		RING_API_ERROR(RING_API_BADPARAVALUE);
		return ;
	}
	
	bio = BIO_new(BIO_s_mem());
	BIO_write(bio, cStr, nSize);
	pKey = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
	BIO_free_all(bio);
	
	if (!pKey) {
		/* try public key import */
		bio = BIO_new(BIO_s_mem());
		BIO_write(bio, cStr, nSize);
		pKey = PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
		BIO_free_all(bio);
	}
	
	if (!pKey) {
		/* invalid PEM */
		RING_API_ERROR(RING_API_BADPARAVALUE);
		return ;
	}
	
	/* check that this is indeed an RSA key */
	#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	if (EVP_PKEY_base_id(pKey) != EVP_PKEY_RSA) {
		EVP_PKEY_free(pKey);
		RING_API_ERROR(RING_API_BADPARAVALUE);
	}
	#else
	if (EVP_PKEY_type(pKey->type) != EVP_PKEY_RSA) {
		EVP_PKEY_free(pKey);
		RING_API_ERROR(RING_API_BADPARAVALUE);
	}
	#endif
	
	/* success case: return the object */
	RING_API_RETMANAGEDCPOINTER(pKey,"OSSL_PKEY",ring_vm_openssl_pkeyfree);
}

void ring_vm_openssl_rsa_encrypt_pkcs ( void *pPointer )
{
	EVP_PKEY *pKey = NULL;
	RSA *pRsa = NULL  ;
	int nSize, nModulusLen;
	unsigned char *cInput, *cOutput;
	if ( RING_API_PARACOUNT != 2  ) {
		RING_API_ERROR(RING_API_BADPARACOUNT);
		return ;
	}
	if ( ! RING_API_ISCPOINTER(1) || ! RING_API_ISSTRING(2) ) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	
	pKey = (EVP_PKEY*) RING_API_GETCPOINTER(1,"OSSL_PKEY");
	if ( pKey == NULL) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	
	/* check that this is indeed an RSA key */
	#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	if (EVP_PKEY_base_id(pKey) != EVP_PKEY_RSA) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	#else
	if (EVP_PKEY_type(pKey->type) != EVP_PKEY_RSA) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	#endif
	
	cInput = (unsigned char*) RING_API_GETSTRING(2);
	nSize = RING_API_GETSTRINGSIZE(2);

	nModulusLen = EVP_PKEY_size(pKey);
	
	/* input size must be less than modulusLen - 11 */
	if ( nSize > (nModulusLen - 11) ) {
		RING_API_ERROR(RING_API_BADPARAVALUE);
		return ;
	}
	
	cOutput = (unsigned char*) RING_API_MALLOC(nModulusLen);
	if ( cOutput == NULL ) {
		RING_API_ERROR(RING_OOM);
		return;
	}
	
	pRsa = EVP_PKEY_get1_RSA(pKey);
	
	if (RSA_public_encrypt(nSize, cInput,cOutput,pRsa,RSA_PKCS1_PADDING) != nModulusLen) {
		/* OpenSSL error */
		RING_API_ERROR( ERR_reason_error_string(ERR_get_error()));
	}
	else {
		RING_API_RETSTRING2((const char*) cOutput,nModulusLen);
	}
	
	
	RING_API_FREE(cOutput);
	RSA_free(pRsa);
}

void ring_vm_openssl_rsa_decrypt_pkcs ( void *pPointer )
{
	EVP_PKEY *pKey = NULL;
	RSA *pRsa = NULL  ;
	int nSize, nModulusLen, nOutput;
	unsigned char *cInput, *cOutput;
	if ( RING_API_PARACOUNT != 2  ) {
		RING_API_ERROR(RING_API_BADPARACOUNT);
		return ;
	}
	if ( ! RING_API_ISCPOINTER(1) || ! RING_API_ISSTRING(2) ) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	
	pKey = (EVP_PKEY*) RING_API_GETCPOINTER(1,"OSSL_PKEY");
	if ( pKey == NULL) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	
	/* check that this is indeed an RSA key */
	#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	if (EVP_PKEY_base_id(pKey) != EVP_PKEY_RSA) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	#else
	if (EVP_PKEY_type(pKey->type) != EVP_PKEY_RSA) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	#endif
	
	cInput = (unsigned char*) RING_API_GETSTRING(2);
	nSize = RING_API_GETSTRINGSIZE(2);

	nModulusLen = EVP_PKEY_size(pKey);
	
	/* input size must be equal to modulusLen*/
	if ( nSize != nModulusLen ) {
		RING_API_ERROR(RING_API_BADPARAVALUE);
		return ;
	}
	
	cOutput = (unsigned char*) RING_API_MALLOC(nModulusLen);
	if ( cOutput == NULL ) {
		RING_API_ERROR(RING_OOM);
		return;
	}
	
	pRsa = EVP_PKEY_get1_RSA(pKey);
	
	nOutput = RSA_private_decrypt(nSize, cInput,cOutput,pRsa,RSA_PKCS1_PADDING);
	
	if ( nOutput < 0 ) {
		/* OpenSSL error */
		RING_API_ERROR( ERR_reason_error_string(ERR_get_error()));
	}
	else {
		RING_API_RETSTRING2((const char*) cOutput,nOutput);
	}
	
	
	RING_API_FREE(cOutput);
	RSA_free(pRsa);
}

void ring_vm_openssl_rsa_encrypt_oaep ( void *pPointer )
{
	#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	EVP_PKEY_CTX *ctx ;
    size_t nOutput ;
    int nStatus ;
	#else
	RSA *pRsa ;
    int nOutput ;
	#endif
	EVP_PKEY *pKey ;
	const EVP_MD *md ;
	int nSize,nModulusLen,nMgfHashAlg;
	unsigned char *cInput, *cOutput;
	if ( RING_API_PARACOUNT != 2 && RING_API_PARACOUNT != 3 ) {
		RING_API_ERROR(RING_API_BADPARACOUNT);
		return ;
	}
	if ( ! RING_API_ISCPOINTER(1) || ! RING_API_ISSTRING(2) ) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	
	if ( (RING_API_PARACOUNT == 3) && ! RING_API_ISNUMBER(3) ) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	
	pKey = (EVP_PKEY*) RING_API_GETCPOINTER(1,"OSSL_PKEY");
	if ( pKey == NULL) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	
	/* check that this is indeed an RSA key */
	#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	if (EVP_PKEY_base_id(pKey) != EVP_PKEY_RSA) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	#else
	if (EVP_PKEY_type(pKey->type) != EVP_PKEY_RSA) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	#endif
	
	if (RING_API_PARACOUNT == 3) {
		nMgfHashAlg = (int) RING_API_GETNUMBER(3);
	}
	else {
		/* use SHA1 by default */
		nMgfHashAlg = OSSL_OAEP_SHA1;
	}
	
	#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	switch (nMgfHashAlg) {
	case OSSL_OAEP_MD5: md = EVP_md5(); break;
	case OSSL_OAEP_SHA1: md = EVP_sha1(); break;
	case OSSL_OAEP_SHA256: md = EVP_sha256(); break;
	case OSSL_OAEP_SHA384: md = EVP_sha384(); break;
	case OSSL_OAEP_SHA512: md = EVP_sha512(); break;
	default:
		RING_API_ERROR(RING_API_BADPARAVALUE);
		return ;
	}
	#else
	/* we support only SHA1 for old OpenSSL */
	if (nMgfHashAlg != OSSL_OAEP_SHA1) {
		RING_API_ERROR(RING_API_BADPARAVALUE);
		return ;
	}
	md = EVP_sha1();
	#endif
	
	cInput = (unsigned char*) RING_API_GETSTRING(2);
	nSize = RING_API_GETSTRINGSIZE(2);

	nModulusLen = EVP_PKEY_size(pKey);
	
	/* input size for OAEP must be less than modulusLen - (2*hashSize) - 2 */
	if ( nSize > (nModulusLen - (2 * EVP_MD_size(md)) - 2) ) {
		RING_API_ERROR(RING_API_BADPARAVALUE);
		return ;
	}
	
	cOutput = (unsigned char*) RING_API_MALLOC(nModulusLen);
	if ( cOutput == NULL ) {
		RING_API_ERROR(RING_OOM);
		return;
	}

	#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	nOutput = (size_t) nModulusLen;
	ctx = EVP_PKEY_CTX_new(pKey,NULL);
	if ( 	(EVP_PKEY_encrypt_init(ctx) > 0)
		&&	(EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) > 0)
		&&	(EVP_PKEY_CTX_set_rsa_oaep_md(ctx, md) > 0)
		&&	(EVP_PKEY_CTX_set_rsa_mgf1_md(ctx, md) > 0)
		) {
		nStatus = EVP_PKEY_encrypt(ctx, cOutput, &nOutput, cInput, nSize);
		if (  nStatus <= 0 ) {
			/* error occured */
			RING_API_ERROR( ERR_reason_error_string(ERR_get_error()));
		}
		else {
			RING_API_RETSTRING2((const char*) cOutput,nModulusLen);
		}
	}
	else {
		RING_API_ERROR( ERR_reason_error_string(ERR_get_error()));
	}
	#else
	pRsa = EVP_PKEY_get1_RSA(pKey);
	nOutput = RSA_public_encrypt(nSize, cInput,cOutput,pRsa,RSA_PKCS1_OAEP_PADDING);
	if ( nOutput == nModulusLen) {
		RING_API_RETSTRING2((const char*) cOutput,nModulusLen);
	}
	else {
		/* error occured */
		RING_API_ERROR( ERR_reason_error_string(ERR_get_error()));
	}
	#endif


	RING_API_FREE(cOutput);
	#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	EVP_PKEY_CTX_free(ctx);
	#else
	RSA_free(pRsa);
	#endif
}

void ring_vm_openssl_rsa_decrypt_oaep ( void *pPointer )
{
	#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	const EVP_MD *md;
	EVP_PKEY_CTX *ctx;
    size_t nOutput ;
    int nStatus ;
	#else
	RSA *pRsa ;
    int nOutput ;
	#endif
	EVP_PKEY *pKey;
	int nSize,nModulusLen,nMgfHashAlg;
	unsigned char *cInput, *cOutput;
	if ( RING_API_PARACOUNT != 2 && RING_API_PARACOUNT != 3 ) {
		RING_API_ERROR(RING_API_BADPARACOUNT);
		return ;
	}
	if ( ! RING_API_ISCPOINTER(1) || ! RING_API_ISSTRING(2) ) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	
	if ( (RING_API_PARACOUNT == 3) && ! RING_API_ISNUMBER(3) ) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	
	pKey = (EVP_PKEY*) RING_API_GETCPOINTER(1,"OSSL_PKEY");
	if ( pKey == NULL) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	
	/* check that this is indeed an RSA key */
	#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	if (EVP_PKEY_base_id(pKey) != EVP_PKEY_RSA) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	#else
	if (EVP_PKEY_type(pKey->type) != EVP_PKEY_RSA) {
		RING_API_ERROR(RING_API_BADPARATYPE);
		return ;
	}
	#endif
	
	if (RING_API_PARACOUNT == 3) {
		nMgfHashAlg = (int) RING_API_GETNUMBER(3);
	}
	else {
		/* use SHA1 by default */
		nMgfHashAlg = OSSL_OAEP_SHA1;
	}
	
	#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	switch (nMgfHashAlg) {
	case OSSL_OAEP_MD5: md = EVP_md5(); break;
	case OSSL_OAEP_SHA1: md = EVP_sha1(); break;
	case OSSL_OAEP_SHA256: md = EVP_sha256(); break;
	case OSSL_OAEP_SHA384: md = EVP_sha384(); break;
	case OSSL_OAEP_SHA512: md = EVP_sha512(); break;
	default:
		RING_API_ERROR(RING_API_BADPARAVALUE);
		return ;
	}
	#else
	/* we support only SHA1 for old OpenSSL */
	if (nMgfHashAlg != OSSL_OAEP_SHA1) {
		RING_API_ERROR(RING_API_BADPARAVALUE);
		return ;
	}
	#endif
	
	cInput = (unsigned char*) RING_API_GETSTRING(2);
	nSize = RING_API_GETSTRINGSIZE(2);

	nModulusLen = EVP_PKEY_size(pKey);
	
	/* input size must be equal to modulusLen */
	if ( nSize != nModulusLen ) {
		RING_API_ERROR(RING_API_BADPARAVALUE);
		return ;
	}
	
	cOutput = (unsigned char*) RING_API_MALLOC(nModulusLen);
	if ( cOutput == NULL ) {
		RING_API_ERROR(RING_OOM);
		return;
	}

	#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	nOutput = (size_t) nModulusLen;
	ctx = EVP_PKEY_CTX_new(pKey,NULL);
	if ( 	(EVP_PKEY_decrypt_init(ctx) > 0)
		&&	(EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) > 0)
		&&	(EVP_PKEY_CTX_set_rsa_oaep_md(ctx, md) > 0)
		&&	(EVP_PKEY_CTX_set_rsa_mgf1_md(ctx, md) > 0)
		) {
		if ( EVP_PKEY_decrypt(ctx, cOutput, &nOutput, cInput, (size_t) nSize) <= 0 ) {
			/* error occured */
			RING_API_ERROR( ERR_reason_error_string(ERR_get_error()));
		}
		else {
			RING_API_RETSTRING2((const char*) cOutput,(int) nOutput);
		}
	}
	else {
		/* error occured */
		RING_API_ERROR( ERR_reason_error_string(ERR_get_error()));
	}
	#else
	pRsa = EVP_PKEY_get1_RSA(pKey);
	nOutput = RSA_private_decrypt(nSize, cInput,cOutput,pRsa,RSA_PKCS1_OAEP_PADDING);
	if ( nOutput < 0 ) {
		/* unexpected error */
		RING_API_ERROR( ERR_reason_error_string(ERR_get_error()));
	}
	else {
		RING_API_RETSTRING2((const char*) cOutput,nOutput);
	}
	#endif

	RING_API_FREE(cOutput);
	#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	EVP_PKEY_CTX_free(ctx);
	#else
	RSA_free(pRsa);
	#endif
}
