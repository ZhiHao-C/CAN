#include "bps_can.h"

#define Pass_ID     (uint32_t)0x1314
//���������豸�Ľ��շ������Żػ�ģʽ���漰����
void CAN_GPIO_Config(void)//�������˸��ù�����ӳ�����Լ���GPIO_PinRemapConfig��������AFIOʱ��
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//���ø��ù���AFIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	//��ӳ������
	GPIO_PinRemapConfig(GPIO_Remap1_CAN1,ENABLE);
	
	 /* Configure CAN TX pins */
  GPIO_InitStructure.GPIO_Pin = CAN_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		         // �����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(CAN_TX_GPIO_PORT, &GPIO_InitStructure);
	
	/* Configure CAN RX  pins */
  GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	             // ��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
}

void CAN_Mode_Config(void)
{
	//���ó�ʼ���ṹ��
	CAN_InitTypeDef CAN_InitStructure;
	//��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);
	
	CAN_InitStructure.CAN_ABOM=ENABLE;     //MCR-ABOM  �Զ����߹��� 
	CAN_InitStructure.CAN_AWUM=ENABLE;     //MCR-AWUM  ʹ���Զ�����ģʽ
	CAN_InitStructure.CAN_NART=ENABLE;     //MCR-NART  �����Զ��ش������ͳɹ�Ϊֹ��  
	CAN_InitStructure.CAN_RFLM=ENABLE;     //MCR-RFLM  ����FIFO ����ģʽ  DISABLE-���ʱ�±��ĻḲ��ԭ�б���
	CAN_InitStructure.CAN_TTCM=DISABLE;    //MCR-TTCM  �ر�ʱ�䴥��ͨ��ģʽʹ��
	CAN_InitStructure.CAN_TXFP=DISABLE;    //MCR-TXFP  ����FIFO���ȼ� DISABLE-���ȼ�ȡ���ڱ��ı�ʾ��
	CAN_InitStructure.CAN_SJW=CAN_SJW_2tq;//BTR-SJW ����ͬ����Ծ��� 2��ʱ�䵥Ԫ
	CAN_InitStructure.CAN_Mode=CAN_Mode_LoopBack;//�ػ�ģʽ������ʱ�������ģʽ��
	//�����ʼ��㹫ʽ
	/* ss=1tq bs1=5tq bs2=3tq λʱ����Ϊ(1+5+3) �����ʼ�Ϊʱ������(1+3+5)=9tq  */
	CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;
	CAN_InitStructure.CAN_BS2=CAN_BS2_5tq;
	//tq=1/��36M/4��=1/��9M��  ������Ĳ�����Ϊ1/��9*tq��=1Mbps
	CAN_InitStructure.CAN_Prescaler=4;
	
	
	
	
	CAN_Init(CAN1,&CAN_InitStructure);
}

void CAN_Filter_Config(void)
{
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	//ʹ��ɸѡ��
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	//ɸѡ�õ��Ľ������FIFO0��
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;
	//ҪɸѡID�ĸ�λ
	CAN_FilterInitStructure.CAN_FilterIdHigh=(((Pass_ID<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0XFFFF0000)>>16;//��ID������������ƴ������ȥ��16λ
	//ҪɸѡID�ĵ�λ
	CAN_FilterInitStructure.CAN_FilterIdLow=((Pass_ID<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0XFFFF;
	//ɸѡ����16λ������Ϊ1˵��Ҫ����ID��λΪ0������ȣ�
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0XFFFF;//��������Ϊ��ʮ��λ����Ҫ��id��ʮ��λһ�²ŷ���
	//ɸѡ����16λ
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0XFFFF;//��ʮ��λҲȫҪ���
	//����������ģʽ
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	//ɸѡ����0
	CAN_FilterInitStructure.CAN_FilterNumber=0;
	//ɸѡ��λ��Ϊ����32λ��
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;//
	
	CAN_FilterInit(&CAN_FilterInitStructure);
}

/*
 * ��������CAN_NVIC_Config
 * ����  ��CAN��NVIC ����,��1���ȼ��飬0��0���ȼ�
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_NVIC_Config(void)
{
   	NVIC_InitTypeDef NVIC_InitStructure;
		/* Configure one bit for preemption priority */
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	 	/*�ж�����*/
		NVIC_InitStructure.NVIC_IRQChannel = CAN_RX_IRQ;	   //CAN1 RX0�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		   //��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			   //�����ȼ�Ϊ0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/*
 * ��������CAN_Config
 * ����  ����������CAN�Ĺ���
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void CAN_Config(void)
{
  CAN_GPIO_Config();
  CAN_NVIC_Config();
  CAN_Mode_Config();
  CAN_Filter_Config();
	//�жϲ�����FIFO0���յ�һ���±��ģ�CAN_RF0R�Ĵ�����FMP0λ�����ǡ�00��
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);	
}
