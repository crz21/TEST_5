/*   
 *
 *
 *
 *
 */
#include "sys.h"
#include "delay.h" 
#include "led.h" 
#include "usart.h" 
#include "ESP8266.h" 
  
int main(void)
{ 
//	u8 t;
//	u8 len;	
	u16 times=0; 
	u8 rx_str[50] = {0};
	u16 tx_len = 0;
	u8 *tx_str = "AT";
  
	Stm32_Clock_Init(360,25,2,8);//设置时钟,180Mhz
	delay_init(180);		//初始化延时函数 
	uart_init(45,115200);	//串口初始化为115200
	LED_Init();		  		//初始化与LED连接的硬件接口   
	delay_ms(10);
	ESP8266_init();
	while(1)
	{
	//	uart_send(tx_str, 6);
		if(USART_RX_STA&0x8000)
		{					   
			tx_len = uart_recv(rx_str);
			uart_send(rx_str, tx_len);
		}
		else
		{
//			times++;
//			if(times%5000==0)
//			{
//				printf("\r\nALIENTEK 阿波罗STM32F4/F7开发板 串口实验\r\n");
//				printf("正点原子@ALIENTEK\r\n\r\n\r\n");
//			}
//			if(times%200==0)
//				printf("请输入数据,以回车键结束\r\n");  

  
			uart_send(tx_str, 6);	
delay_ms(10);			
		}
	}
}

















