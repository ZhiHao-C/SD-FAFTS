#include "stm32f10x.h"                  // Device header
#include "bps_led.h"
#include "bps_usart.h"
#include "sdio_test.h"
#include "bsp_sdio_sdcard.h"
#include "bps_key.h"
#include "ff.h"
#include <stdio.h>
#include <string.h>

FATFS FSobjec;//����һ���������f_mountʹ��
uint16_t i;

FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FIL fnew;													/* �ļ����� */
FRESULT res;                      /* �ļ�������� */
UINT fnum;            					  /* �ļ��ɹ���д���� */
BYTE ReadBuffer[1024]={0};        /* �������� */
BYTE WriteBuffer[] =              /* д������*/
" �Һ������ˣ��ҵ�δ�������Σ������Ǹ������ӣ��½��ļ�ϵͳ�����ļ�\r\n"; 

int main(void)
{
	/* ��ʼ��LED */
	LED_GPIO_Config();	
	LED_BLUE;
	
	/* ��ʼ�����Դ��ڣ�һ��Ϊ����1 */
	USART_Config();	
  printf("****** ����һ�� sd���ļ�ϵͳʵ�� ******\r\n");
  printf("\r\n ʹ��ָ���ߵװ�ʱ ���Ͻ�����λ�� ��Ҫ��PC0������ñ ��ֹӰ��PC0��SPIFLASHƬѡ�� \r\n");
  
	//���ⲿSDIO SD�����ļ�ϵͳ���ļ�ϵͳ����ʱ���SD�豸��ʼ��
	//��ʼ������������������
	//f_mount()->find_volume()->disk_initialize->SD_Init()
	res = f_mount(&fs,"0:",1);
	
/*----------------------- ��ʽ������ -----------------*/  
	/* ���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ */
	if(res == FR_NO_FILESYSTEM)
	{
		printf("��SD��û���ļ�ϵͳ���������и�ʽ��...\r\n");
    /* ��ʽ�� */
		res=f_mkfs("0:",0,0);							
		
		if(res == FR_OK)
		{
			printf("��SD�ѳɹ���ʽ���ļ�ϵͳ��\r\n");
      /* ��ʽ������ȡ������ */
			res = f_mount(NULL,"0:",1);			
      /* ���¹���	*/			
			res = f_mount(&fs,"0:",1);
		}
		else
		{
			LED_RED;
			printf("������ʽ��ʧ�ܡ�����\r\n");
			while(1);
		}
	}
  else if(res!=FR_OK)
  {
    printf("�����ⲿSD�����ļ�ϵͳʧ�ܡ�(%d)\r\n",res);
    printf("��������ԭ��SD��ʼ�����ɹ���\r\n");
		while(1);
  }
  else
  {
    printf("���ļ�ϵͳ���سɹ������Խ��ж�д����\r\n");
  }
  
/*----------------------- �ļ�ϵͳ���ԣ�д���� -------------------*/
	/* ���ļ���ÿ�ζ����½�����ʽ�򿪣�����Ϊ��д */
	printf("\r\n****** ���������ļ�д�����... ******\r\n");	
	res = f_open(&fnew, "0:FATFS��д����.txt",FA_CREATE_ALWAYS | FA_WRITE );
	if ( res == FR_OK )
	{
		printf("����/����FatFs��д�����ļ�.txt�ļ��ɹ������ļ�д�����ݡ�\r\n");
    /* ��ָ���洢������д�뵽�ļ��� */
		res=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
    if(res==FR_OK)
    {
      printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
      printf("�����ļ�д�������Ϊ��\r\n%s\r\n",WriteBuffer);
    }
    else
    {
      printf("�����ļ�д��ʧ�ܣ�(%d)\n",res);
    }    
		/* ���ٶ�д���ر��ļ� */
    f_close(&fnew);
	}
	else
	{	
		LED_RED;
		printf("������/�����ļ�ʧ�ܡ�  res=%d\r\n",res);
	}
	
/*------------------- �ļ�ϵͳ���ԣ������� --------------------------*/
	printf("****** ���������ļ���ȡ����... ******\r\n");
	res = f_open(&fnew, "0:FATFS��д����.txt",FA_OPEN_EXISTING | FA_READ); 	 
	if(res == FR_OK)
	{
		LED_GREEN;
		printf("�����ļ��ɹ���\r\n");
		res = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(res==FR_OK)
    {
      printf("���ļ���ȡ�ɹ�,�����ֽ����ݣ�%d\r\n",fnum);
      printf("����ȡ�õ��ļ�����Ϊ��\r\n%s \r\n", ReadBuffer);	
    }
    else
    {
      printf("�����ļ���ȡʧ�ܣ�(%d)\n",res);
    }		
	}
	else
	{
		LED_RED;
		printf("�������ļ�ʧ�ܡ�res=%d\r\n",res);
	}
	/* ���ٶ�д���ر��ļ� */
	f_close(&fnew);	
  
	/* ����ʹ���ļ�ϵͳ��ȡ�������ļ�ϵͳ */
	f_mount(NULL,"0:",1);
  
  /* ������ɣ�ͣ�� */
	while(1)
	{
	}


}



/*
 * ��������SDIO_IRQHandler
 * ����  ����SDIO_ITConfig(���������������sdio�ж�	��
 			���ݴ������ʱ�����ж�
 * ����  ����		 
 * ���  ����
 */
void SDIO_IRQHandler(void) 
{
  /* Process All SDIO Interrupt Sources */
  SD_ProcessIRQSrc();
}




