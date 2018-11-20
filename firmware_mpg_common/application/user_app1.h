/**********************************************************************************************************************
File: user_app1.h                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
1. Follow the instructions at the top of user_app1.c
2. Use ctrl-h to find and replace all instances of "user_app1" with "yournewtaskname"
3. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
4. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
5. Add #include yournewtaskname.h" to configuration.h
6. Add/update any special configurations required in configuration.h (e.g. peripheral assignment and setup values)
7. Delete this text (between the dashed lines)
----------------------------------------------------------------------------------------------------------------------

Description:
Header file for user_app1.c

**********************************************************************************************************************/

#ifndef __USER_APP1_H
#define __USER_APP1_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/


/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/


/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/
#define ANT_CHANNEL_TYPE_USERAPP        CHANNEL_TYPE_SLAVE   // Device is a MASTER
#define ANT_CHANNEL_USERAPP             ANT_CHANNEL_0         // Channel 0 - 7
#define ANT_CHANNEL_PERIOD_LO_USERAPP   (u8)0x86              // LO; 0x0001 - 0x7fff
#define ANT_CHANNEL_PERIOD_HI_USERAPP   (u8)0x1f              // HI; 0x0001 - 0x7fff
#define ANT_DEVICEID_LO_USERAPP         (u8)0              // Device # Low byte
#define ANT_DEVICEID_HI_USERAPP         (u8)0              // Device # High byte
#define ANT_DEVICE_TYPE_USERAPP         (u8)120               // 1 - 255
#define ANT_TRANSMISSION_TYPE_USERAPP   (u8)0                 // 1-127; MSB is pairing
#define ANT_FREQUENCY_USERAPP           (u8)0x39                // 2400MHz + 0 - 99 MHz
#define ANT_TX_POWER_USERAPP            RADIO_TX_POWER_0DBM   // Max tx power


#define COUNTER_LIMIT_MS (u32)500
#define COLOR_CYCLE_TIME (u16)60
/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/
void UserApp1Initialize(void);
void UserApp1RunActiveState(void);


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void UserApp1SM_Idle(void);    

static void UserApp1SM_Error(void);         
static void UserApp1SM_AntChannelAssign(void);

#endif /* __USER_APP1_H */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
