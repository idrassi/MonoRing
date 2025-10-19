aLibrary = [:name = :qt ,
	 :title = "RingQt",
	 :windowsfiles = [
		"Qt5*.dll",
		"ringqt.dll",
		"icudt54.dll",
		"icuin54.dll",
		"icuuc54.dll",
		"QtWebEngineProcess.exe",
		"icudtl.dat",
		"qtwebengine_devtools_resources.pak",
		"qtwebengine_resources.pak",
		"qtwebengine_resources_100p.pak",
		"qtwebengine_resources_200p.pak",
		"msvcp140*.dll",
		"vcruntime140*.dll",
		"ucrtbase.dll",
		"api-ms-win-*.dll",
		"libEGL.dll",
		"libGLESv2.dll",
		"d3dcompiler_47.dll",
		"opengl32sw.dll"
	 ] ,
	 :windowsfolders = [
		:audio,
		:bearer,
		:geometryloaders,
		:texttospeech,
		:iconengines,
		:imageformats,
		:mediaservice,
		:platforms,
		:playlistformats,
		:position,
		:printsupport,
		:renderers,
		:resources,
		:sensors,
		:sensorgestures,
		:sqldrivers,
		:translations,
		:styles
	 ],
	 :linuxfiles = [
		"libringqt.so"
	 ],
	 :macosxfiles = [
		"libringqt.dylib"
	 ],
	 :ubuntudep = "qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools qtmultimedia5-dev "+
		      "libqt5webkit5-dev libqt5serialport5-dev qtconnectivity5-dev qtdeclarative5-dev "+
		      "libqt5opengl5-dev libqt5texttospeech5-dev qtpositioning5-dev qt3d5-dev "+
		      "qt3d5-dev-tools libqt5charts5 libqt5charts5-dev libqt5svg5-dev qtwebengine5-dev "+
		      "qml-module-qtquick-controls qml-module-qtcharts",
	 :fedoradep = "qt5-qtbase-devel qt5-qtmultimedia-devel "+
		      "qt5-qtwebkit-devel qt5-qtserialport-devel qt5-qtconnectivity-devel qt5-qtdeclarative-devel qt5-qtlocation-devel",
	 :macosxdep = "qt@5"
	]