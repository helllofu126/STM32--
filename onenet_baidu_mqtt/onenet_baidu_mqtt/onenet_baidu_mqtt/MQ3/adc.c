#include "adc.h"
#include "stm32f1xx_hal_adc.h"

ADC_HandleTypeDef g_adc_handle;

/*ADC单通道**/
void adc_init(void)
	{
		 //ADC1初始化
		ADC_ChannelConfTypeDef adc_ch_conf;
    g_adc_handle.Instance = ADC1;
    g_adc_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;//不使用扫描模式
    g_adc_handle.Init.ContinuousConvMode = ENABLE;//连续模式
    g_adc_handle.Init.DiscontinuousConvMode = DISABLE;
    g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;//软件触发
    g_adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;//数据右对齐
    g_adc_handle.Init.NbrOfConversion = 1; 
		HAL_ADC_Init(&g_adc_handle);
		
		HAL_ADCEx_Calibration_Start(&g_adc_handle);
		
		//通道初始化
		adc_ch_conf.Channel = ADC_CHANNEL_1;
    adc_ch_conf.Rank = ADC_REGULAR_RANK_1;
    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
		HAL_ADC_ConfigChannel(&g_adc_handle,&adc_ch_conf);
		
		
	}
	
	/**ADC MSP初始化函数**/
	void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
	{
		if(hadc->Instance==ADC1){
			GPIO_InitTypeDef gpio_init_struct;
			RCC_PeriphCLKInitTypeDef adc_clk_init={0};
			
			__HAL_RCC_GPIOA_CLK_ENABLE();
			__HAL_RCC_ADC1_CLK_ENABLE();
			
			
			gpio_init_struct.Pin=GPIO_PIN_1;
			gpio_init_struct.Mode=GPIO_MODE_ANALOG;
			HAL_GPIO_Init(GPIOA,&gpio_init_struct);
			
			adc_clk_init.PeriphClockSelection=RCC_PERIPHCLK_ADC;
			adc_clk_init.AdcClockSelection=RCC_ADCPCLK2_DIV6;			
			HAL_RCCEx_PeriphCLKConfig(&adc_clk_init);	
		}
		
	}
	
	/*获取ADC转换后的结果*/
	uint32_t adc_get_result(void)
	{
		HAL_ADC_Start(&g_adc_handle);
		HAL_ADC_PollForConversion(&g_adc_handle,10);
		return (uint16_t)HAL_ADC_GetValue(&g_adc_handle);
	}
		
	
		
	
	