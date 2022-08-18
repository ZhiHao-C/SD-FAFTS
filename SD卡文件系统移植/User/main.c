#include "stm32f10x.h"                  // Device header
#include "bps_led.h"
#include "bps_usart.h"
#include "sdio_test.h"
#include "bsp_sdio_sdcard.h"
#include "bps_key.h"
#include "ff.h"
#include <stdio.h>
#include <string.h>

FATFS FSobjec;//定义一个变量配合f_mount使用
uint16_t i;

FATFS fs;													/* FatFs文件系统对象 */
FIL fnew;													/* 文件对象 */
FRESULT res;                      /* 文件操作结果 */
UINT fnum;            					  /* 文件成功读写数量 */
BYTE ReadBuffer[1024]={0};        /* 读缓冲区 */
BYTE WriteBuffer[] =              /* 写缓冲区*/
" 家和万事兴，我的未来不是梦，今天是个好日子，新建文件系统测试文件\r\n"; 

int main(void)
{
	/* 初始化LED */
	LED_GPIO_Config();	
	LED_BLUE;
	
	/* 初始化调试串口，一般为串口1 */
	USART_Config();	
  printf("****** 这是一个 sd卡文件系统实验 ******\r\n");
  printf("\r\n 使用指南者底板时 左上角排针位置 不要将PC0盖有跳帽 防止影响PC0做SPIFLASH片选脚 \r\n");
  
	//在外部SDIO SD挂载文件系统，文件系统挂载时会对SD设备初始化
	//初始化函数调用流程如下
	//f_mount()->find_volume()->disk_initialize->SD_Init()
	res = f_mount(&fs,"0:",1);
	
/*----------------------- 格式化测试 -----------------*/  
	/* 如果没有文件系统就格式化创建创建文件系统 */
	if(res == FR_NO_FILESYSTEM)
	{
		printf("》SD还没有文件系统，即将进行格式化...\r\n");
    /* 格式化 */
		res=f_mkfs("0:",0,0);							
		
		if(res == FR_OK)
		{
			printf("》SD已成功格式化文件系统。\r\n");
      /* 格式化后，先取消挂载 */
			res = f_mount(NULL,"0:",1);			
      /* 重新挂载	*/			
			res = f_mount(&fs,"0:",1);
		}
		else
		{
			LED_RED;
			printf("《《格式化失败。》》\r\n");
			while(1);
		}
	}
  else if(res!=FR_OK)
  {
    printf("！！外部SD挂载文件系统失败。(%d)\r\n",res);
    printf("！！可能原因：SD初始化不成功。\r\n");
		while(1);
  }
  else
  {
    printf("》文件系统挂载成功，可以进行读写测试\r\n");
  }
  
/*----------------------- 文件系统测试：写测试 -------------------*/
	/* 打开文件，每次都以新建的形式打开，属性为可写 */
	printf("\r\n****** 即将进行文件写入测试... ******\r\n");	
	res = f_open(&fnew, "0:FATFS读写测试.txt",FA_CREATE_ALWAYS | FA_WRITE );
	if ( res == FR_OK )
	{
		printf("》打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");
    /* 将指定存储区内容写入到文件内 */
		res=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
    if(res==FR_OK)
    {
      printf("》文件写入成功，写入字节数据：%d\n",fnum);
      printf("》向文件写入的数据为：\r\n%s\r\n",WriteBuffer);
    }
    else
    {
      printf("！！文件写入失败：(%d)\n",res);
    }    
		/* 不再读写，关闭文件 */
    f_close(&fnew);
	}
	else
	{	
		LED_RED;
		printf("！！打开/创建文件失败。  res=%d\r\n",res);
	}
	
/*------------------- 文件系统测试：读测试 --------------------------*/
	printf("****** 即将进行文件读取测试... ******\r\n");
	res = f_open(&fnew, "0:FATFS读写测试.txt",FA_OPEN_EXISTING | FA_READ); 	 
	if(res == FR_OK)
	{
		LED_GREEN;
		printf("》打开文件成功。\r\n");
		res = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(res==FR_OK)
    {
      printf("》文件读取成功,读到字节数据：%d\r\n",fnum);
      printf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);	
    }
    else
    {
      printf("！！文件读取失败：(%d)\n",res);
    }		
	}
	else
	{
		LED_RED;
		printf("！！打开文件失败。res=%d\r\n",res);
	}
	/* 不再读写，关闭文件 */
	f_close(&fnew);	
  
	/* 不再使用文件系统，取消挂载文件系统 */
	f_mount(NULL,"0:",1);
  
  /* 操作完成，停机 */
	while(1)
	{
	}


}



/*
 * 函数名：SDIO_IRQHandler
 * 描述  ：在SDIO_ITConfig(）这个函数开启了sdio中断	，
 			数据传输结束时产生中断
 * 输入  ：无		 
 * 输出  ：无
 */
void SDIO_IRQHandler(void) 
{
  /* Process All SDIO Interrupt Sources */
  SD_ProcessIRQSrc();
}




