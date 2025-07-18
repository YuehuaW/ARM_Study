#ifndef _IR_DRV_H_
#define _IR_DRV_H_

#include <stdint.h>
#include <stdbool.h>

#define KEY1_CODE   0X45
#define KEY2_CODE   0X46

/**
***********************************************************
* @brief 红外接收硬件初始化函数
* @param
* @return
***********************************************************
*/
void IrDrvInit(void);

/**
***********************************************************
* @brief 获取遥控按键码值
* @param code，输出，按键码值
* @return 返回是否成功获取到按键码值
***********************************************************
*/
bool GetIrCode(uint8_t *code);

#endif
