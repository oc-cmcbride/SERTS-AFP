#include "cmsis_os.h"  // CMSIS RTOS header file
#include "Board_LED.h"
#include "UART_driver.h"
#include "stdint.h"                     // data type definitions
#include "stdio.h"                      // file I/O functions
#include "rl_usb.h"                     // Keil.MDK-Pro::USB:CORE
#include "rl_fs.h"                      // Keil.MDK-Pro::File System:CORE
#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_audio.h"
#include "math.h"
#include "arm_math.h" // header for DSP library
#include <stdio.h>

// LED constants
#define LED_Green   0
#define LED_Orange  1
#define LED_Red     2
#define LED_Blue    3

// File reading constants
#define Show_Files_char "1"
#define Save_File_char "4"
enum commands{
  IndexPressed,
  IndexingDone
};

// State Machine definitions
enum state{
  NoState,
  Stopped,
  Indexing,
  Playing,
  Paused
};

// State machine actions
enum action {
	endActions,
	redOff,
	redOn,
	blueOff,
	blueOn,
	greenOff,
	greenOn,
	yellowOff,
	yellowOn,
	startListFiles,
	startStreamAudio,
	stopStreamAudio
};


/*
 * FUNCTION PROTOTYPES AND THREAD DEFINITIONS
 */
void Process_Event(uint16_t event, uint32_t actionList[10]);

void Rx_Command(const void *arg);
osThreadId tid_Rx_Command;
osThreadDef(Rx_Command, osPriorityNormal, 1, 0);

void Control(void const *arg);
osThreadId tid_Control;
osThreadDef(Control, osPriorityNormal, 1, 0);

void File_System(const void *arg);
osThreadId tid_File_System;
osThreadDef(File_System, osPriorityNormal, 1, 0);


/*
 * MESSAGE QUEUE DEFINITIONS
 */
// Command queue from Rx_Command and File_System to Control
osMessageQId mid_CMDQueue;
osMessageQDef(CMDQueue, 1, uint32_t);

// Command Queue from Control to File_System
osMessageQId mid_FSQueue;
osMessageQDef(FSQueue, 1, uint32_t);


/*
 * FUNCTION DEFINITIONS
 */
// Thread initialization function
void Init_Thread (void) {
	LED_Initialize(); // Initialize the LEDs
	UART_Init(); // Initialize the UART
	
	// Create queues
	mid_CMDQueue = osMessageCreate(osMessageQ(CMDQueue), NULL);
	if (!mid_CMDQueue) return;
	mid_FSQueue = osMessageCreate(osMessageQ(FSQueue), NULL);
	if (!mid_FSQueue) return;

	// Create threads
	tid_Rx_Command = osThreadCreate(osThread(Rx_Command), NULL);
	if (!tid_Rx_Command) return;
	tid_Control = osThreadCreate(osThread(Control), NULL);
	if (!tid_Control) return;
	tid_File_System = osThreadCreate(osThread(File_System), NULL);
	if (!tid_File_System) return;
}


// State machine function (passive object)
void Process_Event(uint16_t event, uint32_t actionList[10]) {
	// Declare locals
	static uint16_t Current_State = NoState;
	int actionIndex = 0;

	// Process received event
	switch(Current_State) {
	case NoState:
		// Next State
		Current_State = Stopped;
		// Exit Actions
		// Transition actions
		// Stop entry actions
		actionList[actionIndex++] = redOff;
		break;
	case Stopped:
		if (event == IndexPressed) {
			// Next State
			Current_State = Indexing;
			// Exit Actions
			actionList[actionIndex++] = redOff;
			// Transition actions
			// Indexing entry actions
			actionList[actionIndex++] = blueOn;
			actionList[actionIndex++] = startListFiles;
		}
		break;
	case Indexing:
		if (event == IndexingDone) {
			// Next State
			Current_State = Stopped;
			// Exit Actions
			actionList[actionIndex++] = blueOff;
			// Transition actions
			// Stop entry actions
			actionList[actionIndex++] = redOn;
		}
		break;
	default:
		break;
	} // end case(Current_State)

	// Add endActions to end of action list
	actionList[actionIndex++] = endActions;
}


// Receive command thread
void Rx_Command (void const *arg) {
	// Declare locals
	char rx_char[2] = {0, 0};
	char receivedFile[256];

	// Thread loop
	while(1) {
		// Wait for command from PC GUI
		UART_receive(rx_char, 1);

		// Check type of char received
		if (!strcmp(rx_char, Show_Files_char)) {
			// Show Files char received
			osMessagePut(mid_CMDQueue, IndexPressed, osWaitForever);
		}
		else if (!strcmp(rx_char, Save_File_char)) {
			// Save the next string into the receivedFile buffer
			LED_On(LED_Green);
			UART_receivestring(receivedFile, 256);
			LED_Off(LED_Green);
		}
	}
}

// Main control thread
void Control (void const *arg) {
	// Declare locals
	osEvent evt;
	uint32_t actionList[10];
	int i;

	// Initialize state machine
	Process_Event(0, actionList);
	LED_On(LED_Red);

	// Main thread loop
	while(1) {
		// Check for command on message queue
		evt = osMessageGet(mid_CMDQueue, osWaitForever);

		// Check for valid message and process
		if (evt.status == osEventMessage) {
			Process_Event(evt.value.v, actionList);

			// Perform actions
			for (i = 0; (i < 10) && (actionList[i] != endActions); i++) {
				switch(actionList[i]) {
				case redOff:
					LED_Off(LED_Red);
					break;
				case redOn:
					LED_On(LED_Red);
					break;
				case blueOff:
					LED_Off(LED_Blue);
					break;
				case blueOn:
					LED_On(LED_Blue);
					break;
				case greenOff:
					LED_Off(LED_Green);
					break;
				case greenOn:
					LED_On(LED_Green);
					break;
				case yellowOff:
					LED_Off(LED_Orange);
					break;
				case yellowOn:
					LED_On(LED_Orange);
					break;
				case startListFiles:
					osMessagePut(mid_FSQueue, IndexPressed, osWaitForever);
					break;
				default:
					break;
				}
			}
		}
	}
}

// File System thread
void File_System (void const *arg) {
	// Declare locals
	usbStatus ustatus;			// USB driver status variable
	uint8_t drivenum = 0;		// U0: drive number
	char *drive_name = "U0:";	// USB drive name
	fsStatus fstatus;			// File system status variable
	osEvent evt;				// Receive message object
	char *StartFileList_msg = "2\n";
	char *EndFileList_msg = "3\n";
	fsFileInfo fileInfo;


	// Initialize USB device
	ustatus = USBH_Initialize(drivenum);
	if (ustatus == usbOK) {
		// Loop until device is OK
		ustatus = USBH_Device_GetStatus(drivenum);
		while (ustatus != usbOK) {
			ustatus = USBH_Device_GetStatus(drivenum);
		}

		// Initialize the drive
		fstatus = finit(drive_name);
		if (fstatus != fsOK) {
			// Handle finit error
		}

		// Mount the drive
		fstatus = fmount(drive_name);
		if (fstatus != fsOK) {
			// Handle fmount error
		}

		// Main thread loop
		while (1) {
			// Wait for message
			evt = osMessageGet(mid_FSQueue, osWaitForever);

			// Check for valid message
			if ((evt.status == osEventMessage) && (evt.value.v == IndexPressed)) {
				// File system enabled, send start message
				UART_send(StartFileList_msg, 2);

				// File system loop
				fileInfo.fileID = 0;
				while (ffind("*.wav", &fileInfo) == fsOK) {
					if (fileInfo.attrib == 32) {
						UART_send(fileInfo.name, strlen(fileInfo.name));
						UART_send("\n", 1);
					}
				}

				// File system disabled, send end messages
				UART_send(EndFileList_msg, 2);
				osMessagePut(mid_CMDQueue, IndexingDone, osWaitForever);
			}
		} // end while (1)
	} // end if (ustatus == usbOK)
}


