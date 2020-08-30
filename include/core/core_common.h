#ifndef __CORE_COMMON_H
#define __CORE_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/*The following program is modified by the user according to the hardware device, otherwise the driver cannot run.*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
//#include "stm32f1xx_hal.h"
//#include "stm32l4xx_hal.h"
#include "stdint.h"
#include "stdbool.h"

//#define DESIGN_VERIFICATION_AK8975
#define DESIGN_VERIFICATION_AT24CXX
#define DESIGN_VERIFICATION_BASICTIMER
//#define DESIGN_VERIFICATION_CHINESE
//#define DESIGN_VERIFICATION_CRC
#define DESIGN_VERIFICATION_DELAY
//#define DESIGN_VERIFICATION_FATFS
//#define DESIGN_VERIFICATION_FSM
//#define DESIGN_VERIFICATION_GENERAL
//#define DESIGN_VERIFICATION_BUTTON
//#define DESIGN_VERIFICATION_LED
//#define DESIGN_VERIFICATION_RS485
#define DESIGN_VERIFICATION_RTC
//#define DESIGN_VERIFICATION_RTCTASK
//#define DESIGN_VERIFICATION_SEIRALPORT
//#define DESIGN_VERIFICATION_SWITCH
//#define DESIGN_VERIFICATION_TIMEOUT
//#define DESIGN_VERIFICATION_TIMTASK
//#define DESIGN_VERIFICATION_TOUCHSCREEN
//#define DESIGN_VERIFICATION_DS3231
//#define DESIGN_VERIFICATION_GT9271
#define DESIGN_VERIFICATION_HC_05
//#define DESIGN_VERIFICATION_HMC5883L
#define DESIGN_VERIFICATION_HYDROLOGY
//#define DESIGN_VERIFICATION_ICM20602
//#define DESIGN_VERIFICATION_IIC
//#define DESIGN_VERIFICATION_MAX30205
#define DESIGN_VERIFICATION_MEMORY
//#define DESIGN_VERIFICATION_MPU6050
//#define DESIGN_VERIFICATION_MT29F4G08
//#define DESIGN_VERIFICATION_MY9221
//#define DESIGN_VERIFICATION_NBIOT
//#define DESIGN_VERIFICATION_OLED
//#define DESIGN_VERIFICATION_RAK477
//#define DESIGN_VERIFICATION_SHT20
//#define DESIGN_VERIFICATION_SPL06
//#define DESIGN_VERIFICATION_SSD1306
//#define DESIGN_VERIFICATION_TLC5971
//#define DESIGN_VERIFICATION_W25QXXX
//#define DESIGN_VERIFICATION_XMODEM

/*The above procedure is modified by the user according to the hardware device, otherwise the driver cannot run.*/


#ifdef __cplusplus
}
#endif

#endif /* __CORE_COMMON_H */
