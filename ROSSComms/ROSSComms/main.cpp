/*
 * ROSSComms.cpp
 *
 * Created: 3/12/2018 6:57:23 PM
 * Author : Nick McComb | www.nickmccomb.net
 */ 

#define FIRMWARE_VERSION_STR "0.1"
#define FIRMWARE_VERSION .1

#include <avr/io.h>

#include "main.h"

struct RSSI_type {
	//"User" class variables
	uint16_t value;
	uint8_t timeoutOccured;
	
	//"System" class variables
	uint8_t measuring;
	uint16_t timeDifference;
	uint16_t countDifference;
	uint16_t sampleCount;
	uint16_t sampleCountTemp;
};

//GLOBAL Variables *gasp*
volatile uint8_t broadcastStatus = 0;
volatile uint8_t globalToggle = 0;
volatile RSSI_type RSSI;



int main(void)
{

	configureIO();	
	configureExternalOscillator();
	configureUSART();
	configureSerialNumber();
	configureTimerCounter();

	uint8_t receivedUSARTData;  //Holds received bytes from serial devices

	LOW_LEVEL_INTERRUPTS_ENABLE();
	sei();
	
	//Setup radios
	XBEE_SLEEP();
	IRIDIUM_SLEEP();
	
	//PORTD.OUTSET = PIN5_bm; //RGB LED Test

	//Init string with basic documentation
	SendStringPC("\n\n\r#[INIT ROSE COMMS]\n\r");
	SendStringPC("#Firmware version ");
	SendStringPC(FIRMWARE_VERSION_STR);
	SendStringPC("\n\r#Serial Number: ");
	if(serialNumber == -1)
		SendStringPC("NOT SET");
	else
		SendNumPC(serialNumber);
	SendStringPC("\n\r#Msg format: Iridium Status | RSSI Value | Comms Status\n\r");
	
	RED_OFF();
	GREEN_OFF();
	BLUE_OFF();
		
	
    while (1) 
    {
		if(broadcastStatus){
			broadcastStatus = 0;
			//SendStringPC("Broadcast, yo\n\r");
			
			//Assemble Iridium Status byte
			uint8_t iridiumStatus = (CHECK_IRID_PG() << 1) | (CHECK_IRID_NETWORK() << 0);
			
			//Assemble Comms Value Byte
			uint8_t commsStatus = ((!CHECK_TX_SW()) << 0);
			
			SendNumPC(iridiumStatus);
			SendStringPC("|");
			SendStringPC("RSSI");
			SendStringPC("|");
			SendNumPC(commsStatus);
			SendStringPC("\n\r");
		}
		
		_delay_ms(25);
		
		//Process misc. communication
		if(USART_IsRXComplete(&COMP_USART)){
			SendStringPC("Received data. ");
			SendStringPC("\n\r");
			receivedUSARTData = USART_GetChar(&COMP_USART);
			
			//Iridium Controls
			if(receivedUSARTData == 40){ //Turn off Iridium Modem
				SendStringPC("Turning off Iridium Modem\n\r");
				IRIDIUM_SLEEP();
			}
			if(receivedUSARTData == 41){ //Turn on Iridium Modem
				SendStringPC("Turning on Iridium Modem\n\r");
				IRIDIUM_WAKE();
			}
			
			//Xbee controls
			if(receivedUSARTData == 50){ //Turn off XBee
				SendStringPC("Turning off Xbee\n\r");
				XBEE_SLEEP();
			}
			if(receivedUSARTData == 51){ //Turn on XBee
				SendStringPC("Turning on XBee\n\r");
				XBEE_WAKE();
			}
				
			
		}
		
		if (CHECK_TX_SW()) {
			//XBEE_WAKE();
			IRIDIUM_WAKE();
		}
		else{
			//XBEE_SLEEP();
			IRIDIUM_SLEEP();
		}
		
		if(CHECK_LED_SW()){
			GREEN_ON();
		}
		else{
			GREEN_OFF();	
		}
    }
}

//Processes the RSSI Signal from an XTend, defines can be found in main.h
//Function should timeout if no signal is detected.
void processRSSI(){
	uint16_t timeoutCounter = 0;
	uint16_t timeoutThreshold = RSSI_READ_TIMEOUT / RSSI_DEBOUNCE_DELAY + 1;
	RSSI.timeoutOccured = 0;
	
	ERROR_CLR();
	
	//Get RSSI from XTend
	RSSI.measuring = 0;
	do{   //Wait until we have a "Low" signal on the RSSI (wait for this ----\_____)
		_delay_us(RSSI_DEBOUNCE_DELAY);
		if(timeoutCounter++ > timeoutThreshold){
			ERROR_SET();
			break;
		}
	}while(READ_RSSI_PIN());
	
	timeoutCounter = 0;
	do{  //Wait until we have a "High" signal on the RSSI (wait for this ____/----)
		_delay_us(RSSI_DEBOUNCE_DELAY);
		if(timeoutCounter++ > timeoutThreshold){  //if the pulse is always low, this will timeout
			ERROR_SET();
			RSSI.timeoutOccured = 1;
			break;
		}
	}while(!READ_RSSI_PIN());
	
	RTC.CNT = 0;  //Start counting
	
	timeoutCounter = 0;
	do{   //Wait until we have a "Low" signal on the RSSI (wait for this ----\_____)
		_delay_us(RSSI_DEBOUNCE_DELAY);
		if(timeoutCounter++ > timeoutThreshold){
			STATUS_SET();
			RSSI.timeoutOccured = 1;
			break;
		}
	}while(READ_RSSI_PIN());
	
	RSSI.countDifference = RTC.CNT;
	
	//if(!RSSI.timeoutOccured){
	RSSI.value = RSSI.countDifference / 3.4;
	//}
	
	
	RSSI.sampleCount++;
}