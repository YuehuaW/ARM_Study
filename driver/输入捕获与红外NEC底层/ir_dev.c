/**
  ************************************* Copyright ******************************
  *
  *                 (C) Copyright 2025 SleepyCats,China, WFU.
  *                            All Rights Reserved
  *
  *                     By(SleepyCats)
  *
  *
  * FileName   : ir_drv.c
  * Version    : v1.0
  * Author     : SleepyCats
  * Date       : 2025-07-18
  * Description:使用输入捕获解析 红外NEC通讯底层函数实现

  ******************************************************************************
 */



#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "gd32f30x.h"

static void GpioInit(void)
{
    rcu_periph_clock_enable(RCU_GPIOC);
    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_10MHZ, GPIO_PIN_6);
}

static void TimerInit(void)
{
    /* 使能定时器时钟；*/
    rcu_periph_clock_enable(RCU_TIMER7);
    /* 复位定时器；*/
    timer_deinit(TIMER7);

    timer_parameter_struct timerInitPara;
    timer_struct_para_init(&timerInitPara);
    /* 设置预分频器值；*/
    timerInitPara.prescaler = 120 - 1;     // 输入给计数器的时钟频率为1Mhz，周期1us
    /* 设置自动重装载值；*/
    timerInitPara.period = 65535;
    timer_init(TIMER7, &timerInitPara);

    timer_ic_parameter_struct icInitPara;
    timer_channel_input_struct_para_init(&icInitPara);
    /* 设置上升沿/下降沿捕获；*/
    icInitPara.icpolarity = TIMER_IC_POLARITY_FALLING;
    /* 设置输入通道；*/
    icInitPara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_input_capture_config(TIMER7, TIMER_CH_0, &icInitPara);

    /* 使能定时器的捕获中断；*/
    timer_interrupt_flag_clear(TIMER7, TIMER_INT_FLAG_CH0);
    timer_interrupt_enable(TIMER7, TIMER_INT_CH0);
    /* 使能定时器中断和优先级；*/
    nvic_irq_enable(TIMER7_Channel_IRQn, 0, 0);

    /* 使能定时器；*/
    timer_enable(TIMER7);
}

#define TICK_HEAD_MAX     20000    // 表示引导码周期最大值20000us
#define TICK_HEAD_MIN     10000    // 表示引导码周期最小值10000us
#define TICK_0_MAX        1800     // 表示二进制0周期最大值1800us
#define TICK_0_MIN        500      // 表示二进制0周期最小值500us
#define TICK_1_MAX        3000     // 表示二进制1周期最大值3000us
#define TICK_1_MIN        1800     // 表示二进制1周期最小值1800us

static uint8_t g_irCode[4];
static bool g_irCodeFlag = false;

/**
***********************************************************
* @brief 解析按键码值
* @param tickNum，捕获计数值，单位us
* @return
***********************************************************
*/
static void ParseIrFrame(uint32_t tickNum)
{
    static bool s_headFlag = false;
    static uint8_t s_index = 0;

    if (tickNum > TICK_HEAD_MIN && tickNum < TICK_HEAD_MAX)
    {
        s_headFlag = true;
        return;
    }

    if (!s_headFlag)
    {
        return;
    }
    if (tickNum > TICK_1_MIN && tickNum < TICK_1_MAX)
    {
        g_irCode[s_index / 8] >>= 1;
        g_irCode[s_index / 8] |= 0x80;
        s_index++;
    }
    if (tickNum > TICK_0_MIN && tickNum < TICK_0_MAX)
    {
        g_irCode[s_index / 8] >>= 1;
        s_index++;
    }

    if (s_index == 32)
    {
        if ((g_irCode[2] & g_irCode[3]) == 0)   //(g_irCode[2] == (uint8_t)~g_irCode[3])
        {
            g_irCodeFlag = true;
        }
        else
        {
            g_irCodeFlag = false;
        }
        s_headFlag = false;
        s_index = 0;
    }
}

/**
***********************************************************
* @brief 获取遥控按键码值
* @param code，输出，按键码值
* @return 返回是否成功获取到按键码值
***********************************************************
*/
bool GetIrCode(uint8_t *code)
{
    if (!g_irCodeFlag)
    {
        return false;
    }
    *code = g_irCode[2];
    g_irCodeFlag = false;
    return true;
}

void TIMER7_Channel_IRQHandler(void)
{
    uint32_t icValue;
    if (timer_interrupt_flag_get(TIMER7, TIMER_INT_FLAG_CH0) == SET)
    {
        timer_interrupt_flag_clear(TIMER7, TIMER_INT_FLAG_CH0);
        icValue = timer_channel_capture_value_register_read(TIMER7, TIMER_CH_0) + 1;
        timer_counter_value_config(TIMER7, 0);
        ParseIrFrame(icValue);
    }
}

/**
***********************************************************
* @brief 红外接收硬件初始化函数
* @param
* @return
***********************************************************
*/
void IrDrvInit(void)
{
    GpioInit();
    TimerInit();
}
