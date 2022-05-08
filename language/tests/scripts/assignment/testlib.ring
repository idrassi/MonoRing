load "stdlib.ring"

class sqlite_ex from sqlite
	Func getInstance cDataBase
		inst = new sqlite
		inst.open(cDataBase)
		return inst

