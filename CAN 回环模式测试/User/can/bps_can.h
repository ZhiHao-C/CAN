#ifndef __BPS_CAN_H__
#define __BPS_CAN_H__

#include "stm32f10x.h" 

#define CAN_RX_PIN                 GPIO_Pin_8
#define CAN_TX_PIN                 GPIO_Pin_9
#define CAN_TX_GPIO_PORT          GPIOB
#define CAN_RX_GPIO_PORT          GPIOB


#define CAN_RX_IRQ									USB_LP_CAN1_RX0_IRQn
#define CAN_RX_IRQHandler					USB_LP_CAN1_RX0_IRQHandler

void CAN_Config(void);
#endif
