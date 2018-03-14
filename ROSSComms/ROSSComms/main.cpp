/*
 * ROSSComms.cpp
 *
 * Created: 3/12/2018 6:57:23 PM
 * Author : Nick McComb | www.nickmccomb.net
 */ 

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

	//configure32MhzInternalOsc();
	configureIO();	
	configureExternalOscillator();
	//
	configureUSART();


//	LOW_LEVEL_INTERRUPTS_ENABLE();
//	sei();
	
	//Setup radios
	XBEE_SLEEP();
	IRIDIUM_SLEEP();
	
	//PORTD.OUTSET = PIN5_bm; //RGB LED Test
	
	STATUS_SET();
	
	SendStringPC("Hello, world :)\n\r");
    while (1) 
    {
		if(broadcastStatus){
			broadcastStatus = 0;
		}
		STATUS_TOGGLE();
		_delay_ms(25);
		
		SendStringPC("Potatoes are the coolest!\n\r");
		
		if (CHECK_TX_SW()) {
			ERROR_SET();
			//XBEE_WAKE();
			IRIDIUM_WAKE();
		}
		else{
			ERROR_CLR();
			//XBEE_SLEEP();
			IRIDIUM_SLEEP();
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