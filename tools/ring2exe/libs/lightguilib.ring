aLibrary = [:name = :lightguilib ,
	 :title = "LightGuiLib",
	 :windowsfiles = [
		"Qt5Core.dll",
		"Qt5Gui.dll",
		"Qt5Test.dll",
		"Qt5Widgets.dll",
		"ringqt_light.dll",
		"msvcp140*.dll",
		"vcruntime140*.dll",
		"ucrtbase.dll",
		"libEGL.dll",
		"libGLESv2.dll",
		"d3dcompiler_47.dll",
		"api-ms-win-*.dll",
		"platforms\qwindows.dll",
		"styles\qwindowsvistastyle.dll"
	 ] ,
	 :windowsfolders = [
		"imageformats"
	 ],
	 :linuxfiles = [
		"libringqt_light.so"
	 ],
	 :macosxfiles = [
		"libringqt_light.dylib"
	 ],
	 :ubuntudep = "qt5-default qtbase5-dev",
	 :fedoradep = "qt5-qtbase-devel",
	 :macosxdep = "qt@5"
	]