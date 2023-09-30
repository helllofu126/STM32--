#include "esp8266.h"

enum MqttPacketType
{
  MQTT_PKT_CONNECT = 1, /**< 1 �����������ݰ�*/
  MQTT_PKT_CONNACK,     /**< 2 ����ȷ�����ݰ�*/
  MQTT_PKT_PUBLISH,     /**< 3 �����������ݰ�*/
  MQTT_PKT_PUBACK,      /**< 4 ����ȷ�����ݰ�*/
  MQTT_PKT_PUBREC,      /**< 5 ���������ѽ������ݰ���Qos 2ʱ���ظ�MQTT_PKT_PUBLISH */
  MQTT_PKT_PUBREL,      /**< 6 ���������ͷ����ݰ��� Qos 2ʱ���ظ�MQTT_PKT_PUBREC */
	MQTT_PKT_PUBCOMP,     /**< 7 ����������ݰ��� Qos 2ʱ���ظ�MQTT_PKT_PUBREL */
	MQTT_PKT_SUBSCRIBE,   /**< 8 �������ݰ� */
	MQTT_PKT_SUBACK,      /**< 9 ����ȷ�����ݰ� */
	MQTT_PKT_UNSUBSCRIBE, /**< 10ȡ���������ݰ� */
	MQTT_PKT_UNSUBACK,    /**< 11ȡ������ȷ�����ݰ� */
	MQTT_PKT_PINGREQ,     /**< 12ping ���ݰ� */
	MQTT_PKT_PINGRESP,    /**< 13ping ��Ӧ���ݰ� */
	MQTT_PKT_DISCONNECT,  /**< 14�Ͽ��������ݰ� */
//	MQTT_PKT_CMD  		    /**< 15�����·����ݰ� */
};

enum MqttAuthenticateResult
{
	MQTT_SUCCESS = 0,    /**< 0 ��Ȩͨ�����ɹ����ӷ�����*/
	MQTT_ERR_PRO_LEVEL,  /**< 1 Э��汾���� */
	MQTT_ERR_CLIENTID,   /**< 2 �Ƿ���clientID */
	MQTT_ERR_SRV_UNAV,   /**< 3 ���񲻿��� */
	MQTT_ERR_USER_KEY,   /**< 4 �û������������ */
	MQTT_ERR_ILL_LNK     /**< 5 �Ƿ�����(����token�Ƿ�) */
};


/*��WIFIģ�鷢��ATָ�����鷵���ִ����Ƿ���Ŀ��ؼ��ʣ��趨��ʱ�ȴ�ʱ�䣨��λΪms��*/
_Bool ESP8266_SendCmd(uint8_t *cmd, uint8_t *res, uint16_t time);

/*���á�AT+CIPSEND��ָ��ͨ��8266�������ݰ�*/
_Bool ESP8266_SendData(unsigned char *data, unsigned len);



UART_HandleTypeDef *huartWifi = NULL;
GPIO_TypeDef * GPIOx;
uint16_t RESET_PIN;

uint8_t esp8266_buf[200],trans_buf[200],*ptrTrans=NULL;
uint16_t esp8266_cnt=0,total_len=0;
uint16_t packetID = 0;


/*�����ڻ�������esp8266_buf�� �� ��������ֽ�����esp8266_cnt��*/
void ESP8266_Clear(void)
{
	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;
}



void Transbuf_Clear(void)
{
	memset(trans_buf, 0, sizeof(trans_buf));
	total_len = 0;
}



void UartWifi_SendString(unsigned char *str, unsigned len)
{
  HAL_UART_Transmit(huartWifi,str,len,100);
	return;
}



_Bool ESP8266_SendCmd(uint8_t *cmd, uint8_t *res, uint16_t time)
{	
	UartWifi_SendString((unsigned char *)cmd, strlen((const char *)cmd));
	time /= 10;
	do{
		HAL_Delay(10);
		if(strstr((const char *)esp8266_buf, res) != NULL)		//����������ؼ���
		{
			HAL_Delay(50);
			break;
		}
	}while(--time);
	HAL_Delay(200);
	if(time>0) return 1;
	
	return 0;
}



char cmdBuf[32];
_Bool ESP8266_SendData(unsigned char *data, unsigned len)
{
	uint8_t tim = 120;
	_Bool res = 0;
	memset(cmdBuf,0,sizeof(cmdBuf));	
	ESP8266_Clear();								//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//��������
	if(ESP8266_SendCmd(cmdBuf, ">", 200))				//�յ���>��ʱ���Է�������
	{
		UartWifi_SendString(data, len);		//�����豸������������   
	}
	ESP8266_Clear();								//��ս��ջ���
	
	do{
		HAL_Delay(20);
	  if(strstr((const char *)esp8266_buf,"SEND OK"))
		{			 
			res = 1;
			HAL_Delay(30);
			break;
		}
	}while(--tim);	
//	HAL_UART_Transmit(&huart1,esp8266_buf,esp8266_cnt,50);  //cs
	return res;
}



void ESP8266_Init(UART_HandleTypeDef *huart,GPIO_TypeDef *reset_port,uint16_t reset_pin)
{
	huartWifi = huart;
	
	if(reset_port!=NULL)
	{
		GPIOx = reset_port;
		RESET_PIN = reset_pin;
		GPIO_InitTypeDef GPIO_InitStruct = {0};	
		switch((uint32_t)GPIOx)
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
		//ESP8266��λ����
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pin = RESET_PIN;					
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
		HAL_GPIO_WritePin(GPIOx, RESET_PIN, GPIO_PIN_RESET);
		HAL_Delay(250);
		HAL_GPIO_WritePin(GPIOx, RESET_PIN, GPIO_PIN_SET);
	}
  HAL_Delay(500);
	ESP8266_Clear();
	while(!ESP8266_SendCmd("AT+RESTORE\r\n", "OK", 300)) HAL_Delay(500);
//	printf(esp8266_buf);   //cs
	
	ESP8266_Clear();
	while(!ESP8266_SendCmd("AT+RST\r\n", "OK", 500)) HAL_Delay(500);
//	printf(esp8266_buf);   //cs
	
	ESP8266_Clear();
	while(!ESP8266_SendCmd("ATE0\r\n", "OK", 200)) HAL_Delay(500);
//	printf(esp8266_buf);   //cs
	
	ESP8266_Clear();
	while(!ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK", 200)) HAL_Delay(500);
//	printf(esp8266_buf);   //cs
	
	ESP8266_Clear();
	while(!ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK", 200))  HAL_Delay(500);
//	printf(esp8266_buf);   //cs
	
	ESP8266_Clear();
	while(!ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP", 200)) HAL_Delay(500);
//	printf(esp8266_buf);   //cs
	
	ESP8266_Clear();
	while(!ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT", 200)) HAL_Delay(500);
//	printf(esp8266_buf);   //cs
	
	printf("ESP8266 Init OK\r\n");
	ESP8266_Clear();
	return;
}



uint8_t Authenticate(void)
{
  uint8_t res = 6;
	uint8_t tim = 200, *ptrIPD = NULL;
	Transbuf_Clear();
	
	/*VariableHeader*/
	total_len = 3;
	trans_buf[total_len++] = 0x00;
	trans_buf[total_len++] = 0x04;
	trans_buf[total_len++] = 'M';
	trans_buf[total_len++] = 'Q';
	trans_buf[total_len++] = 'T';
	trans_buf[total_len++] = 'T';
	trans_buf[total_len++] = 0x04;    //Protocol Level 
	trans_buf[total_len++] = 0xc2;
	trans_buf[total_len++] = (uint8_t)(1000>>8);
	trans_buf[total_len++] = (uint8_t)1000;
	
	/*Payload*/
	uint16_t size_s = strlen(DEVID);
	trans_buf[total_len++] = (size_s>>8)&0x00ff;
	trans_buf[total_len++] = size_s&0x00ff;
	memcpy(trans_buf+total_len,DEVID,size_s);
	total_len += size_s;
	size_s = strlen(PROID);
	trans_buf[total_len++] = (size_s>>8)&0x00ff;
	trans_buf[total_len++] = size_s&0x00ff;
	memcpy(trans_buf+total_len,PROID,size_s);
	total_len += size_s;
	size_s = strlen(AUTH_INFO);
	trans_buf[total_len++] = (size_s>>8)&0x00ff;
	trans_buf[total_len++] = size_s&0x00ff;
	memcpy(trans_buf+total_len,AUTH_INFO,size_s);
	total_len += size_s;
	
	/*Fixed Header*/
	trans_buf[0] = (MQTT_PKT_CONNECT<<4) | 0x00;
	if(total_len-3<128) 
	{
		total_len--;
		trans_buf[1] = trans_buf[0];
		trans_buf[2] = total_len - 2;
		ptrTrans = trans_buf+1;
	}
	else
  {
		ptrTrans = trans_buf;
		trans_buf[1] = total_len-3;
		trans_buf[2] = 0x01;
	}
//	HAL_UART_Transmit(&huart1,trans_buf,total_len,50);  //cs
	ESP8266_Clear();
	if(!ESP8266_SendData(ptrTrans,total_len)) HAL_NVIC_SystemReset();
	while(--tim>0)
	{
		HAL_Delay(10);
		if(ptrIPD=strstr((const char *)esp8266_buf,"IPD,"))
		{
			HAL_Delay(10);
			if(ptrIPD=strstr(ptrIPD,":")) ptrIPD++;	
			break;
		}
	}
	if((tim>0) && (*ptrIPD==MQTT_PKT_CONNACK<<4) && (*(ptrIPD+1)==0x02)) 
	{
		switch(*(ptrIPD+3))
		{
			case MQTT_SUCCESS        :     res = 0; break;
			case MQTT_ERR_PRO_LEVEL  :     res = 1; break;
			case MQTT_ERR_CLIENTID   :     res = 2; break;
			case MQTT_ERR_SRV_UNAV   :     res = 3; break;
			case MQTT_ERR_USER_KEY   :     res = 4; break;
			case MQTT_ERR_ILL_LNK    :     res = 5; break;
			default                  :     break;
		}
    ESP8266_Clear();		
	}
	return res;
}



void Publish(uint8_t *topic,uint16_t topiclen,uint8_t *data,uint16_t datlen)
{
  Transbuf_Clear();
	
	/*Variable Header  QoS0 */
	total_len = 3;
	trans_buf[total_len++] = topiclen>>8;
	trans_buf[total_len++] = (uint8_t)topiclen;
	memcpy(trans_buf+total_len,topic,topiclen);
	total_len += topiclen;
	
	/*Payload*/
	if(0==strcmp(topic,"$dp")) 
		//��"$dp"Ϊ������onenetƽ̨�Ϸ�������ʱ������Э���ĵ�����ȱ������3���ֽڣ�
	  //�Զ��巢������ʱ����3�ֽڿ���ʡ�ԣ�Onenet�ṩ�ĵ��Թ���MQTT Simulate Device -v1.0.7��û����3�ֽڣ�
	{
		trans_buf[total_len++] = 0x03;  // JSON��ʽ2�ַ��� {��datastream_id1��:��value1��,��datastream_id2��:��value2��,......}
		trans_buf[total_len++] = (datlen+2)>>8;
		trans_buf[total_len++] = (uint8_t)(datlen+2);
	}
	trans_buf[total_len++] = '{';
	memcpy(trans_buf+total_len,data,datlen);
	total_len += datlen;
	trans_buf[total_len++] = '}';
	
	/*Fixed Header*/
	trans_buf[0] = (MQTT_PKT_PUBLISH<<4) | 0x00;   //QoS0
	if(total_len-3<128) 
	{
		total_len--;
		trans_buf[1] = trans_buf[0];
		trans_buf[2] = total_len - 2;
		ptrTrans = trans_buf+1;
	}
	else
  {
		ptrTrans = trans_buf;
		trans_buf[1] = total_len-3;
		trans_buf[2] = 0x01;
	}
	
	ESP8266_Clear();
	if(!ESP8266_SendData(ptrTrans,total_len)) HAL_NVIC_SystemReset();
	ESP8266_Clear();    //��Ҫɾ�������
	return;
}
	


_Bool Subscribe(uint8_t *topicName,uint16_t len)
{
	_Bool res = 0;
	uint8_t tim = 200, *ptrIPD = NULL;
	HAL_Delay(200);
  Transbuf_Clear();
	
	/*Variable Header*/
	total_len = 3;
	trans_buf[total_len++] = (++packetID)>>8;
	trans_buf[total_len++] = (uint8_t)(packetID&0x00ff);
	/*Payload*/
	trans_buf[total_len++] = (len)>>8;
	trans_buf[total_len++] = (uint8_t)(len&0x00ff);
	memcpy(trans_buf+total_len,topicName,len);
	total_len += len;
	trans_buf[total_len++] = 0x00;  //QoS0
	/*Fixed Header*/
	#ifdef ONENET
	trans_buf[0] = (MQTT_PKT_SUBSCRIBE<<4) | 0x00;
	#endif 
	#if defined(BAIDU) || defined(NEWLAND) 
	trans_buf[0] = (MQTT_PKT_SUBSCRIBE<<4) | 0x02;
	#endif
	if(total_len-3<128) 
	{
		total_len--;
		trans_buf[1] = trans_buf[0];
		trans_buf[2] = total_len - 2;
		ptrTrans = trans_buf+1;
	}
	else
  {
		ptrTrans = trans_buf;
		trans_buf[1] = total_len-3;
		trans_buf[2] = 0x01;
	}
	
	ESP8266_Clear();
//	HAL_UART_Transmit(&huart1,trans_buf,total_len,50);  //cs
	if(!ESP8266_SendData(ptrTrans,total_len)) 
	{
		HAL_NVIC_SystemReset();
	}
	while(--tim>0)
	{
		HAL_Delay(10);
		if(ptrIPD=strstr((const char *)esp8266_buf,"IPD,"))
		{
			HAL_Delay(10);
			if(ptrIPD=strstr(ptrIPD,":")) ptrIPD++;	
			break;
		}
	}
	if((tim>0) && (*ptrIPD==MQTT_PKT_SUBACK<<4) && (*(ptrIPD+1)==0x03)) 
	{
		if((*(ptrIPD+2)==packetID>>8) && (*(ptrIPD+3)==packetID) && (*(ptrIPD+4)<3))
		{
			res = 1;	
		}
	}
	ESP8266_Clear();  //��Ҫɾ�������
	return res;
}



_Bool Unsubscribe(uint8_t *topicName,uint16_t len)
{
	_Bool res = 0;
	uint8_t tim = 200, *ptrIPD=NULL;
	HAL_Delay(200);
  Transbuf_Clear();
	
	/*Variable Header*/
	total_len = 3;
	trans_buf[total_len++] = (++packetID)>>8;
	trans_buf[total_len++] = (uint8_t)(packetID&0x00ff);
	/*Payload*/
	trans_buf[total_len++] = (len)>>8;
	trans_buf[total_len++] = (uint8_t)(len&0x00ff);
	memcpy(trans_buf+total_len,topicName,len);
	total_len += len;
	/*Fixed Header*/
	#ifdef ONENET
	trans_buf[0] = (MQTT_PKT_UNSUBSCRIBE<<4) | 0x00;
	#endif
	#if defined(BAIDU) || defined(NEWLAND) 
	trans_buf[0] = (MQTT_PKT_UNSUBSCRIBE<<4) | 0x02;
	#endif
	if(total_len-3<128) 
	{
		total_len--;
		trans_buf[1] = trans_buf[0];
		trans_buf[2] = total_len - 2;
		ptrTrans = trans_buf+1;
	}
	else
  {
		ptrTrans = trans_buf;
		trans_buf[1] = total_len-3;
		trans_buf[2] = 0x01;
	}
	ESP8266_Clear();
//	HAL_UART_Transmit(&huart1,trans_buf,total_len,50);  //cs
	if(!ESP8266_SendData(ptrTrans,total_len)) HAL_NVIC_SystemReset();
	while(--tim>0)
	{
		HAL_Delay(10);
		if(ptrIPD=strstr((const char *)esp8266_buf,"IPD,"))
		{
			HAL_Delay(10);
			if(ptrIPD=strstr(ptrIPD,":")) ptrIPD++;	
			break;
		}
	}
	if((tim>0) && (*ptrIPD==MQTT_PKT_UNSUBACK<<4) && (*(ptrIPD+1)==0x02)) 
	{
		if((*(ptrIPD+2)==packetID>>8) && (*(ptrIPD+3)==packetID))
		{
			res = 1;	
		}
	}
	ESP8266_Clear();  //��Ҫɾ�������
	return res;
}



_Bool KeepAlive(void)
{
	_Bool res = 0;
	uint8_t tim=200, *ptrIPD=NULL;
	HAL_Delay(200);
  Transbuf_Clear();
	
	/*Fixed Header*/ 
	trans_buf[total_len++] = (MQTT_PKT_PINGREQ<<4) | 0x00;
	trans_buf[total_len++] = 0;
	
	ESP8266_Clear();
	if(!ESP8266_SendData(trans_buf,total_len)) HAL_NVIC_SystemReset();
	while(--tim>0)
	{
		HAL_Delay(10);
		if(ptrIPD=strstr((const char *)esp8266_buf,"IPD,"))
		{
			HAL_Delay(10);
			if(ptrIPD=strstr(ptrIPD,":")) ptrIPD++;	
			break;
		}
	}
	if((tim>0) && (*ptrIPD==MQTT_PKT_PINGRESP<<4) && (*(ptrIPD+1)==0x00))		
	{
		res = 1;  
	}
	ESP8266_Clear();   //��Ҫɾ�������
	return res;
}



void Disconnect(void)
{
	HAL_Delay(200);
	Transbuf_Clear();
	
	/*Fixed Header*/  
	trans_buf[total_len++] = (MQTT_PKT_DISCONNECT<<4) | 0x00;
	trans_buf[total_len++] = 0;
	
	ESP8266_Clear();
	if(!ESP8266_SendData(trans_buf,total_len)) HAL_NVIC_SystemReset();
	return;
}



uint8_t Topic[60]={0},cmdStr[100]={0},cmdAckStr[100];
_Bool CmdAnalyz(void)
{
 _Bool res = 0;
 uint8_t *ptrIPD = NULL,*ptrTopic=NULL;
 uint16_t TopicLen = 0;

 memset(Topic,0,sizeof(Topic));
 if(ptrIPD=strstr((const char *)esp8266_buf,"IPD,"))
 {
	 HAL_Delay(10);
	 if(ptrIPD=strstr(ptrIPD,":")) ptrIPD++; 
	 else  
	 {
		 ESP8266_Clear();
		 return res;
	 }
 }
 if(ptrIPD[0] == MQTT_PKT_PUBLISH<<4)
 {
   if(ptrIPD[1]<128)
   {
     TopicLen = (uint16_t)(ptrIPD[2])<<8 | ptrIPD[3];
     ptrTopic = ptrIPD + 4;
   }
   else 
   {
     TopicLen = (uint16_t)(ptrIPD[3])<<8 | ptrIPD[4];
     ptrTopic = ptrIPD + 5;
   }
	 if(!strstr(ptrTopic,"$creq"))  //onenetƽ̨�涨���·��������⣬����ƽ̨��һ����ѭ
	 {
		 HAL_Delay(10);
		 res = GetSubscribeMsgCallback(ptrTopic);  //QoS0
	 }
	 else
   {
		 HAL_Delay(10);
		 memcpy(Topic,ptrTopic,TopicLen);
		 sprintf(cmdStr,ptrTopic+TopicLen);
		 memset(cmdAckStr,0,sizeof(cmdAckStr));
		 if(res = CmdAnalyzCallback(cmdStr,cmdAckStr))
		 {
			 Transbuf_Clear();
			 /*PUBLISH Variable Header*/
			 total_len = 3;
			 trans_buf[total_len++] = TopicLen>>8;
			 trans_buf[total_len++] = (uint8_t)TopicLen;
			 memcpy(trans_buf+total_len,Topic,TopicLen);
			 total_len += TopicLen;
			 trans_buf[8] = 's';
			 trans_buf[9] = 'p';  //����ظ� topicName��ͷ��$crsp
			 /*PUBLISH Payload*/
			 uint16_t cmdAckStrLen = 0;
			 if((cmdAckStrLen=strlen(cmdAckStr))>0)
			 {
				 memcpy(trans_buf+total_len,cmdAckStr,cmdAckStrLen);
				 total_len += cmdAckStrLen;
			 }
			 /*PUBLISH Fix Header*/
			 trans_buf[0] = MQTT_PKT_PUBLISH<<4;  //ע�⣬�ظ��������MQTT_PKT_PUBACK
			 if(total_len-3<128) 
			 {
				 total_len--;
				 trans_buf[1] = trans_buf[0];
				 trans_buf[2] = total_len - 2;
				 ptrTrans = trans_buf+1;
			 }
			 else
			 {
				 ptrTrans = trans_buf;
				 trans_buf[1] = total_len-3;
				 trans_buf[2] = 0x01;
			 }
			 ESP8266_Clear();
			 if(!ESP8266_SendData(ptrTrans,total_len)) HAL_NVIC_SystemReset();
			 else res = 1;
		}			 
  }
	ESP8266_Clear();
 }
 return res;
}


__weak _Bool CmdAnalyzCallback(uint8_t *cmdStr,uint8_t *cmdAckStr)
{
 sprintf(cmdAckStr,cmdStr);
 return 1;
}

__weak _Bool GetSubscribeMsgCallback(uint8_t *topicAndDatas)
{
	printf(topicAndDatas);
	return 1;
}