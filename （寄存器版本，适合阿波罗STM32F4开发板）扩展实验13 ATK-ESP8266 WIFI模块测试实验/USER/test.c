#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h" 
#include "lcd.h" 
#include "ltdc.h"   
#include "sdram.h"    
#include "key.h" 
#include "malloc.h" 
#include "usmart.h"  
#include "w25qxx.h"    
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"  
#include "text.h" 
#include "touch.h"
#include "common.h"
#include "usart3.h"
#include "usmart.h"
//ALIENTEK 阿波罗STM32F429开发板扩展实验
//ATK-ESP8266 WIFI模块显示实验-寄存器版
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
//作者：正点原子 @ALIENTEK

  
int main(void)
{ 
	u8 key,fontok=0;
	Stm32_Clock_Init(360,25,2,8);//设置时钟,180Mhz
	delay_init(180);			//初始化延时函数 
	uart_init(90,115200);		//初始化串口波特率为115200
	usart3_init(45,115200);
	usmart_dev.init(90);//初始化USMART
  LED_Init();					//初始化与LED连接的硬件接口
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	KEY_Init();					//初始化按键
	W25QXX_Init();				//初始化W25Q256
	tp_dev.init();				//初始化触摸屏
 	my_mem_init(SRAMIN);		//初始化内部内存池
	my_mem_init(SRAMEX);		//初始化外部内存池
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();				//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH. 
	key=KEY_Scan(0);  
	if(key==KEY0_PRES)		//强制校准
	{
		LCD_Clear(WHITE);	//清屏
		TP_Adjust();  		//屏幕校准 
		TP_Save_Adjdata();	  
		LCD_Clear(WHITE);	//清屏
	}	
	fontok=font_init();		//检查字库是否OK
	if(fontok||key==KEY1_PRES)//需要更新字库				 
	{
		LCD_Clear(WHITE);		   	//清屏
 		POINT_COLOR=RED;			//设置字体为红色	   	   	  
		LCD_ShowString(60,50,200,16,16,"ALIENTEK STM32");
		while(SD_Init())			//检测SD卡
		{
			LCD_ShowString(60,70,200,16,16,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(60,70,200+60,70+16,WHITE);
			delay_ms(200);		    
		}								 						    
		LCD_ShowString(60,70,200,16,16,"SD Card OK");
		LCD_ShowString(60,90,200,16,16,"Font Updating...");
		key=update_font(20,110,16,"0:");//从SD卡更新
		while(key)//更新失败		
		{			 		  
			LCD_ShowString(60,110,200,16,16,"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(20,110,200+20,110+16,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(60,110,200,16,16,"Font Update Success!");
		delay_ms(1500);	
		LCD_Clear(WHITE);//清屏	       
	}  
	atk_8266_test();
}






