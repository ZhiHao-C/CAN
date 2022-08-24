#include "bps_can.h"

#define Pass_ID     (uint32_t)0x1314
//用作两个设备的接收发送引脚回环模式不涉及这里
void CAN_GPIO_Config(void)//这里用了复用功能重映射所以加上GPIO_PinRemapConfig（）并打开AFIO时钟
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//配置复用功能AFIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	//重映射引脚
	GPIO_PinRemapConfig(GPIO_Remap1_CAN1,ENABLE);
	
	 /* Configure CAN TX pins */
  GPIO_InitStructure.GPIO_Pin = CAN_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		         // 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(CAN_TX_GPIO_PORT, &GPIO_InitStructure);
	
	/* Configure CAN RX  pins */
  GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	             // 上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
}

void CAN_Mode_Config(void)
{
	//配置初始化结构体
	CAN_InitTypeDef CAN_InitStructure;
	//打开时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);
	
	CAN_InitStructure.CAN_ABOM=ENABLE;     //MCR-ABOM  自动离线管理 
	CAN_InitStructure.CAN_AWUM=ENABLE;     //MCR-AWUM  使用自动唤醒模式
	CAN_InitStructure.CAN_NART=ENABLE;     //MCR-NART  报文自动重传（发送成功为止）  
	CAN_InitStructure.CAN_RFLM=ENABLE;     //MCR-RFLM  接收FIFO 锁定模式  DISABLE-溢出时新报文会覆盖原有报文
	CAN_InitStructure.CAN_TTCM=DISABLE;    //MCR-TTCM  关闭时间触发通信模式使能
	CAN_InitStructure.CAN_TXFP=DISABLE;    //MCR-TXFP  发送FIFO优先级 DISABLE-优先级取决于报文标示符
	CAN_InitStructure.CAN_SJW=CAN_SJW_2tq;//BTR-SJW 重新同步跳跃宽度 2个时间单元
	CAN_InitStructure.CAN_Mode=CAN_Mode_LoopBack;//回环模式（测试时先用这个模式）
	//波特率计算公式
	/* ss=1tq bs1=5tq bs2=3tq 位时间宽度为(1+5+3) 波特率即为时钟周期(1+3+5)=9tq  */
	CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;
	CAN_InitStructure.CAN_BS2=CAN_BS2_5tq;
	//tq=1/（36M/4）=1/（9M）  所以算的波特率为1/（9*tq）=1Mbps
	CAN_InitStructure.CAN_Prescaler=4;
	
	
	
	
	CAN_Init(CAN1,&CAN_InitStructure);
}

void CAN_Filter_Config(void)
{
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	//使能筛选器
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	//筛选得到的结果存入FIFO0中
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;
	//要筛选ID的高位
	CAN_FilterInitStructure.CAN_FilterIdHigh=(((Pass_ID<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0XFFFF0000)>>16;//将ID号与其他数据拼起来再去高16位
	//要筛选ID的低位
	CAN_FilterInitStructure.CAN_FilterIdLow=((Pass_ID<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0XFFFF;
	//筛选器高16位（设置为1说明要等于ID的位为0则不需相等）
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0XFFFF;//这里设置为高十六位必须要与id高十六位一致才符合
	//筛选器低16位
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0XFFFF;//第十六位也全要相等
	//工作在掩码模式
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	//筛选器组0
	CAN_FilterInitStructure.CAN_FilterNumber=0;
	//筛选器位宽为单个32位。
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;//
	
	CAN_FilterInit(&CAN_FilterInitStructure);
}

/*
 * 函数名：CAN_NVIC_Config
 * 描述  ：CAN的NVIC 配置,第1优先级组，0，0优先级
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_NVIC_Config(void)
{
   	NVIC_InitTypeDef NVIC_InitStructure;
		/* Configure one bit for preemption priority */
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	 	/*中断设置*/
		NVIC_InitStructure.NVIC_IRQChannel = CAN_RX_IRQ;	   //CAN1 RX0中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		   //抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			   //子优先级为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/*
 * 函数名：CAN_Config
 * 描述  ：完整配置CAN的功能
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void CAN_Config(void)
{
  CAN_GPIO_Config();
  CAN_NVIC_Config();
  CAN_Mode_Config();
  CAN_Filter_Config();
	//中断产生：FIFO0接收到一个新报文，CAN_RF0R寄存器的FMP0位不再是’00’
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);	
}
