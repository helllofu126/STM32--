#include "adc.h"
#include "MQ3.h"

void MQ_init(void)
{
		adc_init();
}

float get_MQ_value(void){
	uint16_t adcx;
	float temp;
	adcx=adc_get_result();
//	temp=((float)adcx*(3.3/4096))*0.36-1.08;
	temp=(float)adcx*(3.3/4096);
	adcx=temp;
	temp-=adcx;
	temp*=1000;
	
	return temp;
}


