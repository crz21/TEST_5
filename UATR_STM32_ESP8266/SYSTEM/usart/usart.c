/**************************************************************************
 * @file     usart.c
 * @brief    usart����
 * @version  V1.00
 * @date     20. June 2020
 * @author   CRZ
***************************************************************************/
#include "sys.h"
#include "usart.h"	  
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)  
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout; 
//���HAL��ʹ��ʱ,ĳЩ������ܱ����bug
/***************************************************
 * @brief    ���HAL��ʹ��ʱ,ĳЩ������ܱ����bug 
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
 * @brief    ����_sys_exit()�Ա���ʹ�ð�����ģʽ
 * @param    int x
 * @retval   ��
 * @version  V1.00
 * @date     20. June 2020
 * @author   CRZ
 **************************************************/
void _sys_exit(int x) 
{ 
	x = x; 
} 
/***************************************************
 * @brief    �ض���fputc���� 
 * @param    int ch 
 *					 FILE *f
 * @retval   int ch
 * @version  V1.00
 * @date     20. June 2020
 * @author   CRZ
***************************************************/
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 


//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
/***************************************************
 * @brief    USART1�жϾ�� 
 * @param    None 
 * @retval   None
 * @version  V1.00
 * @date     20. June 2020
 * @author   CRZ
***************************************************/
void USART1_IRQHandler(void)
{
	u8 res;	

	if(USART1->SR&(1<<5))//���յ�����
	{	 
		res = USART1->DR; 
		if((USART_RX_STA&0x8000) == 0)//����δ���
		{
			if(USART_RX_STA&0x4000)//���յ���0x0d
			{
				if(res!=0x0a)
					USART_RX_STA = 0;//���մ���,���¿�ʼ
				else 
					USART_RX_STA|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(res==0x0d)
					USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))
						USART_RX_STA = 0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}  		 									     
	} 
} 										 
/***************************************************
 * @brief    USART1��ʼ��
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
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV@OVER8=0
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������@OVER8=0 
  mantissa<<=4;
	mantissa+=fraction; 
	RCC->AHB1ENR|=1<<0;   	//ʹ��PORTA��ʱ��  
	RCC->APB2ENR|=1<<4;  	//ʹ�ܴ���1ʱ�� 
	GPIO_Set(GPIOA,PIN9|PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PA9,PA10,���ù���,�������
 	GPIO_AF_Set(GPIOA,9,7);	//PA9,AF7
	GPIO_AF_Set(GPIOA,10,7);//PA10,AF7  	   
	//����������
 	USART1->BRR=mantissa; 	//����������	 
	USART1->CR1&=~(1<<15); 	//����OVER8=0 
	USART1->CR1|=1<<3;  	//���ڷ���ʹ�� 

	//ʹ�ܽ����ж� 
	USART1->CR1|=1<<2;  	//���ڽ���ʹ��
	USART1->CR1|=1<<5;    	//���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(3,3,USART1_IRQn,2);//��2��������ȼ� 

	USART1->CR1|=1<<13;  	//����ʹ��
}
/***************************************************
 * @brief    ���ڽ��պ���
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
		rx_len = USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
		for(i=0; i<rx_len; i++)
		{
			rx_str[i] = USART_RX_BUF[i];
		}
		USART_RX_STA=0;
	}
	return rx_len;
}
/***************************************************
 * @brief    ���ڷ��ͺ���
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
		while((USART1->SR&0X40)==0);//�ȴ����ͽ���
	}
}







