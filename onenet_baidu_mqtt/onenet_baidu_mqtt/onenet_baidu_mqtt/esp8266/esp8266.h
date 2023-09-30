#ifndef _ESP8266
#define _ESP8266

#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"  
#include "usart.h"
#include <string.h> 
#include <stdio.h>



/***************************onenet?? ????? mqtt****************************/

#define ONENET   //???,onenet??????????fix head????0x80
#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"183.230.40.39\",6002\r\n"    

#define PROID	 	 ""      //??ID  (onenet_old  mqtt_old_test_01)

#define AUTH_INFO	""     //Master-APIkey??   access_key?????,???????token???
					
#define DEVID		 ""	//??ID  (onenet_old  jack01)
  #ifndef AUTH_INFO
  #define AUTH_INFO	""     //?????????????auth_info?? ??
#endif

//#ifndef DEVID
//	#define DEVID		 ""	//??ID  (onenet_old  jack02)
//	#ifndef AUTH_INFO
//	#define AUTH_INFO	""     //?????????????auth_info?? ??
//	#endif
//#endif

/***********************************************************************************/



///******************************?????? mqtt************************************/

//#define NEWLAND  //???,???????????fix head?????0x80,??0x82
//#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtt.nlecloud.com\",1883\r\n"
//#define PROID	 	 ""
//#define AUTH_INFO	""
//#define DEVID		 ""

///***********************************************************************************/



/*********************************????? mqtt***********************************/

//#define BAIDU   //???,??????????fix head?????0x80,??0x82
//#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"anzgwrt.iot.gz.baidubce.com\",1883\r\n"
//#define PROID	 	 ""  //mqtt.fx:User Name
//#define AUTH_INFO	""   //mqtt.fx:Password
//#define DEVID		 ""     //mqtt.fx:Client ID 

/***********************************************************************************/


#define ESP8266_WIFI_INFO		"AT+CWJAP=\"hao\",\"12345678\"\r\n"


extern uint8_t esp8266_buf[200];  //MCU?8266???????????????
extern uint16_t esp8266_cnt;      //???????????????????


/*???esp8266,??MCU?WIFI???????,??WIFI?????????(?????????NULL),
  ????????????8266??*/
void ESP8266_Init(UART_HandleTypeDef *huart,GPIO_TypeDef * reset_port,uint16_t reset_pin);


/*???????,?????ESP8266_Init()??????,???????QoS0*/
uint8_t Authenticate(void);


/*????(QoS0)  
??,onenet???????"$dp"????????,payload?????3???(????????????);
???????,?3?????,onenet?????mqtt????(MQTT Simulate Device -v1.0.7)????3??
data??: { datastream_id1 :value1, datastream_id2 :value2,...} 
           ????????????,????????????????;value????Json??  */
void Publish(uint8_t *topic, uint16_t topiclen, uint8_t *data, uint16_t datlen);


/*????(QoS0)*/
_Bool Subscribe(uint8_t *topicName,uint16_t len);


/*????*/
_Bool Unsubscribe(uint8_t *topicName,uint16_t len);


/*tcp????*/
_Bool KeepAlive(void);


/*???????????*/
void Disconnect(void);


/*????????????????????????
  ?????????,?????????????:
	<onenet ??????????????,?????????????????>
    topic/PROID/+/dp
    topic/PROID/DEVID/cmdp
	<baidu ?????????????????????????,?????????????>
	  topic/MQTT_TEST_DEV01/dp
		topic/MQTT_TEST_DEV02/dp
		topic/MQTT_TEST_DEV01/cmdp
		topic/MQTT_TEST_DEV02/cmdp
  ?????????????,??????GetSubscribeMsgCallback()???????????		*/
_Bool CmdAnalyz(void);


/*????????(???),?main.c????
  ??????cmdStr??(???):{"cmd":{"LED0":1}}  
	??????????cmdAckStr??:{"RESULT":"OK","LED0":"OFF"} ? {"RESULT":"ERROR"}*/
_Bool CmdAnalyzCallback(uint8_t *cmdStr,uint8_t *cmdAckStr);


/*???????????????(???),?main.c????
  ????topicAndDatas??(???):topic/505050/926875824/dp{"aaaa":1} */
_Bool GetSubscribeMsgCallback(uint8_t *topicAndDatas);





#endif