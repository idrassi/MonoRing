/*
**  Copyright (c) 2013-2025 Mahmoud Fayed <msfclipper@yahoo.com>
**  pClassesMap ( cClass Name ,  iPC , cParentClass, aMethodsList , nFlagIsParentClassInformation, PointerToPackage )
**  pClassesMap ( cClass Name, Pointer to List that represent class inside a Package, Pointer to File )
**  pFunctionsMap ( Name, PC, FileName, Private Flag )
**  Packages List ( Package Name , Classes List )
**  Object ( is a list of two items , (1) Class Pointer  (2) Object Data  )
**  pVM->pScopeNewObj : (1) Previous scope (2) nListStart (3) pNestedLists (4) nSP
**  pVM->pObjState  (  [ Pointer to Scope, Pointer to Methods , Pointer to Class, Optional True)
**  The optional True used with LoadMethod so we can Know that we are inside class method during RT
**  We don't check the True value, we just check that the size of the list is 4
**  used in ring_vmfuncs , function ring_vm_loadfunc2()
**  used in ring_vmvars , function ring_vm_findvar2()
**  pBraceObject : The list that represent the object directly (not variable/list item)
**  pBraceObjects ( pBraceObject, nSP, nListStart, pNestedLists)
**  pSetProperty ( Object Pointer , Type (Variable/ListItem)  , Property Name, Property Variable ,
*nBeforeEqual,Value,PtrType)
*/
#include "ring.h"

void ring_vm_oop_newobj(VM *pVM) {
	const char *cClassName, *cClassName2;
	unsigned int x, nLimit, nClassPC, nType;
	List *pList, *pList2, *pList3, *pList4, *pVar, *pSelf;
	Item *pItem;
	pList2 = NULL;
	pVar = NULL;
	pItem = NULL;
	cClassName = RING_VM_IR_READC;
	/* Check using variable to get the class name */
	if (RING_VM_IR_READIVALUE(RING_VM_IR_REG2)) {
		if (ring_vm_findvar(pVM, cClassName)) {
			pVar = (List *)RING_VM_STACK_READP;
			if (ring_list_isstring(pVar, RING_VAR_VALUE)) {
				if (strcmp(ring_list_getstring(pVar, RING_VAR_VALUE), RING_CSTR_EMPTY) != 0) {
					cClassName = ring_list_getstring(pVar, RING_VAR_VALUE);
				} else {
					ring_vm_error(pVM, RING_VM_ERROR_USINGNULLVARIABLE);
					return;
				}
			} else {
				ring_vm_error(pVM, RING_VM_ERROR_VARISNOTSTRING);
				return;
			}
			RING_VM_STACK_POP;
			pVar = NULL;
		} else {
			ring_vm_error(pVM, RING_VM_ERROR_NOTVARIABLE);
			return;
		}
	}
	nLimit = ring_vm_oop_visibleclassescount(pVM);
	if (nLimit > 0) {
		for (x = 1; x <= nLimit; x++) {
			pList = ring_vm_oop_visibleclassitem(pVM, x);
			cClassName2 = ring_list_getstring(pList, RING_CLASSMAP_CLASSNAME);
			pList = ring_vm_oop_checkpointertoclassinpackage(pVM, pList);
			if (pList == NULL) {
				continue;
			}
			nClassPC = ring_list_getint(pList, RING_CLASSMAP_PC);
			if (strcmp(cClassName, cClassName2) == 0) {
				if (ring_vm_notusingvarduringdef(pVM)) {
					/* Create the Temp Variable */
					ring_vm_createtemplist(pVM);
					pVar = (List *)RING_VM_STACK_READP;
					nType = RING_VM_STACK_OBJTYPE;
					ring_list_setint_gc(pVM->pRingState, pVar, RING_VAR_TYPE, RING_VM_LIST);
					ring_list_setlist_gc(pVM->pRingState, pVar, RING_VAR_VALUE);
					pList2 = ring_list_getlist(pVar, RING_VAR_VALUE);
					/* When using something like Ref(new myclass) don't create new reference */
					if (ring_vm_funccallbeforecall(pVM)) {
						ring_vm_oop_cleansetpropertylist(pVM);
						ring_list_enabledontref_gc(pVM->pRingState, pList2);
					}
				} else {
					/* Prepare Object List */
					if (RING_VM_STACK_OBJTYPE == RING_OBJTYPE_VARIABLE) {
						pVar = (List *)RING_VM_STACK_READP;
						/* Check before assignment */
						if (ring_vm_gc_checkbeforeassignment(pVM, pVar)) {
							return;
						}
						ring_list_setint_gc(pVM->pRingState, pVar, RING_VAR_TYPE, RING_VM_LIST);
						ring_list_setlist_gc(pVM->pRingState, pVar, RING_VAR_VALUE);
						pList2 = ring_list_getlist(pVar, RING_VAR_VALUE);
					} else if (RING_VM_STACK_OBJTYPE == RING_OBJTYPE_LISTITEM) {
						pItem = (Item *)RING_VM_STACK_READP;
						/* Check before assignment */
						if (ring_vm_gc_checkitemerroronassignment(pVM, pItem)) {
							return;
						}
						ring_item_settype_gc(pVM->pRingState, pItem, ITEMTYPE_LIST);
						pVar = ring_item_getlist(pItem);
						pList2 = pVar;
					}
					nType = RING_VM_STACK_OBJTYPE;
					ring_vm_stackdup(pVM);
				}
				ring_list_deleteallitems_gc(pVM->pRingState, pList2);
				/* Store the Class Pointer in the Object Data */
				ring_list_addpointer_gc(pVM->pRingState, pList2, pList);
				/* Create List for the Object State */
				pList3 = ring_list_newlist_gc(pVM->pRingState, pList2);
				/* Create Self variable in the state list */
				pSelf = ring_vm_newvar2(pVM, RING_CSTR_SELF, pList3);
				ring_list_setint_gc(pVM->pRingState, pSelf, RING_VAR_TYPE, RING_VM_POINTER);
				if (nType == RING_OBJTYPE_VARIABLE) {
					ring_list_setpointer_gc(pVM->pRingState, pSelf, RING_VAR_VALUE, pVar);
				} else if (nType == RING_OBJTYPE_LISTITEM) {
					ring_list_setpointer_gc(pVM->pRingState, pSelf, RING_VAR_VALUE, pItem);
				}
				ring_list_setint_gc(pVM->pRingState, pSelf, RING_VAR_PVALUETYPE, nType);
				/* Save the State */
				ring_vm_savestatefornewobjects(pVM);
				/* Push Class Package */
				ring_vm_oop_pushclasspackage(pVM, pList);
				/* Jump to Class INIT Method */
				ring_vm_blockflag2(pVM, pVM->nPC);
				/* Execute Parent Classes Init first */
				if (strcmp(ring_list_getstring(pList, RING_CLASSMAP_PARENTCLASS), RING_CSTR_EMPTY) !=
				    0) {
					ring_vm_blockflag2(pVM, nClassPC);
					if (!ring_vm_oop_parentinit(pVM, pList)) {
						return;
					}
				} else {
					pVM->nPC = nClassPC;
				}
				/* Set Object State as the Current Scope */
				pVM->pActiveMem = pList3;
				/* Prepare to Make Object State & Methods visible while executing the INIT method */
				pList4 = ring_list_newlist_gc(pVM->pRingState, pVM->pObjState);
				ring_list_addpointer_gc(pVM->pRingState, pList4, pList3);
				ring_list_addpointer_gc(pVM->pRingState, pList4, NULL);
				ring_list_addpointer_gc(pVM->pRingState, pList4, pList);
				/* Create the Super Virtual Object */
				ring_vm_oop_newsuperobj(pVM, pList3, pList);
				/* Enable NULL variables (To be class attributes) */
				pVM->nInClassRegion++;
				return;
			}
		}
	}
	ring_vm_error2(pVM, RING_VM_ERROR_CLASSNOTFOUND, cClassName);
}

unsigned int ring_vm_oop_parentinit(VM *pVM, List *pList) {
	const char *cClassName, *cClassName2;
	unsigned int x, x2, nFound, nMark, lOutput;
	List *pList2, *pClassesList;
	String *pString;
	lOutput = 1;
	/* Get the parent class name from the Class List Pointer */
	cClassName = ring_list_getstring(pList, RING_CLASSMAP_PARENTCLASS);
	/* Create List for Classes Pointers */
	pClassesList = ring_list_new_gc(pVM->pRingState, RING_ZERO);
	ring_list_addpointer_gc(pVM->pRingState, pClassesList, pList);
	while (strcmp(cClassName, RING_CSTR_EMPTY) != 0) {
		/* Mark Packages Count */
		nMark = ring_list_getsize(pVM->pActivePackage);
		nFound = 0;
		for (x = 1; x <= ring_vm_oop_visibleclassescount(pVM); x++) {
			pList2 = ring_vm_oop_visibleclassitem(pVM, x);
			cClassName2 = ring_list_getstring(pList2, RING_CLASSMAP_CLASSNAME);
			pList2 = ring_vm_oop_checkpointertoclassinpackage(pVM, pList2);
			if (pList2 == NULL) {
				continue;
			}
			if (strcmp(cClassName, cClassName2) == 0) {
				/* Check that the parent class is not one of the subclasses */
				for (x2 = 1; x2 <= ring_list_getsize(pClassesList); x2++) {
					if (((List *)ring_list_getpointer(pClassesList, x2)) == pList2) {
						pString = ring_string_new_gc(pVM->pRingState, "When creating class ");
						ring_string_add_gc(pVM->pRingState, pString,
								   ring_list_getstring(pList, RING_CLASSMAP_CLASSNAME));
						ring_string_add_gc(pVM->pRingState, pString, " from class ");
						ring_string_add_gc(pVM->pRingState, pString, cClassName);
						ring_vm_error2(pVM, RING_VM_ERROR_PARENTCLASSLIKESUBCLASS,
							       ring_string_get(pString));
						ring_string_delete_gc(pVM->pRingState, pString);
						/* Delete Classes Pointers List */
						ring_list_delete_gc(pVM->pRingState, pClassesList);
						return RING_FALSE;
					}
				}
				ring_list_addpointer_gc(pVM->pRingState, pClassesList, pList2);
				/* Push Class Package */
				ring_vm_oop_pushclasspackage(pVM, pList2);
				cClassName = ring_list_getstring(pList2, RING_CLASSMAP_PARENTCLASS);
				if (strcmp(cClassName, RING_CSTR_EMPTY) != 0) {
					/* Add Class Init Method to be called */
					ring_vm_blockflag2(pVM, ring_list_getint(pList2, RING_CLASSMAP_PC));
				} else {
					pVM->nPC = ring_list_getint(pList2, RING_CLASSMAP_PC);
				}
				nFound = 1;
				break;
			}
		}
		if (nFound == 0) {
			/* Error Message */
			ring_vm_error2(pVM, RING_VM_ERROR_PARENTCLASSNOTFOUND, cClassName);
			lOutput = 0;
			break;
		}
		/* Restore Mark */
		ring_vm_oop_deletepackagesafter(pVM, nMark);
		pList = pList2;
	}
	/* Delete Classes Pointers List */
	ring_list_delete_gc(pVM->pRingState, pClassesList);
	return lOutput;
}

void ring_vm_oop_newclass(VM *pVM) {
	List *pClass, *pList;
	unsigned int x;
	pClass = (List *)RING_VM_IR_READPVALUE(RING_VM_IR_REG2);
	/* Find the Class Pointer using the Class Name */
	if (pClass == NULL) {
		for (x = 1; x <= ring_list_getsize(pVM->pRingState->pRingClassesMap); x++) {
			pList = ring_list_getlist(pVM->pRingState->pRingClassesMap, x);
			if (strcmp(ring_list_getstring(pList, RING_CLASSMAP_CLASSNAME),
				   RING_VM_IR_READCVALUE(RING_VM_IR_REG1)) == 0) {
				if (ring_list_getsize(pList) == RING_CLASSMAP_IMPORTEDCLASSSIZE) {
					/* Here the class is stored inside a package - we have the class pointer (item
					 * 2) */
					pClass = (List *)ring_list_getpointer(
					    pList, RING_CLASSMAP_POINTERTOLISTOFCLASSINSIDEPACKAGE);
				} else {
					pClass = pList;
				}
				RING_VM_IR_READPVALUE(RING_VM_IR_REG2) = (void *)pClass;
				break;
			}
		}
	}
	pClass = ring_vm_oop_checkpointertoclassinpackage(pVM, pClass);
	/* Make object methods visible while executing the Class Init method */
	pList = ring_list_getlist(pVM->pObjState, ring_list_getsize(pVM->pObjState));
	ring_list_setpointer_gc(pVM->pRingState, pList, RING_OBJSTATE_METHODS,
				ring_list_getlist(pClass, RING_CLASSMAP_METHODSLIST));
	/* Get Parent Classes Methods */
	ring_vm_oop_parentmethods(pVM, pClass);
	/* Attributes Scope is Public */
	pVM->lPrivateFlag = 0;
	/* Support using This in the class region */
	ring_vm_oop_setthethisvariableinclassregion(pVM);
	/* Set the current global scope */
	pVM->nCurrentGlobalScope = 0;
}

void ring_vm_oop_setscope(VM *pVM) {
	/* This function will be called after creating a new object and executing the class init() method */
	/* Restore State */
	ring_vm_restorestatefornewobjects(pVM);
}

unsigned int ring_vm_oop_isobject(VM *pVM, List *pList) { return ring_list_isobject(pList); }

List *ring_vm_oop_getobj(VM *pVM) {
	List *pVar;
	Item *pItem;
	pVar = NULL;
	/* Get Object Data */
	if (!RING_VM_STACK_ISPOINTER) {
		ring_vm_error(pVM, RING_VM_ERROR_NOTOBJECT);
		return NULL;
	}
	if (RING_VM_STACK_OBJTYPE == RING_OBJTYPE_VARIABLE) {
		pVar = (List *)RING_VM_STACK_READP;
		if (ring_list_getint(pVar, RING_VAR_TYPE) == RING_VM_NULL) {
			ring_vm_error2(pVM, RING_VM_ERROR_USINGNULLVARIABLE, ring_list_getstring(pVar, RING_VAR_NAME));
			return NULL;
		}
		if (!ring_list_islist(pVar, RING_VAR_VALUE)) {
			ring_vm_error(pVM, RING_VM_ERROR_NOTOBJECT);
			return NULL;
		}
		pVar = ring_list_getlist(pVar, RING_VAR_VALUE);
	} else if (RING_VM_STACK_OBJTYPE == RING_OBJTYPE_LISTITEM) {
		pItem = (Item *)RING_VM_STACK_READP;
		if (!ring_item_islist(pItem)) {
			ring_vm_error(pVM, RING_VM_ERROR_NOTOBJECT);
			return NULL;
		}
		pVar = ring_item_getlist(pItem);
	}
	if (ring_vm_oop_isobject(pVM, pVar) == 0) {
		ring_vm_error(pVM, RING_VM_ERROR_NOTOBJECT);
		return NULL;
	}
	RING_VM_STACK_POP;
	return pVar;
}

void ring_vm_oop_property(VM *pVM) {
	List *pVar, *pScope;
	/* Get Object Pointer Before being a list by getobj */
	if (RING_VM_STACK_ISPOINTER) {
		pVM->pGetSetObject = RING_VM_STACK_READP;
		pVM->nGetSetObjType = RING_VM_STACK_OBJTYPE;
	}
	/* Get Object Data */
	pVar = ring_vm_oop_getobj(pVM);
	if (pVar == NULL) {
		return;
	}
	/* Get Object State */
	pScope = pVM->pActiveMem;
	pVM->pActiveMem = ring_list_getlist(pVar, RING_OBJECT_OBJECTDATA);
	pVM->lGetSetProperty = 1;
	if (ring_vm_findvar(pVM, RING_VM_IR_READC) == 0) {
		/* Create the attribute if we are in the class region after the class name */
		if (pVM->nInClassRegion) {
			ring_vm_newvar(pVM, RING_VM_IR_READC);
			/* Support for Private Flag */
			ring_vm_setvarprivateflag(pVM, (List *)RING_VM_STACK_READP, pVM->lPrivateFlag);
			RING_VM_STACK_POP;
			ring_vm_findvar(pVM, RING_VM_IR_READC);
			pVM->pActiveMem = pScope;
			pVM->lGetSetProperty = 0;
			pVM->pGetSetObject = NULL;
			return;
		}
		pVM->pActiveMem = pScope;
		pVM->lGetSetProperty = 0;
		pVM->pGetSetObject = NULL;
		if (pVM->lActiveCatch == 0) {
			/*
			**  We check lActiveCatch because we may have error "accessing private attribute'
			**  while we are using ring_vm_findvar
			**  And we are using try catch done in the code
			**  In this case we don't add another error message
			**  So the try catch done can work as expected and avoid the need to another one
			**  Error Message
			*/
			ring_vm_error2(pVM, RING_VM_ERROR_PROPERTYNOTFOUND, RING_VM_IR_READC);
		}
		return;
	}
	pVM->pActiveMem = pScope;
	/* Note: We don't set pVM->pGetSetObject to NULL because it could be used by ICO_DUPLICATE */
	pVM->lGetSetProperty = 0;
}

void ring_vm_oop_loadmethod(VM *pVM) { ring_vm_oop_loadmethod2(pVM, RING_VM_IR_READC); }

void ring_vm_oop_loadmethod2(VM *pVM, const char *cMethod) {
	List *pVar, *pList, *pList2, *pList3, *pSuper;
	unsigned int lResult;
	/* Check calling method related to Parent Class */
	pSuper = ring_vm_oop_getsuperobj(pVM);
	if (pSuper != NULL) {
		ring_vm_oop_loadsuperobjmethod(pVM, pSuper);
		/* Move list from pObjState to pBeforeObjState */
		ring_vm_oop_movetobeforeobjstate(pVM);
		return;
	}
	/* Get Object Data */
	pVar = ring_vm_oop_getobj(pVM);
	if (pVar == NULL) {
		return;
	}
	/* Get Object Class */
	pList = (List *)ring_list_getpointer(pVar, RING_OBJECT_CLASSPOINTER);
	/* Push Class Package */
	ring_vm_oop_pushclasspackage(pVM, pList);
	/* Get Object State */
	pList2 = ring_list_newlist_gc(pVM->pRingState, pVM->pObjState);
	ring_list_addpointer_gc(pVM->pRingState, pList2, ring_list_getlist(pVar, RING_OBJECT_OBJECTDATA));
	/* Get Class Methods */
	pList3 = ring_list_getlist(pList, RING_CLASSMAP_METHODSLIST);
	ring_list_addpointer_gc(pVM->pRingState, pList2, pList3);
	/* Add Pointer to Class */
	ring_list_addpointer_gc(pVM->pRingState, pList2, pList);
	/* Add Logical Value (True) , That we are inside the class method */
	ring_list_addint_gc(pVM->pRingState, pList2, RING_TRUE);
	/* Get Parent Classes Methods */
	ring_vm_oop_parentmethods(pVM, pList);
	/* Call Method */
	pVar = pVM->pFunctionsMap;
	pVM->pFunctionsMap = pList3;
	pVM->lCallMethod = 1;
	lResult = ring_vm_loadfunc2(pVM, cMethod, RING_FALSE);
	pVM->lCallMethod = 0;
	pVM->pFunctionsMap = pVar;
	/* Move list from pObjState to pBeforeObjState */
	if (lResult) {
		ring_vm_oop_movetobeforeobjstate(pVM);
	}
}

void ring_vm_oop_movetobeforeobjstate(VM *pVM) {
	List *pList, *pList2;
	if (pVM->lActiveCatch == 1) {
		/* Try/Catch restore aObjState and may become empty */
		return;
	}
	/* Move list from pObjState to pBeforeObjState */
	pList = ring_list_newlist_gc(pVM->pRingState, pVM->pBeforeObjState);
	pList2 = ring_list_getlist(pVM->pObjState, ring_list_getsize(pVM->pObjState));
	ring_list_copy_gc(pVM->pRingState, pList, pList2);
	ring_list_deleteitem_gc(pVM->pRingState, pVM->pObjState, ring_list_getsize(pVM->pObjState));
}

void ring_vm_oop_parentmethods(VM *pVM, List *pList) {
	const char *cClassName, *cClassName2;
	unsigned int x, nFound, nMark;
	List *pList3, *pList4;
	pList3 = ring_list_getlist(pList, RING_CLASSMAP_METHODSLIST);
	if (ring_list_getint(pList, RING_CLASSMAP_ISPARENTINFO) == RING_FALSE) {
		ring_list_setint_gc(pVM->pRingState, pList, RING_CLASSMAP_ISPARENTINFO, RING_TRUE);
		cClassName = ring_list_getstring(pList, RING_CLASSMAP_PARENTCLASS);
		/* Mark Packages Count */
		nMark = ring_list_getsize(pVM->pActivePackage);
		while (strcmp(cClassName, RING_CSTR_EMPTY) != 0) {
			/* Push Class Package */
			ring_vm_oop_pushclasspackage(pVM, pList);
			nFound = 0;
			for (x = 1; x <= ring_vm_oop_visibleclassescount(pVM); x++) {
				pList4 = ring_vm_oop_visibleclassitem(pVM, x);
				cClassName2 = ring_list_getstring(pList4, RING_CLASSMAP_CLASSNAME);
				/* Prev. Step must be before Next. step - We check the name include the package */
				pList4 = ring_vm_oop_checkpointertoclassinpackage(pVM, pList4);
				if (pList4 == NULL) {
					continue;
				}
				if (strcmp(cClassName, cClassName2) == 0) {
					/* Push Class Package */
					ring_vm_oop_pushclasspackage(pVM, pList4);
					ring_list_copy_gc(pVM->pRingState, pList3,
							  ring_list_getlist(pList4, RING_CLASSMAP_METHODSLIST));
					cClassName = ring_list_getstring(pList4, RING_CLASSMAP_PARENTCLASS);
					nFound = 1;
					break;
				}
			}
			if (nFound == 0) {
				ring_vm_error(pVM, RING_VM_ERROR_PARENTCLASSNOTFOUND);
				break;
			}
			/* Exit when the parent class already contains it's parent classes data */
			if (ring_list_getint(pList, RING_CLASSMAP_ISPARENTINFO) == 1) {
				break;
			}
		}
		/* Restore Mark */
		ring_vm_oop_deletepackagesafter(pVM, nMark);
	}
}

void ring_vm_oop_aftercallmethod(VM *pVM) {
	if (ring_list_getsize(pVM->pObjState) != 0) {
		ring_list_deleteitem_gc(pVM->pRingState, pVM->pObjState, ring_list_getsize(pVM->pObjState));
	}
}

void ring_vm_oop_printobj(VM *pVM, List *pList) { ring_list_printobj(pList, pVM->nDecimals); }

void ring_vm_oop_setbraceobj(VM *pVM, List *pList) {
	/* Support using { } to access object after object name */
	if (ring_vm_oop_isobject(pVM, pList)) {
		pVM->pBraceObject = pList;
	}
}

void ring_vm_oop_bracestart(VM *pVM) {
	List *pList, *pVar, *pStateList;
	Item *pItem;
	unsigned int lShowError;
	/* Check Error */
	lShowError = 0;
	if ((pVM->pBraceObject == NULL) || (!RING_VM_STACK_ISPOINTER)) {
		lShowError = 1;
	} else if (RING_VM_STACK_ISPOINTER) {
		if (RING_VM_STACK_OBJTYPE == RING_OBJTYPE_VARIABLE) {
			pVar = (List *)RING_VM_STACK_READP;
			if (ring_list_islist(pVar, RING_VAR_VALUE)) {
				pList = ring_list_getlist(pVar, RING_VAR_VALUE);
				lShowError = (ring_vm_oop_isobject(pVM, pList) == 0);
			} else {
				lShowError = 1;
			}
		} else if (RING_VM_STACK_OBJTYPE == RING_OBJTYPE_LISTITEM) {
			pItem = (Item *)RING_VM_STACK_READP;
			if (ring_item_islist(pItem)) {
				pList = (List *)ring_item_getlist(pItem);
				lShowError = (ring_vm_oop_isobject(pVM, pList) == 0);
			} else {
				lShowError = 1;
			}
		}
	}
	if (lShowError) {
		/* Disable handling this error using BraceError() Method */
		pVM->lCheckBraceError = 0;
		ring_vm_error(pVM, RING_VM_ERROR_BRACEWITHOUTOBJECT);
		return;
	}
	pStateList = ring_list_newlist_gc(pVM->pRingState, pVM->pObjState);
	ring_vm_savestateforbraces(pVM, pStateList);
	/* Check Don't Ref. and Don't Ref Again */
	ring_list_disabledontref_gc(pVM->pRingState, pList);
	ring_list_disabledontrefagain_gc(pVM->pRingState, pList);
}

void ring_vm_oop_braceend(VM *pVM) {
	List *pList;
	pList = ring_list_getlist(pVM->pBraceObjects, ring_list_getsize(pVM->pBraceObjects));
	ring_vm_restorestateforbraces(pVM, pList);
}

void ring_vm_oop_bracestack(VM *pVM) {
	List *pList;
	pList = ring_list_getlist(pVM->pBraceObjects, ring_list_getsize(pVM->pBraceObjects));
	pVM->nSP = ring_list_getint(pList, RING_BRACEOBJECTS_NSP);
	if (pVM->nFuncSP > pVM->nSP) {
		/*
		**  This fixes a problem when we use oObject {  eval(code) } return cString
		**  Where pVM->nSP maybe less than pVM->nFuncSP while we are inside function
		*/
		if (RING_VM_FUNCCALLSCOUNT > 0) {
			pVM->nSP = pVM->nFuncSP;
		}
	}
}

void ring_vm_oop_newsuperobj(VM *pVM, List *pState, List *pClass) {
	List *pSuper, *pSuper2, *pMethods, *pList;
	const char *cParentClassName, *cClassName;
	unsigned int x;
	pSuper = ring_vm_newvar2(pVM, "super", pState);
	ring_list_setint_gc(pVM->pRingState, pSuper, RING_VAR_TYPE, RING_VM_LIST);
	ring_list_setlist_gc(pVM->pRingState, pSuper, RING_VAR_VALUE);
	pSuper2 = ring_list_getlist(pSuper, RING_VAR_VALUE);
	pMethods = ring_list_getlist(pClass, RING_CLASSMAP_METHODSLIST);
	ring_list_addpointer_gc(pVM->pRingState, pSuper2, pMethods);
	cParentClassName = ring_list_getstring(pClass, RING_CLASSMAP_PARENTCLASS);
	while (strcmp(cParentClassName, RING_CSTR_EMPTY) != 0) {
		for (x = 1; x <= ring_vm_oop_visibleclassescount(pVM); x++) {
			pList = ring_vm_oop_visibleclassitem(pVM, x);
			cClassName = ring_list_getstring(pList, RING_CLASSMAP_CLASSNAME);
			pList = ring_vm_oop_checkpointertoclassinpackage(pVM, pList);
			if (pList == NULL) {
				continue;
			}
			if (strcmp(cClassName, cParentClassName) == 0) {
				cParentClassName = ring_list_getstring(pList, RING_CLASSMAP_PARENTCLASS);
				pMethods = ring_list_getlist(pList, RING_CLASSMAP_METHODSLIST);
				ring_list_addpointer_gc(pVM->pRingState, pSuper2, pMethods);
				break;
			}
		}
	}
	/* Disable deleting the Super list through Ring code */
	ring_list_enableerroronassignment2_gc(pVM->pRingState, pSuper2);
}

List *ring_vm_oop_getsuperobj(VM *pVM) {
	List *pVar;
	if ((RING_VM_STACK_ISPOINTER) && (ring_list_getsize(pVM->pObjState) != 0)) {
		if (RING_VM_STACK_OBJTYPE == RING_OBJTYPE_VARIABLE) {
			pVar = (List *)RING_VM_STACK_READP;
			if ((ring_list_islist(pVar, RING_VAR_VALUE)) &&
			    (strcmp(ring_list_getstring(pVar, RING_VAR_NAME), "super") == 0)) {
				pVar = ring_list_getlist(pVar, RING_VAR_VALUE);
				RING_VM_STACK_POP;
				return pVar;
			}
		}
	}
	return NULL;
}

void ring_vm_oop_loadsuperobjmethod(VM *pVM, List *pSuper) {
	unsigned int x;
	List *pState, *pMethods, *pClass, *pVar, *pList;
	pList = ring_list_getlist(pVM->pObjState, ring_list_getsize(pVM->pObjState));
	pState = ring_list_getlist(pList, RING_OBJSTATE_SCOPE);
	pMethods = ring_list_getlist(pList, RING_OBJSTATE_METHODS);
	pClass = ring_list_getlist(pList, RING_OBJSTATE_CLASS);
	for (x = 1; x <= ring_list_getsize(pSuper); x++) {
		if (ring_list_getpointer(pSuper, x) == pMethods) {
			if ((x + 1) <= ring_list_getsize(pSuper)) {
				pMethods = (List *)ring_list_getpointer(pSuper, (x + 1));
				break;
			} else {
				ring_vm_error(pVM, RING_VM_ERROR_SUPERCLASSNOTFOUND);
				return;
			}
		}
	}
	pList = ring_list_newlist_gc(pVM->pRingState, pVM->pObjState);
	ring_list_addpointer_gc(pVM->pRingState, pList, pState);
	ring_list_addpointer_gc(pVM->pRingState, pList, pMethods);
	ring_list_addpointer_gc(pVM->pRingState, pList, pClass);
	/* Call Method */
	pVar = pVM->pFunctionsMap;
	pVM->pFunctionsMap = pMethods;
	pVM->lCallMethod = 1;
	ring_vm_loadfunc(pVM);
	pVM->lCallMethod = 0;
	pVM->pFunctionsMap = pVar;
}

void ring_vm_oop_import(VM *pVM) { ring_vm_oop_import2(pVM, RING_VM_IR_READC); }

void ring_vm_oop_import2(VM *pVM, const char *cPackage) {
	unsigned int x;
	List *pList, *pList2;
	const char *cPackage2;
	for (x = 1; x <= ring_list_getsize(pVM->pPackagesMap); x++) {
		pList = ring_list_getlist(pVM->pPackagesMap, x);
		cPackage2 = ring_list_getstring(pList, RING_PACKAGES_PACKAGENAME);
		if (strcmp(cPackage, cPackage2) == 0) {
			/* Get Package Classes */
			pList2 = ring_list_getlist(pList, RING_PACKAGES_CLASSESLIST);
			ring_vm_oop_import3(pVM, pList2);
			/* Set Active Package Name */
			ring_string_set_gc(pVM->pRingState, pVM->pPackageName, cPackage);
			return;
		}
	}
	/* Error Message */
	ring_vm_error2(pVM, RING_VM_ERROR_PACKAGENOTFOUND, cPackage);
}

void ring_vm_oop_import3(VM *pVM, List *pList) {
	unsigned int x;
	List *pList2, *pList3;
	/* Import Package Classes */
	for (x = 1; x <= ring_list_getsize(pList); x++) {
		pList2 = ring_list_getlist(pList, x);
		pList3 = ring_list_newlist_gc(pVM->pRingState, pVM->pClassesMap);
		ring_list_addstring_gc(pVM->pRingState, pList3, ring_list_getstring(pList2, RING_CLASSMAP_CLASSNAME));
		ring_list_addpointer_gc(pVM->pRingState, pList3, pList2);
		ring_list_addpointer_gc(pVM->pRingState, pList3, (void *)pVM->cFileName);
	}
}

List *ring_vm_oop_checkpointertoclassinpackage(VM *pVM, List *pList) {
	if (ring_list_getsize(pList) == RING_CLASSMAP_IMPORTEDCLASSSIZE) {
		if (ring_list_getpointer(pList, RING_CLASSMAP_POINTERTOFILENAME) != NULL) {
			if (strcmp((char *)ring_list_getpointer(pList, RING_CLASSMAP_POINTERTOFILENAME),
				   pVM->cFileName) != 0) {
				return NULL;
			}
		}
		return (List *)ring_list_getpointer(pList, RING_CLASSMAP_POINTERTOLISTOFCLASSINSIDEPACKAGE);
	}
	return pList;
}

unsigned int ring_vm_oop_visibleclassescount(VM *pVM) {
	List *pList;
	if (ring_list_getsize(pVM->pActivePackage) > 0) {
		pList = (List *)ring_list_getpointer(pVM->pActivePackage, ring_list_getsize(pVM->pActivePackage));
		pList = ring_list_getlist(pList, RING_PACKAGES_CLASSESLIST);
		return ring_list_getsize(pVM->pClassesMap) + ring_list_getsize(pList);
	}
	return ring_list_getsize(pVM->pClassesMap);
}

List *ring_vm_oop_visibleclassitem(VM *pVM, unsigned int nIndex) {
	List *pList;
	if (nIndex <= ring_list_getsize(pVM->pClassesMap)) {
		return ring_list_getlist(pVM->pClassesMap, nIndex);
	}
	pList = (List *)ring_list_getpointer(pVM->pActivePackage, ring_list_getsize(pVM->pActivePackage));
	pList = ring_list_getlist(pList, RING_PACKAGES_CLASSESLIST);
	pList = ring_list_getlist(pList, nIndex - ring_list_getsize(pVM->pClassesMap));
	return pList;
}

void ring_vm_oop_pushclasspackage(VM *pVM, List *pList) {
	List *pList2;
	pList2 = ring_list_getlist(pList, RING_CLASSMAP_POINTERTOPACKAGE);
	if (pList2 != NULL) {
		ring_list_addpointer_gc(pVM->pRingState, pVM->pActivePackage, pList2);
	}
}

void ring_vm_oop_deletepackagesafter(VM *pVM, unsigned int nIndex) {
	unsigned int t;
	if (nIndex <= ring_list_getsize(pVM->pActivePackage)) {
		for (t = ring_list_getsize(pVM->pActivePackage); t > nIndex; t--) {
			ring_list_deleteitem_gc(pVM->pRingState, pVM->pActivePackage, t);
		}
	}
}

unsigned int ring_vm_oop_callmethodinsideclass(VM *pVM) {
	List *pList, *pList2;
	FuncCall *pFuncCall;
	unsigned int x;
	/*
	**  This function tell us if we are inside Class method during runtime or not
	**  pObjState is used when we Call Method or We use braces { } to access object
	**  if the size of pObjState List is 4 , then it's class method execution not brace
	**  Braces can be used before calling class methods
	**  Also braces can be used inside class methods to access objects
	**  Inside class method you can access any object using { } , you can access the self object
	**  Braces & Methods calls can be nested
	**  Check Calling from function
	*/
	if (RING_VM_FUNCCALLSCOUNT > 0) {
		for (x = RING_VM_FUNCCALLSCOUNT; x >= 1; x--) {
			pFuncCall = RING_VM_GETFUNCCALL(x);
			/* Be sure that the function is already called using ICO_CALL */
			if (pFuncCall->nCallerPC != 0) {
				if (pFuncCall->lMethod == 0) {
					return RING_FALSE;
				} else {
					break;
				}
			}
		}
	}
	/*
	**  pObjState can know about method call if it's called using callmethod
	**  Or it's called from inside { } as function
	**  Return 1 if last item is a method
	*/
	if (ring_list_getsize(pVM->pObjState) >= 1) {
		pList = ring_list_getlist(pVM->pObjState, ring_list_getsize(pVM->pObjState));
		if ((ring_list_getsize(pList) == 4) && (pVM->lCallMethod == 0)) {
			return RING_TRUE;
		}
	}
	/* Check using braces { } to access object from a method in the Class */
	if (ring_list_getsize(pVM->pObjState) >= 2) {
		pList = (List *)ring_list_getpointer(
		    ring_list_getlist(pVM->pObjState, ring_list_getsize(pVM->pObjState)), RING_OBJSTATE_CLASS);
		for (x = ring_list_getsize(pVM->pObjState) - 1; x >= 1; x--) {
			pList2 = ring_list_getlist(pVM->pObjState, x);
			if (ring_list_getsize(pList2) == 4) {
				pList2 = (List *)ring_list_getpointer(pList2, RING_OBJSTATE_CLASS);
				if (pList == pList2) {
					return RING_TRUE;
				} else {
					break;
				}
			}
		}
	}
	return RING_FALSE;
}

void ring_vm_oop_setget(VM *pVM, List *pVar) {
	List *pList, *pList2;
	Item *pGetSetItem;
	String *pString, *pString2;
	RING_VM_IR_ITEMTYPE *pItem;
	unsigned int nIns;
	RING_VM_BYTECODE_START;
	/* Check Setter & Getter for Public Attributes */
	RING_VM_IR_LOAD;
	if (RING_VM_IR_OPCODE != ICO_ASSIGNMENTPOINTER) {
		RING_VM_IR_UNLOAD;
		/*
		**  Get Property
		**  Check to do a Stack POP for the Attribute List
		*/
		pString2 = ring_string_new_gc(pVM->pRingState, "get");
		ring_string_add_gc(pVM->pRingState, pString2, ring_list_getstring(pVar, RING_VAR_NAME));
		/* Check Type */
		pList2 = NULL;
		if (pVM->nGetSetObjType == RING_OBJTYPE_VARIABLE) {
			pList2 = ring_list_getlist((List *)(pVM->pGetSetObject), RING_VAR_VALUE);
		} else if (pVM->nGetSetObjType == RING_OBJTYPE_LISTITEM) {
			pGetSetItem = (Item *)pVM->pGetSetObject;
			pList2 = ring_item_getlist(pGetSetItem);
		}
		if (ring_vm_oop_ismethod(pVM, pList2, ring_string_get(pString2))) {
			/* Create String */
			pString = ring_string_new_gc(pVM->pRingState, "get");
			ring_string_add_gc(pVM->pRingState, pString, ring_list_getstring(pVar, RING_VAR_NAME));
			/* Set Variable ring_gettemp_var */
			pList = ring_list_getlist(pVM->pDefinedGlobals, RING_GLOBALVARPOS_GETTEMPVAR);
			ring_list_setpointer_gc(pVM->pRingState, pList, RING_VAR_VALUE, pVM->pGetSetObject);
			ring_list_setint_gc(pVM->pRingState, pList, RING_VAR_PVALUETYPE, pVM->nGetSetObjType);
			RING_VM_STACK_POP;
			if (pVM->lGetSetProperty == 0) {
				/* For Better Performance : Don't Eval() when we call Getter Method from Braces */
				ring_vm_loadfunc2(pVM, ring_string_get(pString2), RING_FALSE);
				ring_vm_call2(pVM);
				/* Prepare the Object State */
				ring_vm_oop_preparecallmethodfrombrace(pVM);
				ring_string_delete_gc(pVM->pRingState, pString2);
				ring_string_delete_gc(pVM->pRingState, pString);
				return;
			}
			if (RING_VM_IR_READIVALUE(RING_VM_IR_REG2) == 0) {
				nIns = pVM->nPC - 2;
				/* Create the Byte Code */
				RING_VM_BYTECODE_INSSTR(ICO_LOADADDRESS, RING_CSTR_GETTEMPVAR);
				RING_VM_BYTECODE_INSSTR(ICO_LOADMETHOD, ring_string_get(pString));
				RING_VM_BYTECODE_INSINTINT(ICO_CALL, RING_ZERO, RING_ONE);
				RING_VM_BYTECODE_INS(ICO_AFTERCALLMETHOD);
				RING_VM_BYTECODE_INS(ICO_PUSHV);
				RING_VM_BYTECODE_INS(ICO_RETURN);
				/* Use the Byte Code */
				RING_VM_BYTECODE_END;
				/* Note: Reallocation may change mem. locations */
				pItem = RING_VM_IR_ITEMATINS(nIns, RING_VM_IR_REG2);
				RING_VM_IR_ITEMSETINT(pItem, pVM->nPC);
			} else {
				ring_vm_blockflag2(pVM, pVM->nPC);
				pVM->nPC = RING_VM_IR_READIVALUE(RING_VM_IR_REG2);
			}
			/* Delete String */
			ring_string_delete_gc(pVM->pRingState, pString);
		}
		ring_string_delete_gc(pVM->pRingState, pString2);
	} else {
		RING_VM_IR_UNLOAD;
		/*
		**  Set Property
		**  Delete All Items to avoid a memory leak in real time applications
		*/
		ring_list_deleteallitems_gc(pVM->pRingState, pVM->pSetProperty);
		pList = ring_list_newlist_gc(pVM->pRingState, pVM->pSetProperty);
		/* Add object pointer & Type */
		ring_list_addpointer_gc(pVM->pRingState, pList, pVM->pGetSetObject);
		ring_list_addint_gc(pVM->pRingState, pList, pVM->nGetSetObjType);
		/* Add property name */
		ring_list_addstring_gc(pVM->pRingState, pList, ring_list_getstring(pVar, RING_VAR_NAME));
		/* Property Variable */
		ring_list_addpointer_gc(pVM->pRingState, pList, pVar);
		/*
		**  Check if we don't have the Setter Method
		**  We do this to enable the Assignment Pointer and Disable Set Property for Lists and Objects
		*/
		pString2 = ring_string_new_gc(pVM->pRingState, "set");
		ring_string_add_gc(pVM->pRingState, pString2, ring_list_getstring(pVar, RING_VAR_NAME));
		/* Check Type */
		pList2 = NULL;
		if (pVM->nGetSetObjType == RING_OBJTYPE_VARIABLE) {
			pList2 = ring_list_getlist((List *)(pVM->pGetSetObject), RING_VAR_VALUE);
		} else if (pVM->nGetSetObjType == RING_OBJTYPE_LISTITEM) {
			pGetSetItem = (Item *)pVM->pGetSetObject;
			pList2 = ring_item_getlist(pGetSetItem);
		}
		pVM->nNoSetterMethod = RING_NOSETTERMETHOD_DEFAULT;
		if (!ring_vm_oop_ismethod(pVM, pList2, ring_string_get(pString2))) {
			pVM->nNoSetterMethod = RING_NOSETTERMETHOD_ENABLE;
		}
		ring_string_delete_gc(pVM->pRingState, pString2);
	}
}

void ring_vm_oop_setproperty(VM *pVM) {
	List *pList, *pList2;
	Item *pGetSetItem;
	String *pString, *pString2;
	unsigned int nIns;
	RING_VM_BYTECODE_START;
	/* If we don't have a setter method and we have a new list or new object */
	if (pVM->nNoSetterMethod == RING_NOSETTERMETHOD_IGNORESETPROPERTY) {
		pVM->nNoSetterMethod = RING_NOSETTERMETHOD_DEFAULT;
		return;
	}
	/* To Access Property Data */
	if (ring_list_getsize(pVM->pSetProperty) < 1) {
		/* This case happens when using This.Attribute inside nested braces in a class method */
		ring_vm_assignment(pVM);
		return;
	}
	pList = ring_list_getlist(pVM->pSetProperty, ring_list_getsize(pVM->pSetProperty));
	/* Add Before Equal Flag */
	if (ring_list_getsize(pList) == 4) {
		ring_list_addint_gc(pVM->pRingState, pList, pVM->nBeforeEqual);
	}
	/* Before (First Time) */
	if (RING_VM_IR_GETFLAGREG2 == RING_FALSE) {
		/* Set Before/After SetProperty Flag to After */
		RING_VM_IR_SETFLAGREG2(RING_TRUE);
		/* Set Variable ring_gettemp_var */
		pList2 = ring_list_getlist(pVM->pDefinedGlobals, RING_GLOBALVARPOS_GETTEMPVAR);
		ring_list_setpointer_gc(pVM->pRingState, pList2, RING_VAR_VALUE,
					ring_list_getpointer(pList, RING_SETPROPERTY_OBJPTR));
		ring_list_setint_gc(pVM->pRingState, pList2, RING_VAR_PVALUETYPE,
				    ring_list_getint(pList, RING_SETPROPERTY_OBJTYPE));
		/* Set Variable ring_settemp_var */
		pList2 = ring_list_getlist(pVM->pDefinedGlobals, RING_GLOBALVARPOS_SETTEMPVAR);
		if (RING_VM_STACK_ISNUMBER) {
			ring_list_setint_gc(pVM->pRingState, pList2, RING_VAR_TYPE, RING_VM_NUMBER);
			ring_list_setdouble_gc(pVM->pRingState, pList2, RING_VAR_VALUE, RING_VM_STACK_READN);
			ring_list_adddouble_gc(pVM->pRingState, pList, RING_VM_STACK_READN);
		} else if (RING_VM_STACK_ISSTRING) {
			ring_list_setint_gc(pVM->pRingState, pList2, RING_VAR_TYPE, RING_VM_STRING);
			ring_list_setstring2_gc(pVM->pRingState, pList2, RING_VAR_VALUE, RING_VM_STACK_READC,
						RING_VM_STACK_STRINGSIZE);
			ring_list_addstring2_gc(pVM->pRingState, pList, RING_VM_STACK_READC, RING_VM_STACK_STRINGSIZE);
		} else if (RING_VM_STACK_ISPOINTER) {
			ring_list_setint_gc(pVM->pRingState, pList2, RING_VAR_TYPE, RING_VM_POINTER);
			ring_list_setpointer_gc(pVM->pRingState, pList2, RING_VAR_VALUE, RING_VM_STACK_READP);
			ring_list_setint_gc(pVM->pRingState, pList2, RING_VAR_PVALUETYPE, RING_VM_STACK_OBJTYPE);
			ring_list_addpointer_gc(pVM->pRingState, pList, RING_VM_STACK_READP);
			ring_list_addint_gc(pVM->pRingState, pList, RING_VM_STACK_OBJTYPE);
		}
		/* Set the Flag Register to Zero */
		RING_VM_IR_SETFLAGREG(0);
		/* Check if the Setter method exist */
		pString2 = ring_string_new_gc(pVM->pRingState, "set");
		ring_string_add_gc(pVM->pRingState, pString2, ring_list_getstring(pList, RING_SETPROPERTY_ATTRNAME));
		/* Check Type */
		pList2 = NULL;
		if (ring_list_getint(pList, RING_SETPROPERTY_OBJTYPE) == RING_OBJTYPE_VARIABLE) {
			pList2 = ring_list_getlist((List *)(ring_list_getpointer(pList, RING_SETPROPERTY_OBJPTR)),
						   RING_VAR_VALUE);
		} else if (ring_list_getint(pList, RING_SETPROPERTY_OBJTYPE) == RING_OBJTYPE_LISTITEM) {
			pGetSetItem = (Item *)ring_list_getpointer(pList, RING_SETPROPERTY_OBJPTR);
			pList2 = ring_item_getlist(pGetSetItem);
		}
		/* The Flag Reg size is 1 bit while ring_vm_oop_ismethod could return 0, 1 or 2 */
		RING_VM_IR_SETFLAGREG(ring_vm_oop_ismethod(pVM, pList2, ring_string_get(pString2)) !=
				      RING_ISMETHOD_NOTFOUND);
		ring_string_delete_gc(pVM->pRingState, pString2);
		/* Execute the same instruction again (next time the part "After (Second Time)" will run ) */
		pVM->nPC--;
		if (RING_VM_IR_GETFLAGREG) {
			if (RING_VM_IR_GETINTREG == RING_ZERO) {
				/* Create String */
				pString = ring_string_new_gc(pVM->pRingState, "set");
				ring_string_add_gc(pVM->pRingState, pString,
						   ring_list_getstring(pList, RING_SETPROPERTY_ATTRNAME));
				/*
				**  Get Instruction Position
				**  We use -1 instead of -2 because we already used pVM->nPC--
				*/
				nIns = pVM->nPC - 1;
				/* Create the Byte Code */
				RING_VM_BYTECODE_INSSTR(ICO_LOADADDRESS, RING_CSTR_GETTEMPVAR);
				RING_VM_BYTECODE_INSSTR(ICO_LOADMETHOD, ring_string_get(pString));
				RING_VM_BYTECODE_INSSTR(ICO_LOADAPUSHV, RING_CSTR_SETTEMPVAR);
				RING_VM_BYTECODE_INSINTINT(ICO_CALL, RING_ZERO, RING_ONE);
				RING_VM_BYTECODE_INS(ICO_AFTERCALLMETHOD);
				RING_VM_BYTECODE_INS(ICO_PUSHV);
				RING_VM_BYTECODE_INS(ICO_RETURN);
				/* Use the Byte Code */
				RING_VM_BYTECODE_END;
				/* Note: Reallocation may change mem. locations */
				RING_VM_IR_SETINTREGATINS(nIns, pVM->nPC);
				/* Delete String */
				ring_string_delete_gc(pVM->pRingState, pString);
			} else {
				ring_vm_blockflag2(pVM, pVM->nPC);
				pVM->nPC = RING_VM_IR_GETINTREG;
			}
		}
	}
	/* After (Second Time) */
	else {
		/* Set Before/After SetProperty Flag to Before */
		RING_VM_IR_SETFLAGREG2(RING_FALSE);
		if (!RING_VM_IR_GETFLAGREG) {
			/*
			**  The set method is not found!, we have to do the assignment operation
			**  Push Variable Then Push Value then Assignment
			*/
			RING_VM_STACK_PUSHPVALUE(ring_list_getpointer(pList, RING_SETPROPERTY_ATTRVAR));
			RING_VM_STACK_OBJTYPE = RING_OBJTYPE_VARIABLE;
			/* Restore Before Equal Flag */
			pVM->nBeforeEqual = ring_list_getint(pList, RING_SETPROPERTY_NBEFOREEQUAL);
			/* Push Value */
			if (ring_list_isdouble(pList, RING_SETPROPERTY_VALUE)) {
				RING_VM_STACK_PUSHNVALUE(ring_list_getdouble(pList, RING_SETPROPERTY_VALUE));
			} else if (ring_list_isstring(pList, RING_SETPROPERTY_VALUE)) {
				RING_VM_STACK_PUSHCVALUE2(ring_list_getstring(pList, RING_SETPROPERTY_VALUE),
							  ring_list_getstringsize(pList, RING_SETPROPERTY_VALUE));
			} else if (ring_list_ispointer(pList, RING_SETPROPERTY_VALUE)) {
				RING_VM_STACK_PUSHPVALUE(ring_list_getpointer(pList, RING_SETPROPERTY_VALUE));
				RING_VM_STACK_OBJTYPE = ring_list_getint(pList, RING_SETPROPERTY_VALUEOBJTYPE);
			}
			ring_vm_assignment(pVM);
		}
		ring_list_deleteitem_gc(pVM->pRingState, pVM->pSetProperty, ring_list_getsize(pVM->pSetProperty));
	}
}

List *ring_vm_oop_objvarfromobjlist(VM *pVM, List *pList) {
	unsigned int nType;
	Item *pItem;
	/* Get Object State List */
	pList = ring_list_getlist(pList, RING_OBJECT_OBJECTDATA);
	/* Get Self Attribute List */
	pList = ring_list_getlist(pList, RING_OBJECT_SELFATTRIBUTE);
	/* Get Object Pointer from Self Attribute List */
	nType = ring_list_getint(pList, RING_VAR_PVALUETYPE);
	if (nType == RING_OBJTYPE_VARIABLE) {
		pList = (List *)ring_list_getpointer(pList, RING_VAR_VALUE);
	} else if (nType == RING_OBJTYPE_LISTITEM) {
		pItem = (Item *)ring_list_getpointer(pList, RING_VAR_VALUE);
		pList = (List *)ring_item_getlist(pItem);
	}
	return pList;
}

unsigned int ring_vm_oop_objtypefromobjlist(VM *pVM, List *pList) {
	unsigned int nType;
	/* Get Object State List */
	pList = ring_list_getlist(pList, RING_OBJECT_OBJECTDATA);
	/* Get Self Attribute List */
	pList = ring_list_getlist(pList, RING_OBJECT_SELFATTRIBUTE);
	/* Get Object Type from Self Attribute List */
	nType = ring_list_getint(pList, RING_VAR_PVALUETYPE);
	return nType;
}

Item *ring_vm_oop_objitemfromobjlist(VM *pVM, List *pList) {
	Item *pItem;
	/* Get Object State List */
	pList = ring_list_getlist(pList, RING_OBJECT_OBJECTDATA);
	/* Get Self Attribute List */
	pList = ring_list_getlist(pList, RING_OBJECT_SELFATTRIBUTE);
	/* Get Object Pointer from Self Attribute List */
	pItem = (Item *)ring_list_getpointer(pList, RING_VAR_VALUE);
	return pItem;
}

void ring_vm_oop_operatoroverloading(VM *pVM, List *pObj, const char *cStr1, unsigned int nType, const char *cStr2,
				     double nNum1, void *pPointer, unsigned int nPointerType) {
	RING_VM_STACK_POP;
	/* Check Method */
	if (!ring_vm_oop_ismethod(pVM, pObj, RING_CSTR_OPERATOR)) {
		ring_vm_error(pVM, RING_VM_ERROR_NOOPERATORMETHOD);
		return;
	}
	ring_vm_oop_operatoroverloading2(pVM, pObj, cStr1, nType, cStr2, nNum1, pPointer, nPointerType);
}

void ring_vm_oop_operatoroverloading2(VM *pVM, List *pObj, const char *cStr1, unsigned int nType, const char *cStr2,
				      double nNum1, void *pPointer, unsigned int nPointerType) {
	List *pList2;
	Item *pItem;
	unsigned int nObjType, nIns;
	RING_VM_BYTECODE_START;
	nObjType = ring_vm_oop_objtypefromobjlist(pVM, pObj);
	/* Set Variable ring_gettemp_var */
	pList2 = ring_list_getlist(pVM->pDefinedGlobals, RING_GLOBALVARPOS_GETTEMPVAR);
	if (nObjType == RING_OBJTYPE_VARIABLE) {
		pObj = ring_vm_oop_objvarfromobjlist(pVM, pObj);
		ring_list_setpointer_gc(pVM->pRingState, pList2, RING_VAR_VALUE, pObj);
	} else if (nObjType == RING_OBJTYPE_LISTITEM) {
		pItem = ring_vm_oop_objitemfromobjlist(pVM, pObj);
		ring_list_setpointer_gc(pVM->pRingState, pList2, RING_VAR_VALUE, pItem);
	}
	ring_list_setint_gc(pVM->pRingState, pList2, RING_VAR_PVALUETYPE, nObjType);
	/* Set Variable ring_settemp_var */
	pList2 = ring_list_getlist(pVM->pDefinedGlobals, RING_GLOBALVARPOS_SETTEMPVAR);
	if (nType == RING_OOPARA_STRING) {
		ring_list_setint_gc(pVM->pRingState, pList2, RING_VAR_TYPE, RING_VM_STRING);
		ring_list_setstring_gc(pVM->pRingState, pList2, RING_VAR_VALUE, cStr2);
	} else if (nType == RING_OOPARA_NUMBER) {
		ring_list_setint_gc(pVM->pRingState, pList2, RING_VAR_TYPE, RING_VM_NUMBER);
		ring_list_setdouble_gc(pVM->pRingState, pList2, RING_VAR_VALUE, nNum1);
	} else if (nType == RING_OOPARA_POINTER) {
		ring_list_setint_gc(pVM->pRingState, pList2, RING_VAR_TYPE, RING_VM_POINTER);
		ring_list_setpointer_gc(pVM->pRingState, pList2, RING_VAR_VALUE, pPointer);
		ring_list_setint_gc(pVM->pRingState, pList2, RING_VAR_PVALUETYPE, nPointerType);
	}
	/* Get instruction position */
	nIns = pVM->nPC - 2;
	if (RING_VM_IR_INTATINS(nIns) == 0) {
		/* Create the Byte Code */
		RING_VM_BYTECODE_INSSTR(ICO_LOADADDRESS, RING_CSTR_GETTEMPVAR);
		RING_VM_BYTECODE_INSSTR(ICO_LOADMETHOD, RING_CSTR_OPERATOR);
		RING_VM_BYTECODE_INSSTR(ICO_PUSHC, cStr1);
		RING_VM_BYTECODE_INSSTR(ICO_LOADAPUSHV, RING_CSTR_SETTEMPVAR);
		RING_VM_BYTECODE_INSINTINT(ICO_CALL, RING_ZERO, RING_ONE);
		RING_VM_BYTECODE_INS(ICO_AFTERCALLMETHOD);
		RING_VM_BYTECODE_INS(ICO_PUSHV);
		RING_VM_BYTECODE_INS(ICO_RETURN);
		/* Use the Byte Code */
		RING_VM_BYTECODE_END;
		/* Note: Reallocation may change mem. locations */
		RING_VM_IR_INTATINS(nIns) = pVM->nPC;
	} else {
		ring_vm_blockflag2(pVM, pVM->nPC);
		pVM->nPC = RING_VM_IR_INTATINS(nIns);
	}
}

void ring_vm_oop_callmethodfrombrace(VM *pVM) {
	/*
	**  We uses AfterCallMethod2 instead of AfterCallMethod to avoid conflict with normal method call
	**  AfterCallMethod2 is the same instruction as AfterCallMethod
	**  Change NOOP to After Call Method2
	*/
	RING_VM_IR_LOAD;
	if ((RING_VM_IR_OPCODE == ICO_NOOP) || (RING_VM_IR_OPCODE == ICO_AFTERCALLMETHOD2)) {
		RING_VM_IR_OPCODE = ICO_AFTERCALLMETHOD2;
		ring_vm_oop_preparecallmethodfrombrace(pVM);
	}
	RING_VM_IR_UNLOAD;
}

void ring_vm_oop_preparecallmethodfrombrace(VM *pVM) {
	List *pList, *pList2;
	FuncCall *pFuncCall;
	const char *cStr;
	pList = ring_list_getlist(pVM->pObjState, ring_list_getsize(pVM->pObjState));
	/* Pass Brace when we call class init , using new object() */
	if ((ring_list_getsize(pVM->pObjState) > 1) && (pVM->nCallClassInit)) {
		if (RING_VM_FUNCCALLSCOUNT > 0) {
			pFuncCall = RING_VM_LASTFUNCCALL;
			cStr = pFuncCall->cName;
			if (strcmp(cStr, RING_CSTR_INIT) != 0) {
				pList = ring_list_getlist(pVM->pObjState, ring_list_getsize(pVM->pObjState) - 1);
			}
		} else {
			pList = ring_list_getlist(pVM->pObjState, ring_list_getsize(pVM->pObjState) - 1);
		}
	}
	pList2 = ring_list_newlist_gc(pVM->pRingState, pVM->pObjState);
	ring_list_copy_gc(pVM->pRingState, pList2, pList);
	/* Add Logical Value (True) , That we are inside the class method */
	ring_list_addint_gc(pVM->pRingState, pList2, RING_TRUE);
	/* Push Class Package */
	pList = (List *)ring_list_getpointer(pList2, RING_OBJSTATE_CLASS);
	ring_vm_oop_pushclasspackage(pVM, pList);
}

unsigned int ring_vm_oop_isattribute(VM *pVM, List *pList, const char *cStr) {
	unsigned int x;
	pList = ring_list_getlist(pList, RING_OBJECT_OBJECTDATA);
	for (x = RING_OBJECT_ISATTRIBUTESEARCHSTART; x <= ring_list_getsize(pList); x++) {
		if (strcmp(cStr, ring_list_getstring(ring_list_getlist(pList, x), RING_VAR_NAME)) == RING_ZERO) {
			return RING_TRUE;
		}
	}
	return RING_FALSE;
}

unsigned int ring_vm_oop_ismethod(VM *pVM, List *pList, const char *cStr) {
	List *pList2, *pList3;
	unsigned int x;
	/* Get Object Class */
	pList = (List *)ring_list_getpointer(pList, RING_OBJECT_CLASSPOINTER);
	/* Get Class Methods */
	pList2 = ring_list_getlist(pList, RING_CLASSMAP_METHODSLIST);
	/* Get Parent Classes Methods */
	ring_vm_oop_parentmethods(pVM, pList);
	/* Find the Method */
	if (ring_list_getsize(pList2) > 0) {
		for (x = 1; x <= ring_list_getsize(pList2); x++) {
			pList3 = ring_list_getlist(pList2, x);
			if (strcmp(ring_list_getstring(pList3, RING_FUNCMAP_NAME), cStr) == 0) {
				if (ring_list_getint(pList3, RING_FUNCMAP_PRIVATEFLAG)) {
					return RING_ISMETHOD_PRIVATEMETHOD;
				}
				return RING_ISMETHOD_PUBLICMETHOD;
			}
		}
	}
	return RING_ISMETHOD_NOTFOUND;
}

void ring_vm_oop_updateselfpointer(VM *pVM, List *pObj, unsigned int nType, void *pContainer) {
	List *pList;
	/* Get Object State */
	pList = ring_list_getlist(pObj, RING_OBJECT_OBJECTDATA);
	/* Get Self Attribute */
	pList = ring_list_getlist(pList, RING_OBJECT_SELFATTRIBUTE);
	/* Set Object Pointer */
	ring_list_setpointer_gc(pVM->pRingState, pList, RING_VAR_VALUE, pContainer);
	/* Set Object Type */
	ring_list_setint_gc(pVM->pRingState, pList, RING_VAR_PVALUETYPE, nType);
}

void ring_vm_oop_setthethisvariable(VM *pVM) {
	List *pList, *pThis;
	pThis = ring_list_getlist(pVM->pDefinedGlobals, RING_GLOBALVARPOS_THIS);
	if ((ring_list_getsize(pVM->pObjState) < 1) || (ring_vm_oop_callmethodinsideclass(pVM) == 0)) {
		ring_list_setpointer_gc(pVM->pRingState, pThis, RING_VAR_VALUE, NULL);
		ring_list_setint_gc(pVM->pRingState, pThis, RING_VAR_PVALUETYPE, RING_OBJTYPE_NOTYPE);
		return;
	}
	pList = ring_list_getlist(pVM->pObjState, ring_list_getsize(pVM->pObjState));
	/* Get Object Scope */
	pList = ring_list_getlist(pList, RING_OBJSTATE_SCOPE);
	if (pList == NULL) {
		ring_list_setpointer_gc(pVM->pRingState, pThis, RING_VAR_VALUE, NULL);
		ring_list_setint_gc(pVM->pRingState, pThis, RING_VAR_PVALUETYPE, RING_OBJTYPE_NOTYPE);
		return;
	}
	/* Get Self Attribute List */
	pList = ring_list_getlist(pList, RING_OBJECT_SELFATTRIBUTE);
	/* Save this */
	ring_list_setpointer_gc(pVM->pRingState, pThis, RING_VAR_VALUE, ring_list_getpointer(pList, RING_VAR_VALUE));
	ring_list_setint_gc(pVM->pRingState, pThis, RING_VAR_PVALUETYPE, ring_list_getint(pList, RING_VAR_PVALUETYPE));
}

void ring_vm_oop_setthethisvariableinclassregion(VM *pVM) {
	List *pList, *pThis;
	pThis = ring_list_getlist(pVM->pDefinedGlobals, RING_GLOBALVARPOS_THIS);
	pList = ring_list_getlist(pVM->pObjState, ring_list_getsize(pVM->pObjState));
	/* Get Object Scope */
	pList = ring_list_getlist(pList, RING_OBJSTATE_SCOPE);
	/* Get Self Attribute List */
	pList = ring_list_getlist(pList, RING_OBJECT_SELFATTRIBUTE);
	/* Save this */
	ring_list_setpointer_gc(pVM->pRingState, pThis, RING_VAR_VALUE, ring_list_getpointer(pList, RING_VAR_VALUE));
	/* Create the Temp Variable for the new object */
	ring_list_setint_gc(pVM->pRingState, pThis, RING_VAR_PVALUETYPE, ring_list_getint(pList, RING_VAR_PVALUETYPE));
}

unsigned int ring_vm_oop_callingclassmethodfromclassregion(VM *pVM, List *pMethods) {
	List *pList, *pThis, *pClass, *pVar;
	Item *pItem;
	if (pVM->nInClassRegion != 0) {
		pThis = ring_list_getlist(pVM->pDefinedGlobals, RING_GLOBALVARPOS_THIS);
		if (pThis == NULL) {
			return RING_FALSE;
		}
		if (ring_list_getint(pThis, RING_VAR_PVALUETYPE) == RING_OBJTYPE_VARIABLE) {
			pVar = (List *)ring_list_getpointer(pThis, RING_VAR_VALUE);
			pList = ring_list_getlist(pVar, RING_VAR_VALUE);
		} else if (ring_list_getint(pThis, RING_VAR_PVALUETYPE) == RING_OBJTYPE_LISTITEM) {
			pItem = (Item *)ring_list_getpointer(pThis, RING_VAR_VALUE);
			if (pItem == NULL) {
				return RING_FALSE;
			}
			pList = ring_item_getlist(pItem);
		} else {
			return RING_FALSE;
		}
		if (pList == NULL) {
			return RING_FALSE;
		}
		/* Get Object Class */
		pClass = (List *)ring_list_getpointer(pList, RING_OBJECT_CLASSPOINTER);
		if (pClass == NULL) {
			return RING_FALSE;
		}
		if (ring_list_getsize(pClass) == 0) {
			return RING_FALSE;
		}
		/* Get Class Methods */
		pList = ring_list_getlist(pClass, RING_CLASSMAP_METHODSLIST);
		if (pList == pMethods) {
			return RING_TRUE;
		}
	}
	return RING_FALSE;
}

void ring_vm_oop_callclassinit(VM *pVM) {
	if (RING_VM_IR_READIVALUE(RING_VM_IR_REG1)) {
		pVM->nCallClassInit++;
	} else {
		pVM->nCallClassInit--;
	}
}

void ring_vm_oop_checkbracemethod(VM *pVM) {
	List *pList;
	unsigned int lResult;
	pList = ring_list_getlist(pVM->pBraceObjects, ring_list_getsize(pVM->pBraceObjects));
	lResult = ring_vm_oop_ismethod(pVM, ring_list_getlist(pList, RING_BRACEOBJECTS_BRACEOBJECT), RING_VM_IR_READC);
	RING_VM_STACK_PUSHNVALUE(lResult);
}

unsigned int ring_vm_oop_addattribute(VM *pVM, List *pList, char *cStr) {
	if (ring_vm_oop_isattribute(pVM, pList, cStr)) {
		ring_vm_error(pVM, RING_VM_ERROR_ATTRREDEFINE);
		return RING_FALSE;
	}
	pList = ring_list_getlist(pList, RING_OBJECT_OBJECTDATA);
	ring_general_lower(cStr);
	ring_vm_newvar2(pVM, cStr, pList);
	return RING_TRUE;
}

void ring_vm_oop_cleansetpropertylist(VM *pVM) {
	if (ring_list_getsize(pVM->pSetProperty) > 0) {
		ring_list_deleteitem_gc(pVM->pRingState, pVM->pSetProperty, ring_list_getsize(pVM->pSetProperty));
	}
}
