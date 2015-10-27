/***********************************************************************************************************************
File: main.h

Description:
Header file for main.c.
*******************************************************************************/

#ifndef __MAIN_H
#define __MAIN_H

/***********************************************************************************************************************
* Firmware Version - Printed FIRMWARE_MAIN_REV.FIRMWARE_SUB_REV1 FIRMWARE_SUB_REV2
* See releasenotes.txt for firmware details.
***********************************************************************************************************************/
#define FIRMWARE_MAIN_REV               '0'
#define FIRMWARE_SUB_REV1               '0'
#define FIRMWARE_SUB_REV2               '1'


/***********************************************************************************************************************
* Constant Definitions
***********************************************************************************************************************/
#define MAX_DRINKS          (u8)10      /* Maximum number of drinks a server can hold */


/***********************************************************************************************************************
* Type Definitions
***********************************************************************************************************************/
typedef enum {EMPTY, BEER, SHOOTER, WINE, HIBALL} DrinkType;

typedef struct
{
  u8 u8ServerNumber;                    /* Unique token for this message */
  DrinkType asServingTray[MAX_DRINKS];  /* Data payload array */
  void* psNextServer;                   /* Pointer to next ServerType*/
} ServerType;




#endif /* __MAIN_H */