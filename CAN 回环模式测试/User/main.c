#include "stm32f10x.h"                  // Device header
#include "bps_led.h"
#include "bps_usart.h"
#include "bps_can.h"
#include "bps_key.h"
#include <stdio.h>
#include <string.h>

//typedef struct
//{
//  uint32_t StdId;//储存了报文标识符的11位
//  uint32_t ExtId;//储存了报文标识符的29位
//  uint8_t IDE;//储存IDE扩展标志
//  uint8_t RTR;//储存判断是否为数据帧还是遥控帧标志
//  uint8_t DLC;//报文数据段的长度只能为0-8
//  uint8_t Data[8];//储存报文数据段的内容
//  uint8_t FMI;//经过筛选器储存进FIFO的报文

//} CanRxMsg;

#define Pass_ID     (uint32_t)0x1314

CanRxMsg Receive_Data;
CanTxMsg Transmit_Data;
uint8_t flag;
uint8_t buff[8]={1,2,3,4,5,6,7,8};
void delay_ms(int n)//延时函数
{
	int i;
	while (n--)
	{
		for(i=0;i<5520;i++);
	}
}



int main()
{
	USART_Config();
	CAN_Config();
	KEY1_GPIO_Config();
	printf("这是一个CAN实验\n");
	printf("按下key1按键发送数据\n");
	while(1)
	{
		if(key_scan(GPIOA,GPIO_Pin_0))
		{
			uint8_t mailbox=0;
			memcpy(Transmit_Data.Data,buff,8);       //发送值
			Transmit_Data.DLC=8;             //发送一个字节
			Transmit_Data.ExtId=Pass_ID;     //发送的扩展ID值
			Transmit_Data.IDE=CAN_Id_Extended;//IDE扩展标志说明使用扩展
			Transmit_Data.RTR=CAN_RTR_Data;//数据帧标志
			Transmit_Data.StdId=0;           //
			mailbox=CAN_Transmit(CAN1,&Transmit_Data);//
			while(CAN_TransmitStatus(CAN1,mailbox)!=CAN_TxStatus_Ok);
		}
		if(flag==1)
		{
			uint8_t i=0;
			for(i=0;i<8;i++)
			{
				if(i==0)
				{
					printf("接收到的数据为：");
				}
				printf(" %d ",Receive_Data.Data[i]);
				if(i==7)
				{
					printf("\n");
				}
			}
			
			flag=0;
		}
		else
		{
			
		}
	}
	
	
}



void USB_LP_CAN1_RX0_IRQHandler(void)
{
		/*从邮箱中读出报文*/
	CAN_Receive(CAN1, CAN_FIFO0, &Receive_Data);
	if(Receive_Data.ExtId==Pass_ID)
	flag=1;
}


