# no problem

load "assignment/testlib.ring"

o1 = new test  { go() }

? :done

class test

	
	func go

	myobj = new sqlite_ex()
	myobj = myobj.getInstance("QSQLITE") 
	myobj.close()