#include "kinetis/hydrology.h"
#include "kinetis/hydrology-config.h"
#include "kinetis/hydrology-cmd.h"
#include "kinetis/hydrology-identifier.h"
#include "kinetis/idebug.h"
#include "kinetis/basic-timer.h"
#include "kinetis/delay.h"

#include "string.h"

#include <linux/slab.h>
#include <linux/crc16.h>

/* The following program is modified by the user according to the hardware device, otherwise the driver cannot run. */

/**
  * @step 1:  Modify the corresponding function according to the modified area and the corresponding function name.
  * @step 2:  Modify four areas: GPIO_PORT/GPIO_PIN/Led_TypeDef/LEDn.
  * @step 3:  .
  * @step 4:  .
  * @step 5:
  */


/* The above procedure is modified by the user according to the hardware device, otherwise the driver cannot run. */

int hydrology_device_init_send(u8 cnt, enum hydrology_body_type funcode)
{
    int i = 0;
    struct hydrology_up_body *upbody;

    g_hydrology.up_packet = kmalloc(sizeof(struct hydrology_packet), __GFP_ZERO);

    if (g_hydrology.up_packet == NULL) {
        printk(KERN_DEBUG "g_hydrology.up_packet malloc failed\n");
        return false;
    }

    g_hydrology.up_packet->header = kmalloc(sizeof(struct hydrology_up_header), __GFP_ZERO);

    if (g_hydrology.up_packet->header == NULL) {
        printk(KERN_DEBUG "g_hydrology.up_packet->header malloc failed\n");
        return false;
    }

    g_hydrology.up_packet->body = kmalloc(sizeof(struct hydrology_up_body), __GFP_ZERO);

    if (g_hydrology.up_packet->body == NULL) {
        printk(KERN_DEBUG "g_hydrology.up_packet->body malloc failed\n");
        return false;
    }

    upbody = (struct hydrology_up_body *)g_hydrology.up_packet->body;
    upbody->count = cnt;

    switch (funcode) {
        case LinkMaintenance:
        case InitializeSolidStorage:
        case Reset:
        case SetClock:
        case Time:
            upbody->count = 0;
            break;

        case Test:
        case EvenPeriodInformation:
        case TimerReport:
        case AddReport:
        case Hour:
        case Realtime:
        case Period:
        case Specifiedelement:
        case ConfigurationModification:
        case ConfigurationRead:
        case ParameterModification:
        case ParameterRead:
        case WaterPumpMotor:
            break;

        case ArtificialNumber:
        case Picture:
        case InquireArtificialNumber:
        case SoftwareVersion:
        case Status:
        case ChangePassword:
        case SetICCard:
        case Pump:
        case Valve:
        case Gate:
        case WaterSetting:
        case Record:
            upbody->count = 1;
            break;
    }

    if (upbody->count > 0) {
        upbody->element = kmalloc(sizeof(struct hydrology_element *) * upbody->count, __GFP_ZERO);

        if (upbody->element == NULL) {
            printk(KERN_DEBUG "upbody->element malloc failed\n");
            return false;
        }
    }

    for (i = 0; i < upbody->count; ++i) {
        upbody->element[i] = kmalloc(sizeof(struct hydrology_element), __GFP_ZERO);

        if (upbody->element[i] == NULL) {
            printk(KERN_DEBUG "upbody->element[%d] malloc failed\n", i);
            return false;
        }
    }

    return true;
}

void hydrology_device_exit_send(void)
{
    int i = 0;
    struct hydrology_up_body *upbody = (struct hydrology_up_body *)g_hydrology.up_packet->body;

    for (i = 0; i < upbody->count; i++) {
        if (upbody->element[i]->value != NULL) {
            kfree(upbody->element[i]->value);
            upbody->element[i]->value = NULL;
        }

        if (upbody->element[i] != NULL) {
            kfree(upbody->element[i]);
            upbody->element[i] = NULL;
        }
    }

    if (upbody->element != NULL) {
        kfree(upbody->element);
        upbody->element = NULL;
    }

    if (g_hydrology.up_packet->header != NULL) {
        kfree(g_hydrology.up_packet->header);
        g_hydrology.up_packet->header = NULL;
    }

    if (g_hydrology.up_packet->body != NULL) {
        kfree(g_hydrology.up_packet->body);
        g_hydrology.up_packet->body = NULL;
    }

    if (g_hydrology.up_packet->buffer != NULL) {
        kfree(g_hydrology.up_packet->buffer);
        g_hydrology.up_packet->buffer = NULL;
    }

    if (g_hydrology.up_packet != NULL) {
        kfree(g_hydrology.up_packet);
        g_hydrology.up_packet = NULL;
    }
}

int hydrology_device_init_receieve()
{
    g_hydrology.down_packet = kmalloc(sizeof(struct hydrology_packet), __GFP_ZERO);

    if (g_hydrology.down_packet == NULL) {
        printk(KERN_DEBUG "g_hydrology.down_packet malloc failed\n");
        return false;
    }

    g_hydrology.down_packet->header = kmalloc(sizeof(struct hydrology_down_header), __GFP_ZERO);

    if (g_hydrology.down_packet->header == NULL) {
        printk(KERN_DEBUG "g_hydrology.down_packet->header malloc failed\n");
        return false;
    }

    g_hydrology.down_packet->body = kmalloc(sizeof(struct hydrology_down_body), __GFP_ZERO);

    if (g_hydrology.down_packet->body == NULL) {
        printk(KERN_DEBUG "g_hydrology.down_packet->body malloc failed\n");
        return false;
    }

    return true;
}

void hydrology_device_exit_receieve()
{
    int i = 0;
    struct hydrology_down_body *down_body = (struct hydrology_down_body *)g_hydrology.down_packet->body;

    for (i = 0; i < down_body->count; i++) {
        if (down_body->element[i]->value != NULL) {
            kfree(down_body->element[i]->value);
            down_body->element[i]->value = NULL;
        }

        if (down_body->element[i] != NULL) {
            kfree(down_body->element[i]);
            down_body->element[i] = NULL;
        }
    }

    if (down_body->element != NULL) {
        kfree(down_body->element);
        down_body->element = NULL;
    }

    if (g_hydrology.down_packet->header != NULL) {
        kfree(g_hydrology.down_packet->header);
        g_hydrology.down_packet->header = NULL;
    }

    if (g_hydrology.down_packet->body != NULL) {
        kfree(g_hydrology.down_packet->body);
        g_hydrology.down_packet->body = NULL;
    }

    if (g_hydrology.down_packet->buffer != NULL) {
        kfree(g_hydrology.down_packet->buffer);
        g_hydrology.down_packet->buffer = NULL;
    }

    if (g_hydrology.down_packet != NULL) {
        kfree(g_hydrology.down_packet);
        g_hydrology.down_packet = NULL;
    }
}

static void hydrology_device_set_up_header_sequence(u16 cnt, u16 Total)
{
    struct hydrology_up_header *header = g_hydrology.up_packet->header;

    header->count_seq[0] = Total >> 4;
    header->count_seq[1] = ((Total & 0x000F) << 4) + (cnt >> 8);
    header->count_seq[2] = cnt & 0x00FF;
}

static void hydrology_device_make_up_header(enum hydrology_mode mode, enum hydrology_body_type funcode)
{
    struct hydrology_up_header *header = g_hydrology.up_packet->header;

    header->frame_start[0] = SOH;
    header->frame_start[1] = SOH;
    header->len += 2;

    hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_BA_CENTER,
        &(header->center_addr), 1);
    header->len += 1;
    hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_BA_REMOTE,
        header->remote_addr, 5);
    header->len += 5;

    hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_BA_PASSWORD,
        header->password, 2);
    header->len += 2;

    header->funcode = funcode;
    header->len += 1;
    header->dir_len[0] = 0 << 4;
    header->len += 2;

    switch (mode) {
        case HYDROLOGY_M1:
        case HYDROLOGY_M2:
        case HYDROLOGY_M4:
            header->paket_start = STX;
            header->len += 1;
            break;

        case HYDROLOGY_M3:
            header->paket_start = SYN;
            header->len += 1;
            break;
    }
}

static int hydrology_device_make_up_body(struct hydrology_element_info *element_table, enum hydrology_body_type funcode)
{
    struct hydrology_up_body *upbody = (struct hydrology_up_body *)g_hydrology.up_packet->body;
    struct hydrology_element_info element[1] = {HYDROLOGY_E_PIC};
    int i;

    upbody->len = 0;

    switch (funcode) {
        case LinkMaintenance:
            hydrology_get_stream_id(upbody->stream_id);
            upbody->len += 2;
            hydrology_get_time(upbody->send_time);
            upbody->len += 6;
            break;

        case Test:
        case EvenPeriodInformation:
        case TimerReport:
        case AddReport:
        case Hour:
        case WaterPumpMotor:
        case Realtime:
        case Period:
        case Specifiedelement:
            hydrology_get_stream_id(upbody->stream_id);
            upbody->len += 2;
            hydrology_get_time(upbody->send_time);
            upbody->len += 6;
            upbody->rtu_addr_id[0] = 0xF1;
            upbody->rtu_addr_id[1] = 0xF1;
            upbody->len += 2;
            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_BA_REMOTE,
                upbody->rtu_addr, 5);
            upbody->len += 5;
            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_PDA_RTUTYPE,
                &(upbody->rtu_type), 1);
            upbody->len += 1;
            upbody->observationtimeid[0] = 0xF0;
            upbody->observationtimeid[1] = 0xF0;
            upbody->len += 2;
            hydrology_get_observation_time(&element_table[0], upbody->observation_time);
            upbody->len += 5;

            for (i = 0; i < upbody->count; i++) {
                if (hydrology_malloc_element(element_table[i].ID,
                        element_table[i].D, element_table[i].d,
                        upbody->element[i]) == false)
                    return false;

                hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, element_table[i].addr,
                    upbody->element[i]->value, upbody->element[i]->num);

                upbody->len += upbody->element[i]->num + 2;
            }

            break;

        case ArtificialNumber:
        case InquireArtificialNumber:
            hydrology_get_stream_id(upbody->stream_id);
            upbody->len += 2;
            hydrology_get_time(upbody->send_time);
            upbody->len += 6;
            upbody->element[0]->guide[0] = 0xF2;
            upbody->element[0]->guide[1] = 0xF2;
            hydrology_read_file_size(HYDROLOGY_D_FILE_RGZS, &(upbody->element[0]->num));
            upbody->len += upbody->element[0]->num + 2;
            break;

        case Picture:
            hydrology_get_stream_id(upbody->stream_id);
            upbody->len += 2;
            hydrology_get_time(upbody->send_time);
            upbody->len += 6;
            upbody->rtu_addr_id[0] = 0xF1;
            upbody->rtu_addr_id[1] = 0xF1;
            upbody->len += 2;
            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_BA_REMOTE,
                upbody->rtu_addr, 5);
            upbody->len += 5;
            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_PDA_RTUTYPE,
                &(upbody->rtu_type), 1);
            upbody->len += 1;
            upbody->observationtimeid[0] = 0xF0;
            upbody->observationtimeid[1] = 0xF0;
            upbody->len += 2;
            hydrology_get_observation_time(element, upbody->observation_time);
            upbody->len += 5;

            upbody->element[0]->guide[0] = 0xF3;
            upbody->element[0]->guide[1] = 0xF3;
            hydrology_read_file_size(HYDROLOGY_D_FILE_PICTURE,
                &(upbody->element[0]->num));
            upbody->len += upbody->element[0]->num + 2;

            break;

        case Status:
        case ConfigurationModification:
        case ConfigurationRead:
        case ParameterModification:
        case ParameterRead:
            hydrology_get_stream_id(upbody->stream_id);
            upbody->len += 2;
            hydrology_get_time(upbody->send_time);
            upbody->len += 6;
            upbody->rtu_addr_id[0] = 0xF1;
            upbody->rtu_addr_id[1] = 0xF1;
            upbody->len += 2;
            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_BA_REMOTE,
                upbody->rtu_addr, 5);
            upbody->len += 5;

            for (i = 0; i < upbody->count; i++) {
                if (hydrology_malloc_element(element_table[i].ID,
                        element_table[i].D, element_table[i].d,
                        upbody->element[i]) == false)
                    return false;

                hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, element_table[i].addr,
                    upbody->element[i]->value, upbody->element[i]->num);

                upbody->len += upbody->element[i]->num + 2;
            }

            break;

        case SoftwareVersion:
            hydrology_get_stream_id(upbody->stream_id);
            upbody->len += 2;
            hydrology_get_time(upbody->send_time);
            upbody->len += 6;
            upbody->rtu_addr_id[0] = 0xF1;
            upbody->rtu_addr_id[1] = 0xF1;
            upbody->len += 2;
            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_BA_REMOTE,
                upbody->rtu_addr, 5);
            upbody->len += 5;
            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_PDA_SW_VERSION_LEN,
                upbody->element[0]->guide, 1);
            upbody->element[0]->num = upbody->element[0]->guide[0];
            upbody->element[0]->value = kmalloc(upbody->element[0]->num, __GFP_ZERO);

            if (NULL == upbody->element[0]->value) {
                printk(KERN_DEBUG "upbody->element[0]->value malloc failed\n");
                return false;
            }

            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_PDA_SW_VERSION,
                upbody->element[0]->value, upbody->element[0]->num);
            upbody->len += upbody->element[0]->num + 1;
            break;

        case InitializeSolidStorage:
        case Reset:
        case SetClock:
            hydrology_get_stream_id(upbody->stream_id);
            upbody->len += 2;
            hydrology_get_time(upbody->send_time);
            upbody->len += 6;
            upbody->rtu_addr_id[0] = 0xF1;
            upbody->rtu_addr_id[1] = 0xF1;
            upbody->len += 2;
            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_BA_REMOTE,
                upbody->rtu_addr, 5);
            upbody->len += 5;
            break;

        case ChangePassword:
        case SetICCard:
            hydrology_get_stream_id(upbody->stream_id);
            upbody->len += 2;
            hydrology_get_time(upbody->send_time);
            upbody->len += 6;
            upbody->rtu_addr_id[0] = 0xF1;
            upbody->rtu_addr_id[1] = 0xF1;
            upbody->len += 2;
            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_BA_REMOTE,
                upbody->rtu_addr, 5);
            upbody->len += 5;

            if (hydrology_malloc_element(element_table[0].ID,
                    element_table[0].D, element_table[0].d,
                    upbody->element[0]) == false)
                return false;

            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, element_table[0].addr,
                upbody->element[0]->value, upbody->element[0]->num);

            upbody->len += upbody->element[0]->num + 2;
            break;

        case Pump:
            hydrology_get_stream_id(upbody->stream_id);
            upbody->len += 2;
            hydrology_get_time(upbody->send_time);
            upbody->len += 6;
            upbody->rtu_addr_id[0] = 0xF1;
            upbody->rtu_addr_id[1] = 0xF1;
            upbody->len += 2;
            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_BA_REMOTE,
                upbody->rtu_addr, 5);
            upbody->len += 5;
            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_PDA_PUMP_LEN,
                upbody->element[0]->guide, 1);
            upbody->element[0]->num = upbody->element[0]->guide[0];
            upbody->element[0]->value = kmalloc(upbody->element[0]->num, __GFP_ZERO);

            if (NULL == upbody->element[0]->value) {
                printk(KERN_DEBUG "upbody->element[0]->value malloc failed\n");
                return false;
            }

            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_PDA_PUMP,
                upbody->element[0]->value, upbody->element[0]->num);
            upbody->len += upbody->element[0]->num + 1;
            break;

        case Valve:
            hydrology_get_stream_id(upbody->stream_id);
            upbody->len += 2;
            hydrology_get_time(upbody->send_time);
            upbody->len += 6;
            upbody->rtu_addr_id[0] = 0xF1;
            upbody->rtu_addr_id[1] = 0xF1;
            upbody->len += 2;
            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_BA_REMOTE,
                upbody->rtu_addr, 5);
            upbody->len += 5;
            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_PDA_VALVE_LEN,
                upbody->element[0]->guide, 1);
            upbody->element[0]->num = upbody->element[0]->guide[0];
            upbody->element[0]->value = kmalloc(upbody->element[0]->num, __GFP_ZERO);

            if (NULL == upbody->element[0]->value) {
                printk(KERN_DEBUG "upbody->element[0]->value malloc failed\n");
                return false;
            }

            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_PDA_VALVE,
                upbody->element[0]->value, upbody->element[0]->num);
            upbody->len += upbody->element[0]->num + 1;
            break;

        case Gate:
            hydrology_get_stream_id(upbody->stream_id);
            upbody->len += 2;
            hydrology_get_time(upbody->send_time);
            upbody->len += 6;
            upbody->rtu_addr_id[0] = 0xF1;
            upbody->rtu_addr_id[1] = 0xF1;
            upbody->len += 2;
            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_BA_REMOTE,
                upbody->rtu_addr, 5);
            upbody->len += 5;
            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_PDA_GATE_LEN,
                upbody->element[0]->guide, 1);

            if (upbody->element[0]->guide[0] % 8 == 0)
                upbody->element[0]->num = upbody->element[0]->guide[0] / 8;
            else
                upbody->element[0]->num = upbody->element[0]->guide[0] / 8 + 1;

            upbody->element[0]->value = kmalloc(upbody->element[0]->num, __GFP_ZERO);

            if (NULL == upbody->element[0]->value) {
                printk(KERN_DEBUG "upbody->element[0]->value malloc failed\n");
                return false;
            }

            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_PDA_GATE,
                upbody->element[0]->value, upbody->element[0]->num);
            upbody->len += upbody->element[0]->num + 1;
            break;

        case WaterSetting:
            hydrology_get_stream_id(upbody->stream_id);
            upbody->len += 2;
            hydrology_get_time(upbody->send_time);
            upbody->len += 6;
            upbody->rtu_addr_id[0] = 0xF1;
            upbody->rtu_addr_id[1] = 0xF1;
            upbody->len += 2;
            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_BA_REMOTE,
                upbody->rtu_addr, 5);
            upbody->len += 5;
            upbody->element[0]->guide[0] = 1;
            upbody->element[0]->num = upbody->element[0]->guide[0];
            upbody->element[0]->value = kmalloc(upbody->element[0]->num, __GFP_ZERO);

            if (NULL == upbody->element[0]->value) {
                printk(KERN_DEBUG "upbody->element[0]->value malloc failed\n");
                return false;
            }

            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_PDA_WATERSETTING,
                upbody->element[0]->value, upbody->element[0]->num);
            upbody->len += upbody->element[0]->num;
            break;

        case Record:
            hydrology_get_stream_id(upbody->stream_id);
            upbody->len += 2;
            hydrology_get_time(upbody->send_time);
            upbody->len += 6;
            upbody->rtu_addr_id[0] = 0xF1;
            upbody->rtu_addr_id[1] = 0xF1;
            upbody->len += 2;
            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_BA_REMOTE,
                upbody->rtu_addr, 5);
            upbody->len += 5;
            upbody->element[0]->guide[0] = 64;
            upbody->element[0]->num = upbody->element[0]->guide[0];
            upbody->element[0]->value = kmalloc(upbody->element[0]->num, __GFP_ZERO);

            if (NULL == upbody->element[0]->value) {
                printk(KERN_DEBUG "upbody->element[0]->value malloc failed\n");
                return false;
            }

            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_PDA_RECORD,
                upbody->element[0]->value, upbody->element[0]->num);
            upbody->len += upbody->element[0]->num;
            break;

        case Time:
            hydrology_get_stream_id(upbody->stream_id);
            upbody->len += 2;
            hydrology_get_time(upbody->send_time);
            upbody->len += 6;
            upbody->rtu_addr_id[0] = 0xF1;
            upbody->rtu_addr_id[1] = 0xF1;
            upbody->len += 2;
            hydrology_read_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_BA_REMOTE,
                upbody->rtu_addr, 5);
            upbody->len += 5;
            break;
    }

    return true;
}

static int hydrology_device_split_transfer(u8 *body_buffer, u8 *buffer,
    struct hydrology_up_header *header, struct hydrology_up_body *upbody, u16 i,  u16 total)
{
    u16 pointer;

    hydrology_device_set_up_header_sequence(i, total);

    header->dir_len[0] = 0;
    header->dir_len[1] = 0;

    if (i == total) {
        header->dir_len[0] |= (upbody->len % HYDROLOGY_BODY_MAX_LEN) >> 8;
        header->dir_len[1] |= (upbody->len % HYDROLOGY_BODY_MAX_LEN) & 0xFF;
    } else {
        header->dir_len[0] |= HYDROLOGY_BODY_MAX_LEN >> 8;
        header->dir_len[1] |= HYDROLOGY_BODY_MAX_LEN & 0xFF;
    }

    memcpy(buffer, header, sizeof(struct hydrology_up_header) - 1);
    pointer = sizeof(struct hydrology_up_header) - 1;

    if (i == total) {
        memcpy(&buffer[pointer], &body_buffer[(i - 1) * HYDROLOGY_BODY_MAX_LEN],
            upbody->len % HYDROLOGY_BODY_MAX_LEN);
        pointer += upbody->len % HYDROLOGY_BODY_MAX_LEN;
        g_hydrology.up_packet->end = ETX;
    } else {
        memcpy(&buffer[pointer], &body_buffer[(i - 1) * HYDROLOGY_BODY_MAX_LEN],
            HYDROLOGY_BODY_MAX_LEN);
        pointer += HYDROLOGY_BODY_MAX_LEN;
        g_hydrology.up_packet->end = ETB;
    }

    buffer[pointer] = g_hydrology.up_packet->end;
    pointer += 1;

    g_hydrology.up_packet->crc16 = crc16(0xFFFF, buffer, pointer);
    buffer[pointer] = g_hydrology.up_packet->crc16 >> 8;
    pointer += 1;
    buffer[pointer] = g_hydrology.up_packet->crc16 & 0xFF;
    pointer += 1;
    g_hydrology.up_packet->len = pointer;

    if (hydrology_port_transmmit(g_hydrology.up_packet->buffer, g_hydrology.up_packet->len) == false)
        return false;

    return true;
}
static int hydrology_device_make_up_tail_and_send(enum hydrology_body_type funcode)
{
    u8 *buffer;
    u8 *body_buffer;
    u16 i, j, k, l;
    u16 buffer_size;
    u16 pointer, body_pointer;
    u16 total;
    struct hydrology_up_header *header = g_hydrology.up_packet->header;
    struct hydrology_up_body *upbody = (struct hydrology_up_body *)g_hydrology.up_packet->body;

    if (upbody->len <= HYDROLOGY_BODY_MAX_LEN) {
        buffer_size = header->len + upbody->len + 3;
        g_hydrology.up_packet->buffer = kmalloc(buffer_size, __GFP_ZERO);

        if (g_hydrology.up_packet->buffer == NULL) {
            printk(KERN_DEBUG "g_hydrology.up_packet->buffer malloc failed\n");
            return false;
        }

        buffer = g_hydrology.up_packet->buffer;

        header->dir_len[0] |= (upbody->len) >> 8;
        header->dir_len[1] |= (upbody->len) & 0xFF;
        memcpy(buffer, header, sizeof(struct hydrology_up_header));
        pointer = sizeof(struct hydrology_up_header) - 4;

        switch (funcode) {
            case LinkMaintenance:
                memcpy(&buffer[pointer], upbody, 8);
                pointer += 8;
                break;

            case EvenPeriodInformation:
            case Period:
                memcpy(&buffer[pointer], upbody, 23);
                pointer += 23;
                memcpy(&buffer[pointer], upbody->element[0]->guide, 2);
                pointer += 2;
                memcpy(&buffer[pointer], upbody->element[0]->value,
                    upbody->element[0]->num);
                pointer += upbody->element[0]->num;

                for (i = 1; i < upbody->count; i++) {
                    memcpy(&buffer[pointer], upbody->element[i]->guide, 2);
                    pointer += 2;
                }

                for (k = 0, j = 0, l = 0; k < 12; k++) {
                    for (i = 1; i < upbody->count; i++) {
                        if (upbody->element[i]->num / 12 == 1)
                            memcpy(&buffer[pointer], &upbody->element[i]->value[j],
                                upbody->element[i]->num / 12);
                        else if (upbody->element[i]->num / 12 == 2)
                            memcpy(&buffer[pointer], &upbody->element[i]->value[l],
                                upbody->element[i]->num / 12);

                        pointer += upbody->element[i]->num / 12;
                    }

                    j += 1;
                    l += 2;
                }

                break;

            case Test:
            case TimerReport:
            case AddReport:
            case Hour:
            case Realtime:
            case Specifiedelement:
            case WaterPumpMotor:
                memcpy(&buffer[pointer], upbody, 23);
                pointer += 23;

                for (i = 0; i < upbody->count; i++) {
                    memcpy(&buffer[pointer], upbody->element[i]->guide, 2);
                    pointer += 2;
                    memcpy(&buffer[pointer], upbody->element[i]->value, upbody->element[i]->num);
                    pointer += upbody->element[i]->num;
                }

                break;

            case ArtificialNumber:
            case InquireArtificialNumber:
                memcpy(&buffer[pointer], upbody, 8);
                pointer += 8;
                memcpy(&buffer[pointer], upbody->element[0]->guide, 2);
                pointer += 2;
                hydrology_read_store_info(HYDROLOGY_D_FILE_RGZS,
                    0, &buffer[pointer],  upbody->element[i]->num);
                pointer += upbody->element[i]->num;
                break;

            case Picture:
                memcpy(&buffer[pointer], upbody, 8);
                pointer += 8;
                memcpy(&buffer[pointer], upbody->element[0]->guide, 2);
                pointer += 2;
                hydrology_read_store_info(HYDROLOGY_D_FILE_PICTURE,
                    0, &buffer[pointer],  upbody->element[i]->num);
                pointer += upbody->element[i]->num;
                break;

            case ConfigurationModification:
            case ConfigurationRead:
            case ParameterModification:
            case ParameterRead:
            case Status:
                memcpy(&buffer[pointer], upbody, 15);
                pointer += 15;

                for (i = 0; i < upbody->count; i++) {
                    memcpy(&buffer[pointer], upbody->element[i]->guide, 2);
                    pointer += 2;
                    memcpy(&buffer[pointer], upbody->element[i]->value, upbody->element[i]->num);
                    pointer += upbody->element[i]->num;
                }

                break;

            case SoftwareVersion:
                memcpy(&buffer[pointer], upbody, 15);
                pointer += 15;
                memcpy(&buffer[pointer], upbody->element[0]->guide, 1);
                pointer += 1;
                memcpy(&buffer[pointer], upbody->element[0]->value, upbody->element[0]->num);
                pointer += upbody->element[0]->num;
                break;

            case InitializeSolidStorage:
            case Reset:
            case SetClock:
            case Time:
                memcpy(&buffer[pointer], upbody, 15);
                pointer += 15;
                break;

            case ChangePassword:
            case SetICCard:
                memcpy(&buffer[pointer], upbody, 15);
                pointer += 15;
                memcpy(&buffer[pointer], upbody->element[0]->guide, 2);
                pointer += 2;
                memcpy(&buffer[pointer], upbody->element[0]->value, upbody->element[0]->num);
                pointer += upbody->element[0]->num;
                break;

            case Pump:
            case Valve:
            case Gate:
                memcpy(&buffer[pointer], upbody, 15);
                pointer += 15;
                memcpy(&buffer[pointer], upbody->element[0]->guide, 1);
                pointer += 1;
                memcpy(&buffer[pointer], upbody->element[0]->value, upbody->element[0]->num);
                pointer += upbody->element[0]->num;
                break;

            case WaterSetting:
            case Record:
                memcpy(&buffer[pointer], upbody, 15);
                pointer += 15;
                memcpy(&buffer[pointer], upbody->element[0]->value, upbody->element[0]->num);
                pointer += upbody->element[0]->num;
                break;
        }

        g_hydrology.up_packet->end = ETX;
        buffer[pointer] = g_hydrology.up_packet->end;
        pointer += 1;

        g_hydrology.up_packet->crc16 = crc16(0xFFFF, buffer, pointer);
        buffer[pointer] = g_hydrology.up_packet->crc16 >> 8;
        pointer += 1;
        buffer[pointer] = g_hydrology.up_packet->crc16 & 0xFF;
        pointer += 1;

        g_hydrology.up_packet->len = pointer;

        if (hydrology_port_transmmit(g_hydrology.up_packet->buffer, g_hydrology.up_packet->len) == false)
            return false;
    } else {
        printk(KERN_DEBUG "[warnning]upbody->len(%d) > HYDROLOGY_BODY_MAX_LEN(%d)\n",
            upbody->len, HYDROLOGY_BODY_MAX_LEN);
        printk(KERN_DEBUG "It will execute split transfer.\n");

        buffer_size = header->len + HYDROLOGY_BODY_MAX_LEN + 3;
        g_hydrology.up_packet->buffer = kmalloc(buffer_size, __GFP_ZERO);

        if (g_hydrology.up_packet->buffer == NULL) {
            printk(KERN_DEBUG "g_hydrology.up_packet->buffer malloc failed\n");
            return false;
        }

        buffer = g_hydrology.up_packet->buffer;

        body_buffer = kmalloc(upbody->len, __GFP_ZERO);

        if (body_buffer == NULL) {
            printk(KERN_DEBUG "body_buffer malloc failed\n");
            return false;
        }

        if (upbody->len % HYDROLOGY_BODY_MAX_LEN == 0)
            total = upbody->len / HYDROLOGY_BODY_MAX_LEN;
        else
            total = upbody->len / HYDROLOGY_BODY_MAX_LEN + 1;

        header->paket_start = SYN;
        body_pointer = 0;

        switch (funcode) {
            case LinkMaintenance:
                memcpy(&body_buffer[body_pointer], upbody, 8);
                body_pointer += 8;

                for (i = 1; i <= total; ++i)
                    hydrology_device_split_transfer(body_buffer, buffer, header, upbody, i, total);

                break;

            case EvenPeriodInformation:
            case Period:
                memcpy(&body_buffer[body_pointer], upbody, 23);
                body_pointer += 23;
                memcpy(&body_buffer[body_pointer], upbody->element[0]->guide, 2);
                body_pointer += 2;
                memcpy(&body_buffer[body_pointer], upbody->element[0]->value,
                    upbody->element[0]->num);
                body_pointer += upbody->element[0]->num;

                for (i = 1; i < upbody->count; i++) {
                    memcpy(&body_buffer[body_pointer], upbody->element[i]->guide, 2);
                    body_pointer += 2;
                }

                for (k = 0, j = 0, l = 0; k < 12; k++) {
                    for (i = 1; i < upbody->count; i++) {
                        if (upbody->element[i]->num / 12 == 1)
                            memcpy(&body_buffer[body_pointer], &upbody->element[i]->value[j],
                                upbody->element[i]->num / 12);
                        else if (upbody->element[i]->num / 12 == 2)
                            memcpy(&body_buffer[body_pointer], &upbody->element[i]->value[l],
                                upbody->element[i]->num / 12);

                        body_pointer += upbody->element[i]->num / 12;
                    }

                    j += 1;
                    l += 2;
                }

                for (i = 1; i <= total; ++i)
                    hydrology_device_split_transfer(body_buffer, buffer, header, upbody, i, total);

                break;

            case Test:
            case TimerReport:
            case AddReport:
            case Hour:
            case Realtime:
            case Specifiedelement:
            case WaterPumpMotor:
                memcpy(&body_buffer[body_pointer], upbody, 23);
                body_pointer += 23;

                for (i = 0; i < upbody->count; i++) {
                    memcpy(&body_buffer[body_pointer], upbody->element[i]->guide, 2);
                    body_pointer += 2;
                    memcpy(&body_buffer[body_pointer], upbody->element[i]->value, upbody->element[i]->num);
                    body_pointer += upbody->element[i]->num;
                }

                for (i = 1; i <= total; ++i)
                    hydrology_device_split_transfer(body_buffer, buffer, header, upbody, i, total);

                break;

            case ConfigurationModification:
            case ConfigurationRead:
            case ParameterModification:
            case ParameterRead:
            case Status:
                memcpy(&body_buffer[body_pointer], upbody, 15);
                body_pointer += 15;

                for (i = 0; i < upbody->count; i++) {
                    memcpy(&body_buffer[body_pointer], upbody->element[i]->guide, 2);
                    body_pointer += 2;
                    memcpy(&body_buffer[body_pointer], upbody->element[i]->value, upbody->element[i]->num);
                    body_pointer += upbody->element[i]->num;
                }

                for (i = 1; i <= total; ++i)
                    hydrology_device_split_transfer(body_buffer, buffer, header, upbody, i, total);

                break;

            case SoftwareVersion:
                memcpy(&body_buffer[body_pointer], upbody, 15);
                body_pointer += 15;
                memcpy(&body_buffer[body_pointer], upbody->element[0]->guide, 1);
                body_pointer += 1;
                memcpy(&body_buffer[body_pointer], upbody->element[0]->value, upbody->element[0]->num);
                body_pointer += upbody->element[0]->num;

                for (i = 1; i <= total; ++i)
                    hydrology_device_split_transfer(body_buffer, buffer, header, upbody, i, total);

                break;

            case InitializeSolidStorage:
            case Reset:
            case SetClock:
            case Time:
                memcpy(&body_buffer[body_pointer], upbody, 15);
                body_pointer += 15;

                for (i = 1; i <= total; ++i)
                    hydrology_device_split_transfer(body_buffer, buffer, header, upbody, i, total);

                break;

            case ChangePassword:
            case SetICCard:
                memcpy(&body_buffer[body_pointer], upbody, 15);
                body_pointer += 15;
                memcpy(&body_buffer[body_pointer], upbody->element[0]->guide, 2);
                body_pointer += 2;
                memcpy(&body_buffer[body_pointer], upbody->element[0]->value, upbody->element[0]->num);
                body_pointer += upbody->element[0]->num;

                for (i = 1; i <= total; ++i)
                    hydrology_device_split_transfer(body_buffer, buffer, header, upbody, i, total);

                break;

            case Pump:
            case Valve:
            case Gate:
                memcpy(&body_buffer[body_pointer], upbody, 15);
                body_pointer += 15;
                memcpy(&body_buffer[body_pointer], upbody->element[0]->guide, 1);
                body_pointer += 1;
                memcpy(&body_buffer[body_pointer], upbody->element[0]->value, upbody->element[0]->num);
                body_pointer += upbody->element[0]->num;

                for (i = 1; i <= total; ++i)
                    hydrology_device_split_transfer(body_buffer, buffer, header, upbody, i, total);

                break;

            case WaterSetting:
            case Record:
                memcpy(&body_buffer[body_pointer], upbody, 15);
                body_pointer += 15;
                memcpy(&body_buffer[body_pointer], upbody->element[0]->value, upbody->element[0]->num);
                body_pointer += upbody->element[0]->num;

                for (i = 1; i <= total; ++i)
                    hydrology_device_split_transfer(body_buffer, buffer, header, upbody, i, total);

                break;

            case ArtificialNumber:
            case InquireArtificialNumber:
                for (i = 1; i <= total; ++i) {
                    hydrology_device_set_up_header_sequence(i, total);

                    header->dir_len[0] = 0;
                    header->dir_len[1] = 0;

                    if (i == total) {
                        header->dir_len[0] |= (upbody->len % HYDROLOGY_BODY_MAX_LEN) >> 8;
                        header->dir_len[1] |= (upbody->len % HYDROLOGY_BODY_MAX_LEN) & 0xFF;
                    } else {
                        header->dir_len[0] |= HYDROLOGY_BODY_MAX_LEN >> 8;
                        header->dir_len[1] |= HYDROLOGY_BODY_MAX_LEN & 0xFF;
                    }

                    memcpy(buffer, header, sizeof(struct hydrology_up_header) - 1);
                    pointer = sizeof(struct hydrology_up_header) - 1;

                    if (i == total) {
                        hydrology_read_store_info(HYDROLOGY_D_FILE_RGZS,
                            HYDROLOGY_BODY_MAX_LEN - 10 + (i - 2) * HYDROLOGY_BODY_MAX_LEN,
                            &buffer[pointer],
                            (upbody->element[0]->num - (HYDROLOGY_BODY_MAX_LEN - 10)) % HYDROLOGY_BODY_MAX_LEN);
                        pointer += (upbody->element[0]->num - (HYDROLOGY_BODY_MAX_LEN - 10)) % HYDROLOGY_BODY_MAX_LEN;
                        g_hydrology.up_packet->end = ETX;
                    } else if (i == 1) {
                        memcpy(&buffer[pointer], upbody, 8);
                        pointer += 8;
                        memcpy(&buffer[pointer], upbody->element[0]->guide, 2);
                        pointer += 2;
                        hydrology_read_store_info(HYDROLOGY_D_FILE_RGZS,
                            0, &buffer[pointer],  HYDROLOGY_BODY_MAX_LEN - 10);
                        pointer += HYDROLOGY_BODY_MAX_LEN - 10;
                        g_hydrology.up_packet->end = ETB;
                    } else {
                        hydrology_read_store_info(HYDROLOGY_D_FILE_RGZS,
                            HYDROLOGY_BODY_MAX_LEN - 10 + (i - 2) * HYDROLOGY_BODY_MAX_LEN,
                            &buffer[pointer], HYDROLOGY_BODY_MAX_LEN);
                        pointer += HYDROLOGY_BODY_MAX_LEN;
                        g_hydrology.up_packet->end = ETB;
                    }

                    buffer[pointer] = g_hydrology.up_packet->end;
                    pointer += 1;

                    g_hydrology.up_packet->crc16 = crc16(0xFFFF, buffer, pointer);
                    buffer[pointer] = g_hydrology.up_packet->crc16 >> 8;
                    pointer += 1;
                    buffer[pointer] = g_hydrology.up_packet->crc16 & 0xFF;
                    pointer += 1;

                    g_hydrology.up_packet->len = pointer;

                    if (hydrology_port_transmmit(g_hydrology.up_packet->buffer, g_hydrology.up_packet->len) == false)
                        return false;
                }

                break;

            case Picture:
                for (i = 1; i <= total; ++i) {
                    hydrology_device_set_up_header_sequence(i, total);

                    header->dir_len[0] = 0;
                    header->dir_len[1] = 0;

                    if (i == total) {
                        header->dir_len[0] |= (upbody->len % HYDROLOGY_BODY_MAX_LEN) >> 8;
                        header->dir_len[1] |= (upbody->len % HYDROLOGY_BODY_MAX_LEN) & 0xFF;
                    } else {
                        header->dir_len[0] |= HYDROLOGY_BODY_MAX_LEN >> 8;
                        header->dir_len[1] |= HYDROLOGY_BODY_MAX_LEN & 0xFF;
                    }

                    memcpy(buffer, header, sizeof(struct hydrology_up_header) - 1);
                    pointer = sizeof(struct hydrology_up_header) - 1;

                    if (i == total) {
                        hydrology_read_store_info(HYDROLOGY_D_FILE_PICTURE,
                            HYDROLOGY_BODY_MAX_LEN - 25 + (i - 2) * HYDROLOGY_BODY_MAX_LEN,
                            &buffer[pointer],
                            (upbody->element[0]->num - (HYDROLOGY_BODY_MAX_LEN - 25)) % HYDROLOGY_BODY_MAX_LEN);
                        pointer += (upbody->element[0]->num - (HYDROLOGY_BODY_MAX_LEN - 25)) % HYDROLOGY_BODY_MAX_LEN;
                        g_hydrology.up_packet->end = ETX;
                    } else if (i == 1) {
                        memcpy(&buffer[pointer], upbody, 23);
                        pointer += 23;
                        memcpy(&buffer[pointer], upbody->element[0]->guide, 2);
                        pointer += 2;
                        hydrology_read_store_info(HYDROLOGY_D_FILE_PICTURE,
                            0, &buffer[pointer],  HYDROLOGY_BODY_MAX_LEN - 25);
                        pointer += HYDROLOGY_BODY_MAX_LEN - 25;
                        g_hydrology.up_packet->end = ETB;
                    } else {
                        hydrology_read_store_info(HYDROLOGY_D_FILE_PICTURE,
                            HYDROLOGY_BODY_MAX_LEN - 25 + (i - 2) * HYDROLOGY_BODY_MAX_LEN,
                            &buffer[pointer], HYDROLOGY_BODY_MAX_LEN);
                        pointer += HYDROLOGY_BODY_MAX_LEN;
                        g_hydrology.up_packet->end = ETB;
                    }

                    buffer[pointer] = g_hydrology.up_packet->end;
                    pointer += 1;

                    g_hydrology.up_packet->crc16 = crc16(0xFFFF, buffer, pointer);
                    buffer[pointer] = g_hydrology.up_packet->crc16 >> 8;
                    pointer += 1;
                    buffer[pointer] = g_hydrology.up_packet->crc16 & 0xFF;
                    pointer += 1;

                    g_hydrology.up_packet->len = pointer;

                    if (hydrology_port_transmmit(g_hydrology.up_packet->buffer, g_hydrology.up_packet->len) == false)
                        return false;
                }

                break;
        }

        kfree(body_buffer);
    }

    return true;
}

static int hydrology_device_make_err_up_tail_and_send(enum hydrology_body_type funcode)
{
    u8 *buffer;
    u8 *body_buffer;
    u16 seq, i, j, k, l;
    u16 buffer_size;
    u16 pointer, body_pointer;
    u16 total;
    struct hydrology_up_header *upheader = g_hydrology.up_packet->header;
    struct hydrology_down_header *downheader = g_hydrology.down_packet->header;
    struct hydrology_up_body *upbody = (struct hydrology_up_body *)g_hydrology.up_packet->body;

    seq = (downheader->count_seq[1] & 0xFF) + downheader->count_seq[2];
    printk(KERN_DEBUG "[warnning]Packet %u isn't recevied, ready to resend\n", seq);

    buffer_size = upheader->len + HYDROLOGY_BODY_MAX_LEN + 3;
    g_hydrology.up_packet->buffer = kmalloc(buffer_size, __GFP_ZERO);

    if (g_hydrology.up_packet->buffer == NULL) {
        printk(KERN_DEBUG "g_hydrology.up_packet->buffer malloc failed\n");
        return false;
    }

    buffer = g_hydrology.up_packet->buffer;

    body_buffer = kmalloc(upbody->len, __GFP_ZERO);

    if (body_buffer == NULL) {
        printk(KERN_DEBUG "body_buffer malloc failed\n");
        return false;
    }

    if (upbody->len % HYDROLOGY_BODY_MAX_LEN == 0)
        total = upbody->len / HYDROLOGY_BODY_MAX_LEN;
    else
        total = upbody->len / HYDROLOGY_BODY_MAX_LEN + 1;

    upheader->paket_start = SYN;
    body_pointer = 0;

    switch (funcode) {
        case LinkMaintenance:
            memcpy(&body_buffer[body_pointer], upbody, 8);
            body_pointer += 8;
            hydrology_device_split_transfer(body_buffer, buffer, upheader, upbody, seq, total);
            break;

        case EvenPeriodInformation:
        case Period:
            memcpy(&body_buffer[body_pointer], upbody, 23);
            body_pointer += 23;
            memcpy(&body_buffer[body_pointer], upbody->element[0]->guide, 2);
            body_pointer += 2;
            memcpy(&body_buffer[body_pointer], upbody->element[0]->value,
                upbody->element[0]->num);
            body_pointer += upbody->element[0]->num;

            for (i = 1; i < upbody->count; i++) {
                memcpy(&body_buffer[body_pointer], upbody->element[i]->guide, 2);
                body_pointer += 2;
            }

            for (k = 0, j = 0, l = 0; k < 12; k++) {
                for (i = 1; i < upbody->count; i++) {
                    if (upbody->element[i]->num / 12 == 1)
                        memcpy(&body_buffer[body_pointer], &upbody->element[i]->value[j],
                            upbody->element[i]->num / 12);
                    else if (upbody->element[i]->num / 12 == 2)
                        memcpy(&body_buffer[body_pointer], &upbody->element[i]->value[l],
                            upbody->element[i]->num / 12);

                    body_pointer += upbody->element[i]->num / 12;
                }

                j += 1;
                l += 2;
            }

            hydrology_device_split_transfer(body_buffer, buffer, upheader, upbody, seq, total);
            break;

        case Test:
        case TimerReport:
        case AddReport:
        case Hour:
        case Realtime:
        case Specifiedelement:
        case WaterPumpMotor:
            memcpy(&body_buffer[body_pointer], upbody, 23);
            body_pointer += 23;

            for (i = 0; i < upbody->count; i++) {
                memcpy(&body_buffer[body_pointer], upbody->element[i]->guide, 2);
                body_pointer += 2;
                memcpy(&body_buffer[body_pointer], upbody->element[i]->value, upbody->element[i]->num);
                body_pointer += upbody->element[i]->num;
            }

            hydrology_device_split_transfer(body_buffer, buffer, upheader, upbody, seq, total);
            break;

        case ConfigurationModification:
        case ConfigurationRead:
        case ParameterModification:
        case ParameterRead:
        case Status:
            memcpy(&body_buffer[body_pointer], upbody, 15);
            body_pointer += 15;

            for (i = 0; i < upbody->count; i++) {
                memcpy(&body_buffer[body_pointer], upbody->element[i]->guide, 2);
                body_pointer += 2;
                memcpy(&body_buffer[body_pointer], upbody->element[i]->value, upbody->element[i]->num);
                body_pointer += upbody->element[i]->num;
            }

            hydrology_device_split_transfer(body_buffer, buffer, upheader, upbody, seq, total);
            break;

        case SoftwareVersion:
            memcpy(&body_buffer[body_pointer], upbody, 15);
            body_pointer += 15;
            memcpy(&body_buffer[body_pointer], upbody->element[0]->guide, 1);
            body_pointer += 1;
            memcpy(&body_buffer[body_pointer], upbody->element[0]->value, upbody->element[0]->num);
            body_pointer += upbody->element[0]->num;
            hydrology_device_split_transfer(body_buffer, buffer, upheader, upbody, seq, total);
            break;

        case InitializeSolidStorage:
        case Reset:
        case SetClock:
        case Time:
            memcpy(&body_buffer[body_pointer], upbody, 15);
            body_pointer += 15;
            hydrology_device_split_transfer(body_buffer, buffer, upheader, upbody, seq, total);
            break;

        case ChangePassword:
        case SetICCard:
            memcpy(&body_buffer[body_pointer], upbody, 15);
            body_pointer += 15;
            memcpy(&body_buffer[body_pointer], upbody->element[0]->guide, 2);
            body_pointer += 2;
            memcpy(&body_buffer[body_pointer], upbody->element[0]->value, upbody->element[0]->num);
            body_pointer += upbody->element[0]->num;
            hydrology_device_split_transfer(body_buffer, buffer, upheader, upbody, seq, total);
            break;

        case Pump:
        case Valve:
        case Gate:
            memcpy(&body_buffer[body_pointer], upbody, 15);
            body_pointer += 15;
            memcpy(&body_buffer[body_pointer], upbody->element[0]->guide, 1);
            body_pointer += 1;
            memcpy(&body_buffer[body_pointer], upbody->element[0]->value, upbody->element[0]->num);
            body_pointer += upbody->element[0]->num;
            hydrology_device_split_transfer(body_buffer, buffer, upheader, upbody, seq, total);
            break;

        case WaterSetting:
        case Record:
            memcpy(&body_buffer[body_pointer], upbody, 15);
            body_pointer += 15;
            memcpy(&body_buffer[body_pointer], upbody->element[0]->value, upbody->element[0]->num);
            body_pointer += upbody->element[0]->num;
            hydrology_device_split_transfer(body_buffer, buffer, upheader, upbody, seq, total);
            break;

        case ArtificialNumber:
        case InquireArtificialNumber:
            hydrology_device_set_up_header_sequence(seq, total);

            upheader->dir_len[0] = 0;
            upheader->dir_len[1] = 0;

            if (seq == total) {
                upheader->dir_len[0] |= (upbody->len % HYDROLOGY_BODY_MAX_LEN) >> 8;
                upheader->dir_len[1] |= (upbody->len % HYDROLOGY_BODY_MAX_LEN) & 0xFF;
            } else {
                upheader->dir_len[0] |= HYDROLOGY_BODY_MAX_LEN >> 8;
                upheader->dir_len[1] |= HYDROLOGY_BODY_MAX_LEN & 0xFF;
            }

            memcpy(buffer, upheader, sizeof(struct hydrology_up_header) - 1);
            pointer = sizeof(struct hydrology_up_header) - 1;

            if (seq == total) {
                hydrology_read_store_info(HYDROLOGY_D_FILE_RGZS,
                    HYDROLOGY_BODY_MAX_LEN - 10 + (seq - 2) * HYDROLOGY_BODY_MAX_LEN,
                    &buffer[pointer],
                    (upbody->element[0]->num - (HYDROLOGY_BODY_MAX_LEN - 10)) % HYDROLOGY_BODY_MAX_LEN);
                pointer += (upbody->element[0]->num - (HYDROLOGY_BODY_MAX_LEN - 10)) % HYDROLOGY_BODY_MAX_LEN;
                g_hydrology.up_packet->end = ETX;
            } else if (seq == 1) {
                memcpy(&buffer[pointer], upbody, 8);
                pointer += 8;
                memcpy(&buffer[pointer], upbody->element[0]->guide, 2);
                pointer += 2;
                hydrology_read_store_info(HYDROLOGY_D_FILE_RGZS,
                    0, &buffer[pointer],  HYDROLOGY_BODY_MAX_LEN - 10);
                pointer += HYDROLOGY_BODY_MAX_LEN - 10;
                g_hydrology.up_packet->end = ETX;
            } else {
                hydrology_read_store_info(HYDROLOGY_D_FILE_RGZS,
                    HYDROLOGY_BODY_MAX_LEN - 10 + (seq - 2) * HYDROLOGY_BODY_MAX_LEN,
                    &buffer[pointer], HYDROLOGY_BODY_MAX_LEN);
                pointer += HYDROLOGY_BODY_MAX_LEN;
                g_hydrology.up_packet->end = ETX;
            }

            buffer[pointer] = g_hydrology.up_packet->end;
            pointer += 1;

            g_hydrology.up_packet->crc16 = crc16(0xFFFF, buffer, pointer);
            buffer[pointer] = g_hydrology.up_packet->crc16 >> 8;
            pointer += 1;
            buffer[pointer] = g_hydrology.up_packet->crc16 & 0xFF;
            pointer += 1;

            g_hydrology.up_packet->len = pointer;

            if (hydrology_port_transmmit(g_hydrology.up_packet->buffer, g_hydrology.up_packet->len) == false)
                return false;

            break;

        case Picture:
            hydrology_device_set_up_header_sequence(seq, total);

            upheader->dir_len[0] = 0;
            upheader->dir_len[1] = 0;

            if (seq == total) {
                upheader->dir_len[0] |= (upbody->len % HYDROLOGY_BODY_MAX_LEN) >> 8;
                upheader->dir_len[1] |= (upbody->len % HYDROLOGY_BODY_MAX_LEN) & 0xFF;
            } else {
                upheader->dir_len[0] |= HYDROLOGY_BODY_MAX_LEN >> 8;
                upheader->dir_len[1] |= HYDROLOGY_BODY_MAX_LEN & 0xFF;
            }

            memcpy(buffer, upheader, sizeof(struct hydrology_up_header) - 1);
            pointer = sizeof(struct hydrology_up_header) - 1;

            if (seq == total) {
                hydrology_read_store_info(HYDROLOGY_D_FILE_PICTURE,
                    HYDROLOGY_BODY_MAX_LEN - 25 + (seq - 2) * HYDROLOGY_BODY_MAX_LEN,
                    &buffer[pointer],
                    (upbody->element[0]->num - (HYDROLOGY_BODY_MAX_LEN - 25)) % HYDROLOGY_BODY_MAX_LEN);
                pointer += (upbody->element[0]->num - (HYDROLOGY_BODY_MAX_LEN - 25)) % HYDROLOGY_BODY_MAX_LEN;
                g_hydrology.up_packet->end = ETX;
            } else if (seq == 1) {
                memcpy(&buffer[pointer], upbody, 23);
                pointer += 23;
                memcpy(&buffer[pointer], upbody->element[0]->guide, 2);
                pointer += 2;
                hydrology_read_store_info(HYDROLOGY_D_FILE_PICTURE,
                    0, &buffer[pointer],  HYDROLOGY_BODY_MAX_LEN - 25);
                pointer += HYDROLOGY_BODY_MAX_LEN - 25;
                g_hydrology.up_packet->end = ETX;
            } else {
                hydrology_read_store_info(HYDROLOGY_D_FILE_PICTURE,
                    HYDROLOGY_BODY_MAX_LEN - 25 + (seq - 2) * HYDROLOGY_BODY_MAX_LEN,
                    &buffer[pointer], HYDROLOGY_BODY_MAX_LEN);
                pointer += HYDROLOGY_BODY_MAX_LEN;
                g_hydrology.up_packet->end = ETX;
            }

            buffer[pointer] = g_hydrology.up_packet->end;
            pointer += 1;

            g_hydrology.up_packet->crc16 = crc16(0xFFFF, buffer, pointer);
            buffer[pointer] = g_hydrology.up_packet->crc16 >> 8;
            pointer += 1;
            buffer[pointer] = g_hydrology.up_packet->crc16 & 0xFF;
            pointer += 1;

            g_hydrology.up_packet->len = pointer;

            if (hydrology_port_transmmit(g_hydrology.up_packet->buffer, g_hydrology.up_packet->len) == false)
                return false;

            break;
    }

    kfree(body_buffer);

    return true;
}

int hydrology_device_process_send(struct hydrology_element_info *element_table, u8 cnt,
    enum hydrology_mode mode, enum hydrology_body_type funcode)
{
    if (hydrology_device_init_send(cnt, funcode) == false)
        return false;

    hydrology_device_make_up_header(mode, funcode);

    if (hydrology_device_make_up_body(element_table, funcode) == false)
        return false;

    if (hydrology_device_make_up_tail_and_send(funcode) == false)
        return false;

    hydrology_device_exit_send();

    return true;
}

static int hydrology_device_check_down_packet(u8 *input, int inputlen)
{
    u16 crcRet = 0;
    u16 inputCrc = 0;
    u16 bodylen = 0;

    crcRet = crc16(0xFFFF, input, inputlen - 2);

    inputCrc = (input[inputlen - 2] << 8) | input[inputlen - 1];

    if (crcRet != inputCrc) {
        printk(KERN_DEBUG "Device crc(0x%04x) != Host crc(0x%04x)\n",
            inputCrc, crcRet);
        printk(KERN_DEBUG "CRC check failed !\n");
        return false;
    }

    if ((input[0] != SOH) || (input[1] != SOH)) {
        printk(KERN_DEBUG "Device Frame head(0x%02x, 0x%02x) != Host Frame head(0x%02x, 0x%02x)\n",
            input[0], input[1], SOH, SOH);
        printk(KERN_DEBUG "Frame head check failed !\n");
        return false;
    }

    bodylen = (input[11] & 0x0F) * 256 + input[12];

    if (bodylen != (inputlen - 17)) {
        printk(KERN_DEBUG "Device length(0x%x) != Host length(0x%x)\n",
            bodylen, inputlen - 17);
        printk(KERN_DEBUG "Hydrolog length check failed !\n");
        return false;
    }

    return true;
}

static int hydrology_device_make_down_header(u8 *input, int inputlen, int *position, int *bodylen)
{
    struct hydrology_down_header *header = (struct hydrology_down_header *)g_hydrology.down_packet->header;

    if (hydrology_device_check_down_packet(input, inputlen) != true) {
        printk(KERN_DEBUG "Hydrology check fail !\n");
        return false;
    }

    memcpy(header->frame_start, &input[*position], 2);
    *position += 2;

    memcpy(header->remote_addr, &input[*position], 5);
    *position += 5;

    memcpy(&(header->center_addr), &input[*position], 1);
    *position += 1;

    memcpy(header->password, &input[*position], 2);
    *position += 2;

    memcpy(&(header->funcode), &input[*position], 1);
    *position += 1;

    memcpy(header->dir_len, &input[*position], 1);
    header->dir_len[0] >>= 4;

    *bodylen = (input[*position] & 0x0F) * 256 + input[*position + 1];
    *position += 2;

    memcpy(&(header->paket_start), &input[*position], 1);
    *position += 1;

    if (header->paket_start == SYN) {
        memcpy(header->count_seq, &input[*position], 3);
        *position += 3;
    }

    return true;
}

static int hydrology_device_make_down_body(u8 *input, int len, int position,
    enum hydrology_mode mode, enum hydrology_body_type funcode)
{
    u16 i, offset;
    s16 tmp_len;
    u16 tmp_position;
    struct hydrology_down_body *down_body = (struct hydrology_down_body *)g_hydrology.down_packet->body;

    memcpy(down_body->stream_id, &input[position], 2);
    position += 2;
    len -= 2;

    memcpy(down_body->send_time, &input[position], 6);
    position += 6;
    len -= 6;

    down_body->count = 0;

    switch (funcode) {
        case Test:
        case EvenPeriodInformation:
        case TimerReport:
        case AddReport:
        case Hour:
        case ArtificialNumber:
        case Picture:
        case Realtime:
        case InquireArtificialNumber:
        case WaterPumpMotor:
        case SoftwareVersion:
        case Status:
        case SetClock:
        case Record:
        case Time:
            return true;

        case Period:
            hydrology_write_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_PDA_PERIOD_BT,
                &input[position], 4);
            position += 4;
            len -= 4;
            hydrology_write_store_info(HYDROLOGY_D_FILE_E_DATA, HYDROLOGY_PDA_PERIOD_ET,
                &input[position], 4);
            position += 4;
            len -= 4;
            down_body->count = 2;
            break;

        case Specifiedelement:
            tmp_len = len;
            tmp_position = position;

            while (tmp_len > 0) {
                tmp_position += 2;
                tmp_len -= 2;

                down_body->count++;
            }

            if (down_body->count == 0)
                return false;

            break;

        case ConfigurationModification:
        case ParameterModification:
            tmp_len = len;
            tmp_position = position;

            while (tmp_len > 0) {
                offset = (input[tmp_position + 1] >> 3) + 2;
                tmp_position += offset;
                tmp_len -= offset;

                down_body->count++;
            }

            if (down_body->count == 0)
                return false;

            break;

        case ConfigurationRead:
        case ParameterRead:
            tmp_len = len;
            tmp_position = position;

            while (tmp_len > 0) {
                tmp_position += 2;
                tmp_len -= 2;

                down_body->count++;
            }

            if (down_body->count == 0)
                return false;

            break;

        case InitializeSolidStorage:
        case Reset:
        case SetICCard:
        case Pump:
        case Valve:
        case Gate:
        case WaterSetting:
            down_body->count = 1;
            break;

        case ChangePassword:
            down_body->count = 2;
            break;

        default:
            break;
    }

    if (down_body->count > 0) {
        down_body->element = kmalloc(sizeof(struct hydrology_element *) * down_body->count, __GFP_ZERO);

        if (down_body->element == NULL) {
            printk(KERN_DEBUG "down_body->element malloc failed\n");
            return false;
        }
    }

    for (i = 0; i < down_body->count; ++i) {
        down_body->element[i] = kmalloc(sizeof(struct hydrology_element), __GFP_ZERO);

        if (down_body->element[i] == NULL) {
            printk(KERN_DEBUG "down_body->element[%d] malloc failed\n", i);
            return false;
        }
    }

    switch (funcode) {
        case Test:
        case EvenPeriodInformation:
        case TimerReport:
        case AddReport:
        case Hour:
        case ArtificialNumber:
        case Picture:
        case Realtime:
        case InquireArtificialNumber:
        case WaterPumpMotor:
        case SoftwareVersion:
        case Status:
        case SetClock:
        case Record:
        case Time:
            break;

        case Period:
            for (i = 0; i < 2; ++i) {
                memcpy(down_body->element[i]->guide, &input[position], 2);
                position += 2;
                len -= 2;

                if (i == 0) {
                    down_body->element[i]->num =
                        (down_body->element[i]->guide[1] >> 3);
                    down_body->element[i]->value = kmalloc(down_body->element[i]->num, __GFP_ZERO);

                    if (NULL == down_body->element[i]->value) {
                        printk(KERN_DEBUG "down_body->element[0]->value malloc failed\n");
                        return false;
                    }

                    memcpy(down_body->element[i]->value, &input[position],
                        down_body->element[i]->num);
                    position += down_body->element[i]->num;
                    len -= down_body->element[i]->num;
                }
            }

            break;

        case Specifiedelement:
        case ConfigurationRead:
        case ParameterRead:
            for (i = 0; i < down_body->count; ++i) {
                memcpy(down_body->element[i]->guide, &input[position], 2);
                position += 2;
                len -= 2;
            }

            break;

        case ConfigurationModification:
        case ParameterModification:
        case ChangePassword:
        case SetICCard:
            for (i = 0; i < down_body->count; ++i) {
                memcpy(down_body->element[i]->guide, &input[position], 2);
                position += 2;
                len -= 2;

                down_body->element[i]->num =
                    (down_body->element[i]->guide[1] >> 3);
                down_body->element[i]->value = kmalloc(down_body->element[i]->num, __GFP_ZERO);

                if (NULL == down_body->element[i]->value) {
                    printk(KERN_DEBUG "down_body->element[%d]->value malloc failed\n", i);
                    return false;
                }

                memcpy(down_body->element[i]->value, &input[position],
                    down_body->element[i]->num);
                position += down_body->element[i]->num;
                len -= down_body->element[i]->num;
            }

            break;

        case InitializeSolidStorage:
        case Reset:
            memcpy(down_body->element[0]->guide, &input[position], 2);
            position += 2;
            len -= 2;
            break;

        case Pump:
        case Valve:
            down_body->element[0]->num = input[position];
            position += 1;
            len -= 1;
            down_body->element[0]->value = kmalloc(down_body->element[0]->num, __GFP_ZERO);

            if (NULL == down_body->element[0]->value) {
                printk(KERN_DEBUG "down_body->element[%d]->value malloc failed\n", 0);
                return false;
            }

            memcpy(down_body->element[0]->value, &input[position],
                down_body->element[0]->num);
            position += down_body->element[0]->num;
            len -= down_body->element[0]->num;
            break;

        case Gate:
            down_body->element[0]->num = input[position] >> 3;
            position += 1;
            len -= 1;
            down_body->element[0]->value = kmalloc(down_body->element[0]->num, __GFP_ZERO);

            if (NULL == down_body->element[0]->value) {
                printk(KERN_DEBUG "down_body->element[%d]->value malloc failed\n", 0);
                return false;
            }

            memcpy(down_body->element[0]->value, &input[position],
                down_body->element[0]->num);
            position += down_body->element[0]->num;
            len -= down_body->element[0]->num;
            break;

        case WaterSetting:
            down_body->element[0]->num = 1;
            down_body->element[0]->value = kmalloc(down_body->element[0]->num, __GFP_ZERO);

            if (NULL == down_body->element[0]->value) {
                printk(KERN_DEBUG "down_body->element[0]->value malloc failed\n");
                return false;
            }

            memcpy(down_body->element[0]->value, &input[position],
                down_body->element[0]->num);
            position += down_body->element[0]->num;
            len -= down_body->element[0]->num;
            break;

        default:
            break;
    }

    return true;
}

int hydrology_device_process_receieve(u8 *input, int inputlen, enum hydrology_mode mode)
{
    struct hydrology_down_header *header = NULL;
    int i = 0, bodylen = 0;

    if (hydrology_device_init_receieve() == false)
        return false;

    header = g_hydrology.down_packet->header;

    if (hydrology_device_make_down_header(input, inputlen, &i, &bodylen) == false)
        return false;

    if (hydrology_device_make_down_body(input, bodylen, i, mode, (enum hydrology_body_type)header->funcode) == false)
        return false;

    if (hydrology_execute_command((enum hydrology_body_type)header->funcode) == false)
        return false;

    switch (mode) {
        case HYDROLOGY_M1:
        case HYDROLOGY_M2:

            break;

        case HYDROLOGY_M3:
        case HYDROLOGY_M4:
            hydrology_response_downstream((enum hydrology_body_type)header->funcode);
            break;
    }

    g_hydrology.down_packet->end = input[inputlen - 3];

    hydrology_device_exit_receieve();

    return true;
}

int hydrology_device_process_m3_err_packet(struct hydrology_element_info *element_table,
    u8 cnt, enum hydrology_mode mode, enum hydrology_body_type funcode, u8 cerr)
{
    u8 **ppdata;
    u16 length;

    if (hydrology_device_init_send(cnt, funcode) == false)
        return false;

    hydrology_device_make_up_header(mode, funcode);

    if (hydrology_device_make_up_body(element_table, funcode) == false)
        return false;

    if (hydrology_device_make_err_up_tail_and_send(funcode) == false)
        return false;

    hydrology_device_exit_send();

    cerr++;

    if (hydrology_port_receive(ppdata, &length, HYDROLOGY_D_PORT_TIMEOUT) == true) {
        if (hydrology_device_process_receieve(*ppdata, length, HYDROLOGY_M3) == true) {
            switch (ppdata[0][length - 3]) {
                case EOT:
                    printk(KERN_DEBUG "[EOT]Link is disconnecting\n");
                    hydrology_disconnect_link();
                    break;

                case ESC:
                    printk(KERN_DEBUG "[ESC]Transfer is over, keep on live within 10 minutes\n");
                    hydrology_enable_link_packet();
                    break;

                default:
                    printk(KERN_ERR "Unknown end packet identifier\n");
                    break;
            }
        } else
            return false;
    } else {
        printk(KERN_DEBUG "Receive data timeout, retry times %d.\n",
            cerr);

        if (cerr >= 3) {
            hydrology_disconnect_link();
            return false;
        }

        hydrology_device_process_m3_err_packet(element_table, cnt, HYDROLOGY_M3, funcode, cerr);
    }

    return true;
}

int hydrology_device_process_end_identifier(struct hydrology_element_info *element_table, u8 cnt,
    enum hydrology_body_type funcode, u8 End)
{
    u8 cerr = 0;
    int ret;

    switch (End) {
        case ETX:
            printk(KERN_DEBUG "[ETX]Wait disconnecting...\n");
            hydrology_disable_link_packet();
            break;

        case ETB:
            printk(KERN_DEBUG "[ETB]Stay connecting...\n");
            hydrology_enable_link_packet();
            break;

        case ENQ:
            printk(KERN_DEBUG "[ENQ]Query packet\n");
            break;

        case EOT:
            printk(KERN_DEBUG "[EOT]Link is disconnecting\n");
            hydrology_disconnect_link();
            break;

        case ACK:
            printk(KERN_DEBUG "[ACK]There will be another packets in the next transfer\n");
            break;

        case NAK:
            printk(KERN_DEBUG "[NAK]Error packet, resend\n");
            ret = hydrology_device_process_m3_err_packet(element_table, cnt, HYDROLOGY_M3, funcode, cerr);
            break;

        case ESC:
            printk(KERN_DEBUG "[ESC]Transfer is over, keep on live within 10 minutes\n");
            hydrology_enable_link_packet();
            break;

        default:
            printk(KERN_ERR "Unknown end packet identifier\n");
            break;
    }

    return ret;
}

int hydrology_device_process_m1(struct hydrology_element_info *element_table, u8 cnt,
    enum hydrology_mode mode, enum hydrology_body_type funcode)
{
    return hydrology_device_process_send(element_table, cnt, mode, funcode);
}

int hydrology_device_process_m2(struct hydrology_element_info *element_table, u8 cnt,
    enum hydrology_body_type funcode, u8 cerr)
{
    u8 **ppdata;
    u16 length;

    if (hydrology_device_process_send(element_table, cnt, HYDROLOGY_M2, funcode) == false)
        return false;

    cerr++;

    if (hydrology_port_receive(ppdata, &length, HYDROLOGY_D_PORT_TIMEOUT) == true) {
        if (hydrology_device_process_receieve(*ppdata, length, HYDROLOGY_M2) == true)
            hydrology_device_process_end_identifier(element_table, cnt, funcode, ppdata[0][length - 3]);
        else
            return false;
    } else {
        printk(KERN_DEBUG "Receive data timeout, retry times %d.\n",
            cerr);

        if (cerr >= 3) {
            hydrology_disconnect_link();
            return false;
        }

        hydrology_device_process_m2(element_table, cnt, funcode, cerr);
    }

    return true;
}

int hydrology_device_process_m3(struct hydrology_element_info *element_table, u8 cnt,
    enum hydrology_body_type funcode)
{
    u8 **ppdata;
    u16 length;

    if (hydrology_device_process_send(element_table, cnt, HYDROLOGY_M3, funcode) == false)
        return false;

    if (hydrology_port_receive(ppdata, &length, HYDROLOGY_D_PORT_TIMEOUT) == true) {
        if (hydrology_device_process_receieve(*ppdata, length, HYDROLOGY_M3) == true)
            hydrology_device_process_end_identifier(element_table, cnt, funcode, ppdata[0][length - 3]);
        else
            return false;
    } else {
        printk(KERN_DEBUG "Receive data timeout.\n");
        return false;
    }

    return true;
}

int hydrology_device_process_m4(void)
{
    u8 **ppdata;
    u16 length;

    for (;;) {
        if (hydrology_port_receive(ppdata, &length, HYDROLOGY_D_PORT_TIMEOUT) == true)
            hydrology_device_process_receieve(*ppdata, length, HYDROLOGY_M4);
        else {
            printk(KERN_DEBUG "[Warning]Device Port is going to be closed.\n");
            return false;
        }
    }
}

int hydrology_device_process(struct hydrology_element_info *element_table, u8 cnt,
    enum hydrology_mode mode, enum hydrology_body_type funcode)
{
    u8 ret = false;
    u8 cerr = 0;

    switch (mode) {
        case HYDROLOGY_M1:
            ret = hydrology_device_process_m1(element_table, cnt, mode, funcode);
            break;

        case HYDROLOGY_M2:
            ret = hydrology_device_process_m2(element_table, cnt, funcode, cerr);
            break;

        case HYDROLOGY_M3:
            ret = hydrology_device_process_m3(element_table, cnt, funcode);
            break;

        case HYDROLOGY_M4:
            ret = hydrology_device_process_m4();
            break;
    }

    return ret;
}

#ifdef DESIGN_VERIFICATION_HYDROLOGY
#include "kinetis/test-kinetis.h"
#include "kinetis/random-gene.h"

int t_hydrology_device_random_element(enum hydrology_mode mode, enum hydrology_body_type funcode)
{
    struct hydrology_element_info *element_table;
    u8 s_guide[] = {0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC};
    u8 i, j, count, guide;
    int ret;

    switch (funcode) {
        case LinkMaintenance:
            ret = true;
            break;

        case Test:
        case TimerReport:
        case AddReport:
        case Realtime:
        case Specifiedelement:
            count = random_get8bit() % (117 - 100);

            if (count == 0)
                count = 1;

            element_table = kmalloc(sizeof(struct hydrology_element_info) * count, __GFP_ZERO);

            if (element_table == NULL) {
                printk(KERN_DEBUG "element_table malloc failed\n");
                return false;
            }

            for (i = 0;;) {
                guide = random_get8bit() % 0x75;

                if (!guide)
                    continue;

                hydrology_read_specified_element_info(&element_table[i], funcode, guide);

                if (i == count - 1)
                    break;
                else
                    i++;
            }

            ret = hydrology_device_process(element_table, count, mode, funcode);

            kfree(element_table);
            break;

        case EvenPeriodInformation:
        case Hour:
            count = random_get8bit() % sizeof(s_guide);

            if (count == 0)
                count = 1;

            element_table = kmalloc(sizeof(struct hydrology_element_info) * count, __GFP_ZERO);

            if (element_table == NULL) {
                printk(KERN_DEBUG "element_table malloc failed\n");
                return false;
            }

            for (i = 0; i < count; i++) {
                j = random_get8bit() % (sizeof(s_guide) - 1);

                hydrology_read_specified_element_info(&element_table[i], funcode, s_guide[j]);
            }

            ret = hydrology_device_process(element_table, count, mode, funcode);

            kfree(element_table);
            break;

        case Period:
            count = 2;
            element_table = kmalloc(sizeof(struct hydrology_element_info) * count, __GFP_ZERO);

            if (element_table == NULL) {
                printk(KERN_DEBUG "element_table malloc failed\n");
                return false;
            }

            hydrology_read_specified_element_info(&element_table[0], funcode, 0x04);
            j = random_get8bit() % (sizeof(s_guide) - 1);
            hydrology_read_specified_element_info(&element_table[1], funcode, s_guide[j]);

            ret = hydrology_device_process(element_table, count, mode, funcode);

            kfree(element_table);
            break;

        case ConfigurationModification:
        case ConfigurationRead:
            count = random_get8bit() % 15;

            if (count == 0)
                count = 1;

            element_table = kmalloc(sizeof(struct hydrology_element_info) * count, __GFP_ZERO);

            if (element_table == NULL) {
                printk(KERN_DEBUG "element_table malloc failed\n");
                return false;
            }

            for (i = 0;;) {
                guide = random_get8bit() % 0x0F;

                if (!guide)
                    continue;

                hydrology_read_specified_element_info(&element_table[i], funcode, guide);

                if (i == count - 1)
                    break;
                else
                    i++;
            }

            ret = hydrology_device_process(element_table, count, mode, funcode);

            kfree(element_table);
            break;

        case ParameterModification:
        case ParameterRead:
            count = random_get8bit() % (137 - 120);

            if (count == 0)
                count = 1;

            element_table = kmalloc(sizeof(struct hydrology_element_info) * count, __GFP_ZERO);

            if (element_table == NULL) {
                printk(KERN_DEBUG "element_table malloc failed\n");
                return false;
            }

            for (i = 0;;) {
                guide = random_get8bit() % 0xA8;

                if (guide < 0x20)
                    continue;

                hydrology_read_specified_element_info(&element_table[i], funcode, guide);

                if (i == count - 1)
                    break;
                else
                    i++;
            }

            ret = hydrology_device_process(element_table, count, mode, funcode);

            kfree(element_table);
            break;

        case WaterPumpMotor:
            count = 6;
            element_table = kmalloc(sizeof(struct hydrology_element_info) * count, __GFP_ZERO);

            if (element_table == NULL) {
                printk(KERN_DEBUG "element_table malloc failed\n");
                return false;
            }

            hydrology_read_specified_element_info(&element_table[0], funcode, 0x70);
            hydrology_read_specified_element_info(&element_table[0], funcode, 0x71);
            hydrology_read_specified_element_info(&element_table[0], funcode, 0x72);
            hydrology_read_specified_element_info(&element_table[0], funcode, 0x73);
            hydrology_read_specified_element_info(&element_table[0], funcode, 0x74);
            hydrology_read_specified_element_info(&element_table[0], funcode, 0x75);

            ret = hydrology_device_process(element_table, count, mode, funcode);

            kfree(element_table);
            break;

        case Status:
        case SetICCard:
            count = 1;
            element_table = kmalloc(sizeof(struct hydrology_element_info) * count, __GFP_ZERO);

            if (element_table == NULL) {
                printk(KERN_DEBUG "element_table malloc failed\n");
                return false;
            }

            hydrology_read_specified_element_info(&element_table[0], funcode, 0x45);

            ret = hydrology_device_process(element_table, count, mode, funcode);

            kfree(element_table);
            break;

        case ChangePassword:
            count = 1;
            element_table = kmalloc(sizeof(struct hydrology_element_info) * count, __GFP_ZERO);

            if (element_table == NULL) {
                printk(KERN_DEBUG "element_table malloc failed\n");
                return false;
            }

            hydrology_read_specified_element_info(&element_table[0], funcode, 0xB7);

            ret = hydrology_device_process(element_table, count, mode, funcode);

            kfree(element_table);
            break;

        case ArtificialNumber:
        case Picture:
        case InquireArtificialNumber:
        case SoftwareVersion:
        case InitializeSolidStorage:
        case Reset:
        case SetClock:
        case Pump:
        case Valve:
        case Gate:
        case WaterSetting:
        case Record:
        case Time:
            ret = hydrology_device_process(NULL, 0, mode, funcode);
            break;
    }

    return ret;
}

int t_hydrology_device_m1m2(enum hydrology_mode mode)
{
    g_hydrology.source = MSG_FORM_CLIENT;

    if (t_hydrology_device_random_element(mode, LinkMaintenance) == false)
        return false;

    if (t_hydrology_device_random_element(mode, Test) == false)
        return false;

    if (t_hydrology_device_random_element(mode, EvenPeriodInformation) == false)
        return false;

    if (t_hydrology_device_random_element(mode, TimerReport) == false)
        return false;

    if (t_hydrology_device_random_element(mode, AddReport) == false)
        return false;

    if (t_hydrology_device_random_element(mode, Hour) == false)
        return false;

    if (t_hydrology_device_random_element(mode, ArtificialNumber) == false)
        return false;

    if (t_hydrology_device_random_element(mode, Picture) == false)
        return false;

    if (t_hydrology_device_random_element(mode, Pump) == false)
        return false;

    if (t_hydrology_device_random_element(mode, Valve) == false)
        return false;

    if (t_hydrology_device_random_element(mode, Gate) == false)
        return false;

    return true;
}

int t_hydrology_device_m3(void)
{
    g_hydrology.source = MSG_FORM_CLIENT;

    if (t_hydrology_device_random_element(HYDROLOGY_M3, Test) == false)
        return false;

    if (t_hydrology_device_random_element(HYDROLOGY_M3, EvenPeriodInformation) == false)
        return false;

    if (t_hydrology_device_random_element(HYDROLOGY_M3, TimerReport) == false)
        return false;

    if (t_hydrology_device_random_element(HYDROLOGY_M3, AddReport) == false)
        return false;

    if (t_hydrology_device_random_element(HYDROLOGY_M3, Hour) == false)
        return false;

    if (t_hydrology_device_random_element(HYDROLOGY_M3, ArtificialNumber) == false)
        return false;

    if (t_hydrology_device_random_element(HYDROLOGY_M3, Picture) == false)
        return false;

    if (t_hydrology_device_random_element(HYDROLOGY_M3, Pump) == false)
        return false;

    if (t_hydrology_device_random_element(HYDROLOGY_M3, Valve) == false)
        return false;

    if (t_hydrology_device_random_element(HYDROLOGY_M3, Gate) == false)
        return false;

    return true;
}

int t_hydrology_device_m4(void)
{
    g_hydrology.source = MSG_FORM_CLIENT;

    return hydrology_device_process(NULL, 0, HYDROLOGY_M4, (enum hydrology_body_type)NULL);
}

#endif

