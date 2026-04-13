#include "flash.h"



//返回0：正常
uint8_t FlashGetStatus(void)
{
  uint32_t res;
  res=FLASH->SR;
  if(res&(1<<0))//flash busy
    return 1; 
  else if(res&(1<<2))  //Programming error
	return 2; 
  else if(res&(1<<4))  //写保护error
	return 3; 
  return 0;     
}

uint8_t FlashWaitDone(uint16_t time)
{
  uint8_t res;
  do
  {
    res=FlashGetStatus();
    if(res!=1)
	  break;
	delay_us(1);	
    time--;
  }while(time);
  if(time==0)
    res=0xff;
  return res;
}
//单片机FLASH擦除一页
uint8_t FlashErasePage(uint32_t paddr)
{
  uint8_t res=0;
  res=FlashWaitDone(0X5FFF);
  if(res==0)
  {
    FLASH->CR|=1<<1; 
    FLASH->AR=paddr; 
    FLASH->CR|=1<<6; 
    res=FlashWaitDone(0X5FFF); 
    if(res!=1) 
      FLASH->CR&=~(1<<1); 
  }
  return res;
}

//单片机FLASH写16位
uint8_t FlashWriteHalfWord(uint32_t faddr,uint16_t dat)
{
  uint8_t  res;
  res=FlashWaitDone(0XFF);
  if(res==0)
  {
    FLASH->CR|=1<<0;
    *(volatile uint16_t*)faddr=dat; 
    res=FlashWaitDone(0XFF);
	if(res!=1)
    {
      FLASH->CR&=~(1<<0); 
    }
  }
  return res;
}
//单片机FLASH读16位
uint16_t FlashReadHalfWord(uint32_t faddr)
{
  return *(volatile uint16_t*)faddr;
}
  
//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead)   	
{
	uint16_t i;
	for(i=0;i<NumToRead;i++)
	{
        pBuffer[i]=FlashReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}
//不检查的写入
//WriteAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数   
void STMFLASH_Write_NoCheck(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)   
{ 			 		 
	uint16_t i;
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//地址增加2.
	}  
} 
//从指定地址开始写入指定长度的数据
//WriteAddr:起始地址(此地址必须为2的倍数!!)
//pBuffer:数据指针
//NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
#define STM32_FLASH_SIZE STM32_memory_size
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节()
#else 
#define STM_SECTOR_SIZE	2048
#endif		 
uint16_t STMFLASH_BUF[STM_SECTOR_SIZE/2];//最多是2K字节
void STMFLASH_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)	
{
	uint32_t secpos;	   //扇区地址
	uint16_t secoff;	   //扇区内偏移地址(16位字计算)
	uint16_t secremain; //扇区内剩余地址(16位字计算)	   
 	uint16_t i;    
	uint32_t offaddr;   //去掉0X08000000后的地址
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//非法地址
	FLASH_Unlock();						//解锁
	offaddr=WriteAddr-STM32_FLASH_BASE;		//实际偏移地址.
	secpos=offaddr/STM_SECTOR_SIZE;			//扇区地址  0~63 for STM32F030
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//在扇区内的偏移(2个字节为基本单位.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//扇区剩余空间大小   
	if(NumToWrite<=secremain)secremain=NumToWrite;//不大于该扇区范围
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			FlashErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//擦除这个扇区
			for(i=0;i<secremain;i++)//复制
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//写入整个扇区  
		}else STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;				//扇区地址增1
			secoff=0;				//偏移位置为0 	 
		   	pBuffer+=secremain;  	//指针偏移
			WriteAddr+=secremain*2;	//写地址偏移(16位数据地址,需要*2)	   
		   	NumToWrite-=secremain;	//字节(16位)数递减
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//下一个扇区还是写不完
			else secremain=NumToWrite;//下一个扇区可以写完了
		}	 
	};	
	FLASH_Lock();//上锁
}


//单片机FLASH擦除后32K
void FlashErase32K(void)
{
  FlashErasePage(0x08018000);
  FlashErasePage(0x08018400);
  FlashErasePage(0x08018800);
  FlashErasePage(0x08018C00);
  FlashErasePage(0x08019000);
  FlashErasePage(0x08019400);
  FlashErasePage(0x08019800);
  FlashErasePage(0x08019C00);
  FlashErasePage(0x0801A000);
  FlashErasePage(0x0801A400);
  FlashErasePage(0x0801A800);
  FlashErasePage(0x0801AC00);
  FlashErasePage(0x0801B000);
  FlashErasePage(0x0801B400);
  FlashErasePage(0x0801B800);
  FlashErasePage(0x0801BC00);
  FlashErasePage(0x0801C000);
  FlashErasePage(0x0801C400);
  FlashErasePage(0x0801C800);
  FlashErasePage(0x0801CC00);
  FlashErasePage(0x0801D000);
  FlashErasePage(0x0801D400);
  FlashErasePage(0x0801D800);
  FlashErasePage(0x0801DC00);
  FlashErasePage(0x0801E000);
  FlashErasePage(0x0801E400);
  FlashErasePage(0x0801E800);
  FlashErasePage(0x0801EC00);
  FlashErasePage(0x0801F000);
  FlashErasePage(0x0801F400);
  FlashErasePage(0x0801F800); 
  FlashErasePage(0x0801FC00);
}
 
 























