/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserAppInitialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserAppRunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserAppFlags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u32 G_u32AntApiCurrentMessageTimeStamp;                           /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;           /* From ant_api.c */
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES]; /* From ant_api.c */
extern AntExtendedDataType G_stCurrentMessageExtendedData;               /* From ant_api.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp_StateMachine;            /* The state machine function pointer */
static u32 UserApp_u32Timeout;                      /* Timeout counter used across states */


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
Function: UserAppInitialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserAppInitialize(void)
{
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, "ANT MULTICHAN DEMO");
  LCDMessage(LINE2_START_ADDR, "CH0   CH1   CH2  OPN");
  UserApp_StateMachine = UserAppSM_Idle;
//  UserApp_StateMachine = UserAppSM_ChannelSetup;

  /* If good initialization, set state to Idle */
  if( 1 )
  {
    DebugPrintf("User app ready\n\r");
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp_StateMachine = UserAppSM_FailedInit;
    DebugPrintf("User app setup failed\n\r");
  }
  
} /* end UserAppInitialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function UserAppRunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserAppRunActiveState(void)
{
  UserApp_StateMachine();

} /* end UserAppRunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserAppSM_ChannelSetup(void)
{
  bool bSetupOk = TRUE;
  
  AntAssignChannelInfoType sChannelInfo;
  
  /* Setup channel 0 */
  sChannelInfo.AntChannel = 0;
  sChannelInfo.AntChannelType = CHANNEL_TYPE_MASTER;
  sChannelInfo.AntChannelPeriodHi = ANT_CHANNEL_PERIOD_HI_DEFAULT;
  sChannelInfo.AntChannelPeriodLo = ANT_CHANNEL_PERIOD_LO_DEFAULT;
  
  sChannelInfo.AntDeviceIdHi = 0x00;
  sChannelInfo.AntDeviceIdLo = 0x03;
  sChannelInfo.AntDeviceType = ANT_DEVICE_TYPE_DEFAULT;
  sChannelInfo.AntTransmissionType = ANT_TRANSMISSION_TYPE_DEFAULT;
  
  sChannelInfo.AntFrequency = ANT_FREQUENCY_DEFAULT;
  sChannelInfo.AntTxPower = ANT_TX_POWER_DEFAULT;
  
  sChannelInfo.AntNetwork = ANT_NETWORK_DEFAULT;
  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    sChannelInfo.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  }
    
  if(!AntAssignChannel(&sChannelInfo))
  {
    bSetupOk = FALSE;
  }

  if( bSetupOk )
  {
    UserApp_StateMachine = UserAppSM_Idle;
    DebugPrintf("User app ready\n\r");
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp_StateMachine = UserAppSM_FailedInit;
    DebugPrintf("User app setup failed\n\r");
  }
  
} /* end UserAppSM_ChannelSetup */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Process Idle functions */
static void UserAppSM_Idle(void)
{
  AntAssignChannelInfoType sChannelInfo;
  u8 au8DataContent[] = {"ANT_DATA CHX: xx-xx-xx-xx-xx-xx-xx-xx xx-xx-xx-xx xx dBm\n\r"};
  u8 au8TickContent[] = {"ANT_TICK CHX: xx-xx-xx-xx-xx-xx-xx-xx\n\r"};
  u8 u8MessageIndex;
  
  if( AntReadAppMessageBuffer() )
  {
    u8MessageIndex = 11;
    if( G_eAntApiCurrentMessageClass == ANT_TICK)
    {
      /* Get the channel number */
      au8TickContent[u8MessageIndex] = HexToASCIICharUpper(G_stCurrentMessageExtendedData.u8Channel);
      u8MessageIndex = 14;

      /* Read all the data bytes and convert to ASCII for the display string */
      for(u8 i = 0; i < ANT_DATA_BYTES; i++)
      {
       au8TickContent[u8MessageIndex]     = HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[i] / 16);
       au8TickContent[u8MessageIndex + 1] = HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[i] % 16);
       u8MessageIndex += 3;
      }
      
      DebugPrintf(au8TickContent);
    }
    
    if( G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      /* Get the channel number */
      au8DataContent[u8MessageIndex] = HexToASCIICharUpper(G_stCurrentMessageExtendedData.u8Channel);
      u8MessageIndex = 14;

      /* Read all the data bytes and convert to ASCII for the display string */
      for(u8 i = 0; i < ANT_DATA_BYTES; i++)
      {
        au8DataContent[u8MessageIndex]     = HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[i] / 16);
        au8DataContent[u8MessageIndex + 1] = HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[i] % 16);
        u8MessageIndex += 3;
      }

      /* Add the extended data bytes */
    
      /* Device ID */
      au8DataContent[u8MessageIndex++] = HexToASCIICharUpper( (G_stCurrentMessageExtendedData.u16DeviceID >>  4) & 0x000F);
      au8DataContent[u8MessageIndex]   = HexToASCIICharUpper( (G_stCurrentMessageExtendedData.u16DeviceID >>  0) & 0x000F);
      u8MessageIndex += 2;
      au8DataContent[u8MessageIndex++] = HexToASCIICharUpper( (G_stCurrentMessageExtendedData.u16DeviceID >> 12) & 0x000F);
      au8DataContent[u8MessageIndex]   = HexToASCIICharUpper( (G_stCurrentMessageExtendedData.u16DeviceID >>  8) & 0x000F);

      /* Device Type */
      u8MessageIndex += 2;
      au8DataContent[u8MessageIndex++] = HexToASCIICharUpper( (G_stCurrentMessageExtendedData.u8DeviceType >> 4) & 0x0F);
      au8DataContent[u8MessageIndex]   = HexToASCIICharUpper( (G_stCurrentMessageExtendedData.u8DeviceType >> 0) & 0x0F);

      /* Transmission Type */
      u8MessageIndex += 2;
      au8DataContent[u8MessageIndex++] = HexToASCIICharUpper( (G_stCurrentMessageExtendedData.u8TransType >> 4) & 0x0F);
      au8DataContent[u8MessageIndex]   = HexToASCIICharUpper( (G_stCurrentMessageExtendedData.u8TransType >> 0) & 0x0F);
 
      /* RSSI value */
      u8MessageIndex += 3;
      au8DataContent[u8MessageIndex++] = HexToASCIICharUpper( (G_stCurrentMessageExtendedData.s8RSSI >> 4) & 0x0F);
      au8DataContent[u8MessageIndex++] = HexToASCIICharUpper( (G_stCurrentMessageExtendedData.s8RSSI >> 0) & 0x0F);
    
      DebugPrintf(au8DataContent);
    }
  }
  
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    if(AntRadioStatusChannel(ANT_CHANNEL_0) == ANT_UNCONFIGURED)
    {
      /* Setup channel 0 */
      sChannelInfo.AntChannel = ANT_CHANNEL_0;
      sChannelInfo.AntChannelType = CHANNEL_TYPE_MASTER;
      sChannelInfo.AntChannelPeriodHi = ANT_CHANNEL_PERIOD_HI_DEFAULT;
      sChannelInfo.AntChannelPeriodLo = ANT_CHANNEL_PERIOD_LO_DEFAULT;
      
      sChannelInfo.AntDeviceIdHi = 0x00;
      sChannelInfo.AntDeviceIdLo = 0x01;
      sChannelInfo.AntDeviceType = ANT_DEVICE_TYPE_DEFAULT;
      sChannelInfo.AntTransmissionType = ANT_TRANSMISSION_TYPE_DEFAULT;
      
      sChannelInfo.AntFrequency = ANT_FREQUENCY_DEFAULT;
      sChannelInfo.AntTxPower = ANT_TX_POWER_DEFAULT;
      
      sChannelInfo.AntNetwork = ANT_NETWORK_DEFAULT;
      for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
      {
        sChannelInfo.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
      }
      
      AntAssignChannel(&sChannelInfo);
    }
    else
    {
      AntUnassignChannelNumber(ANT_CHANNEL_0);
    }
  } /* end BUTTON0 */
    
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    if(AntRadioStatusChannel(ANT_CHANNEL_1) == ANT_UNCONFIGURED)
    {
      /* Setup channel 1 */
      sChannelInfo.AntChannel = ANT_CHANNEL_1;
      sChannelInfo.AntChannelType = CHANNEL_TYPE_MASTER;
      sChannelInfo.AntChannelPeriodHi = ANT_CHANNEL_PERIOD_HI_DEFAULT;
      sChannelInfo.AntChannelPeriodLo = ANT_CHANNEL_PERIOD_LO_DEFAULT;
      
      sChannelInfo.AntDeviceIdHi = 0x11;
      sChannelInfo.AntDeviceIdLo = 0x11;
      sChannelInfo.AntDeviceType = ANT_DEVICE_TYPE_DEFAULT;
      sChannelInfo.AntTransmissionType = ANT_TRANSMISSION_TYPE_DEFAULT;
      
      sChannelInfo.AntFrequency = ANT_FREQUENCY_DEFAULT;
      sChannelInfo.AntTxPower = ANT_TX_POWER_DEFAULT;
      
      sChannelInfo.AntNetwork = ANT_NETWORK_DEFAULT;
      for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
      {
        sChannelInfo.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
      }
      
      AntAssignChannel(&sChannelInfo);
    }
    else
    {
      AntUnassignChannelNumber(ANT_CHANNEL_1);
    }
  } /* end BUTTON1 */

  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    if(AntRadioStatusChannel(ANT_CHANNEL_2) == ANT_UNCONFIGURED)
    {
      /* Setup channel 2 */
      sChannelInfo.AntChannel = ANT_CHANNEL_2;
      sChannelInfo.AntChannelType = CHANNEL_TYPE_MASTER;
      sChannelInfo.AntChannelPeriodHi = ANT_CHANNEL_PERIOD_HI_DEFAULT;
      sChannelInfo.AntChannelPeriodLo = ANT_CHANNEL_PERIOD_LO_DEFAULT;
      
      sChannelInfo.AntDeviceIdHi = 0x22;
      sChannelInfo.AntDeviceIdLo = 0x22;
      sChannelInfo.AntDeviceType = ANT_DEVICE_TYPE_DEFAULT;
      sChannelInfo.AntTransmissionType = ANT_TRANSMISSION_TYPE_DEFAULT;
      
      sChannelInfo.AntFrequency = ANT_FREQUENCY_DEFAULT;
      sChannelInfo.AntTxPower = ANT_TX_POWER_DEFAULT;
      
      sChannelInfo.AntNetwork = ANT_NETWORK_DEFAULT;
      for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
      {
        sChannelInfo.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
      }
      
      AntAssignChannel(&sChannelInfo);
    }
    else
    {
      AntUnassignChannelNumber(ANT_CHANNEL_2);
    }
  } /* end BUTTON2 */

  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    
    for(u8 i = 0; i < 3; i++)
    {
      /* Manage channels (type cast index to AntChannelNumberType is allowed */
      if(AntRadioStatusChannel( (AntChannelNumberType)i ) == ANT_CLOSED)
      {
        AntOpenChannelNumber( (AntChannelNumberType)i );
      }
      
      if(AntRadioStatusChannel( (AntChannelNumberType)i ) == ANT_OPEN)
      {
        AntCloseChannelNumber( (AntChannelNumberType)i );
      }
    }
  } /* end BUTTON3 */
  
} /* end UserAppSM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserAppSM_Error(void)          
{
  
} /* end UserAppSM_Error() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserAppSM_FailedInit(void)          
{
    
} /* end UserAppSM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
