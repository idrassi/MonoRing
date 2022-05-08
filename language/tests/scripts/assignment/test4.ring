# no problem

load "assignment/testlib.ring"

new test  

? :done

class test

	myobj = new sqlite_ex()
	myobj = myobj.getInstance("QSQLITE") 
	myobj.close()
 