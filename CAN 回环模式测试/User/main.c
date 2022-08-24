#include "stm32f10x.h"                  // Device header
#include "bps_led.h"
#include "bps_usart.h"
#include "bps_can.h"
#include "bps_key.h"
#include <stdio.h>
#include <string.h>

//typedef struct
//{
//  uint32_t StdId;//�����˱��ı�ʶ����11λ
//  uint32_t ExtId;//�����˱��ı�ʶ����29λ
//  uint8_t IDE;//����IDE��չ��־
//  uint8_t RTR;//�����ж��Ƿ�Ϊ����֡����ң��֡��־
//  uint8_t DLC;//�������ݶεĳ���ֻ��Ϊ0-8
//  uint8_t Data[8];//���汨�����ݶε�����
//  uint8_t FMI;//����ɸѡ�������FIFO�ı���

//} CanRxMsg;

#define Pass_ID     (uint32_t)0x1314

CanRxMsg Receive_Data;
CanTxMsg Transmit_Data;
uint8_t flag;
uint8_t buff[8]={1,2,3,4,5,6,7,8};
void delay_ms(int n)//��ʱ����
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
	printf("����һ��CANʵ��\n");
	printf("����key1������������\n");
	while(1)
	{
		if(key_scan(GPIOA,GPIO_Pin_0))
		{
			uint8_t mailbox=0;
			memcpy(Transmit_Data.Data,buff,8);       //����ֵ
			Transmit_Data.DLC=8;             //����һ���ֽ�
			Transmit_Data.ExtId=Pass_ID;     //���͵���չIDֵ
			Transmit_Data.IDE=CAN_Id_Extended;//IDE��չ��־˵��ʹ����չ
			Transmit_Data.RTR=CAN_RTR_Data;//����֡��־
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
					printf("���յ�������Ϊ��");
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
		/*�������ж�������*/
	CAN_Receive(CAN1, CAN_FIFO0, &Receive_Data);
	if(Receive_Data.ExtId==Pass_ID)
	flag=1;
}


