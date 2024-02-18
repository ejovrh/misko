#include "sd/sd.h"

#if defined(USE_SD)	// if this particular device is active

#include "FatFs/fatfs.h"

static FATFS _fat;  // FAT FS object

typedef struct	// sdc_t actual
{
	sd_t public;  // public struct

	SPI_HandleTypeDef *hspi;  // HAL SPI instance
	GPIO_TypeDef *CS_Port;  // SPI chip select GPIO port
	uint16_t CS_Pin;  // SPI chip select GPIO pin
	GPIO_TypeDef *CD_Port;  // SD Card detect GPIO port
	uint16_t CD_Pin;  // SD Card detect GPIO pin
	FATFS *FAT;  //

	uint8_t FlagFATisMounted :1;  // is the SD FAT FS mounted or not?
} __sd_t;

static __sd_t __SD __attribute__ ((section (".data")));  // preallocate __SD object in .data

// returns free space in KiB
static uint32_t _Free(void)
{
	FRESULT fres = 0;  // operation result
	uint32_t free_clusters;  // free clusters on card

	if(__SD.FlagFATisMounted == 0)  // if not mounted ...
		fres = f_mount(__SD.FAT, "", 1);  // ... mount

	if(fres == FR_OK)  // if mounted ...
		fres = f_getfree("", &free_clusters, &__SD.FAT);	// ... get free clusters

	return (uint32_t) ((free_clusters * __SD.FAT->csize) / 2);	// calculate KiB and return
}

// reads n bytes from file into data buffer
static unsigned int _Read(const char *filename, const void *data_in, const unsigned int len)
{
	FIL file;  // file handle
	FRESULT fres = 0;  // operation result
	UINT bytesread = 0;  // bytes written out

	if(__SD.FlagFATisMounted == 0)  // if not mounted ...
		fres = f_mount(__SD.FAT, "", 1);  // ... mount

	if(fres == FR_OK)
		fres = f_open(&file, filename, FA_READ);

	if(fres == FR_OK)
		{
			// TODO - implement gets or getc
//			TCHAR *rres = gets((TCHAR*) data_in, len, &file);
			;
		}
	else
		{
			f_close(&file);  // close the file handle
		}

	return bytesread;

}

// writes n bytes from file into data buffer
static unsigned int _Write(const char *filename, const void *data_out, unsigned int len)
{
	FIL file;  // file handle
	FRESULT fres = 0;  // operation result
	UINT byteswritten = 0;	// bytes written out

	if(__SD.FlagFATisMounted == 0)  // if not mounted ...
		fres = f_mount(__SD.FAT, "", 1);  // ... mount

	if(fres == FR_OK)  // if mounted ...
		fres = f_open(&file, filename, FA_WRITE | FA_OPEN_APPEND);	// ... open for writing

	if(fres == FR_OK)  // if opened for write ...
		fres = f_write(&file, data_out, len, &byteswritten);	// ... write into it & store bytes written

	f_close(&file);  // close file handle

	return byteswritten;	// return bytes written
}

static __sd_t __SD =  // instantiate sdc_t actual and set function pointers
	{  //
	.public.Free = &_Free,  // returns free space in KiB
	.public.Read = &_Read,  // reads n bytes from file into data buffer
	.public.Write = &_Write  // writes n bytes from file into data buffer
	};

sd_t* sd_ctor(SPI_HandleTypeDef *in_hspi, GPIO_TypeDef *_SPI_CS_Port, const uint16_t _SPI_CS_Pin, GPIO_TypeDef *_SD_CD_Port, const uint16_t _SD_CD_Pin)  //
{
	// TODO - SD card ctor - implement card detect logic
	MX_FATFS_Init();

	__SD.hspi = in_hspi;  // store SPI object
	__SD.CS_Port = _SPI_CS_Port;  // store SPI Chip select port
	__SD.CS_Pin = _SPI_CS_Pin;  // store SPI Chip select pin
	__SD.CD_Port = _SD_CD_Port;  // store SD Card detect port
	__SD.CD_Pin = _SD_CD_Pin;  // store SD Card detect pin

	__SD.FAT = &_fat;  // tie in FATFS object

	HAL_GPIO_WritePin(__SD.CS_Port, __SD.CS_Pin, GPIO_PIN_SET);  // chip select is active low

	__SD.FlagFATisMounted = (f_mount(__SD.FAT, "", 1) == 0);	// force mount and save state as Flag

	return &__SD.public;  // set pointer to SD public part
}

sd_t *const SD = &__SD.public;  // set pointer to SD public part

#endif
