#include "gd32f30x.h"                   // Device header
#include "stdint.h"

#define PRESCALER 120-1 // 预分频
#define PERIOD 500-1//PWM 周期
#define PULSE 250-1//PWM 脉宽


static void GPIOInit(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_10MHZ,GPIO_PIN_8);
}

static void TimerInit(void)
{
	rcu_periph_clock_enable(RCU_TIMER0);
	//deinit
	timer_deinit(TIMER0);
	//配置时钟
	timer_parameter_struct TimerParm_t;
	timer_struct_para_init(&TimerParm_t);//初始化配置结构体
	TimerParm_t.prescaler = PRESCALER;
	TimerParm_t.period = PERIOD;
	timer_init(TIMER0,&TimerParm_t);
	//初始化输出结构体
	timer_oc_parameter_struct TimerOCParm_t;
	timer_channel_output_struct_para_init(&TimerOCParm_t);
	//配置极性
	TimerOCParm_t.ocnpolarity = TIMER_OC_POLARITY_HIGH;
	//配置功能
	TimerOCParm_t.outputstate = TIMER_CCXN_ENABLE;
	
	timer_channel_output_config(TIMER0,TIMER_CH_0,&TimerOCParm_t);
	
	timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_0,250-1);
	timer_channel_output_mode_config(TIMER0,TIMER_CH_0,TIMER_OC_MODE_PWM0);
	timer_primary_output_config(TIMER0,ENABLE);
	timer_enable(TIMER0);
}

void PwmDrv_Init(void)
{
	GPIOInit();
	TimerInit();
}
