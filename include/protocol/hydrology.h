#ifndef __MHYDROLOGY_H
#define __MHYDROLOGY_H

#include "stdint.h"
#include "algorithm/k-slist.h"

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
    uint8_t ID;
    uint8_t D;
    uint8_t d;
    uint32_t Addr;
} HydrologyElementInfo;

typedef struct tagHydrologyElement {
    uint8_t guide[2];
    uint8_t *value;
    uint32_t num;
} HydrologyElement;

//ң��վ���б��ı�ͷ�ṹ
typedef struct tagHydrologyUpHeader {
    uint8_t framestart[2];
    uint8_t centeraddr;
    uint8_t remoteaddr[5];
    uint8_t password[2];
    uint8_t funcode;
    uint8_t dir_len[2];
    uint8_t paketstart;
    uint8_t count_seq[3];
    uint8_t len;
} HydrologyUpHeader;

//ң��վ���б��ı�ͷ�ṹ
typedef struct tagHydrologyDownHeader {
    uint8_t framestart[2];
    uint8_t remoteaddr[5];
    uint8_t centeraddr;
    uint8_t password[2];
    uint8_t funcode;
    uint8_t dir_len[2];
    uint8_t paketstart;
    uint8_t count_seq[3];
    uint8_t len;
} HydrologyDownHeader;

//ң��վ���б������Ľṹ
typedef struct tagHydrologyUpBody {
    uint8_t streamid[2];
    uint8_t sendtime[6];
    uint8_t rtuaddrid[2];
    uint8_t rtuaddr[5];
    uint8_t rtutype;
    uint8_t observationtimeid[2];
    uint8_t observationtime[5];
    uint16_t count;
    HydrologyElement **element;
    uint16_t len;
} HydrologyUpBody;

//ң��վ���б������Ľṹ
typedef struct tagHydrologyDownBody {
    uint8_t streamid[2];
    uint8_t sendtime[6];
    uint8_t rtuaddrid[2];
    uint8_t rtuaddr[5];
    uint16_t count;
    HydrologyElement **element;
    uint16_t len;
} HydrologyDownBody;

typedef struct tagHydrologyPacket {
    void *header;
    void *body;
    uint8_t end;
    uint16_t crc16;
    uint8_t *buffer;
    uint16_t len;
} HydrologyPacket;

typedef struct tagHydrology {
    HydrologyPacket *uppacket;
    HydrologyPacket *downpacket;
    unsigned source: 1;
} Hydrology;

//#pragma pack()

int Hydrology_ReadFileSize(char *filename, uint32_t *Size);
int Hydrology_ReadStoreInfo(char *filename, long addr, uint8_t *data, int len);
int Hydrology_WriteStoreInfo(char *filename, long addr, uint8_t *data, int len);
void Hydrology_ReadTime(uint8_t *time);
void Hydrology_SetTime(uint8_t *t_time);
int Hydrology_OpenPort(void);
int Hydrology_ClosePort(void);
int Hydrology_PortTransmmitData(uint8_t *pData, uint16_t Len);
int Hydrology_PortReceiveData(uint8_t **ppData, uint16_t *pLen, uint32_t Timeout);
void Hydrology_ReadObservationTime(HydrologyElementInfo *Element, uint8_t *observationtime);
void Hydrology_SetObservationTime(HydrologyElementInfo *Element);
void Hydrology_GetGuideID(uint8_t *value, uint8_t D, uint8_t d);
int Hydrology_ConvertToHexElement(double input, int D, int d, uint8_t *out);
int Hydrology_MallocElement(uint8_t element, uint8_t D, uint8_t d,
    HydrologyElement *ele);
void Hydrology_GetStreamID(uint8_t *streamid);
int HydrologyD_ProcessSend(HydrologyElementInfo *Element_table, uint8_t Count,
    HydrologyMode Mode, HydrologyBodyType Funcode);
int Hydrology_ReadSpecifiedElementInfo(HydrologyElementInfo *Element,
    HydrologyBodyType Funcode, uint16_t Index);
int HydrologyH_ProcessSend(HydrologyElementInfo *Element_table, uint8_t Count,
    HydrologyMode Mode, HydrologyBodyType Funcode, uint8_t End);
int HydrologyD_Process(HydrologyElementInfo *Element_table, uint8_t Count,
    HydrologyMode Mode, HydrologyBodyType Funcode);
void Hydrology_DisableLinkPacket(void);
void Hydrology_EnableLinkPacket(void);
void Hydrology_DisconnectLink(void);
int HydrologyD_Reboot(void);

extern Hydrology g_Hydrology;

#endif
