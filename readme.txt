miško - the self sufficient gps tracker

1. ABOUT
	TODO
	Once upon a time, the need was to be able to geotag pictures I take with my camera while enjoying outdor activities.
	I order to satisfy this need I purchased the so-popular GPS recievers which run on a single AA battery (and have a bluetooth 
	transmitter) and a bluetooth module that attaches to the camera.
	This has proven to be futile because of battery runtime, storage capacity and so on.
	
	The original GPS tracker started its life as a (curiosity) kit from adafruit out of the need to have a 
		GPS tracker for my outdoor adventures which meets the following criteria:
			- does not run on AA batteries
			- has a sufficiently long standalone runtime
			- provides logging funcionality 
			- provides a bluetooth interface so that I can geotag pictures I take
			- can be charged by various means
			
		There are professional GPS devices which can (partly) do the job but they do cost a bit more than petty cash 
		and im not that rich.
		
		In my Searches on the Internet I did not find a product which has all the above features implemented; i did find, however, 
		a kit which provides logging of GPS data to a SD card.
		
		Now, GPS modules -usually- use the NMEA protocol; NMEA -usually- utilizes RS232 as a transport [TTL is another choice].
		RS232 is used by the kit's logic to parse incoming data and (RS232) can be easily fed into a bluetooth module.
		The camera has a bluetooth module - it is naturaly to experiment and see if this kit can be tailored to my needs.
		
		This repository attepts two things:
			- document the efforts of producing a suitable GPS tracker (its my university thesis, after all....)
			- produce a GPS tracker which has the following main features:
				- records GPS data and writes it to storage media (after vacations, google earth and the visited route are a nice thing to have)
				- provides NMEA sentences to the camera via bluetooth to geotag pictues
				- has sufficiently long runtime
				- comes in a waterproof box, is to some extent shockproof, dustproof and can handle some abuse
				- can be supplied with electricity on the road (by means of bicycle dynamo, solar cells, mains outlet)
				- can, if need be, charge my mobile phone
				- has certain functionalities user-configurable via buttons
				- displays certain tokens of inromation via LCD
				
				bonus features:
					- uploads the route at definable intervals to some server via GSM ( Big Mother is watching... )
					- log other interesting data to storage
					- get rid of the awful Arduino IDE and use a real one - AVR studio, for example
					
		That's the rough story of it...

2. SOFTWARE USED
	Arduino IED - an open source IDE for the atmel chips
		https://www.arduino.cc/en/Main/Donate
		note: in windows 7 you may need to manually opdate drivers 
		reason: it may be that the ATmega2560 board is not recognized with the stock OS driver
				the appropriate driver is located in the arduino installation
				go to device manager, find the faulty USB device, manually update driver from the arduino install path
		
	Fritzing - an open source entry-level PCB design tool
		http://fritzing.org/download/?donation=0
		
	MS Visio 
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
	
4. DEVICES
	4.1 misko-prototype
		The original hardware, in kit-form, lived its life in an Iphone3 box and has seen parts of the world on my bicycle.
		It worked well for its humble beginnings, but not well enough.
		Now it lives its life on a piece of cardboard, connected to breadboards, does not leave the home and serves as a 
		development instance.
		
		The role of this environment is development (hardware and software).
		
	4.2 misko-qa
		TODO
		The QA hardware/instance will be based on the prototype's schematics - PCB's tailored to fit into the enclosure and to 
		support features listed.
		Provided the PCB's are good (free of hardware bugs), it will serve as a software (and partially hardware) QA instance for the 
		jump of changes from development to production hardware.
	
	4.2 misko-prod
		TODO
		The production hardware/instance will be used in a live environment (real travels with rain and wind and everything).
		
		Changes on this environment will go through a controlled change cycle - first dev, then QA, then prod.
		
		Developers who know me from my work environment are entitled to my opinion and may now laugh as much as they want.
		
5. TODO?