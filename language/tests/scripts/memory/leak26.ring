? ringvm_ismempool()
for t=1 to 500000
	x = ref(ref(ref(new obj {}) {}) {}) {}
next
? ringvm_ismempool()

class Obj