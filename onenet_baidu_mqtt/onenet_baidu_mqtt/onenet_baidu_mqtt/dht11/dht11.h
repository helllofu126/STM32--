#ifndef __DHT11_H
#define __DHT11_H 

#include "stm32f1xx_hal.h"
#include "gpio.h"

#define u8  unsigned char
#define u16 unsigned int
	

/*΢�뼶��ʱ����������Ϊ�������Ĳ��������Ϊ�ڴ˲�����SysTick->VALǰ��Ĳ�ֵ
���磺��estimate = 7 ʱ������ֵ = 70  ����������72MHzʱ����ʱ��ԼΪ1us��
      ��estimate = 319ʱ������ֵ = 2878����������72MHzʱ����ʱ��ԼΪ40us��   
      �������������е��øò�����������ֵͨ�������������λ�����в鿴*/
u16 Test_DelayUs_Parameter(uint16_t estimate);  


// ָ��΢�뼶��ʱ�������������72MHzʱ���£�����Ϊ 280��Լ35us��, 7��Լ1us��, 319��Լ40us��
void Assign_delayUsPara(uint16_t delay20_40us,uint16_t delay1us,uint16_t delay40us);


void Assign_DataPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);  //���䵥��������������  PB8


u8 DHT11_Read_Data(u8 *temp,u8 *humi);//��ȡ��ʪ��,����0��ʾ��ȡ�ɹ�



#endif















