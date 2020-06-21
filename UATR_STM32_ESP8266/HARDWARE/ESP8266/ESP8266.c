/**************************************************************************
 * @file     ESP8266.c
 * @brief    ESP8266驱动
 * @version  V1.00
 * @date     20. June 2020
 * @author   CRZ
 **************************************************************************/
#include <string.h>
#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "ESP8266.h"
/*********************************************
 * @brief    初始化ESP8266
 * @param    无
 * @retval   u8 wifi_step_1 or 0
 * @version  V1.00
 * @date     21. June 2020
 * @author   CRZ
 *********************************************/
u8 ESP8266_init()
{
	u8 i = 0;
	u8 *tx_str = "AT";
	u8 rx_str[50] = {0};
	u16 tx_len = 0;
  u16 rx_len = 0;
	
	tx_len = strlen(tx_str);
	delay_ms(10);
	uart_send(tx_str, 7);
	delay_ms(500);
	rx_len = uart_recv(rx_str);
	
	while(strcmp((char *)rx_str, "OK") != 0)
	{
		uart_send(tx_str, tx_len);
		delay_ms(500);
		rx_len = uart_recv(rx_str);
		if( i >= 3)
			return 0;
		i++;
	}
	return wifi_step_1;
}

///*********************************************
// * @brief    初始化控制LED的IO
// * @param    无
// * @retval   无
// * @version  V1.00
// * @date     20. June 2020
// * @author   CRZ
// *********************************************/
//void ESP8266_start()
//{
//	u8 i = 0;
//	u8 *tx_str = "AT";
//	u8 rx_str[50] = {0};
//	u16 tx_len = 0;
//  u16 rx_len = 0;
//	
//	strlen(tx_str);
//	uart_send(u8 *tx_str, u16 tx_len);
//	delay_ms(50);
//	rx_len = uart_recv(rx_str);
//	
//	while(strcmp(rx_str, "OK") != 0)
//	{
//		uart_send(u8 *tx_str, u16 tx_len);
//		delay_ms(50);
//		rx_len = uart_recv(rx_str);
//		if( i >= 3)
//			return 0;
//		i++;
//	}
//	return wifi_step_1;
//}
