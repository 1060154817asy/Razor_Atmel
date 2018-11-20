/**********************************************************************************************************************
File: user_app1.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app1.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp1Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp1RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */
extern u32 G_u32AntApiCurrentMessageTimeStamp;                    
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];  
extern AntExtendedDataType G_sAntApiCurrentMessageExtData; 

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */
static AntAssignChannelInfoType UserApp1_sChannelInfo;
static u32 u32AntDataCount;
static u32 u32AntTickCount;
static u32  u32ledFrequency;
static u8 au8DataContent_Rate[] = "xxx";
static u8 au8DataContent_Battery[] = "   ";
static u8 au8DataContent_MID[]="   ";
static u8 au8DataContent_count[]="xxx";
static u32 UserApp1_u32DataMsgCount = 0;   /* ANT_DATA packets received */
static u32 UserApp1_u32TickMsgCount = 0;   /* ANT_TICK packets received */
static u8 Key = 0;
static u8 abc = 0;
/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserApp1Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserApp1Initialize(void)
{ 
  UserApp1_sChannelInfo.AntChannel          = ANT_CHANNEL_USERAPP;
  UserApp1_sChannelInfo.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  UserApp1_sChannelInfo.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  UserApp1_sChannelInfo.AntChannelType      = ANT_CHANNEL_TYPE_USERAPP;
  UserApp1_sChannelInfo.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
  UserApp1_sChannelInfo.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
  UserApp1_sChannelInfo.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  UserApp1_sChannelInfo.AntFrequency        = ANT_FREQUENCY_USERAPP;
  UserApp1_sChannelInfo.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  UserApp1_sChannelInfo.AntTxPower          = ANT_TX_POWER_USERAPP;
  UserApp1_sChannelInfo.AntNetwork          = ANT_NETWORK_DEFAULT;
  UserApp1_sChannelInfo.AntNetworkKey[0]    = 0xB9;
  UserApp1_sChannelInfo.AntNetworkKey[1]    = 0xA5;
  UserApp1_sChannelInfo.AntNetworkKey[2]    = 0x21;
  UserApp1_sChannelInfo.AntNetworkKey[3]    = 0xFB;
  UserApp1_sChannelInfo.AntNetworkKey[4]    = 0xBD;
  UserApp1_sChannelInfo.AntNetworkKey[5]    = 0x72;
  UserApp1_sChannelInfo.AntNetworkKey[6]    = 0xC3;
  UserApp1_sChannelInfo.AntNetworkKey[7]    = 0x45;
  /* Try to queue the ANT channel setup */
  if(AntAssignChannel(&UserApp1_sChannelInfo))
  {
    LedOff(RED);
    LedOff(YELLOW);
    LedOff(BLUE);
    LedOn(LCD_RED);
    LedOn(LCD_GREEN);
    LedOn(LCD_BLUE);
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_AntChannelAssign;
  }
  else
  {
    /* The Task isn't properly initialized, so shut it down and don't run */
    LedBlink(RED,LED_4HZ);
    DebugPrintf(" initialized failed\n ");
    UserApp1_StateMachine = UserApp1SM_Error;
  }
} /* end UserApp1Initialize() */
static void UserApp1SM_AntChannelAssign()
{
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CONFIGURED )
  {
    /* Channel assignemnt is successful ,so open channel and proceed to idle state */
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  /* Watch for time out */
  if(IsTimeUp(&UserApp1_u32Timeout,3000))
  {
    DebugPrintf(" setup failed\n ");
    UserApp1_StateMachine = UserApp1SM_Error;
  }
} /* end UserApp1SM_AntChannelAssign */
  
/*----------------------------------------------------------------------------------------------------------------------
Function UserApp1RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserApp1RunActiveState(void)
{
  UserApp1_StateMachine();

} /* end UserApp1RunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{
  /* Command to Get the battery page */
    static u8 au8CommonDataPage_B[8]   = {0x46, 0xff, 0xff, 0xff, 0xff, 0x80, 0x07, 0x01};
    /* Command to Get the manufacturer information page */
    static u8 au8CommonDataPage_MID[8] = {0x46, 0xff, 0xff, 0xff, 0xff, 0x80, 0x02, 0x01};
    /* Command to change to run mode */
    static u8 au8CommonDataPage_R[8]   = {0x4C, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01};
    /* Command to change to swim mode */
//    static u8 au8CommonDataPage_S[8]   = {0x4C, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x05};
    /* If BUTTON3 was pressed the HRM will transmit the battery page */
    if(WasButtonPressed(BUTTON3))
    {
      ButtonAcknowledge(BUTTON3);
      AntQueueAcknowledgedMessage(ANT_CHANNEL_0, au8CommonDataPage_B);
    }
    /* If BUTTON1 was pressed the slave transmit the run mode change page to change the mode*/
    if(WasButtonPressed(BUTTON1))
    {
      ButtonAcknowledge(BUTTON1);
      AntQueueAcknowledgedMessage(ANT_CHANNEL_0, au8CommonDataPage_R);
    }
    /* If BUTTON2 was pressed the slave transmit the swim mode change page to change the mode*
    if(WasButtonPressed(BUTTON2))
    {
      AntQueueAcknowledgedMessage(ANT_CHANNEL_0, au8CommonDataPage_S);
    }*/
     /* If BUTTON2 was pressed the slave transmit the swim mode change page to change the mode*/  
    if(WasButtonPressed(BUTTON2))
    {
      ButtonAcknowledge(BUTTON2);
      AntQueueAcknowledgedMessage(ANT_CHANNEL_0, au8CommonDataPage_MID);
    }
    /* Read buffer */
    if( AntReadAppMessageBuffer() )
    {
    /* New data message: check what it is */
      if(G_eAntApiCurrentMessageClass == ANT_DATA)
      {
        /* Check which message it is */
        Key = (G_au8AntApiCurrentMessageBytes[0] & 0x7f);
        switch(Key)
        { 
          /* Page 0 */
          case 0:
            Key = NumberToAscii(G_au8AntApiCurrentMessageBytes[7], au8DataContent_Rate);       
            LCDCommand(LCD_CLEAR_CMD);
            LCDMessage(LINE2_START_ADDR, "heart rate:");
            LCDMessage(LINE2_START_ADDR+5, au8DataContent_Rate);
            LCDMessage(LINE1_START_ADDR + 3, "%");
            LCDMessage(LINE1_START_ADDR, au8DataContent_Battery);
            LCDMessage(LINE1_START_ADDR + 5, "ID:");
            LCDMessage(LINE1_START_ADDR + 10, au8DataContent_MID);
            
            if(G_au8AntApiCurrentMessageBytes[7] >= 120)
            { 
              abc=G_au8AntApiCurrentMessageBytes[7];
              u32ledFrequency=-(abc*abc/1000)+0.24*abc+30;
              LedOn(RED);
              LedBlink(RED,u32ledFrequency);
            }
            else if(G_au8AntApiCurrentMessageBytes[7] <= 50)
            {
              u32ledFrequency=G_au8AntApiCurrentMessageBytes[7];
              LedOn(YELLOW);
              LedBlink(YELLOW,u32ledFrequency);            
            }
            else
            {
              LedOff(RED);
              LedOff(YELLOW);
             }
            break;
          /* Page 1 */
          case 1:
            break;
          /* Page 2 */
          case 2:
            Key = NumberToAscii(G_au8AntApiCurrentMessageBytes[1], au8DataContent_MID);
            LCDMessage(LINE1_START_ADDR + 5, "ID:");
            LCDMessage(LINE1_START_ADDR + 10, au8DataContent_MID);
            break;
          /* Page 3 */
          case 3:
            break;
          /* Page 4 */
          case 4:
            break;
          /* Page 5 */
          case 5:
            break;
          /* Page 6 */
          case 6:
            break;
          /* Page 7 */
          case 7:
            Key = NumberToAscii(G_au8AntApiCurrentMessageBytes[1], au8DataContent_Battery);
            /*LCDMessage(LINE2_START_ADDR + 3, "n/min");*/
            LCDMessage(LINE1_START_ADDR + 3, "%");
           /* LCDMessage(LINE2_START_ADDR, au8DataContent_Rate);*/
            LCDMessage(LINE1_START_ADDR, au8DataContent_Battery);
            if(G_au8AntApiCurrentMessageBytes[1] <= 15)
            {
              LedOff(LCD_GREEN);
              LedOff(LCD_BLUE);
              PWMAudioSetFrequency(BUZZER1, 262);
              PWMAudioOn(BUZZER1);
            }
            else 
            {
              PWMAudioOff(BUZZER1); 
              LedOn(LCD_GREEN);
              LedOn(LCD_BLUE);
            }
            
            break;
          /* Page 8 */
          case 8:
            break;
        }
      }
      /* The process for the ANT_TICK message */
      else if(G_eAntApiCurrentMessageClass == ANT_TICK)
      {
        //AntQueueAcknowledgedMessage(ANT_CHANNEL_0, au8CommonDataPage_B);
      }
    }
} /* end UserApp1SM_Idle() */
    

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/

