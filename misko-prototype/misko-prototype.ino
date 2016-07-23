// libraries
//#include <avr/pgmspace.h>
#include <SoftwareSerial.h> // serial library
#include <SD.h> // sd card library
#include <Wire.h> // I2C library
#include <avr/sleep.h>
#include <avr/power.h>
#include "binary_const.h" // with this we can use something B8(01010101) that it will convert to 85 at compile time
// the above cames really handy and readable when doing per bit configuration in the ADXL345 registers


// local files
#include "gps_config.h" // gps stuff
#include "pin_definitions.h" // pin layout
#include "adxl345_config.h"

#define GPSRATE 4800
#define SERIALRATE 9600
//BLUETOOTHSERIALRATE is hardcoded in device
#define NMEA_BUFFERSIZE 80 // plenty big

// global variables
char NMEA_buffer[NMEA_BUFFERSIZE] = "";        // string buffer for the NMEA sentence
unsigned int bufferid = 0; // holds the current position in the NMEA_buffer array, used fo walk through the buffer


char gps_date[9] = "20XXXXXX"; // 0-7 + 1 for '\0'
char gps_time[7] = "XXXXXX"; // 0-5 + 1 for '\0'
char gps_logfile[13] = "";
unsigned long timer;

#define DEVICE (0x53)    //ADXL345 device address (with SDO tied to ground)
#define TO_READ (6)      //num of bytes we are going to read each time (two bytes for each axis)

byte buff[TO_READ];    //6 bytes buffer for saving data read from the device
char str[512];                      //string buffer to transform data before sending it to the serial port
boolean inspected = 0;

void setup() 
{
#include "pin_modes.h"

  Serial.begin(SERIALRATE); // connect to the serial terminal
  Serial.println("start");

  // initialize GPS
  Serial3.begin(GPSRATE);   // connect to the GPS at the desired rate
  Serial3.print(SERIAL_SET); // set gps serial comm. parameter
  delay(50);

  // initialize SD card  
  if (!SD.begin(sd_ss_pin)) {   // see if the card is present and can be initialized
    Serial.println("SD Card failed, or not present");
    return;
  }

  Wire.begin(); // initialize I2C bus
  
  timer = millis();
#include "adxl345_config.h"

 // interrupts setup
  writeTo(DEVICE, R_INT_MAP, 0); // send all interrupts to ADXL345's INT1 pin
  writeTo(DEVICE, R_INT_ENABLE, 0x18); // enable signle and double tap, activity, inactivity and free fall detection
  
  // free fall configuration
  writeTo(DEVICE, R_TIME_FF, 0x14); // set free fall time
  writeTo(DEVICE, R_THRESH_FF, 0x05); // set free fall threshold
  
  // single tap configuration
  writeTo(DEVICE, R_DUR, 0x1F); // 625us/LSB
  writeTo(DEVICE, R_THRESH_TAP, 48); // 62.5mg/LSB  <==> 3000mg/62.5mg = 48 LSB as datasheet suggestion
  writeTo(DEVICE, R_TAP_AXES, B8(0)); // enable tap detection on x,y,z axes

  // double tap configuration
  writeTo(DEVICE, R_LATENT, 0x10);
  writeTo(DEVICE, R_WINDOW, 0xFF);
  
  // inactivity configuration
  writeTo(DEVICE, R_TIME_INACT, 2); // 5s / LSB
  writeTo(DEVICE, R_THRESH_INACT, 0x3); // 62.5mg / LSB
  
  // activity configuration
  writeTo(DEVICE, R_THRESH_ACT, 0x3); // 62.5mg / LSB
  
  // activity and inctivity control
  writeTo(DEVICE, R_ACT_INACT_CTL, 0xFF); // enable activity and inactivity detection on x,y,z using ac
  
  writeTo(DEVICE, R_POWER_CTL, 0x3C);


} // setup end 

#include "gps_functions.h" // gps functions

void loop() {
  
  
  get_nmea_sentences();
  /*
  
//Serial.println(digitalRead(interrupt_0_pin));
  if(digitalRead(interrupt_0_pin)) {
    int interruptSource = readByte(DEVICE, R_INT_SOURCE);

    if(interruptSource & B8(1000)) {
      Serial.println("### Inactivity");
      // we don't need to put the device in sleep because we set the AUTO_SLEEP bit to 1 in R_POWER_CTL
      // set the LOW_POWER bit to 1 in R_BW_RATE: with this we get worst measurements but we save power
      int bwRate = readByte(DEVICE, R_BW_RATE);
      writeTo(DEVICE, R_BW_RATE, bwRate | B8(10000));
      //enterSleep();
    }
    if(interruptSource & B8(10000)) {
      Serial.println("### Activity");
      
      // get current power mode
      int powerCTL = readByte(DEVICE, R_POWER_CTL);
      // set the device back in measurement mode
      // as suggested on the datasheet, we put it in standby then in measurement mode
      // we do this using a bitwise and (&) so that we keep the current R_POWER_CTL configuration
      writeTo(DEVICE, R_POWER_CTL, powerCTL & B8(11110011));
      delay(10); // let's give it some time (not sure if this is needed)
      writeTo(DEVICE, R_POWER_CTL, powerCTL & B8(11111011));
      
      // set the LOW_POWER bit to 0 in R_BW_RATE: get back to full accuracy measurement (we will consume more power)
      int bwRate = readByte(DEVICE, R_BW_RATE);
      writeTo(DEVICE, R_BW_RATE, bwRate & B8(01111));
    }
  }

  int regAddress = 0x32;    //first axis-acceleration-data register on the ADXL345
  int x, y, z;
  
  readFrom(DEVICE, regAddress, TO_READ, buff); //read the acceleration data from the ADXL345
  
   //each axis reading comes in 10 bit resolution, ie 2 bytes.  Least Significat Byte first!!
   //thus we are converting both bytes in to one int
  x = (((int)buff[1]) << 8) | buff[0];   
  y = (((int)buff[3])<< 8) | buff[2];
  z = (((int)buff[5]) << 8) | buff[4];
  
  //we send the x y z values as a string to the serial port
//  sprintf(str, "%d %d %d", x, y, z);  
//Serial.println(str);
/*
if (digitalRead(bluetooth_mosfet_gate) ==  HIGH){
  
if (millis() - timer > 30000) {
   Serial.println("bluetooth off");
   digitalWrite(bluetooth_mosfet_gate, HIGH);
  timer = millis();
} 
 }
*/


} 




//---------------- Functions
//Writes val to address register on device
void writeTo(int device, byte address, byte val) {
   Wire.beginTransmission(device); //start transmission to device 
   Wire.write(address);        // send register address
   Wire.write(val);        // send value to write
   Wire.endTransmission(); //end transmission
}

// read a single bite and returns the readed value
byte readByte(int device, byte address) {
  Wire.beginTransmission(device); //start transmission to device 
  Wire.write(address);        //sends address to read from
  Wire.endTransmission(); //end transmission
  
  Wire.beginTransmission(device); //start transmission to device
  Wire.requestFrom(device, 1);    // request 1 byte from device
  
  int readed = 0;
  if(Wire.available())
  { 
    readed = Wire.read(); // receive a byte
  }
  Wire.endTransmission(); //end transmission
  return readed;
}

//reads num bytes starting from address register on device in to buff array
void readFrom(int device, byte address, int num, byte buff[]) {
  Wire.beginTransmission(device); //start transmission to device 
  Wire.write(address);        //sends address to read from
  Wire.endTransmission(); //end transmission
  
  Wire.beginTransmission(device); //start transmission to device
  Wire.requestFrom(device, num);    // request 6 bytes from device
  
  int i = 0;
  while(Wire.available())    //device may send less than requested (abnormal)
  { 
    buff[i] = Wire.read(); // receive a byte
    i++;
  }
  Wire.endTransmission(); //end transmission
}


void enterSleep(void)
{
  /* Setup pin2 as an interrupt and attach handler. */
  attachInterrupt(0, pin2Interrupt, HIGH);
//  delay(100);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
  // The program will continue from here. 
  
  sleep_disable(); 
}

void pin2Interrupt(void)
{
  detachInterrupt(0);
}

