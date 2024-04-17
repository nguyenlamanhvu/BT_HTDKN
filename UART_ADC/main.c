
#include "stm32f4xx.h"
#include "system_timetick.h"
#include "string.h"
#include <stdio.h>

#define ADC1_DR_ADDRESS ((uint32_t) 0x4001204C)
#define BUFFER_SIZE 10

int16_t data;
char TxBuffer[BUFFER_SIZE];
uint16_t ADC_Value;

void init_main(void);
void IntToASCII(int n, char *string);

int main(void)
{
	/* Enable SysTick at 10ms interrupt */
	SysTick_Config(SystemCoreClock/100);

	init_main();
	data = 1234;
	while(1){
		if(tick_count == 100){
			tick_count = 0;		
//			IntToASCII(data,TxBuffer);
			sprintf(TxBuffer, "%.2f\n", (float)ADC_Value*3.3/4095.0);
			DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF6);
			DMA1_Stream6->NDTR = strlen(TxBuffer);
			DMA_Cmd(DMA1_Stream6, ENABLE);
		}		 
	}
}

void init_main(void)
{
  GPIO_InitTypeDef 			GPIO_InitStructure; 
	USART_InitTypeDef 		USART_InitStructure; 
	DMA_InitTypeDef  			DMA_InitStructure;  
	ADC_InitTypeDef 			ADC1_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonStructure;
   
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  /* Enable UART clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	/* Enable DMA1 clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	/* Enable ADC1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	/* Enable DMA2 clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);

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
	
	/* GPIO Configuration for ADC1_Channel1 */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
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
	
	/* Enable UART4 DMA */
  USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE); 
	
	/* DMA1 Stream6 Channel4 for USART2 Tx configuration */			
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)TxBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA1_Stream6, &DMA_InitStructure);
  DMA_Cmd(DMA1_Stream6, ENABLE);
	
	/*ADC_Common*/
	ADC_CommonStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	
	ADC_CommonInit(&ADC_CommonStructure);
	
	/*ADC_Init*/
	ADC1_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC1_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC1_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC1_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC1_InitStructure.ADC_NbrOfConversion = 1;
	ADC1_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	
	ADC_Init(ADC1,&ADC1_InitStructure);
	
	/*Configure DMA2_Channel_0/Stream0(support ADC1)*/
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC_Value;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	DMA_Cmd(DMA2_Stream0, ENABLE);
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_3Cycles);
	
//	DMA_ITConfig (DMA2_Stream0,DMA_IT_TC,ENABLE);
//	
//	NVIC_InitTypeDef NVIC_InitStructure;

//	/* Enable the DMA1_Stream6 Interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable DMA request after last transfer (Single-ADC mode) */
	ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE);
	/* Enable ADC1 DMA */
	ADC_DMACmd (ADC1,ENABLE);
	/* Enable ADC1 */
	ADC_Cmd(ADC1,ENABLE);
	// Start ADC conversion
	ADC_SoftwareStartConv(ADC1);
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
	for(index = 0; index < strlen(string)/2; index++){
		char tmp = string[index];
		string[index] = string[strlen(string) - index -1];
		string[strlen(string) - index -1] = tmp;
	}
}

//void DMA2_Stream0_IRQHandler(void){
//	if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0)){
//		
//	}
//	DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TCIF0);
//}
