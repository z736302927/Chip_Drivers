#ifndef __MHYDROLOGY_H
#define __MHYDROLOGY_H

#include <linux/types.h>

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

enum hydrology_mode {
    HYDROLOGY_M1,
    HYDROLOGY_M2,
    HYDROLOGY_M3,
    HYDROLOGY_M4,
};

enum hydrology_rtu_type {
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
};

enum hydrology_body_type {
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
    Specifiedelement,                     //����վ��ѯң��վָ��Ҫ��ʵʱ����
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
};

enum hydrology_msg_src_type {
    MSG_FORM_SERVER,
    MSG_FORM_CLIENT
};

//#pragma pack(1)

struct hydrology_element_info {
    u8 ID;
    u8 D;
    u8 d;
    u32 addr;
};

struct hydrology_element {
    u8 guide[2];
    u8 *value;
    u32 num;
};

//ң��վ���б��ı�ͷ�ṹ
struct hydrology_up_header {
    u8 frame_start[2];
    u8 center_addr;
    u8 remote_addr[5];
    u8 password[2];
    u8 funcode;
    u8 dir_len[2];
    u8 paket_start;
    u8 count_seq[3];
    u8 len;
};

//ң��վ���б��ı�ͷ�ṹ
struct hydrology_down_header {
    u8 frame_start[2];
    u8 remote_addr[5];
    u8 center_addr;
    u8 password[2];
    u8 funcode;
    u8 dir_len[2];
    u8 paket_start;
    u8 count_seq[3];
    u8 len;
};

//ң��վ���б������Ľṹ
struct hydrology_up_body {
    u8 stream_id[2];
    u8 send_time[6];
    u8 rtu_addr_id[2];
    u8 rtu_addr[5];
    u8 rtu_type;
    u8 observationtimeid[2];
    u8 observation_time[5];
    u16 count;
    struct hydrology_element **element;
    u16 len;
};

//ң��վ���б������Ľṹ
struct hydrology_down_body {
    u8 stream_id[2];
    u8 send_time[6];
    u8 rtu_addr_id[2];
    u8 rtu_addr[5];
    u16 count;
    struct hydrology_element **element;
    u16 len;
};

struct hydrology_packet {
    void *header;
    void *body;
    u8 end;
    u16 crc16;
    u8 *buffer;
    u16 len;
};

struct hydrology {
    struct hydrology_packet *up_packet;
    struct hydrology_packet *down_packet;
    unsigned source: 1;
};

//#pragma pack()

int hydrology_read_file_size(char *filename, u32 *Size);
int hydrology_read_store_info(char *filename, long addr, u8 *data, int len);
int hydrology_write_store_info(char *filename, long addr, u8 *data, int len);
void hydrology_get_time(u8 *time);
void hydrology_set_time(u8 *time);
int hydrology_open_port(void);
int hydrology_close_port(void);
int hydrology_port_transmmit(u8 *pdata, u16 length);
int hydrology_port_receive(u8 **ppdata, u16 *plength, u32 Timeout);
void hydrology_get_observation_time(struct hydrology_element_info *element, u8 *observation_time);
void hydrology_set_observation_time(struct hydrology_element_info *element);
void hydrology_get_guide_id(u8 *value, u8 D, u8 d);
int hydrology_convert_to_hex_element(double input, int D, int d, u8 *out);
int hydrology_malloc_element(u8 guide, u8 D, u8 d,
    struct hydrology_element *element);
void hydrology_get_stream_id(u8 *stream_id);
int hydrology_device_process_send(struct hydrology_element_info *element_table, u8 cnt,
    enum hydrology_mode mode, enum hydrology_body_type funcode);
int hydrology_read_specified_element_info(struct hydrology_element_info *element,
    enum hydrology_body_type funcode, u16 index);
int hydrology_host_process_send(struct hydrology_element_info *element_table, u8 cnt,
    enum hydrology_mode mode, enum hydrology_body_type funcode, u8 end);
int hydrology_device_process(struct hydrology_element_info *element_table, u8 cnt,
    enum hydrology_mode mode, enum hydrology_body_type funcode);
void hydrology_disable_link_packet(void);
void hydrology_enable_link_packet(void);
void hydrology_disconnect_link(void);
int hydrology_device_reboot(void);

extern struct hydrology g_hydrology;

#endif
