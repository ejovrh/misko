/*
 * fram.h
 *
 * Created: 25.9.2016. 22:37:51
 *  Author: hivanic
 */ 
#define CMD_WREN 0x06;		//0000 0110 FRAM Set Write Enable Latch
#define CMD_WRDI 0x04;		//0000 0100 FRAM Write Disable
#define CMD_RDSR 0x05;		//0000 0101 FRAM Read Status Register
#define CMD_WRSR 0x01;		//0000 0001 FRAM Write Status Register
#define CMD_READ 0x03;		//0000 0011 FRAM Read Memory Data
#define CMD_WRITE 0x02;		//0000 0010 FRAM Write Memory Data