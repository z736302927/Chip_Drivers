#ifndef __MHYDROLOGY_H
#define __MHYDROLOGY_H

#include "stdint.h"
#include "kinetis/slist.h"

#define ELEMENT_COUNT                   298

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

#define SOH                             0x7E
#define STX                             0x02
#define SYN                             0x16
#define ETX                             0x03
#define ETB                             0x17
#define ENQ                             0x05
#define EOT                             0x04
#define ACK                             0x06
#define NAK                             0x15
#define ESC                             0x1B

typedef enum tagHydrologyMode {
    HYDROLOGY_M1,
    HYDROLOGY_M2,
    HYDROLOGY_M3,
    HYDROLOGY_M4,
} HydrologyMode;

typedef enum tagHydrologyRTUType {
    Rainfall = 0x50,
    RiverCourse = 0x48,
    Reservoir = 0x4B,
    GateDam = 0x5A,
    PumpingStation = 0x44,
    Tide = 0x54,
    SoilMoisture = 0x4D,
    Groundwater = 0x47,
    WaterQuality = 0x51,
    WaterIntake = 0x49,
    Outfall = 0x4F,
} HydrologyRTUType;

typedef enum tagHydrologyBodyType {
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
} HydrologyBodyType;

typedef enum tagHydrologyMsgSrcType {
    MsgFormServer,
    MsgFormClient
} HydrologyMsgSrcType;

//#pragma pack(1)

typedef struct tagHydrologyElementInfo {
    u8 ID;
    u8 D;
    u8 d;
    u32 Addr;
} HydrologyElementInfo;

typedef struct tagHydrologyElement {
    u8 guide[2];
    u8 *value;
    u32 num;
} HydrologyElement;

//ң��վ���б��ı�ͷ�ṹ
typedef struct tagHydrologyUpHeader {
    u8 framestart[2];
    u8 centeraddr;
    u8 remoteaddr[5];
    u8 password[2];
    u8 funcode;
    u8 dir_len[2];
    u8 paketstart;
    u8 count_seq[3];
    u8 len;
} HydrologyUpHeader;

//ң��վ���б��ı�ͷ�ṹ
typedef struct tagHydrologyDownHeader {
    u8 framestart[2];
    u8 remoteaddr[5];
    u8 centeraddr;
    u8 password[2];
    u8 funcode;
    u8 dir_len[2];
    u8 paketstart;
    u8 count_seq[3];
    u8 len;
} HydrologyDownHeader;

//ң��վ���б������Ľṹ
typedef struct tagHydrologyUpBody {
    u8 streamid[2];
    u8 sendtime[6];
    u8 rtuaddrid[2];
    u8 rtuaddr[5];
    u8 rtutype;
    u8 observationtimeid[2];
    u8 observationtime[5];
    u16 count;
    HydrologyElement **element;
    u16 len;
} HydrologyUpBody;

//ң��վ���б������Ľṹ
typedef struct tagHydrologyDownBody {
    u8 streamid[2];
    u8 sendtime[6];
    u8 rtuaddrid[2];
    u8 rtuaddr[5];
    u16 count;
    HydrologyElement **element;
    u16 len;
} HydrologyDownBody;

typedef struct tagHydrologyPacket {
    void *header;
    void *body;
    u8 end;
    u16 crc16;
    u8 *buffer;
    u16 len;
} HydrologyPacket;

typedef struct tagHydrology {
    HydrologyPacket *uppacket;
    HydrologyPacket *downpacket;
    unsigned source: 1;
} Hydrology;

//#pragma pack()

int Hydrology_read_file_size(char *filename, u32 *Size);
int Hydrology_ReadStoreInfo(char *filename, long addr, u8 *data, int len);
int Hydrology_WriteStoreInfo(char *filename, long addr, u8 *data, int len);
void Hydrology_ReadTime(u8 *time);
void Hydrology_SetTime(u8 *t_time);
int Hydrology_OpenPort(void);
int Hydrology_ClosePort(void);
int Hydrology_PortTransmmitData(u8 *pData, u16 Len);
int Hydrology_PortReceiveData(u8 **ppData, u16 *pLen, u32 Timeout);
void Hydrology_ReadObservationTime(HydrologyElementInfo *Element, u8 *observationtime);
void Hydrology_SetObservationTime(HydrologyElementInfo *Element);
void Hydrology_GetGuideID(u8 *value, u8 D, u8 d);
int Hydrology_ConvertToHexElement(double input, int D, int d, u8 *out);
int Hydrology_MallocElement(u8 element, u8 D, u8 d,
    HydrologyElement *ele);
void Hydrology_GetStreamID(u8 *streamid);
int HydrologyD_ProcessSend(HydrologyElementInfo *Element_table, u8 Count,
    HydrologyMode Mode, HydrologyBodyType Funcode);
int Hydrology_ReadSpecifiedElementInfo(HydrologyElementInfo *Element,
    HydrologyBodyType Funcode, u16 Index);
int HydrologyH_ProcessSend(HydrologyElementInfo *Element_table, u8 Count,
    HydrologyMode Mode, HydrologyBodyType Funcode, u8 End);
int HydrologyD_Process(HydrologyElementInfo *Element_table, u8 Count,
    HydrologyMode Mode, HydrologyBodyType Funcode);
void Hydrology_DisableLinkPacket(void);
void Hydrology_EnableLinkPacket(void);
void Hydrology_DisconnectLink(void);
int HydrologyD_Reboot(void);

extern Hydrology g_Hydrology;

#endif
