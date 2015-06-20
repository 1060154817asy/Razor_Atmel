/**********************************************************************************************************************
File: configuration.h      

Description:
Main configuration header file for project.  This file bridges many of the generic features of the 
firmware to the specific features of the design. The definitions should be updated
to match the target hardware.  
 
Bookmarks:
!!!!! External module peripheral assignments
@@@@@ GPIO board-specific parameters
##### Communication peripheral board-specific parameters

DEBUG UART IS 115200-8-N-1
ANT BOARDTEST CHANNEL CONFIG: 4660 (0x1234), 96(0x60), 1

***********************************************************************************************************************/

#ifndef __CONFIG_H
#define __CONFIG_H

/**********************************************************************************************************************
Runtime switches
***********************************************************************************************************************/
//#define MPGL1             1         /* Use to activate MPG Level 1 specific code */
#define MPGL2             1         /* Use to activate MPG Level 2 (all versions) specific code */
//#define MPGL2_R01          1         /* Use with MPGL2-EHDW-01 revision board */

#define DEBUG_MODE        1         /* Define to enable certain debugging code */
//#define STARTUP_SOUND     1         /* Define to include buzzer sound on startup */
#define LCD_STARTUP_ANIMATION  1   /* Define to show logo animation on startup */

//#define USE_SIMPLE_USART0 1  /* Define to use USART0 as a very simple byte-wise UART for debug purposes */

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/
typedef enum {SPI, UART, USART0, USART1, USART2, USART3} PeripheralType;


/**********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "AT91SAM3U4.h"
#include "exceptions.h"
#include "interrupts.h"
#include "core_cm3.h"
#include "main.h"
#include "typedefs.h"
#include "utilities.h"

/* MPGL1-specific header files */
#ifdef MPGL1
#include "mpgl1-ehdw-02.h"

#include "mpgl1_boardtest.h"
#endif /* MPGL1 */

#ifdef MPGL2
/* MPGL2-specific header files */
#ifdef MPGL2_R01
#include "mpgl2-ehdw-01.h"
#else
#include "mpgl2-ehdw-02.h"
#endif /* MPGL2_R01 */

#include "captouch.h"
#include "lcd_bitmaps.h"
#include "lcd_NHD-C12864LZ.h"

#include "mpgl2_board_test.h"
#endif /* MPGL2 */

/* Driver header files */
#include "messaging.h"
#include "antdefines.h"
#include "antmessage.h"
#include "ant_api.h"
#include "ant.h"
#include "buttons.h"
#include "leds.h" 
#include "sam3u_ssp.h"
#include "sam3u_uart.h"
//#include "sam3u_i2c.h"

/* Application header files */
#include "debug.h"


/**********************************************************************************************************************
!!!!! External peripheral assignments
***********************************************************************************************************************/
/* G_u32ApplicationFlags */
#define _APPLICATION_FLAGS_LED          0x00000001        /* LedStateMachine */
#define _APPLICATION_FLAGS_BUTTON       0x00000002        /* ButtonStateMachine */
#define _APPLICATION_FLAGS_DEBUG        0x00000004        /* DebugStateMachine */
#define _APPLICATION_FLAGS_LCD          0x00000008        /* LcdStateMachine */
#define _APPLICATION_FLAGS_ANT          0x00000010        /* AntStateMachine */
#define _APPLICATION_FLAGS_BOARDTEST    0x00000020        /* BoardTest Task */

#ifdef MPGL1
/* MPGL1 specific application flags */
#define _APPLICATION_FLAGS_SDCARD       0x00000040        /* SdCardStateMachine */

#define NUMBER_APPLICATIONS             (u8)7             /* Total number of applications */
#endif /* MPGL1 specific application flags */

#ifdef MPGL2
/* MPGL2 specific application flags */
#define _APPLICATION_FLAGS_CAPTOUCH     0x00000040        /* CapTouchStateMachine */

#define NUMBER_APPLICATIONS             (u8)7             /* Total number of applications */
#endif /* MPGL2 specific application flags */


/* %UART% Configuration */
/* Blade UART Peripheral Allocation (UART) */
#define BLADE_UART                  UART
#define UART_US_CR_INIT             BLADE_US_CR_INIT
#define UART_US_MR_INIT             BLADE_US_MR_INIT
#define UART_US_IER_INIT            BLADE_US_IER_INIT
#define UART_US_IDR_INIT            BLADE_US_IDR_INIT
#define UART_US_BRGR_INIT           BLADE_US_BRGR_INIT

#define UART_IRQHandler             DBGU_IrqHandler
#define BLADE_UART_PERIPHERAL       AT91C_ID_DBGU


/* Debug UART Peripheral Allocation (USART0) */
#define DEBUG_UART                  USART0
#define USART0_US_CR_INIT           DEBUG_US_CR_INIT
#define USART0_US_MR_INIT           DEBUG_US_MR_INIT
#define USART0_US_IER_INIT          DEBUG_US_IER_INIT
#define USART0_US_IDR_INIT          DEBUG_US_IDR_INIT
#define USART0_US_BRGR_INIT         DEBUG_US_BRGR_INIT

#define UART0_IRQHandler            USART0_IrqHandler
#define DEBUG_UART_PERIPHERAL       AT91C_ID_US0


#ifdef MPGL2
/* %SSP% Configuration */
/* LCD SPI Peripheral Allocation (USART1) */
#define LCD_SPI                     USART1
#define USART1_US_CR_INIT           LCD_US_CR_INIT
#define USART1_US_MR_INIT           LCD_US_MR_INIT
#define USART1_US_IER_INIT          LCD_US_IER_INIT
#define USART1_US_IDR_INIT          LCD_US_IDR_INIT
#define USART1_US_BRGR_INIT         LCD_US_BRGR_INIT

#define SSP1_IRQHandler             USART1_IrqHandler
#endif /* MPGL2 */

/* ANT SPI Peripheral Allocation (USART2) */
#define ANT_SPI                     USART2
#define USART2_US_CR_INIT           ANT_US_CR_INIT
#define USART2_US_MR_INIT           ANT_US_MR_INIT
#define USART2_US_IER_INIT          ANT_US_IER_INIT
#define USART2_US_IDR_INIT          ANT_US_IDR_INIT
#define USART2_US_BRGR_INIT         ANT_US_BRGR_INIT

#define SSP2_IRQHandler             USART2_IrqHandler

#define ANT_SPI_CS_GPIO             AT91C_BASE_PIOB
#define ANT_SPI_CS_PIN              PB_22_ANT_USPI2_CS

/* Blade I�C (TWI0) / Accelerometer (MPGL2_R01 only) */


/***********************************************************************************************************************
@@@@@ GPIO board-specific parameters
***********************************************************************************************************************/
/*----------------------------------------------------------------------------------------------------------------------
%LED% LED Configuration                                                                                                  
------------------------------------------------------------------------------------------------------------------------
Update the values below for the LEDs on the board.  Any name can be used for the LED definitions.
Open the LED source.c and edit Led_au32BitPositions and Leds_asLedArray with the correct values for the LEDs in the system.  
*/

#ifdef MPGL1
#define TOTAL_LEDS            (u8)11        /* Total number of LEDs in the system */
#endif /* MPGL1 */


#ifdef MPGL2

#ifdef MPGL2_R01
#define TOTAL_LEDS            (u8)5         /* Total number of LEDs in the system */
#else
#define TOTAL_LEDS            (u8)13        /* Total number of LEDs in the system */
#endif /* MPGL2_R01 */

#endif /* MPGL2 */


/*----------------------------------------------------------------------------------------------------------------------
%BUTTON% Button Configuration                                                                                                  
------------------------------------------------------------------------------------------------------------------------
Update the values below for the buttons on the board.  Any name can be used for the BUTTON definitions.
Open buttons.c and edit the GPIO definitions sections with the bit numbers for the buttons.
The order of the definitions below must match the order of the definitions provided in buttons.c. 
*/

#ifdef MPGL1
#define TOTAL_BUTTONS         (u8)4       /* Total number of Buttons in the system */

#define BUTTON0               (u32)0
#define BUTTON1               (u32)1
#define BUTTON2               (u32)2
#define BUTTON3               (u32)3

/* All buttons on each port must be ORed together here: set to 0 if no buttons on the port */
#define GPIOA_BUTTONS         (u32)( PA_17_BUTTON0 )
#define GPIOB_BUTTONS         (u32)( PB_00_BUTTON1 | PB_01_BUTTON2 | PB_02_BUTTON3 )
#endif /* MPGL1 */

#ifdef MPGL2
#define TOTAL_BUTTONS         (u8)2       /* Total number of Buttons in the system */

#define BUTTON0               (u32)0
#define BUTTON1               (u32)1

/* All buttons on each port must be ORed together here: set to 0 if no buttons on the port */
#define GPIOA_BUTTONS         (u32)( PA_17_BUTTON0 )
#define GPIOB_BUTTONS         (u32)( PB_00_BUTTON1 )
#endif /* MPGL2 */

#endif /* __CONFIG_H */


/*----------------------------------------------------------------------------------------------------------------------
%BUZZER% Buzzer Configuration                                                                                                  
------------------------------------------------------------------------------------------------------------------------
MPGL1 has two buzzers, MPGL2 only has one */

#define BUZZER1               AT91C_PWMC_CHID0

#ifdef MPGL1
#define BUZZER2               AT91C_PWMC_CHID1
#endif /* MPGL1 */


/*----------------------------------------------------------------------------------------------------------------------
%ANT% Interface Configuration                                                                                                  
------------------------------------------------------------------------------------------------------------------------
Board-specific ANT definitions are kept here
*/
//#define ANT_SSP_FLAGS          (AT91C_BASE_PIOB->PIO_PDSR & PB_22_ANT_USPI2_CS) 
#define ANT_SSP_FLAGS           G_u32Ssp2ApplicationFlags 

#define ANT_MRDY_READ_REG      (AT91C_BASE_PIOB->PIO_ODSR & PB_23_ANT_MRDY) 
#define ANT_MRDY_CLEAR_REG     (AT91C_BASE_PIOB->PIO_CODR = PB_23_ANT_MRDY)     
#define ANT_MRDY_SET_REG       (AT91C_BASE_PIOB->PIO_SODR = PB_23_ANT_MRDY)

#define ANT_SRDY_CLEAR_REG     (AT91C_BASE_PIOB->PIO_CODR = PB_24_ANT_SRDY)            
#define ANT_SRDY_SET_REG       (AT91C_BASE_PIOB->PIO_SODR = PB_24_ANT_SRDY)

#define ANT_RESET_CLEAR_REG    (AT91C_BASE_PIOB->PIO_CODR = PB_21_ANT_RESET)
#define ANT_RESET_SET_REG      (AT91C_BASE_PIOB->PIO_SODR = PB_21_ANT_RESET)

#define ANT_PIOA_PINS          (PA_25_ANT_USPI2_SCK | PA_23_ANT_USPI2_MOSI | PA_22_ANT_USPI2_MISO)
#define ANT_PIOB_PINS          (PB_21_ANT_RESET | PB_22_ANT_USPI2_CS | PB_23_ANT_MRDY | PB_24_ANT_SRDY)


/***********************************************************************************************************************
##### Communication peripheral board-specific parameters
***********************************************************************************************************************/
/*----------------------------------------------------------------------------------------------------------------------
%UART%  Configuration                                                                                                  
----------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------------------
Blade UART Setup

The Blade UART is used for the daughter board interface (serial: 115.2k, 8-N-1) .
*/
/* USART Control Register - Page 734 */
#define BLADE_US_CR_INIT (u32)0x00000050
/*
    31 - 20 [0] Reserved

    19 [0] RTSDIS/RCS no release/force RTS to 1
    18 [0] RTSEN/FCS no drive/force RTS to 0
    17 [0] DTRDIS no drive DTR to 1
    16 [0] DTREN no drive DTR to 0

    15 [0] RETTO no restart timeout
    14 [0] RSTNACK N/A
    13 [0] RSTIT N/A
    12 [0] SENDA N/A

    11 [0] STTTO no start time-out
    10 [0] STPBRK no stop break
    09 [0] STTBRK no transmit break
    08 [0] RSTSTA status bits not reset

    07 [0] TXDIS transmitter not disabled
    06 [1] TXEN transmitter enabled
    05 [0] RXDIS receiver not disabled
    04 [1] RXEN receiver enabled

    03 [0] RSTTX not reset
    02 [0] RSTRX not reset
    01 [0] Reserved
    00 [0] "
*/

/* USART Mode Register - page 737 */
#define BLADE_US_MR_INIT (u32)0x004008C0
/*
    31 [0] ONEBIT start frame delimiter is COMMAND or DATA SYNC
    30 [0] MODSYNC Manchester start bit N/A
    29 [0] MAN Machester encoding disabled
    28 [0] FILTER no filter on Rx line

    27 [0] Reserved
    26 [0] MAX_ITERATION (ISO7816 mode only)
    25 [0] "
    24 [0] "

    23 [0] INVDATA data is not inverted
    22 [1] VAR_SYNC sync field is updated on char to US_THR
    21 [0] DSNACK delicious! NACK is sent on ISO line immeidately on parity error
    20 [0] INACK NACK generated (N/A for async)

    19 [0] OVER 16x oversampling
    18 [0] CLKO USART does not drive the SCK pin
    17 [0] MODE9 CHRL defines char length
    16 [0] MSBF/CPOL LSB first

    15 [0] CHMODE normal mode
    14 [0] "
    13 [0] NBSTOP 1 stop bit
    12 [0] "

    11 [1] PAR no parity
    10 [0] "
    09 [0] "
    08 [0] SYNC/CPHA asynchronous

    07 [1] CHRL 8 bits
    06 [1] "
    05 [0] USCLKS MCK
    04 [0] "

    03 [0] USART_MODE normal
    02 [0] "
    01 [0] "
    00 [0] "
*/


/* USART Interrupt Enable Register - Page 741 */
#define BLADE_US_IER_INIT (u32)0x00000000
/*
    31 [0] Reserved
    30 [0] "
    29 [0] "
    28 [0] "

    27 [0] "
    26 [0] "
    25 [0] "
    24 [0] MANE Manchester Error interrupt not enabled

    23 [0] Reserved
    22 [0] "
    21 [0] "
    20 [0] "

    19 [0] CTSIC Clear to Send Change interrupt not enabled
    18 [0] DCDIC Data Carrier Detect Change interrupt not enabled
    17 [0] DSRIC Data Set Ready Change interrupt not enabled
    16 [0] RIIC Ring Inidicator Change interrupt not enabled

    15 [0] Reserved
    14 [0] "
    13 [0] NACK Non Ack interrupt not enabled
    12 [0] RXBUFF Reception Buffer Full (PDC) interrupt not enabled

    11 [0] TXBUFE Transmission Buffer Empty (PDC) interrupt not enabled
    10 [0] ITER/UNRE Max number of Repetitions Reached interrupt not enabled
    09 [0] TXEMPTY Transmitter Empty interrupt not enabled (yet)
    08 [0] TIMEOUT Receiver Time-out interrupt not enabled

    07 [0] PARE Parity Error interrupt not enabled
    06 [0] FRAME Framing Error interrupt not enabled
    05 [0] OVRE Overrun Error interrupt not enabled
    04 [0] ENDTX End of Transmitter Transfer (PDC) interrupt enabled

    03 [0] ENDRX End of Receiver Transfer (PDC) interrupt enabled
    02 [0] RXBRK Break Received interrupt not enabled
    01 [0] TXRDY Transmitter Ready interrupt not enabled
    00 [0] RXRDY Receiver Ready interrupt not enabled
*/

/* USART Interrupt Disable Register - Page 743 */
#define BLADE_US_IDR_INIT (u32)~BLADE_US_IER_INIT

/* USART Baud Rate Generator Register - Page 752
BAUD = MCK / (8(2-OVER)(CD + FP / 8))
=> CD = (MCK / (8(2-OVER)BAUD)) - (FP / 8)
MCK = 48MHz
OVER = 0 (16-bit oversampling)

BAUD desired = 115200 bps
=> CD = 26.042 - (FP / 8)
Set FP = 0, CD = 26

*/
#define BLADE_US_BRGR_INIT (u32)0x0000001A
/*
    31-20 [0] Reserved

    19 [0] Reserved
    18 [0] FP = 0
    17 [0] "
    16 [0] "

    15 [0] CD = 26 = 0x1A
    14 [0] "
    13 [0] "
    12 [0] "

    11 [0] "
    10 [0] "
    09 [0] "
    08 [0] "

    07 [0] "
    06 [0] "
    05 [0] "
    04 [1] "

    03 [1] "
    02 [0] "
    01 [1] "
    00 [0] "
*/


/*----------------------------------------------------------------------------------------------------------------------
Debug UART Setup

Debug is used for the terminal (serial: 115.2k, 8-N-1) debugging interface.
*/
/* USART Control Register - Page 734 */
#define DEBUG_US_CR_INIT (u32)0x00000050
/*
    31 - 20 [0] Reserved

    19 [0] RTSDIS/RCS no release/force RTS to 1
    18 [0] RTSEN/FCS no drive/force RTS to 0
    17 [0] DTRDIS no drive DTR to 1
    16 [0] DTREN no drive DTR to 0

    15 [0] RETTO no restart timeout
    14 [0] RSTNACK N/A
    13 [0] RSTIT N/A
    12 [0] SENDA N/A

    11 [0] STTTO no start time-out
    10 [0] STPBRK no stop break
    09 [0] STTBRK no transmit break
    08 [0] RSTSTA status bits not reset

    07 [0] TXDIS transmitter not disabled
    06 [1] TXEN transmitter enabled
    05 [0] RXDIS receiver not disabled
    04 [1] RXEN receiver enabled

    03 [0] RSTTX not reset
    02 [0] RSTRX not reset
    01 [0] Reserved
    00 [0] "
*/

/* USART Mode Register - page 737 */
#define DEBUG_US_MR_INIT (u32)0x004008C0
/*
    31 [0] ONEBIT start frame delimiter is COMMAND or DATA SYNC
    30 [0] MODSYNC Manchester start bit N/A
    29 [0] MAN Machester encoding disabled
    28 [0] FILTER no filter on Rx line

    27 [0] Reserved
    26 [0] MAX_ITERATION (ISO7816 mode only)
    25 [0] "
    24 [0] "

    23 [0] INVDATA data is not inverted
    22 [1] VAR_SYNC sync field is updated on char to US_THR
    21 [0] DSNACK delicious! NACK is sent on ISO line immeidately on parity error
    20 [0] INACK NACK generated (N/A for async)

    19 [0] OVER 16x oversampling
    18 [0] CLKO USART does not drive the SCK pin
    17 [0] MODE9 CHRL defines char length
    16 [0] MSBF/CPOL LSB first

    15 [0] CHMODE normal mode
    14 [0] "
    13 [0] NBSTOP 1 stop bit
    12 [0] "

    11 [1] PAR no parity
    10 [0] "
    09 [0] "
    08 [0] SYNC/CPHA asynchronous

    07 [1] CHRL 8 bits
    06 [1] "
    05 [0] USCLKS MCK
    04 [0] "

    03 [0] USART_MODE normal
    02 [0] "
    01 [0] "
    00 [0] "
*/


/* USART Interrupt Enable Register - Page 741 */
#define DEBUG_US_IER_INIT (u32)0x00000008
/*
    31 [0] Reserved
    30 [0] "
    29 [0] "
    28 [0] "

    27 [0] "
    26 [0] "
    25 [0] "
    24 [0] MANE Manchester Error interrupt not enabled

    23 [0] Reserved
    22 [0] "
    21 [0] "
    20 [0] "

    19 [0] CTSIC Clear to Send Change interrupt not enabled
    18 [0] DCDIC Data Carrier Detect Change interrupt not enabled
    17 [0] DSRIC Data Set Ready Change interrupt not enabled
    16 [0] RIIC Ring Inidicator Change interrupt not enabled

    15 [0] Reserved
    14 [0] "
    13 [0] NACK Non Ack interrupt not enabled
    12 [0] RXBUFF Reception Buffer Full (PDC) interrupt not enabled

    11 [0] TXBUFE Transmission Buffer Empty (PDC) interrupt not enabled
    10 [0] ITER/UNRE Max number of Repetitions Reached interrupt not enabled
    09 [0] TXEMPTY Transmitter Empty interrupt not enabled (yet)
    08 [0] TIMEOUT Receiver Time-out interrupt not enabled

    07 [0] PARE Parity Error interrupt not enabled
    06 [0] FRAME Framing Error interrupt not enabled
    05 [0] OVRE Overrun Error interrupt not enabled
    04 [0] ENDTX End of Transmitter Transfer (PDC) interrupt enabled

    03 [1] ENDRX End of Receiver Transfer (PDC) interrupt enabled
    02 [0] RXBRK Break Received interrupt not enabled
    01 [0] TXRDY Transmitter Ready interrupt not enabled
    00 [0] RXRDY Receiver Ready interrupt enabled
*/

/* USART Interrupt Disable Register - Page 743 */
#define DEBUG_US_IDR_INIT (u32)~DEBUG_US_IER_INIT

/* USART Baud Rate Generator Register - Page 752
BAUD = MCK / (8(2-OVER)(CD + FP / 8))
=> CD = (MCK / (8(2-OVER)BAUD)) - (FP / 8)
MCK = 48MHz
OVER = 0 (16-bit oversampling)

BAUD desired = 38400 bps
=> CD = 78.125 - (FP / 8)
Set FP = 1, CD = 78 = 0x4E

BAUD desired = 115200 bps
=> CD = 26.042 - (FP / 8)
Set FP = 0, CD = 26 = 0x1A

*/
#define DEBUG_US_BRGR_INIT (u32)0x0000001A
/*
    31-20 [0] Reserved

    19 [0] Reserved
    18 [0] FP = 0
    17 [0] "
    16 [0] "

    15 [0] CD = 26 = 0x1A
    14 [0] "
    13 [0] "
    12 [0] "

    11 [0] "
    10 [0] "
    09 [0] "
    08 [0] "

    07 [0] "
    06 [0] "
    05 [0] "
    04 [1] "

    03 [1] "
    02 [0] "
    01 [1] "
    00 [0] "
*/


/*----------------------------------------------------------------------------------------------------------------------
%SSP%  Configuration                                                                                                  
----------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------------------
LCD USART Setup in SSP mode

SPI mode to communicate with an SPI LCD screen. 
*/
/* USART Control Register - Page 734 */
#define LCD_US_CR_INIT (u32)0x00000060
/*
    31 - 20 [0] Reserved

    19 [0] RTSDIS/RCS no release/force RTS to 1
    18 [0] RTSEN/FCS no drive/force RTS to 0
    17 [0] DTRDIS no drive DTR to 1
    16 [0] DTREN no drive DTR to 0

    15 [0] RETTO no restart timeout
    14 [0] RSTNACK N/A
    13 [0] RSTIT N/A
    12 [0] SENDA N/A

    11 [0] STTTO no start time-out
    10 [0] STPBRK no stop break
    09 [0] STTBRK no transmit break
    08 [0] RSTSTA status bits not reset

    07 [0] TXDIS transmitter not disabled
    06 [1] TXEN transmitter enabled
    05 [1] RXDIS receiver disabled
    04 [0] RXEN receiver not enabled

    03 [0] RSTTX not reset
    02 [0] RSTRX not reset
    01 [0] Reserved
    00 [0] "
*/

/* USART Mode Register - page 737 */
#define LCD_US_MR_INIT (u32)0x004518CE
/*
    31 [0] ONEBIT start frame delimiter is COMMAND or DATA SYNC
    30 [0] MODSYNC Manchester start bit N/A
    29 [0] MAN Machester encoding disabled
    28 [0] FILTER no filter on Rx line

    27 [0] Reserved
    26 [0] MAX_ITERATION (ISO7816 mode only)
    25 [0] "
    24 [0] "

    23 [0] INVDATA data is not inverted
    22 [1] VAR_SYNC sync field is updated on char to US_THR
    21 [0] DSNACK delicious! NACK is sent on ISO line immeidately on parity error
    20 [0] INACK transmission starts as oons as byte is written to US_THR

    19 [0] OVER 16x oversampling
    18 [1] CLKO USART drives the SCK pin
    17 [0] MODE9 CHRL defines char length
    16 [1] CPOL clock is high when inactive

    15 [0] CHMODE normal mode
    14 [0] "
    13 [0] NBSTOP N/A
    12 [1] "

    11 [1] PAR no parity
    10 [0] "
    09 [0] "
    08 [0] CPHA data captured on leading edge of SPCK (first high to low transition does not count)

    07 [1] CHRL 8 bits
    06 [1] "
    05 [0] USCLKS MCK
    04 [0] "

    03 [1] USART_MODE SPI Master
    02 [1] "
    01 [1] "
    00 [0] "
*/


/* USART Interrupt Enable Register - Page 741 */
#define LCD_US_IER_INIT (u32)0x00000000
/*
    31 [0] Reserved
    30 [0] "
    29 [0] "
    28 [0] "

    27 [0] "
    26 [0] "
    25 [0] "
    24 [0] MANE Manchester Error interrupt not enabled

    23 [0] Reserved
    22 [0] "
    21 [0] "
    20 [0] "

    19 [0] CTSIC Clear to Send Change interrupt not enabled
    18 [0] DCDIC Data Carrier Detect Change interrupt not enabled
    17 [0] DSRIC Data Set Ready Change interrupt not enabled
    16 [0] RIIC Ring Inidicator Change interrupt not enabled

    15 [0] Reserved
    14 [0] "
    13 [0] NACK Non Ack interrupt not enabled
    12 [0] RXBUFF Reception Buffer Full (PDC) interrupt not enabled

    11 [0] TXBUFE Transmission Buffer Empty (PDC) interrupt not enabled
    10 [0] ITER/UNRE Max number of Repetitions Reached interrupt not enabled
    09 [0] TXEMPTY Transmitter Empty interrupt not enabled (yet)
    08 [0] TIMEOUT Receiver Time-out interrupt not enabled

    07 [0] PARE Parity Error interrupt not enabled
    06 [0] FRAME Framing Error interrupt not enabled
    05 [0] OVRE Overrun Error interrupt not enabled
    04 [0] ENDTX End of Transmitter Transfer (PDC) interrupt not enabled for now

    03 [0] ENDRX End of Receiver Transfer (PDC) interrupt not enabled
    02 [0] RXBRK Break Received interrupt not enabled
    01 [0] TXRDY Transmitter Ready interrupt not enabled
    00 [0] RXRDY Receiver Ready interrupt not enabled
*/

/* USART Interrupt Disable Register - Page 743 */
#define LCD_US_IDR_INIT (u32)~LCD_US_IER_INIT

/* USART Baud Rate Generator Register - Page 752
BAUD = MCK / CD 
=> CD = MCK / BAUD
BAUD desired = 1 Mbps
=> CD = 48
*/
#define LCD_US_BRGR_INIT (u32)0x00000030  /* VERIFY SPI CLOCK! */
/*
    31-20 [0] Reserved

    19 [0] Reserved
    18 [0] FP baud disabled
    17 [0] "
    16 [0] "

    15 [0] CD = 48 = 0x30
    14 [0] "
    13 [0] "
    12 [0] "

    11 [0] "
    10 [0] "
    09 [0] "
    08 [0] "

    07 [0] "
    06 [0] "
    05 [1] "
    04 [1] "

    03 [0] "
    02 [0] "
    01 [0] "
    00 [0] "
*/


/*----------------------------------------------------------------------------------------------------------------------
ANT USART Setup in SSP
SPI slave mode to communicate with an ANT device. 
*/
/* USART Control Register - Page 734 */
#define ANT_US_CR_INIT (u32)0x00000050
/*
    31 - 20 [0] Reserved

    19 [0] RTSDIS/RCS no release/force RTS to 1
    18 [0] RTSEN/FCS no drive/force RTS to 0
    17 [0] DTRDIS no drive DTR to 1
    16 [0] DTREN no drive DTR to 0

    15 [0] RETTO no restart timeout
    14 [0] RSTNACK N/A
    13 [0] RSTIT N/A
    12 [0] SENDA N/A

    11 [0] STTTO no start time-out
    10 [0] STPBRK no stop break
    09 [0] STTBRK no transmit break
    08 [0] RSTSTA status bits not reset

    07 [0] TXDIS transmitter not disabled
    06 [1] TXEN transmitter enabled
    05 [0] RXDIS receiver not disabled
    04 [1] RXEN receiver enabled

    03 [0] RSTTX not reset
    02 [0] RSTRX not reset
    01 [0] Reserved
    00 [0] "
*/

/* USART Mode Register - page 737 */
#define ANT_US_MR_INIT (u32)0x004118FF
/*
    31 [0] ONEBIT start frame delimiter is COMMAND or DATA SYNC
    30 [0] MODSYNC Manchester start bit N/A
    29 [0] MAN Machester encoding disabled
    28 [0] FILTER no filter on Rx line

    27 [0] Reserved
    26 [0] MAX_ITERATION (ISO7816 mode only)
    25 [0] "
    24 [0] "

    23 [0] INVDATA data is not inverted
    22 [1] VAR_SYNC sync field is updated on char to US_THR
    21 [0] DSNACK (delicious!) NACK is sent on ISO line immeidately on parity error
    20 [0] INACK transmission starts as soon as byte is written to US_THR

    19 [0] OVER 16x oversampling
    18 [0] CLKO USART does not drive the SCK pin
    17 [0] MODE9 CHRL defines char length
    16 [1] CPOL clock is high when inactive

    15 [0] CHMODE normal mode
    14 [0] "
    13 [0] NBSTOP N/A
    12 [1] "

    11 [1] PAR no parity
    10 [0] "
    09 [0] "
    08 [0] CPHA data captured on leading edge of SPCK (first high to low transition does not count)

    07 [1] CHRL 8 bits
    06 [1] "
    05 [1] USCLKS SCK
    04 [1] "

    03 [1] USART_MODE SPI Slave
    02 [1] "
    01 [1] "
    00 [1] "
*/


/* USART Interrupt Enable Register - Page 741 */
#define ANT_US_IER_INIT (u32)0x00080000
/*
    31 [0] Reserved
    30 [0] "
    29 [0] "
    28 [0] "

    27 [0] "
    26 [0] "
    25 [0] "
    24 [0] MANE Manchester Error interrupt not enabled

    23 [0] Reserved
    22 [0] "
    21 [0] "
    20 [0] "

    19 [1] CTSIC Clear to Send Change interrupt enabled 
    18 [0] DCDIC Data Carrier Detect Change interrupt not enabled
    17 [0] DSRIC Data Set Ready Change interrupt not enabled
    16 [0] RIIC Ring Inidicator Change interrupt not enabled

    15 [0] Reserved
    14 [0] "
    13 [0] NACK Non Ack interrupt not enabled
    12 [0] RXBUFF Reception Buffer Full (PDC) interrupt not enabled 

    11 [0] TXBUFE Transmission Buffer Empty (PDC) interrupt not enabled
    10 [0] ITER/UNRE Max number of Repetitions Reached interrupt not enabled
    09 [0] TXEMPTY Transmitter Empty interrupt not enabled (yet)
    08 [0] TIMEOUT Receiver Time-out interrupt not enabled

    07 [0] PARE Parity Error interrupt not enabled
    06 [0] FRAME Framing Error interrupt not enabled
    05 [0] OVRE Overrun Error interrupt not enabled
    04 [0] ENDTX End of Transmitter Transfer (PDC) interrupt not enabled 

    03 [0] ENDRX End of Receiver Transfer (PDC) interrupt not enabled
    02 [0] RXBRK Break Received interrupt not enabled
    01 [0] TXRDY Transmitter Ready interrupt not enabled YET
    00 [0] RXRDY Receiver Ready interrupt enabled 
*/

/* USART Interrupt Disable Register - Page 743 */
#define ANT_US_IDR_INIT (u32)~LCD_US_IER_INIT

/* USART Baud Rate Generator Register - Page 752
!!!!! Not applicable for slave (note that incoming clock cannot 
exceed MCLK/6 = 8MHz.  To date, ANT devices communicate at 500kHz
or 2MHz, so no issues.
*/
#define ANT_US_BRGR_INIT (u32)0x00000000  


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/

