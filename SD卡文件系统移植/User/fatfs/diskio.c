/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
//#include "sdcard.h"		/* Example: Header file of existing MMC/SDC contorl module */
#include "ff.h"
#include "bsp_sdio_sdcard.h"
#include <string.h>
#define SD_BLOCKSIZE     512 

/* Definitions of physical drive number for each drive */
#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 *///(SD卡)
#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define USB		2	/* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
extern SD_CardInfo SDCardInfo;

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat=STA_NOINIT;
//	int result;

	switch (pdrv) {
	case ATA :
		stat &=~STA_NOINIT;
		break;

	case MMC :
		break;

	case USB :
		break;
	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
//	SD_Error SD_State = SD_OK;
	DSTATUS stat;

	switch (pdrv) {
	case ATA :
		if(SD_Init()!= SD_OK)
		{
			stat=STA_NOINIT;
		}
		else
		{
			stat &=~STA_NOINIT;
		}

		break;

	case MMC :
		break;

	case USB :
		break;
	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	SD_Error SD_State = SD_OK;
	DRESULT res;
//	int result;

	switch (pdrv) {
	case ATA :
		//如果传进来的地址不是四个字节对齐
		if((uint32_t)buff&3)//比如一个数如果%4不余0那么前面两位肯定不是00 那么&011肯定为真
		{
			__align(4) BYTE TempBuff[512];
			while(count--)
			{
				res=disk_read(ATA,TempBuff,sector,1);
				memcpy(buff,TempBuff,SDCardInfo.CardBlockSize);
				buff +=SDCardInfo.CardBlockSize;//将对齐地址中的内容拷贝到buff目标地址上
				
			}
			break;
		}
		else
		{
			SD_State = SD_ReadMultiBlocks(buff, sector*SDCardInfo.CardBlockSize, SDCardInfo.CardBlockSize, count);
			/* Check if the Transfer is finished */
			SD_State = SD_WaitReadOperation();
			while(SD_GetStatus() != SD_TRANSFER_OK);
		}
		
		if(SD_State == SD_OK)
		{
			res=RES_OK;
		}
		else
		{
			
			res=RES_ERROR;
		}
		break;

	case MMC :
		break;

	case USB :

		break;
	}

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	SD_Error SD_State = SD_OK;
	DRESULT res=RES_ERROR;
//	int result;

	switch (pdrv) {
	case ATA :
		if((uint32_t)buff&3)//比如一个数如果%4不余0那么前面两位肯定不是00 那么&011肯定为真
		{
			__align(4) BYTE TempBuff[512];
			while(count--)
			{	
				memcpy(TempBuff,buff,SDCardInfo.CardBlockSize);
				res=disk_write(ATA,TempBuff,sector,1);
				
				buff +=SDCardInfo.CardBlockSize;//将对齐地址中的内容拷贝到buff目标地址上
				
			}
			break;
		}
		else
		{
			SD_State = SD_WriteMultiBlocks((uint8_t*)buff, sector*SDCardInfo.CardBlockSize, SDCardInfo.CardBlockSize, count);
			/* Check if the Transfer is finished */
			SD_State = SD_WaitWriteOperation();
			while(SD_GetStatus() != SD_TRANSFER_OK);
			if(SD_State == SD_OK)
			{
				res=RES_OK;
			}
			else
			{
				res=RES_ERROR;
			}
			break;
		}
		

	case MMC :
		break;

	case USB :
		break;
	default:
		res = RES_PARERR;
	}


	return res;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res=RES_ERROR;

	switch (pdrv) {
	case ATA :
		switch (cmd) 
		{
			/* 扇区数量： */
			case GET_SECTOR_COUNT://可用扇区数量
				*(DWORD*)buff = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;;		
			break;
			/* 扇区大小  */
			case GET_SECTOR_SIZE :
				*(WORD*)buff = SDCardInfo.CardBlockSize;
			break;
			/* 同时擦除扇区个数 */
			case GET_BLOCK_SIZE :
				*(WORD*)buff = 1;
			break;   
		}
		res=RES_OK;

		break;

	case MMC :


		break;

	case USB :


		break;
	}

	return res;
}
#endif

__weak DWORD get_fattime(void) {
	/* 返回当前时间戳 */
	return	  ((DWORD)(2015 - 1980) << 25)	/* Year 2015 */
			| ((DWORD)1 << 21)				/* Month 1 */
			| ((DWORD)1 << 16)				/* Mday 1 */
			| ((DWORD)0 << 11)				/* Hour 0 */
			| ((DWORD)0 << 5)				  /* Min 0 */
			| ((DWORD)0 >> 1);				/* Sec 0 */
}

