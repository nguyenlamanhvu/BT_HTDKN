
#include "stm32f4xx.h"
#include "system_timetick.h"
#include "string.h"

#define BUFFER_SIZE 10
int16_t data;
char DataBuffer[BUFFER_SIZE];

void init_main(void);
void IntToASCII(int n, char *string);
int main(void)
{
	/* Enable SysTick at 10ms interrupt */
	SysTick_Config(SystemCoreClock/100);

	init_main();
	data = 12345;
	while(1){
		if(tick_count == 100){
			tick_count = 0;		
			IntToASCII(data,DataBuffer);
			int8_t index = strlen(DataBuffer);
			while(index >= 0){
				while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
				USART_SendData(USART2, (uint8_t)DataBuffer[--index]);
			}
//			USART_SendData(USART2,(uint8_t)65);		
//			while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
//			USART_SendData(USART2,(uint8_t)66);
		}		 
	}
}

void init_main(void)
{
  GPIO_InitTypeDef 	GPIO_InitStructure; 
	USART_InitTypeDef USART_InitStructure;   
   
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  /* Enable UART clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  /* Connect USART2 pins to AF7 */  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); 

  /* GPIO Configuration for USART2 Tx */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* GPIO Configuration for USART Rx */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
       
  /* USARTx configured as follow:
		- BaudRate = 115200 baud  
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART2, &USART_InitStructure);

  /* Enable USART */
  USART_Cmd(USART2, ENABLE);
}

void IntToASCII(int n, char *string)
{
	uint8_t index = 0;
	while(n){
		string[index++] = (n%10+'0');
		n /= 10;
	}
	while(index < BUFFER_SIZE){
		string[index++] = 0;
	}
}
