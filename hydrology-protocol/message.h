
#ifndef __MESSAGE_H
#define __MESSAGE_H

#include "stdint.h"

#define MAX_ELEMENT                     16

// ---------------- POPULAR POLYNOMIALS ----------------
// CCITT:      x^16 + x^12 + x^5 + x^0                 (0x1021)
// CRC-16:     x^16 + x^15 + x^2 + x^0                 (0x8005)
#define CRC_16_POLYNOMIALS              0x8005

#define HYDROLOGY_M1                    1
#define HYDROLOGY_M2                    2
#define HYDROLOGY_M3                    3
#define HYDROLOGY_M4                    4

#define HYDROLOGY_SOH1                  0x7E
#define HYDROLOGY_SOH2                  0x7E
#define HYDROLOGY_SOH3                  '*'
#define HYDROLOGY_STX                   0x02
#define HYDROLOGY_SYN                   0x16
#define HYDROLOGY_ETX                   0x03
#define HYDROLOGY_ETB                   0x17
#define HYDROLOGY_ENQ                   0x05
#define HYDROLOGY_EOT                   0x04
#define HYDROLOGY_ACK                   0x06
#define HYDROLOGY_NAK                   0x15
#define HYDROLOGY_ESC                   0x1B

#define HYDROLOGY_ANALOG                1
#define HYDROLOGY_PULSE                 2
#define HYDROLOGY_SWITCH                3
#define HYDROLOGY_STORE                 4

#define HYDROLOGY_ADC                   0
#define HYDROLOGY_ISR_COUNT             1
#define HYDROLOGY_IO_STATUS             2
#define HYDROLOGY_RS485                 3

#define ELEMENT_REGISTER(id, type, D, d, Mode) {id, type, D, d, Mode}
#define RS485RegisterCount              2

#define ERC1                            1
#define ERC2                            2
#define ERC3                            3
#define ERC4                            4
#define ERC5                            5
#define ERC6                            6
#define ERC7                            7
#define ERC8                            8
#define ERC9                            9
#define ERC10                           10
#define ERC11                           11
#define ERC12                           12
#define ERC13                           13
#define ERC14                           14
#define ERC15                           15
#define ERC16                           16
#define ERC17                           17
#define ERC18                           18
#define ERC19                           19

typedef enum taghydrologyBodyType
{
  LinkMaintenance = 0x2F,               //ң��վ��·ά�ֱ�
  Test,                                 //ң��վ���Ա�
  EvenPeriodInformation,                //����ʱ��ˮ����Ϣ��
  TimerReport,                          //ң��վ��ʱ��
  AddReport,                            //ң��վ�ӱ���
  Hour,                                 //ң��վСʱ��
  ArtificialNumber,                     //ң��վ�˹�������
  Picture,                              //ң��վͼƬ��
  Realtime,                             //����վ��ѯң��վʵʱ����
  Period,                               //����վ��ѯң��վʱ������
  InquireArtificialNumber,              //����վ��ѯң��վ�˹�����
  SpecifiedElement,                     //����վ��ѯң��վָ��Ҫ��ʵʱ����
  ConfigurationModification = 0x40,     //ң��վ�����޸�
  ConfigurationRead,                    //ң��վ���ö�ȡ
  ParameterModification,                //����վ�޸�ң��վ���в���
  ParameterRead,                        //����վ��ȡң��վ���в���
  WaterPumpMotor,                       //����վ��ѯˮ�õ��ʵʱ��������
  SoftwareVersion,                      //����վ��ѯң��վ��ѯң��վ����汾
  Status,                               //����վ��ѯң��վ״̬��Ϣ
  InitializeSolidStorage,               //��ʼ����̬�洢����
  Reset,                                //�ָ�ң��վ��������
  ChangePassword,                       //����վ�޸Ĵ�������
  SetClock,                             //����վ����ң��վʱ��
  SetICCard,                            //����վ����ң��վIC��״̬
  Pump,                                 //����վ����ң��վˮ�ÿ���������Ӧ/ ˮ��״̬�Ա�
  Valve,                                //����վ����ң��վ���Ʒ��ſ���������Ӧ/ ����״̬�Ա�
  Gate,                                 //����վ����ң��վ����բ�ſ���������Ӧ/ բ��״̬��Ϣ�Ա�
  WaterSetting,                         //����վ����ң��վˮ����ֵ����������Ӧ
  Record,                               //����վ��ѯң��վ�¼���¼
  Time,                                 //����վ��ѯң��վʱ��
  BodyTypeEnd                   
    
}hydrologyBodyType;

typedef enum taghydrologyMsgSrcType
{
  MsgFormServer,
  MsgFormClient
}hydrologyBMsgSrcType;

#pragma pack(1)
//Basic definition of elements
typedef struct tagHydrologyElement
{
  char guide[2];
  char num;
  char* value;
}hydrologyElement;

typedef struct tagHydrologyElementInfo
{
  char ID;
  char type;
  char D;
  char d;
  char Mode;
}hydrologyElementInfo;

//ң��վ���б��ı�ͷ�ṹ
typedef struct taghydrologyHeader
{
  char framestart[2];
  char centeraddr;
  char remoteaddr[5];
  char password[2];
  char funcode;
  char dir_len[2];
  char paketstart;
  char count_seq[3];
}hydrologyHeader;

//ң��վ���б������Ľṹ
typedef struct tagHydrologyUpBody
{
  char streamid[2];
  char sendtime[6];
  char rtuaddrid[2];
  char rtuaddr[5];
  char rtutype;
  char observationtimeid[2];
  char observationtime[5];
  int count;
  hydrologyElement element[MAX_ELEMENT];
  int len;
}hydrologyUpBody;

//ң��վ���б������Ľṹ
typedef struct tagHydrologyDownBody
{
  char streamid[2];
  char sendtime[6];
  char rtuaddrid[2];
  char rtuaddr[5];
  int count;
  hydrologyElement element[MAX_ELEMENT];
  int len;
}hydrologyDownBody;

typedef struct tagpacket
{
  void* header;
  void* upbody;
  void* downbody;
  char end;
  short crc16;
  int len;
}packet;

#pragma pack()


void Hydrology_ReadPassword(char* password);

void Hydrology_ReadCenterAddr(char* centeraddr);

void Hydrology_ReadRemoteAddr(char* remoteaddr);

void Hydrology_ReadTime(char* time);

void Hydrology_SetTime(char* time);

void Hydrology_ReadRTUType(char* type);

void Hydrology_ReadObservationTime(char id, char* observationtime, int index);

void Hydrology_SetObservationTime(char id, int index);

int Hydrology_ReadStoreInfo(long addr, char *data, int len);

int Hydrology_WriteStoreInfo(long addr, char *data, int len);

short hydrologyCRC16(char* pchMsg, int wDataLen);

int hydrologyProcessSend(char funcode);

int hydrologyProcessReceieve(char* input, int inputlen);

void hydrologySetMsgSrc(char Src);

int hydrologyReadMsgSrc(void);

void Hydrology_Printf(char *buff);

extern hydrologyHeader g_HydrologyUpHeader;
extern hydrologyHeader g_HydrologyDownHeader;
extern packet g_HydrologyMsg;
extern const hydrologyElementInfo Element_table[];
extern int SinglePacketSize;


extern uint8_t ADCElementCount;
extern uint8_t ISR_COUNTElementCount;
extern uint8_t IO_STATUSElementCount;
extern uint8_t RS485ElementCount;
#endif
