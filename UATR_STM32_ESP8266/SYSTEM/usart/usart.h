#ifndef __USART_H
#define __USART_H 
#include "sys.h"
#include "stdio.h"	  
//////////////////////////////////////////////////////////////////////////////////	   
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ�� 
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/4
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵�� 
//V1.1   20160120
//����_ttywrch����,�޸�ʹ��HAL��ʱ,���ܱ�__use_no_semihosting�Ĵ���. 
////////////////////////////////////////////////////////////////////////////////// 

#define USART_REC_LEN  			200  	//�����������ֽ��� 200

	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	

void uart_init(u32 pclk2,u32 bound); 
u16 uart_recv(u8 *rx_str);
void uart_send(u8 *tx_str, u16 tx_len);
#endif	   
















