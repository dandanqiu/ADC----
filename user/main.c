#include "stm32f10x.h"
#include "usart.h"
void usart_init(void);
void tim_init(void);
void adc_init(void);
int main(void)
{
	usart_init();
	tim_init();
	adc_init();
	while(1)
	{
		//等待ADC1的注入通道转换完成标志位被置位
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_JEOC) == RESET);
		//获取ADC1的注入通道转换结果
		uint16_t adc_value = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
		//清除ADC1的注入通道转换完成标志位
		ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);
		//转换为电压
		float voltage = adc_value * (3.3/4095);
		//发送电压值
		My_USART_Printf(USART1,"%.3f\n", voltage);

	}
}
void usart_init(void)
{
	//初始化PA9
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//初始化usart1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_InitTypeDef USART_InitStructure = {0};
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);
}
void tim_init(void)
{
	//初始化TIM1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
	TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	//开启TRGO输出
	TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);
	//开启TIM1
	TIM_Cmd(TIM1, ENABLE);
}
void adc_init(void)
{
	//初始化PA0
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//初始化ADC1
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_InitTypeDef ADC_InitStructure = {0};
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	//初始化注入序列
	ADC_InjectedSequencerLengthConfig(ADC1, 1);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_13Cycles5);
	ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_TRGO);
	ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);
	//开启ADC1
	ADC_Cmd(ADC1, ENABLE);
}

