# Remove debug dlls and *.pdb files from ring/bin folder
# 2020, Mahmoud Fayed <msfclipper@yahoo.com>

load "stdlibcore.ring"

aAvoid = [
"Qt5Gamepad.dll",
"Qt5VirtualKeyboard.dll",
"scene2d.dll",
"Qt53DQuickScene2D.dll",
"Qt5Quick3D.dll",
"declarative_gamepad.dll",
"xinputgamepad.dll",
"qsgd3d12backend.dll",
"qdirect2d.dll"
]

aFiles = listallfiles(exefolder(),"*.dll")

? "files" + list2str(aFiles)

aFiles = Filter(aFiles, func item {
	itemName = justFileName(item)
	if right(itemName,5) = "d.dll"
		if not find(aAvoid,itemName) 
			return True
		ok
	ok
	return False
})

DeleteFiles(aFiles)

aFiles = listallfiles(exefolder(),"*.pdb")

DeleteFiles(aFiles)

func DeleteFiles aFiles
	for cFile in aFiles
		? "Delete File: " + cFile
		remove(cFile)
	next
