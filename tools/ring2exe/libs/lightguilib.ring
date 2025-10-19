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
		"styles\qwindowsvistastyle.dll",
		"styles\qwindowsvistastyled.dll"
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
	 :ubuntudep = "qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools",
	 :fedoradep = "qt5-qtbase-devel",
	 :macosxdep = "qt@5"
	]