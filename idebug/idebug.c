#include "idebug.h"

/*The following program is modified by the user according to the hardware device, otherwise the driver cannot run.*/

/**
  * @step 1:  Modify the corresponding function according to the modified area and the corresponding function name.
  * @step 2:  Modify the serial print function UART.
  * @step 3:  Modify variables dbg_level and set the debug level.
  * @step 4:  Modify the time fetch function OS_TIME_MS().
  * @step 5:
  */

#include "usart.h"

int dbg_level = DEBUG_LEVEL_INFO |
                DEBUG_LEVEL_DBG |
                DEBUG_LEVEL_ERR;

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART3 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

/*The above procedure is modified by the user according to the hardware device, otherwise the driver cannot run.*/
