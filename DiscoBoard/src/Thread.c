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
#define Play_File_char "4"

// Action/Event constants
#define MAX_ACTIONS 10

// Audio play constants
#define NUM_CHAN 2						// Number of audio channels
#define NUM_POINTS 1024					// Number of points per channel
#define BUF_LEN NUM_CHAN*NUM_POINTS		// Length of the audio buffer

// Event definitions
enum event {
  IndexPressed,
  IndexingDone,
  PlayPressed,
  ResumePressed,
  PausePressed,
  StopPressed,
  EndOfAudioFile
};

// State Machine definitions
enum state {
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
	resumeStreamAudio,
	pauseStreamAudio,
	stopStreamAudio
};


/*
 * STRUCTS
 */
// WAVE file header format
typedef struct WAVHEADER {
	unsigned char riff[4];						// RIFF string
	uint32_t overall_size;				// overall size of file in bytes
	unsigned char wave[4];						// WAVE string
	unsigned char fmt_chunk_marker[4];		// fmt string with trailing null char
	uint32_t length_of_fmt;					// length of the format data
	uint16_t format_type;					// format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
	uint16_t channels;						// no.of channels
	uint32_t sample_rate;					// sampling rate (blocks per second)
	uint32_t byterate;						// SampleRate * NumChannels * BitsPerSample/8
	uint16_t block_align;					// NumChannels * BitsPerSample/8
	uint16_t bits_per_sample;				// bits per sample, 8- 8bits, 16- 16 bits etc
	unsigned char data_chunk_header [4];		// DATA string or FLLR string
	uint32_t data_size;						// NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read
} WAVHEADER;


/*
 * GLOBAL VARIABLES
 */
char currentFileName[256];		// Current playing file name
int16_t AudioBuffer0[BUF_LEN];	// Buffer0 used for audio playing
int16_t AudioBuffer1[BUF_LEN];	// Buffer1 used for audio playing


/*
 * FUNCTION PROTOTYPES AND THREAD DEFINITIONS
 */
void Process_Event(uint16_t event, uint32_t actionList[MAX_ACTIONS]);

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
 * SEMAPHORE DEFINITIONS
 */
// File name access
osSemaphoreDef(SEM_FILE);
osSemaphoreId SEM_FILE_ID;

// DMA buffer change
osSemaphoreDef(SEM_DMA);
osSemaphoreId SEM_DMA_ID;


/*
 * MESSAGE QUEUE DEFINITIONS
 */
// Command queue from Rx_Command and File_System to Control
osMessageQId mid_CMDQueue;
osMessageQDef(CMDQueue, 1, uint32_t);

// Command Queue from Control to File_System
osMessageQId mid_FSQueue;
osMessageQDef(FSQueue, 1, uint32_t);

// Command queue from File_System to BSP_AUDIO_OUT_TransferComplete_Callback
osMessageQId mid_DMAQueue;
osMessageQDef(DMAQueue, 1, int16_t);


/*
 * FUNCTION DEFINITIONS
 */
// Thread initialization function
void Init_Thread (void) {
	LED_Initialize(); // Initialize the LEDs
	UART_Init(); // Initialize the UART
	
	// Create semaphores
	SEM_FILE_ID = osSemaphoreCreate(osSemaphore(SEM_FILE), 1);
	SEM_DMA_ID = osSemaphoreCreate(osSemaphore(SEM_DMA), 0);

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
void Process_Event(uint16_t event, uint32_t actionList[MAX_ACTIONS]) {
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
		else if (event == PlayPressed) {
			// Next State
			Current_State = Playing;
			// Exit Actions
			actionList[actionIndex++] = redOff;
			// Transition actions
			// Playing entry actions
			actionList[actionIndex++] = greenOn;
			actionList[actionIndex++] = startStreamAudio;
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
	case Playing:
		if (event == EndOfAudioFile) {
			// Next State
			Current_State = Stopped;
			// Exit Actions
			actionList[actionIndex++] = greenOff;
			// Transition Actions
			// Stop entry actions
			actionList[actionIndex++] = redOn;
		}
		break;
	case Paused:

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

	// Thread loop
	while(1) {
		// Wait for command from PC GUI
		UART_receive(rx_char, 1);

		// Check type of char received
		if (!strcmp(rx_char, Show_Files_char)) {
			// Show Files char received
			osMessagePut(mid_CMDQueue, IndexPressed, osWaitForever);
		}
		else if (!strcmp(rx_char, Play_File_char)) {
			// Save the next string into the receivedFile buffer
			osSemaphoreWait(SEM_FILE_ID, osWaitForever);
			UART_receivestring(currentFileName, 256);
			osSemaphoreRelease(SEM_FILE_ID);

			// Play button pressed
			osMessagePut(mid_CMDQueue, PlayPressed, osWaitForever);
		}
	}
}

// Main control thread
void Control (void const *arg) {
	// Declare locals
	osEvent evt;
	uint32_t actionList[MAX_ACTIONS];
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
			for (i = 0; (i < MAX_ACTIONS) && (actionList[i] != endActions); i++) {
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
					osMessagePut(mid_FSQueue, startListFiles, osWaitForever);
					break;
				case startStreamAudio:
					osMessagePut(mid_FSQueue, startStreamAudio, osWaitForever);
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
	static uint8_t rtrn = 0;	// return variable
	usbStatus ustatus;			// USB driver status variable
	uint8_t drivenum = 0;		// U0: drive number
	char *drive_name = "U0:";	// USB drive name
	fsStatus fstatus;			// File system status variable
	osEvent evt;				// Receive message object
	char *StartFileList_msg = "2\n";
	char *EndFileList_msg = "3\n";
	fsFileInfo fileInfo;
	FILE *audioFile;
	WAVHEADER header; 			// header struct for wav file
	size_t rd; 					// number of blocks read using fread
	int16_t bufIndx = 0; 		// Current buffer index (can only be 0 or 1)
	int debugCount = 0;


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
			if (evt.status == osEventMessage) {
				// Handle event
				switch (evt.value.v) {
				case startListFiles:
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

					break;
				case startStreamAudio:
					// Open file
					osSemaphoreWait(SEM_FILE_ID, osWaitForever);
					audioFile = fopen(currentFileName, "r");
					osSemaphoreRelease(SEM_FILE_ID);

					if (audioFile != NULL) {
						// Read header
						fread((void *)&header, sizeof(header), 1, audioFile);

						// Initialize audio output
						rtrn = BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_AUTO, 0x46, header.sample_rate);
						if (rtrn != AUDIO_OK) {
							// Audio init error handling
						}

						// Generate initial data for audio buffer
						rd = fread((void *)AudioBuffer0, sizeof(int16_t), BUF_LEN, audioFile);

						// Start the audio player, size is number of bytes so mult. by 2
						BSP_AUDIO_OUT_Play((uint16_t *)AudioBuffer0, BUF_LEN*2);
					}

					// No break at end of case!

				case resumeStreamAudio:
					// If not at the beginning of the song, resume play
					if (evt.value.v == resumeStreamAudio) {
						// Fill next buffer
						if (bufIndx) {
							// Switch from buffer 1 to buffer 0. Fill buffer 0.
							bufIndx = 0;
							rd = fread((void *)AudioBuffer0, sizeof(int16_t), BUF_LEN, audioFile);
						}
						else {
							// Switch from buffer 0 to buffer 1. Fill buffer 1.
							bufIndx = 1;
							rd = fread((void *)AudioBuffer1, sizeof(int16_t), BUF_LEN, audioFile);
						}

						// Unmute
						BSP_AUDIO_OUT_SetMute(AUDIO_MUTE_OFF);

						// Start DMA again
						osMessagePut(mid_DMAQueue, bufIndx, osWaitForever);
						osSemaphoreWait(SEM_DMA_ID, osWaitForever);
					}

					// Play loop
					while (rd == BUF_LEN) {
						// Fill next buffer
						if (bufIndx) {
							// Switch from buffer 1 to buffer 0. Fill buffer 0.
							bufIndx = 0;
							rd = fread((void *)AudioBuffer0, sizeof(int16_t), BUF_LEN, audioFile);
						}
						else {
							// Switch from buffer 0 to buffer 1. Fill buffer 1.
							bufIndx = 1;
							rd = fread((void *)AudioBuffer1, sizeof(int16_t), BUF_LEN, audioFile);
						}

						debugCount++;

						// Only put more data on the DMA with a full buffer
						if (rd == BUF_LEN) {
							// Send message with next buffer number
							osMessagePut(mid_DMAQueue, bufIndx, osWaitForever);

							if (debugCount == 2) {
								LED_On(LED_Orange);
							}

							// Wait for semaphore to release
							osSemaphoreWait(SEM_DMA_ID, osWaitForever);

							if (debugCount == 2) {
								LED_Off(LED_Orange);
							}
						}

						// Read message queue with no delay
						evt = osMessageGet(mid_FSQueue, 0);

						// Check for valid message
						if (evt.status == osEventMessage) {
							// Check for pause or stop message
							if (evt.value.v == pauseStreamAudio) {
								// Mute audio
								BSP_AUDIO_OUT_SetMute(AUDIO_MUTE_ON);

								// Set read length greater than buffer length; this indicates a pause
								rd = BUF_LEN + 1;
							}
							else if (evt.value.v == stopStreamAudio) {
								// Stop playing; set buffer length to 0
								rd = 0;
							}
						}
					} // end while (rd == BUF_LEN)

					// Audio file not playing. Only stop file if rd is less than BUF_LEN.v
					if (rd < BUF_LEN) {
						// Send song done to state machine
						osMessagePut(mid_CMDQueue, EndOfAudioFile, osWaitForever);

						// End of song or stop
						BSP_AUDIO_OUT_SetMute(AUDIO_MUTE_ON);

						// Close audio file
						fclose(audioFile);
					}
					break; // end case ResumePressed
				default:
					break;
				} // end switch (evt.value.v)
			} // end if (status == osEventMessage)
		} // end while (1)
	} // end if (ustatus == usbOK)
}


/* User Callbacks: user has to implement these functions if they are needed. */
/* This function is called when the requested data has been completely transferred. */
int callbackCount = 0;
void    BSP_AUDIO_OUT_TransferComplete_CallBack(void){
	// Declare locals
	osEvent evt;

	if (callbackCount == 2) LED_On(LED_Blue);
	// Read message queue for next buffer
	evt = osMessageGet(mid_DMAQueue, 0);

	// Check if a message was received
	if (evt.status == osEventMessage) {
		// Check which buffer to read from next
		if (evt.value.v == 0) {
			// Buffer 0
			BSP_AUDIO_OUT_ChangeBuffer((uint16_t*)AudioBuffer0, BUF_LEN);
		}
		else {
			// Buffer 1
			BSP_AUDIO_OUT_ChangeBuffer((uint16_t*)AudioBuffer1, BUF_LEN);
		}
	}

	// Buffer changed, release semaphore
	osSemaphoreRelease(SEM_DMA_ID);

	if (callbackCount == 2) LED_Off(LED_Blue);

	callbackCount++;
}

/* This function is called when half of the requested buffer has been transferred. */
void    BSP_AUDIO_OUT_HalfTransfer_CallBack(void){
}

/* This function is called when an Interrupt due to transfer error or peripheral
   error occurs. */
void    BSP_AUDIO_OUT_Error_CallBack(void){
		while(1){
		}
}


