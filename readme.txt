miško - the self sufficient gps tracker

1. ABOUT
TODO

2. SOFTWARE USED
	Arduino IED - an open source IDE for the atmel chips
		https://www.arduino.cc/en/Main/Donate
		note: in windows 7 you may need to manually opdate drivers 
		reason: it may be that the ATmega2560 board is not recognized with the stock OS driver
				the appropriate driver is located in the arduino installation
				go to device manager, find the faulty USB device, manually update driver from the arduino install path
		
	Fritzing - an open source entry-level PCB design tool
		http://fritzing.org/download/?donation=0
		
	MS visio 
		used for some diagrams

	AutoCAD 
		used for enclosure diagrams, stl files for 3D printing
		
	tortoise git
		https://tortoisegit.org/
		a frontend for GIT, utilizing the github repository
		"git for windows" is needed as the backend for communication/functionality of hte github repository
		
	git for windows
		https://git-for-windows.github.io/
		the backed for git-for-windows
	
	
3. FILES / FOLDERS
	datasheets: contains datasheets, user guides for components
	misko-prototype: prototype firmware code which provides GPS tracker functionality
	battery holder.dwg: AutoCAD drawing of the battery holder
	LiIon battery.dwg: crude AutoCAD model of one battery, contains lateral dimensions
	prototype gps tracker - high level diagram.vsd: MSVisio diagram showing a high level overview of the hardware setup
	readme.txt: this file
	universal hardcase 50.dwg: crude AutoCAD drawing of the enclosure 