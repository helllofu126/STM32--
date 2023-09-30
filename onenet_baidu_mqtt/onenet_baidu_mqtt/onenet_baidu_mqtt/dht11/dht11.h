#ifndef __DHT11_H
#define __DHT11_H 

#include "stm32f1xx_hal.h"
#include "gpio.h"

#define u8  unsigned char
#define u16 unsigned int
	

/*微秒级延时函数，输入为待评估的参数，输出为在此参数下SysTick->VAL前后的差值
例如：当estimate = 7 时，返回值 = 70  （当工作于72MHz时钟下时，约为1us）
      当estimate = 319时，返回值 = 2878（当工作于72MHz时钟下时，约为40us）   
      可以在主函数中调用该参数，将返回值通过串口输出至上位机进行查看*/
u16 Test_DelayUs_Parameter(uint16_t estimate);  


// 指定微秒级延时诸参数，比如在72MHz时钟下，依次为 280（约35us）, 7（约1us）, 319（约40us）
void Assign_delayUsPara(uint16_t delay20_40us,uint16_t delay1us,uint16_t delay40us);


void Assign_DataPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);  //分配单线总线数据引脚  PB8


u8 DHT11_Read_Data(u8 *temp,u8 *humi);//读取温湿度,返回0表示读取成功



#endif















