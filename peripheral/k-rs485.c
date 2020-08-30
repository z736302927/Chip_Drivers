#include "peripheral/k-rs485.h"

/*The following program is modified by the user according to the hardware device, otherwise the driver cannot run.*/

/**
  * @step 1:  Modify the corresponding function according to the modified area and the corresponding function name.
  * @step 2:  Modify four areas: GPIO_PORT/GPIO_PIN/Led_TypeDef/LEDn.
  * @step 3:  .
  * @step 4:  .
  * @step 5:
  */

#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "stdio.h"
#include "usart.h"
#include "peripheral/k-serialport.h"
#include "algorithm/k-crc.h"
#include "linux/gfp.h"

#define DEBUG
#include "core/idebug.h"

#define RS485_printf    p_dbg
#define RS485_error     p_err_fun

#define RS485_BUFFER_SIZE      128

void RS485_Port_Send(uint8_t *pData, uint16_t Len)
{
    HAL_UART_Transmit_IT(&huart1, pData, Len);
}

void RS485_Port_Receive(uint8_t *pData, uint16_t *Len)
{
    uint32_t begintime;
    uint32_t currenttime;
    uint32_t timediff;

    begintime = BasicTimer_GetMSTick();

    while(1)
    {
        SerialPort_Receive(&SerialPort_3, pData, Len);

        if(SerialPort_ReadRxState() == 1)
        {
            SerialPort_SetRxState(0);
            break;
        }
        else
        {
            currenttime = BasicTimer_GetMSTick();
            timediff = currenttime >= begintime ? currenttime - begintime :
                currenttime + UINT32_MAX - begintime;

            if(timediff > 3000) /* 10s */
            {
                kinetis_debug_trace(KERN_DEBUG, "No data came !");
                break;
            }
        }
    }
}

/*The above procedure is modified by the user according to the hardware device, otherwise the driver cannot run.*/

void RS485_Master_Send(uint8_t Dev_addr, uint8_t Fun_code, uint16_t Reg_addr, uint8_t Len)
{
    uint8_t cmd[8];
    uint16_t crc = 0;

    cmd[0] = Dev_addr;
    cmd[1] = Fun_code;
    cmd[2] = Reg_addr >> 8;
    cmd[3] = Reg_addr % 256;
    cmd[4] = Len >> 8;
    cmd[5] = Len % 256;
    crc = CRC16_Calculate((char *)cmd, 6);
    cmd[6] = crc % 256;
    cmd[7] = crc >> 8;
    RS485_Port_Send(cmd, 8);
}

int RS485_Master_Receive(uint8_t *pData, uint16_t *Len)
{
    int retValue = false;
    uint8_t Data[128];
    uint16_t Size;

    RS485_Port_Receive(Data, &Size);

    if(Size == 0)
        return false;

    if(Data[2] != 0)
    {
        if(CRC16_Check((char *)Data, 5 + Data[2]) == true)
        {
            pData = (uint8_t *)kmalloc(Data[2], __GFP_ZERO);

            if(pData == NULL)
            {
                kinetis_debug_trace(KERN_DEBUG, "Data to memory malloc failed");
                RS485_error;
                retValue = false;
            }
            else
            {
                memcpy(pData, &Data[3], Data[2]);
                *Len = Data[2];
                retValue = true;
            }
        }
        else
        {
            RS485_printf("Data check error");
            RS485_error;
            retValue = false;
        }
    }
    else
    {
        RS485_printf("The received data length is 0 and cannot be parsed");
        RS485_error;
        retValue = false;
    }

    return retValue;
}

#ifdef DESIGN_VERIFICATION_RS485

#endif