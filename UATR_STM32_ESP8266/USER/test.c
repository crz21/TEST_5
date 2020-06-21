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
  
	Stm32_Clock_Init(360,25,2,8);//����ʱ��,180Mhz
	delay_init(180);		//��ʼ����ʱ���� 
	uart_init(45,115200);	//���ڳ�ʼ��Ϊ115200
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�   
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
//				printf("\r\nALIENTEK ������STM32F4/F7������ ����ʵ��\r\n");
//				printf("����ԭ��@ALIENTEK\r\n\r\n\r\n");
//			}
//			if(times%200==0)
//				printf("����������,�Իس�������\r\n");  

  
			uart_send(tx_str, 6);	
delay_ms(10);			
		}
	}
}

















