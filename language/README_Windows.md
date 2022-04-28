![Ring](http://ring-lang.sf.net/thering.jpg)

# Ring Programming Language (MonoRing build)

## Building using Microsoft Windows 

### Get the source code

	git clone http://github.com/idrassi/MonoRing.git
	
### Build Ring (Compiler/VM)
	
	cd ring/language/src
	buildvc.bat

### Build Ring2EXE 

	cd ring/tools/ring2exe
	buildring2exe.bat 

### Build RingPM

	cd ring/tools/ringpm
	buildringpm.bat 

### To be able to call ring from any folder 
	
	cd ring/bin
	install.bat
	
#### Add Ring/bin to System path

	Hit "windows key".
	Type "Edit the System environment variables"
	Select "Advanced" tab.
	Click on "Environment Variables..."
	Double click on "Path"
	Add at the end the new path separated by semicolon. 
		;C:\Ring\Bin
	
