// ADXL345 Register Addresses
#define DEVID   			0x00	//Device ID Register
#define THRESH_TAP  		0x1D  	//Tap Threshold
#define OFFSET_X    		0x1E  	//X-axis offset
#define OFSFSET_Y    		0x1F  	//Y-axis offset
#define OFSFSET_Z    		0x20  	//Z-axis offset
#define DURATION  			0x21  	//Tap Duration
#define LATENT    			0x22  	//Tap latency
#define WINDOW    			0x23  	//Tap window
#define THRESH_ACT  		0x24  	//Activity Threshold
#define THRESH_INACT  		0x25  	//Inactivity Threshold
#define TIME_INACT  		0x26  	//Inactivity Time
#define ACT_INACT_CTL 		0x27  	//Axis enable control for activity and inactivity detection
#define THRESH_FF 			0x28  	//free-fall threshold
#define TIME_FF   			0x29  	//Free-Fall Time
#define TAP_AXES  			0x2A  	//Axis control for tap/double tap
#define ACT_TAP_STATUS		0x2B  	//Source of tap/double tap
#define BW_RATE   			0x2C  	//Data rate and power mode control
#define POWER_CTL 			0x2D  	//Power Control Register
#define INT_ENABLE  		0x2E  	//Interrupt Enable Control
#define INT_MAP   			0x2F  	//Interrupt Mapping Control
#define INT_SOURCE  		0x30  	//Source of interrupts
#define DATA_FORMAT 		0x31  	//Data format control
#define DATAX0    			0x32  	//X-Axis Data 0
#define DATAX1    			0x33  	//X-Axis Data 1
#define DATAY0    			0x34  	//Y-Axis Data 0
#define DATAY1    			0x35  	//Y-Axis Data 1
#define DATAZ0    			0x36  	//Z-Axis Data 0
#define DATAZ1    			0x37  	//Z-Axis Data 1
#define FIFO_CTL  			0x38  	//FIFO control
#define FIFO_STATUS 		0x39  	//FIFO status

//config items for act/inact detection
#define	DATA_FORMAT_CFG		0x00	// +-2g range
#define	INT_MAP_CFG			0x00	// send all to INT1 
#define	INT_ENABLE_CFG		0x18	// enable interrupts only for act/inact detection
#define	TIME_INACT_CFG		0x02	// LSB meaning: 0x05 (dec 5) => 5s , 0x3C (dec 60) => 60s
#define	THRESH_INACT_CFG	0x02	// 62.5mg / LSB
#define	THRESH_ACT_CFG		0x02	// 62.5mg / LSB
#define	ACT_INACT_CTL_CFG	0xFF	// enable activity and inactivity detection on x,y,z using ac
#define	POWER_CTL_CFG		0x3B	// 1Hz sampling on sleep, in measure mode, in sleep and link mode
#define	BW_RATE_CFG			0x08	// 
