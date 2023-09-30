/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "adc.h"
#include "i2c.h"
//#include "stm32f1xx_hal_adc.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "esp8266.h"
#include "dht11.h"
#include "bh1750.h"
#include "MQ3.h"
#include "oled.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
_Bool LinkOK=0,u2rxFlag=0;
uint8_t temp=0,humi=0,testcode=0;
uint8_t userSensorList[150]={0};
uint8_t publishTopic[60]={0};
uint32_t timeTick,ill;
float mq3;
uint8_t t[]="tem:";
uint8_t h[]="hum:";
uint8_t m[]="MQ3:";
uint8_t i[]="ill:";
uint8_t A[]="hellow world!!";   //cs
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void oled_view(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
//	MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();

  MQ_init();

//  OLED_Init();
//	OLED_Display_On();
//	HAL_Delay(10);
//	
//	OLED_Clear();
//	OLED_ShowString(0,0,A,sizeof(A));	
//	HAL_Delay(10);
	
	Init_BH1750();
  /* USER CODE BEGIN 2 */
	
  Assign_delayUsPara(280,7,319);      //set for DHT11
	Assign_DataPin(GPIOB, GPIO_PIN_8);  //set for DHT11
	


	__HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE);
	while(!LinkOK)
	{
		ESP8266_Init(&huart2,NULL,0);
		HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,GPIO_PIN_RESET);   //cs
	  HAL_Delay(200);
		
		switch(Authenticate())
		{
			case 0:  LinkOK = 1; 
			         printf("Authentication passed, successfully connected to the server！\r\n");
			         break;
			case 1 : printf("协议版本错误！\r\n");
			         break;
			case 2 : printf("非法的clientID！\r\n");
			         break;
			case 3 : printf("服务不可用！\r\n");
			         break;
			case 4 : printf("用户名或密码错误！\r\n");
			         break;
			case 5 : printf("非法链接(比如token非法)！\r\n");
			         break;
			default: printf("未知错误！\r\n");
			         break;
		}
		HAL_Delay(200);
	}
	HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,GPIO_PIN_SET);   //cs
	HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET);  //cs
	
	#ifdef ONENET 
	uint8_t subscribeTopic[60]= "topic/505050/+/dp";        //+ 表示单级通配 参考https://blog.csdn.net/zsm180/article/details/123736316
	if(1==Subscribe(subscribeTopic,strlen(subscribeTopic)))
		printf("Successfully subscribed to topic .../dp\r\n");
	else printf("主题 .../dp 订阅不成功\r\n");
	
	memset(subscribeTopic,0,sizeof(subscribeTopic));
	sprintf(subscribeTopic,"topic/%s/%s/cmdp",PROID,DEVID);
	if(1==Subscribe(subscribeTopic,strlen(subscribeTopic)))
		printf("Successfully subscribed to topic .../cmdp\r\n");
	else printf("主题 .../cmdp 订阅不成功\r\n");
	#endif
	
	#ifdef NEWLAND
	uint8_t subscribeTopic[60] = "/sensor/cmdreq";
	if(1==Subscribe(subscribeTopic,strlen(subscribeTopic)))
		printf("成功订阅主题 /sensor/cmdreq\r\n");
	else printf("主题 /sensor/cmdreq 订阅不成功\r\n");
	
	memset(subscribeTopic,0,sizeof(subscribeTopic));
	sprintf(subscribeTopic,"topic/%s/%s/cmdp",PROID,DEVID);
	if(1==Subscribe(subscribeTopic,strlen(subscribeTopic)))
		printf("成功订阅主题 ../cmdp\r\n");
	else printf("主题 ../cmdp 订阅不成功\r\n");
	#endif
	
	#ifdef BAIDU
	uint8_t subscribeTopic[60];
	sprintf(subscribeTopic,"topic/%s/dp",DEVID);
	if(1==Subscribe(subscribeTopic,strlen(subscribeTopic)))
		printf("成功订阅主题 .../dp\r\n");
	else printf("主题 .../dp 订阅不成功\r\n");
	
	memset(subscribeTopic,0,sizeof(subscribeTopic));
	sprintf(subscribeTopic,"topic/%s/cmdp",DEVID);
	if(1==Subscribe(subscribeTopic,strlen(subscribeTopic)))
		printf("成功订阅主题 .../cmdp\r\n");
	else printf("主题 .../cmdp 订阅不成功\r\n");
	#endif
	
	HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,GPIO_PIN_RESET);   //cs
	HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET);   //cs
	
	timeTick = HAL_GetTick();
	PublishMsg();
	
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
  while (1)
  {
		if(HAL_GetTick()>0xffffffff-30000) HAL_NVIC_SystemReset();
		else if(HAL_GetTick()-timeTick==6000)
		{
			timeTick = HAL_GetTick();
			PublishMsg();

			printf("%s\n",userSensorList);
//			oled_view();       //OLED显示
		}
		else if(HAL_GetTick()%3000==0)  
		{
			
			if(!KeepAlive()) HAL_NVIC_SystemReset();
		}
			
		if(u2rxFlag)
		{
			u2rxFlag = 0;
			HAL_UART_Transmit(&huart1,esp8266_buf,esp8266_cnt,50);  //cs
			CmdAnalyz();
		}
		

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

int fputc(int ch,FILE *f)
{
	HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,1);
	return ch;
}


void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2)
	{
		if(__HAL_UART_GET_FLAG(huart,UART_FLAG_RXNE))
		{
			__HAL_UART_DISABLE_IT(huart,UART_IT_RXNE);
			__HAL_UART_CLEAR_FLAG(huart,UART_FLAG_RXNE);
			if(esp8266_cnt>=200-3) esp8266_cnt=0;
			esp8266_buf[esp8266_cnt++] = (uint8_t)huart->Instance->DR;
			__HAL_UART_ENABLE_IT(huart,UART_IT_RXNE);
			__HAL_UART_ENABLE_IT(huart,UART_IT_IDLE);
		}
		if(__HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE))
		{
			__HAL_UART_DISABLE_IT(huart,UART_IT_IDLE);
			__HAL_UART_CLEAR_IDLEFLAG(huart);
      u2rxFlag = 1;
		}
	}
}

void PublishMsg(void)
{
	HAL_Delay(100);
	DHT11_Read_Data(&temp,&humi);  //获取温湿度
  ill=Value_GY30();     //获取光照
	mq3=get_MQ_value();   //获取酒精

	
	//补串口
	if(ill>60){
		HAL_GPIO_WritePin(RELAY_GPIO_Port,RELAY_Pin,GPIO_PIN_RESET); //开继电器
	}else{
		HAL_GPIO_WritePin(RELAY_GPIO_Port,RELAY_Pin,GPIO_PIN_SET);  //关继电器
	}

	
	
	memset(userSensorList,0,sizeof(userSensorList));
	
	#if defined(BAIDU) || defined(ONENET)
	sprintf(userSensorList,//"\"data\":{\"tem\":%2d,\"hum\":%2d},"
												 "\"tem\":%2d,"
												 "\"hum\":%2d,"
												 "\"ill\":%d,"
	                       "\"MQ3\":%.2f,"
	                       "\"RELAY\":%1d,"
	                       "\"actuator\":{\"LED0\":%1d,\"LED1\":%1d}",
//	                       "\"LED0\":%1d,"
//	                       "\"LED1\":%1d,,"
//												 "\"actuator\":{\"LED0\":%1d,\"LED1\":%1d,}"
//	                       "\"RELAY\":%1d",
	                       temp,humi,ill,mq3,
												 (uint8_t)HAL_GPIO_ReadPin(RELAY_GPIO_Port,RELAY_Pin)^1,
	                       (uint8_t)HAL_GPIO_ReadPin(LED0_GPIO_Port,LED0_Pin)^1,
	                       (uint8_t)HAL_GPIO_ReadPin(LED1_GPIO_Port,LED1_Pin)^1);  
	memset(publishTopic,0,sizeof(publishTopic));   

//	HAL_UART_Transmit(&huart1,userSensorList,150,50);  //通过串口1将数据转发出去
	
	#endif
	
	#ifdef BAIDU
	sprintf(publishTopic,"topic/%s/dp",DEVID);  //自定义主题，别的终端可以订阅。百度智能云有限制，得先在云上登记主题，然后才允许设备向该主题发布信息
	Publish(publishTopic,strlen(publishTopic),userSensorList,strlen(userSensorList));
	#endif
	
	#ifdef ONENET
  sprintf(publishTopic,"topic/%s/%s/dp",PROID,DEVID);  //自定义主题，别的终端可以订阅
	Publish(publishTopic,strlen(publishTopic),userSensorList,strlen(userSensorList));
	HAL_Delay(200);
	memset(publishTopic,0,sizeof(publishTopic));
	sprintf(publishTopic,"$dp");
	Publish(publishTopic,strlen(publishTopic),userSensorList,strlen(userSensorList));
	#endif
	
	#ifdef NEWLAND
	static uint8_t msgid=0;
	sprintf(userSensorList,"\"datatype\":1,\"datas\":{\"tem\":%2d,\"hum\":%2d,\"LED0\":%1d,\"LED1\":%1d,\"RELAY\":%1d},\"msgid\":%3d",
	                       temp,humi,
	                       (uint8_t)HAL_GPIO_ReadPin(LED0_GPIO_Port,LED0_Pin)^1,
	                       (uint8_t)HAL_GPIO_ReadPin(LED1_GPIO_Port,LED1_Pin)^1,
	                       (uint8_t)HAL_GPIO_ReadPin(RELAY_GPIO_Port,RELAY_Pin)^1,
												 msgid++);   
	memset(publishTopic,0,sizeof(publishTopic));
	sprintf(publishTopic,"/sys/{projectId}/{device}/sensor/datas");
	Publish(publishTopic,strlen(publishTopic),userSensorList,strlen(userSensorList));
	#endif

	return;
}


// 云平台发送的cmdStr形如：{"cmd":{"LED0":1}}
_Bool CmdAnalyzCallback(uint8_t *cmdStr,uint8_t *cmdAck)
{
	_Bool res = 0;
	uint8_t *ptr=strstr(cmdStr,"{\"cmd\":{");
	if(ptr)
	{
		//-------------------------LED0-----------------------------
		HAL_Delay(2);
		if(strstr(ptr,"\"LED0\":1")) 
		{
			HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,GPIO_PIN_RESET);
			sprintf(cmdAck,"{\"RESULT\":\"OK\",\"LED0\":\"ON\"}");
			res = 1;
		}
		if((res==0) && strstr(ptr,"\"LED0\":0")) 
		{
			HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,GPIO_PIN_SET);
			sprintf(cmdAck,"{\"RESULT\":\"OK\",\"LED0\":\"OFF\"}");
			res = 1;
		}
		if((res==0) && strstr(ptr,"\"LED0\":2")) 
		{
			HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
			sprintf(cmdAck,"{\"RESULT\":\"OK\",\"LED0\":\"%s\"}",
			               HAL_GPIO_ReadPin(LED0_GPIO_Port,LED0_Pin)?"OFF":"ON");
			res = 1;
		}
		//-------------------------LED1-----------------------------
		if((res==0) && strstr(ptr,"\"LED1\":1")) 
		{
			HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET);
			sprintf(cmdAck,"{\"RESULT\":\"OK\",\"LED1\":\"ON\"}");
			res = 1;
		}
		if((res==0) && strstr(ptr,"\"LED1\":0")) 
		{
			HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET);
			sprintf(cmdAck,"{\"RESULT\":\"OK\",\"LED1\":\"OFF\"}");
			res = 1;
		}
		if((res==0) && strstr(ptr,"\"LED1\":2")) 
		{
			HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
			sprintf(cmdAck,"{\"RESULT\":\"OK\",\"LED1\":\"%s\"}",
			               HAL_GPIO_ReadPin(LED1_GPIO_Port,LED1_Pin)?"OFF":"ON");
			res = 1;
		}
		//-------------------------RELAY-----------------------------
		if((res==0) && strstr(ptr,"\"RELAY\":1")) 
		{
			HAL_GPIO_WritePin(RELAY_GPIO_Port,RELAY_Pin,GPIO_PIN_RESET);
			sprintf(cmdAck,"{\"RESULT\":\"OK\",\"RELAY\":\"ON\"}");
			res = 1;
		}
		if((res==0) && strstr(ptr,"\"RELAY\":0")) 
		{
			HAL_GPIO_WritePin(RELAY_GPIO_Port,RELAY_Pin,GPIO_PIN_SET);
			sprintf(cmdAck,"{\"RESULT\":\"OK\",\"RELAY\":\"OFF\"}");
			res = 1;
		}
		if((res==0) && strstr(ptr,"\"RELAY\":2")) 
		{
			HAL_GPIO_TogglePin(RELAY_GPIO_Port,RELAY_Pin);
			sprintf(cmdAck,"{\"RESULT\":\"OK\",\"RELAY\":\"%s\"}",
			               HAL_GPIO_ReadPin(RELAY_GPIO_Port,RELAY_Pin)?"OFF":"ON");
			res = 1;
		}
		//------------------------OTHERS----------------------------
		
		
		//----------------------------------------------------------
		
		if(res==0) 
		{
			sprintf(cmdAck,"{\"RESULT\":\"ERROR\"}");
			res = 1;   //这里仍然要置返回值为”真“，否则本客户端就不给云平台发应答信息了
		}
		else
    {
			timeTick = HAL_GetTick();
			PublishMsg();	
		}
	}
	HAL_Delay(2);
  return res;
}


extern uint8_t cmdAckStr[100];
_Bool GetSubscribeMsgCallback(uint8_t *topicAndDatas)
{
	uint8_t *ptr = strstr(topicAndDatas,"/cmdp");
//	printf(topicAndDatas);                    //cs
	HAL_Delay(2);
	if(ptr)	CmdAnalyzCallback(ptr,cmdAckStr);  //借用一下处理平台下发命令时的回调函数
	return 1;
}

//OLED显示
void oled_view(void){
	HAL_Delay(30);
	OLED_Clear();
	//OLED_ShowString(0,0,A,sizeof(A));   //cs
	HAL_Delay(10);
  sprintf(t,"tem:%2d",temp);
  OLED_ShowString(0,0,t,16); 
	HAL_Delay(1);
	sprintf(h,"hum:%2d",humi);
	OLED_ShowString(0,2,h,16); 
	HAL_Delay(1);
	sprintf(m,"MQ-3:%.2f",mq3);
	OLED_ShowString(0,4,m,16);   
	HAL_Delay(1);
	sprintf(i,"ill:%d",ill); 
	OLED_ShowString(0,6,i,16);  
	HAL_Delay(30);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
