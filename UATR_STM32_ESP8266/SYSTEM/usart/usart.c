/**************************************************************************
 * @file     usart.c
 * @brief    usart驱动
 * @version  V1.00
 * @date     20. June 2020
 * @author   CRZ
***************************************************************************/
#include "sys.h"
#include "usart.h"	  
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)  
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout; 
//解决HAL库使用时,某些情况可能报错的bug
/***************************************************
 * @brief    解决HAL库使用时,某些情况可能报错的bug 
 * @param    int ch 
 * @retval   int ch
 * @version  V1.00
 * @date     20. June 2020
 * @author   CRZ
***************************************************/
int _ttywrch(int ch)    
{
    ch=ch;
	return ch;
}  
/***************************************************
 * @brief    定义_sys_exit()以避免使用半主机模式
 * @param    int x
 * @retval   无
 * @version  V1.00
 * @date     20. June 2020
 * @author   CRZ
 **************************************************/
void _sys_exit(int x) 
{ 
	x = x; 
} 
/***************************************************
 * @brief    重定义fputc函数 
 * @param    int ch 
 *					 FILE *f
 * @retval   int ch
 * @version  V1.00
 * @date     20. June 2020
 * @author   CRZ
***************************************************/
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 


//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
/***************************************************
 * @brief    USART1中断句柄 
 * @param    None 
 * @retval   None
 * @version  V1.00
 * @date     20. June 2020
 * @author   CRZ
***************************************************/
void USART1_IRQHandler(void)
{
	u8 res;	

	if(USART1->SR&(1<<5))//接收到数据
	{	 
		res = USART1->DR; 
		if((USART_RX_STA&0x8000) == 0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(res!=0x0a)
					USART_RX_STA = 0;//接收错误,重新开始
				else 
					USART_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(res==0x0d)
					USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))
						USART_RX_STA = 0;//接收数据错误,重新开始接收	  
				}		 
			}
		}  		 									     
	} 
} 										 
/***************************************************
 * @brief    USART1初始化
 * @param    u32 pclk2
 *           u32 bound
 * @retval   None
 * @version  V1.00
 * @date     20. June 2020
 * @author   CRZ
***************************************************/
void uart_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV@OVER8=0
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分@OVER8=0 
  mantissa<<=4;
	mantissa+=fraction; 
	RCC->AHB1ENR|=1<<0;   	//使能PORTA口时钟  
	RCC->APB2ENR|=1<<4;  	//使能串口1时钟 
	GPIO_Set(GPIOA,PIN9|PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PA9,PA10,复用功能,上拉输出
 	GPIO_AF_Set(GPIOA,9,7);	//PA9,AF7
	GPIO_AF_Set(GPIOA,10,7);//PA10,AF7  	   
	//波特率设置
 	USART1->BRR=mantissa; 	//波特率设置	 
	USART1->CR1&=~(1<<15); 	//设置OVER8=0 
	USART1->CR1|=1<<3;  	//串口发送使能 

	//使能接收中断 
	USART1->CR1|=1<<2;  	//串口接收使能
	USART1->CR1|=1<<5;    	//接收缓冲区非空中断使能	    	
	MY_NVIC_Init(3,3,USART1_IRQn,2);//组2，最低优先级 

	USART1->CR1|=1<<13;  	//串口使能
}
/***************************************************
 * @brief    串口接收函数
 * @param    u8 *rx_str 
 * @retval   u16 rx_len
 * @version  V1.00
 * @date     20. June 2020
 * @author   CRZ
***************************************************/
u16 uart_recv(u8 *rx_str)
{
	u16 rx_len = 0;
	u16 i = 0;
	
	if(USART_RX_STA&0x8000)
	{					   
		rx_len = USART_RX_STA&0x3fff;//得到此次接收到的数据长度
		for(i=0; i<rx_len; i++)
		{
			rx_str[i] = USART_RX_BUF[i];
		}
		USART_RX_STA=0;
	}
	return rx_len;
}
/***************************************************
 * @brief    串口发送函数
 * @param    u16 rx_len 
 *           u8 *tx_str
 * @retval   None
 * @version  V1.00
 * @date     20. June 2020
 * @author   CRZ
***************************************************/
void uart_send(u8 *tx_str, u16 tx_len)
{
	u16 i = 0;
	
	for(i=0; i<tx_len; i++)
	{
		USART1->DR=tx_str[i];
		while((USART1->SR&0X40)==0);//等待发送结束
	}
}







