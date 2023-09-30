#include "dht11.h"

//IO��������
//#define DHT11_IO_IN()  {GPIOB->CRH&=0XFFFFFFF0;GPIOB->CRH|=0X00000008;}
//#define DHT11_IO_OUT() {GPIOB->CRH&=0XFFFFFFF0;GPIOB->CRH|=0X00000003;}
////IO��������											   
//#define	DHT11_DQ_OUT PBout(8) //���ݶ˿�	PB8
//#define	DHT11_DQ_IN  PBin(8)  //���ݶ˿�	PB8

u8 DHT11_Init(void);//��ʼ��DHT11
u8 DHT11_Read_Byte(void);//����һ���ֽ�
u8 DHT11_Read_Bit(void);//����һ��λ
u8 DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11    

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
//��ʼ��DHT11��IO�� ͬʱ���DHT11�Ĵ���
//����1:������
//����0:����    	 
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
	HAL_GPIO_Init(GPIO_X, &GPIO_InitStructure);//��ʼ��
	DHT11_Rst();
	return DHT11_Check();
}		
		
//��λDHT11
void DHT11_Rst(void)	   
{                 
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIO_X, &GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIO_X,GPIO_PIN,GPIO_PIN_RESET); //����DQ
	HAL_Delay(25);    	//��������18ms
	HAL_GPIO_WritePin(GPIO_X,GPIO_PIN,GPIO_PIN_RESET); 	//DQ=1 
	Test_DelayUs_Parameter(delayUs[0]);     	//��������20~40us
}

//�ȴ�DHT11�Ļ�Ӧ
//����1:δ��⵽DHT11�Ĵ���
//����0:����
u8 DHT11_Check(void) 	   
{   
	u8 retry[2]={0};
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIO_X, &GPIO_InitStructure);//SET INPUT	 
  while ((GPIO_X->IDR & GPIO_PIN)&&retry[0]++<100)//DHT11������40~80us
	{
		Test_DelayUs_Parameter(delayUs[1]);   //�ȴ�1us (72MHz��Ƶ)
	};	 
	if(retry[0]>=100)return 1;  
	else retry[1]=0;              
  while (!(GPIO_X->IDR & GPIO_PIN)&&retry[1]++<100)//DHT11���ͺ���ٴ�����40~80us
	{
		Test_DelayUs_Parameter(delayUs[1]);   //�ȴ�1us (72MHz��Ƶ)
	};
	if(retry[1]>=100)return 1;	   
	return 0;                    
}
//��DHT11��ȡһ��λ
//����ֵ��1/0
u8 DHT11_Read_Bit(void) 			 
{
 	u8 retry[2]={0};
	while((GPIO_X->IDR & GPIO_PIN) && retry[0]++<100)//�ȴ���Ϊ�͵�ƽ
	{
		Test_DelayUs_Parameter(delayUs[1]);   //�ȴ�1us (72MHz��Ƶ)
	}
	while(!(GPIO_X->IDR & GPIO_PIN)&& retry[1]++<100)//�ȴ���ߵ�ƽ
	{
		Test_DelayUs_Parameter(delayUs[1]);   //�ȴ�1us (72MHz��Ƶ)
	}
	Test_DelayUs_Parameter(delayUs[2]);   //�ȴ�40us
	if(GPIO_X->IDR & GPIO_PIN)return 1;
	else return 0;		   
}

//��DHT11��ȡһ���ֽ�
//����ֵ������������
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
//��DHT11��ȡһ������
//temp:�¶�ֵ(��Χ:0~50��)
//humi:ʪ��ֵ(��Χ:20%~90%)
//����ֵ��0,����;1,��ȡʧ��
u8 DHT11_Read_Data(u8 *temp,u8 *humi)    
{        
	u8 i,buf[5]={0};
	if((buf[0] = DHT11_Init())==0)    //  DHT11_Init    DHT11_Check
	{
		for(i=0;i<5;i++)//��ȡ40λ����
		{
			buf[i] = DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0] + 12;   //����ʪ��ֵ  //cs zj
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























