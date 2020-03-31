#include "chip/gt9271.h"

/*The following program is modified by the user according to the hardware device, otherwise the driver cannot run.*/

/**
  * @step 1:  Modify the corresponding function according to the modified area and the corresponding function name.
  * @step 2:  Modify four areas: GPIO_PORT/GPIO_PIN/Led_TypeDef/LEDn.
  * @step 3:  .
  * @step 4:  .
  * @step 5:
  */

#include "peripheral/iic_soft.h"
#include "i2c.h"
#include "string.h"

#define DEBUG
#include "idebug.h"

#define gt9271_printf                   p_dbg

#define GT9271_ADDR                     0x0014
#define USING_I2C_SOFT

static inline void gt9271_Delayus(uint32_t ticks)
{
  HAL_Delay(ticks);
}

static inline void gt9271_Delayms(uint32_t ticks)
{
  HAL_Delay(ticks);
}

static inline void gt9271_PortTransmmit(uint16_t Addr, uint8_t Data)
{
#ifdef USING_I2C_SOFT
  IIC_Soft_WriteSingleByteWithAddr(GT9271_ADDR, Addr, Data);
#else
  HAL_I2C_Mem_Write(&hi2c1, (GT9271_ADDR << 1),
                            Addr, I2C_MEMADD_SIZE_8BIT, &Data, 1, 10000);
#endif
}

static inline void gt9271_PortReceive(uint16_t Addr, uint8_t *pData)
{
#ifdef USING_I2C_SOFT
  IIC_Soft_ReadSingleByteWithAddr(GT9271_ADDR, Addr, pData);
#else
  HAL_I2C_Mem_Read (&hi2c1, (GT9271_ADDR << 1),
                            Addr, I2C_MEMADD_SIZE_8BIT, pData, 1, 10000);
#endif
}

static inline void gt9271_PortMultiTransmmit(uint16_t Addr, uint8_t *pData, uint32_t Length)
{
#ifdef USING_I2C_SOFT
  IIC_Soft_WriteMultiByteWithAddr(GT9271_ADDR, Addr, pData, Length);
#else
  HAL_I2C_Mem_Write(&hi2c1, (GT9271_ADDR << 1),
                            Addr, I2C_MEMADD_SIZE_8BIT, pData, Length, 10000);
#endif
}

static inline void gt9271_PortMultiReceive(uint16_t Addr, uint8_t *pData, uint32_t Length)
{
#ifdef USING_I2C_SOFT
  IIC_Soft_ReadMultiByteWithAddr(GT9271_ADDR, Addr, pData, Length);
#else
  HAL_I2C_Mem_Read (&hi2c1, (GT9271_ADDR << 1),
                            Addr, I2C_MEMADD_SIZE_8BIT, pData, Length, 10000);
#endif
}

void I2C_ResetChip(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
  
  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_RESET);
  gt9271_Delayms(10);

  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_SET);
  gt9271_Delayms(10);

  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
  gt9271_Delayms(100);
}

/*The above procedure is modified by the user according to the hardware device, otherwise the driver cannot run.*/

#define GT9271_CTRL_REG                 0x8040   //GT9271���ƼĴ���
#define GT9271_CFGS_REG                 0x8047   //GT9271������ʼ��ַ�Ĵ���
#define GT9271_CHECK_REG                0x80FF   //GT9271У��ͼĴ���
#define GT9271_PID_REG                  0x8140   //GT9271��ƷID�Ĵ���
#define GT9271_GSTID_REG                0x814E   //GT9271��ǰ��⵽�Ĵ������
#define GT9271_TP1_REG                  0x8150   //��һ�����������ݵ�ַ
#define GT9271_TP2_REG                  0x8158   //�ڶ������������ݵ�ַ
#define GT9271_TP3_REG                  0x8160   //���������������ݵ�ַ
#define GT9271_TP4_REG                  0x8168   //���ĸ����������ݵ�ַ
#define GT9271_TP5_REG                  0x8170   //��������������ݵ�ַ 
#define GT9271_TP6_REG                  0x8178   //��������������ݵ�ַ 
#define GT9271_TP7_REG                  0x8180   //��������������ݵ�ַ 
#define GT9271_TP8_REG                  0x8188   //��������������ݵ�ַ 
#define GT9271_TP9_REG                  0x8190   //��������������ݵ�ַ 
#define GT9271_TP10_REG                 0x8198   //��������������ݵ�ַ
#define GTP_INT_TRIGGER                 0
#define GTP_MAX_TOUCH                   5
#define GTP_READ_COOR_ADDR              0x814E
#define GTP_REG_SLEEP                   0x8040
#define GTP_REG_SENSOR_ID               0x814A
#define GTP_REG_CONFIG_DATA             0x8047
#define GTP_REG_VERSION                 0x8140
#define GT9271_ID                       0x0811
#define GTP_ADDR_LENGTH                 2
#define GTP_REG_COMMAND                 0x8040
#define GTP_COMMAND_READSTATUS          0

static uint8_t CTP_CFG_GT9271[] ={ 
  0x41,0x00,0x05,0x20,0x03,0x0A,0x3D,0x20,0x01,0x0A,
  0x28,0x0F,0x6E,0x5A,0x03,0x05,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x18,0x1A,0x1E,0x14,0x8F,0x2F,0xAA,
  0x26,0x24,0x0C,0x08,0x00,0x00,0x00,0x81,0x03,0x2D,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x1A,0x3C,0x94,0xC5,0x02,0x07,0x00,0x00,0x04,
  0x9E,0x1C,0x00,0x89,0x21,0x00,0x77,0x27,0x00,0x68,
  0x2E,0x00,0x5B,0x37,0x00,0x5B,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x19,0x18,0x17,0x16,0x15,0x14,0x11,0x10,
  0x0F,0x0E,0x0D,0x0C,0x09,0x08,0x07,0x06,0x05,0x04,
  0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x02,0x04,0x06,0x07,0x08,0x0A,0x0C,
  0x0D,0x0F,0x10,0x11,0x12,0x13,0x14,0x19,0x1B,0x1C,
  0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
  0x28,0x29,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x5D,0x01
};

const uint16_t GT9271_TPX_TBL[10] = {
  GT9271_TP1_REG,
  GT9271_TP2_REG,
  GT9271_TP3_REG,
  GT9271_TP4_REG,
  GT9271_TP5_REG,
  GT9271_TP6_REG,
  GT9271_TP7_REG,
  GT9271_TP8_REG,
  GT9271_TP9_REG,
  GT9271_TP10_REG,
};

/*******************************************************
Function:
    I2c test Function.
Input:
    client:i2c client.
Output:
    Executive outcomes.
        2: succeed, otherwise failed.
*******************************************************/
static int8_t GTP_I2C_Test(void)
{
  uint8_t Data[3];

  gt9271_PortMultiReceive(GTP_REG_CONFIG_DATA, Data, sizeof(Data));
  
  return 1;
}

/**
  * @brief   ���ڴ���򱨸津����⵽����
  * @param
  *    @arg  id: ����˳��trackID
  *    @arg  x:  ������ x ����
  *    @arg  y:  ������ y ����
  *    @arg  w:  ������ ��С
  * @retval ��
  */
/*���ڼ�¼��������ʱ(����)����һ�δ���λ�ã�����ֵ��ʾ��һ���޴�������*/
static int16_t pre_x[GTP_MAX_TOUCH] ={-1,-1,-1,-1,-1};
static int16_t pre_y[GTP_MAX_TOUCH] ={-1,-1,-1,-1,-1};

static void GTP_Touch_Down(int32_t id, int32_t x, int32_t y, int32_t w)
{

  /*ȡx��y��ʼֵ������Ļ����ֵ*/
  gt9271_printf("ID:%d, X:%d, Y:%d, W:%d", id, x, y, w);

//  /* ��������ť�����ڴ������� */
//  Touch_Button_Down(x,y); 
//
//  /*�������켣�����ڴ������� */
//  Draw_Trail(pre_x[id],pre_y[id],x,y,&brush);

  /************************************/
  /*�ڴ˴�����Լ��Ĵ����㰴��ʱ������̼���*/
  /* (x,y) ��Ϊ���µĴ����� *************/
  /************************************/

  /*prex,prey����洢��һ�δ�����λ�ã�idΪ�켣���(��㴥��ʱ�ж�켣)*/
  pre_x[id] = x; pre_y[id] =y;
}

/**
  * @brief   ���ڴ���򱨸津���ͷ�
  * @param �ͷŵ��id��
  * @retval ��
  */
static void GTP_Touch_Up(int32_t id)
{
  /*�������ͷ�,���ڴ�������*/
  Touch_Button_Up(pre_x[id],pre_y[id]);

  /*****************************************/
  /*�ڴ˴�����Լ��Ĵ������ͷ�ʱ�Ĵ�����̼���*/
  /* pre_x[id],pre_y[id] ��Ϊ���µ��ͷŵ� ****/
  /*******************************************/  
  /***idΪ�켣���(��㴥��ʱ�ж�켣)********/


  /*�����ͷţ���pre xy ����Ϊ��*/
  pre_x[id] = -1;
  pre_y[id] = -1;    

  gt9271_printf("Touch id[%2d] release!", id);
}

/**
  * @brief   ��������������ѯ�����ڴ����жϵ���
  * @param ��
  * @retval ��
  */
static void Goodix_TS_Work_Func(void)
{
  uint8_t  end_cmd[3];
  uint8_t  point_data[2 + 1 + 8 * GTP_MAX_TOUCH + 1];
  uint8_t  touch_num = 0;
  uint8_t  finger = 0;
  static uint16_t pre_touch = 0;
  static uint8_t pre_id[GTP_MAX_TOUCH] = {0};

  uint8_t* coor_data = NULL;
  int32_t input_x = 0;
  int32_t input_y = 0;
  int32_t input_w = 0;
  uint8_t id = 0;

  int32_t i = 0,j;

  gt9271_PortMultiReceive(GTP_READ_COOR_ADDR, point_data, 10);

  finger = point_data[GTP_ADDR_LENGTH];//״̬�Ĵ�������

  if(finger == 0x00)    //û�����ݣ��˳�
  {
    return;
  }

  if((finger & 0x80) == 0)//�ж�buffer statusλ
  {
    goto exit_work_func;//����δ������������Ч
  }

  touch_num = finger & 0x0F;//�������
  if(touch_num > GTP_MAX_TOUCH)
  {
    goto exit_work_func;//�������֧�ֵ����������˳�
  }

  if(touch_num > 1)//��ֹһ����
  {
    uint8_t buf[8 * GTP_MAX_TOUCH];

    gt9271_PortMultiReceive(GTP_READ_COOR_ADDR, buf, 8 * (touch_num - 1));
    memcpy(&point_data[12], buf, 8 * (touch_num - 1));  //����������������ݵ�point_data
  }

  if(pre_touch>touch_num)        //pre_touch>touch_num,��ʾ�еĵ��ͷ���
  {
    for(i = 0;i < pre_touch;i++)      //һ����һ���㴦��
    {
      for(j = 0;j < touch_num;j++)
      {
        coor_data = &point_data[j * 8 + 3];
        id = coor_data[0] & 0x0F;      //track id
        if(pre_id[i] == id)
          break;

        if(j >= touch_num - 1)        //������ǰ����id���Ҳ���pre_id[i]����ʾ���ͷ�
          GTP_Touch_Up( pre_id[i]);
      }
    }
  }

  if(touch_num)
  {
    for(i = 0;i < touch_num;i++)      //һ����һ���㴦��
    {
      coor_data = &point_data[i * 8 + 3];

      id = coor_data[0] & 0x0F;        //track id
      pre_id[i] = id;

      input_x  = coor_data[1] | (coor_data[2] << 8);  //x����
      input_y  = coor_data[3] | (coor_data[4] << 8);  //y����
      input_w  = coor_data[5] | (coor_data[6] << 8);  //size
  
      GTP_Touch_Down( id, input_x, input_y, input_w);//���ݴ���
    }
  }
  else if(pre_touch)    //touch_ num=0 ��pre_touch��=0
  {
    for(i=0;i<pre_touch;i++)
      GTP_Touch_Up(pre_id[i]);
  }

  pre_touch = touch_num;

exit_work_func:
  {
    gt9271_PortMultiTransmmit(GTP_READ_COOR_ADDR, end_cmd, 1);
  }
}

/**
  * @brief   ������оƬ���¸�λ
  * @param ��
  * @retval ��
  */
int8_t GTP_Reset_Guitar(void)
{
#if 1
  I2C_ResetChip();
  return 0;
#else     //�����λ
  int8_t ret = -1;

  gt9271_PortMultiTransmmit(GTP_REG_COMMAND, 0x02, 1);

  return ret;
#endif

}

/**
 * @brief   ����˯��ģʽ
 * @param ��
 * @retval 1Ϊ�ɹ�������Ϊʧ��
 */
int8_t GTP_Enter_Sleep(void)
{
  int8_t ret = -1;

//  gt9271_PortMultiTransmmit(GTP_REG_COMMENT, 0x05, 1);
  
  return ret;
}


//int8_t GTP_Send_Command(uint8_t command)
//{
//  int8_t ret = -1;
//
//  gt9271_PortMultiTransmmit(GTP_REG_COMMAND, GTP_COMMAND_READSTATUS, 1);
//    
//  return ret;
//}

/**
  * @brief   ���Ѵ�����
  * @param ��
  * @retval 0Ϊ�ɹ�������Ϊʧ��
  */
int8_t GTP_WakeUp_Sleep(void)
{
  int8_t ret = -1;

  GTP_I2C_Test();
  
  GTP_Reset_Guitar();
  
  return ret;
}

static int32_t GTP_Get_Info(void)
{
  uint8_t opr_buf[4] = {0};

  uint16_t abs_x_max = GTP_MAX_WIDTH;
  uint16_t abs_y_max = GTP_MAX_HEIGHT;
  uint8_t int_trigger_type = GTP_INT_TRIGGER;

  gt9271_PortMultiReceive(GTP_REG_CONFIG_DATA + 1, opr_buf, 4);

  abs_x_max = (opr_buf[1] << 8) + opr_buf[0];
  abs_y_max = (opr_buf[3] << 8) + opr_buf[2];

  gt9271_PortMultiReceive(GTP_REG_CONFIG_DATA + 6, opr_buf, 1);
  
  int_trigger_type = opr_buf[0] & 0x03;

  gt9271_printf("X_MAX = %d, Y_MAX = %d, TRIGGER = 0x%02x", abs_x_max,abs_y_max,int_trigger_type);

  return SUCCESS;    
}


/*******************************************************
Function:
    Read chip version.
Input:
    client:  i2c device
    version: buffer to keep ic firmware version
Output:
    read operation return.
        2: succeed, otherwise: failed
*******************************************************/
int32_t GTP_Read_Version(void)
{
  int32_t ret = -1;
  uint8_t version[4];

  gt9271_PortMultiReceive(GTP_REG_VERSION, version, 4);
  
  if(version[0] == '9' && version[1] == '2' && version[2] == '7' && version[3] == '1')
    return GT9271_ID;

  return ret;
}

/*******************************************************
Function:
    Initialize gtp.
Input:
    ts: goodix private data
Output:
    Executive outcomes.
        0: succeed, otherwise: failed
*******************************************************/
int32_t GTP_Init_Panel(void)
{
  
  int32_t ret = -1;
  int32_t i = 0;
  uint8_t config_data[2];
  uint8_t temp;
  
  I2C_ResetChip();
  if(GTP_I2C_Test() < 0)
  {
    gt9271_printf("I2C communication ERROR!");
    return ret;
  }
              
  //��ȡ����IC���ͺ�
  //����IC���ͺ�ָ��ͬ������
  if(GTP_Read_Version() == GT9271_ID)
  {
    temp = 0x02;
    gt9271_PortMultiTransmmit(GT9271_CTRL_REG, &temp, 1);
    gt9271_PortMultiReceive(GT9271_CFGS_REG, config_data, 1);
    
    if(config_data[0] < 0x41)
    {
      config_data[0] = 0;
      for(i = 0;i < sizeof(CTP_CFG_GT9271) - 2;i++)
        config_data[0] += CTP_CFG_GT9271[i];  

      config_data[0] = (~config_data[0]) + 1;

      gt9271_PortMultiTransmmit(GTP_REG_CONFIG_DATA, CTP_CFG_GT9271, sizeof(CTP_CFG_GT9271));
      gt9271_PortMultiTransmmit(GT9271_CHECK_REG, config_data, 2);
    }
    gt9271_Delayms(10);
    temp = 0x00;
    gt9271_PortMultiTransmmit(GT9271_CTRL_REG, &temp, 1);
    
  }
  
//  //���������������д����Ƿ���ͬ
//  uint8_t buf[200],j;
//
//  ret = gt9271_PortMultiReceive(GT9271_CFGS_REG, buf, sizeof(CTP_CFG_GT9271));
//
//  for(i = 0;i < sizeof(CTP_CFG_GT9271);i++)
//  {
//    if(CTP_CFG_GT9271[i] != buf[i])
//    {
////      gt9271_printf("Config fail ! i = %d ",i);
////      return -1;
//      j++;
//    }
//  }
//  gt9271_printf("Config success ! i = %d ",j);
      
//  GTP_Get_Info();
//  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  return 0;
}

void GTP_ReadCurrentTSCase(uint8_t *pData)
{
  gt9271_PortReceive(GT9271_GSTID_REG, pData);
}

void GTP_WriteCurrentTSCase(uint8_t Data)
{
  gt9271_PortTransmmit(GT9271_GSTID_REG, Data);
}

void GTP_ReadCurrentTSPoint(uint16_t Addr, uint8_t *pData, uint16_t Len)
{
  gt9271_PortMultiReceive(Addr, pData, Len);
}
