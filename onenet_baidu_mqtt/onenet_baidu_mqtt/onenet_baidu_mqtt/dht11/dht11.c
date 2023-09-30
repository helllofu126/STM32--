#include "dht11.h"

//IO方向设置
//#define DHT11_IO_IN()  {GPIOB->CRH&=0XFFFFFFF0;GPIOB->CRH|=0X00000008;}
//#define DHT11_IO_OUT() {GPIOB->CRH&=0XFFFFFFF0;GPIOB->CRH|=0X00000003;}
////IO操作函数											   
//#define	DHT11_DQ_OUT PBout(8) //数据端口	PB8
//#define	DHT11_DQ_IN  PBin(8)  //数据端口	PB8

u8 DHT11_Init(void);//初始化DHT11
u8 DHT11_Read_Byte(void);//读出一个字节
u8 DHT11_Read_Bit(void);//读出一个位
u8 DHT11_Check(void);//检测是否存在DHT11
void DHT11_Rst(void);//复位DHT11    

uint16_t delayUs[3] = {0};
GPIO_InitTypeDef  GPIO_InitStructure;	
GPIO_TypeDef *GPIO_X; 
uint32_t GPIO_PIN;

void Assign_DataPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_X = GPIOx;
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
	GPIO_InitStructure.Pin = GPIO_PIN = GPIO_Pin;
	return;
}

void Assign_delayUsPara(uint16_t delay20_40us,uint16_t delay1us,uint16_t delay40us)
{
	delayUs[0] = delay20_40us;
	delayUs[1] = delay1us;
	delayUs[2] = delay40us;
	return;
}
//初始化DHT11的IO口 同时检测DHT11的存在
//返回1:不存在
//返回0:存在    	 
u8 DHT11_Init(void)
{
	switch((uint32_t)GPIO_X)
	{
		case (uint32_t)GPIOA : 
			__HAL_RCC_GPIOA_CLK_ENABLE();
		  break;
		case (uint32_t)GPIOB : 
			__HAL_RCC_GPIOB_CLK_ENABLE();
		  break;
		case (uint32_t)GPIOC : 
			__HAL_RCC_GPIOC_CLK_ENABLE();
		  break;
		case (uint32_t)GPIOD : 
			__HAL_RCC_GPIOD_CLK_ENABLE();
		  break;
		case (uint32_t)GPIOE : 
			__HAL_RCC_GPIOE_CLK_ENABLE();
		  break;
    default :break;
	}
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP; 
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIO_X, &GPIO_InitStructure);//初始化
	DHT11_Rst();
	return DHT11_Check();
}		
		
//复位DHT11
void DHT11_Rst(void)	   
{                 
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIO_X, &GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIO_X,GPIO_PIN,GPIO_PIN_RESET); //拉低DQ
	HAL_Delay(25);    	//拉低至少18ms
	HAL_GPIO_WritePin(GPIO_X,GPIO_PIN,GPIO_PIN_RESET); 	//DQ=1 
	Test_DelayUs_Parameter(delayUs[0]);     	//主机拉高20~40us
}

//等待DHT11的回应
//返回1:未检测到DHT11的存在
//返回0:存在
u8 DHT11_Check(void) 	   
{   
	u8 retry[2]={0};
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIO_X, &GPIO_InitStructure);//SET INPUT	 
  while ((GPIO_X->IDR & GPIO_PIN)&&retry[0]++<100)//DHT11会拉低40~80us
	{
		Test_DelayUs_Parameter(delayUs[1]);   //等待1us (72MHz主频)
	};	 
	if(retry[0]>=100)return 1;  
	else retry[1]=0;              
  while (!(GPIO_X->IDR & GPIO_PIN)&&retry[1]++<100)//DHT11拉低后会再次拉高40~80us
	{
		Test_DelayUs_Parameter(delayUs[1]);   //等待1us (72MHz主频)
	};
	if(retry[1]>=100)return 1;	   
	return 0;                    
}
//从DHT11读取一个位
//返回值：1/0
u8 DHT11_Read_Bit(void) 			 
{
 	u8 retry[2]={0};
	while((GPIO_X->IDR & GPIO_PIN) && retry[0]++<100)//等待变为低电平
	{
		Test_DelayUs_Parameter(delayUs[1]);   //等待1us (72MHz主频)
	}
	while(!(GPIO_X->IDR & GPIO_PIN)&& retry[1]++<100)//等待变高电平
	{
		Test_DelayUs_Parameter(delayUs[1]);   //等待1us (72MHz主频)
	}
	Test_DelayUs_Parameter(delayUs[2]);   //等待40us
	if(GPIO_X->IDR & GPIO_PIN)return 1;
	else return 0;		   
}

//从DHT11读取一个字节
//返回值：读到的数据
u8 DHT11_Read_Byte(void)    
{  
  u8 i,dat;	
  dat=0;
	for (i=0;i<8;i++) 
	{
   	dat<<=1; 
	  dat |= DHT11_Read_Bit();
  }						    
  return dat;
}
//从DHT11读取一次数据
//temp:温度值(范围:0~50°)
//humi:湿度值(范围:20%~90%)
//返回值：0,正常;1,读取失败
u8 DHT11_Read_Data(u8 *temp,u8 *humi)    
{        
	u8 i,buf[5]={0};
	if((buf[0] = DHT11_Init())==0)    //  DHT11_Init    DHT11_Check
	{
		for(i=0;i<5;i++)//读取40位数据
		{
			buf[i] = DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0] + 12;   //修正湿度值  //cs zj
			*temp=buf[2];
		}
	}
	else return 1;   
	return 0;	         
}

u16 Test_DelayUs_Parameter(uint16_t estimate)
{
	uint16_t tt[2],t;     
	tt[0] = SysTick->VAL;  
	for(uint16_t i=0;i<estimate;i++) __NOP();   
	tt[1] = SysTick->VAL;
	t = ( tt[0]>tt[1] ? tt[0]-tt[1] :  tt[0]+SysTick->LOAD-tt[1]);
	return t;
}























