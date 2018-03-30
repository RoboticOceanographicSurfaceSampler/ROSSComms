/*
 * config.cpp
 *
 * Contains Microcontroller configuration functions 
 * Created: 5/18/2016 2:15:09 PM
 *  Author: Nick McComb
 */ 

#include "config.h"

/* Global Variables */
uint8_t DeviceSignature[11];
uint64_t UC_LOT_NUMBER;		//Contains a number genrated by the lot number
uint64_t UC_WAFER_ID;		//Contains a number generated by the wafer position
int16_t serialNumber;		//Creates an OMG Custom Serial number

void configureIO(){
	
	//Set the RSSI Pin as an Input
	PORTA.DIRCLR = PIN2_bm;
	
	//Set the NETWORK and PG inputs from the Iridium Modem as inputs
	PORTC.DIRCLR = PIN4_bm;
	PORTC.DIRCLR = PIN5_bm;
	
	//Set the TX ON/OFF switch as an input
	PORTA.DIRCLR = PIN4_bm;
	
	//Set the Iridium ON/OFF output
	PORTA.DIRSET = PIN5_bm;
	
	//Set the XBee S\L\E\E\P\_RQ connection (HIGH to sleep, LOW to power XBee ON)
	PORTA.DIRSET = PIN3_bm;
	
	//Set the RGB LED Outputs
	PORTC.DIRSET = PIN0_bm;  //red
	PORTD.DIRSET = PIN5_bm;  //green
	PORTD.DIRSET = PIN4_bm;  //blue
	
	//Set the status/error outputs
	PORTC.DIRSET = PIN2_bm;
	PORTC.DIRSET = PIN3_bm;
	
	//Setup the XTend Interface
	PORTD.DIRCLR = PIN6_bm;  //Setup the RX as an input
	//PORTD.DIRSET = PIN7_bm;  //Setup the TX as an output
}


void configureExternalOscillator(){
	int temp = 0;																			//Temporary variable for helping avoid 4 clock cycle limitation when updating secure registers

	//Enable external 4MHz oscillator
	OSC.XOSCCTRL = (OSC_FRQRANGE_2TO9_gc | OSC_XOSCSEL_XTAL_16KCLK_gc);						//Set external oscillator to be between 2 and 9 MHz and select it
	OSC.CTRL |= OSC_XOSCEN_bm;																//Enable the external oscillator
	while(!(OSC.STATUS & OSC_XOSCRDY_bm)){ERROR_SET();};									//While the external oscillator is not ready, set the error led
	ERROR_CLR();																			//Clear the error led if the external oscillator has stabilized

	//Enable phase locked loop to multiply external oscillator by 8 to get 32MHz
	temp = ((OSC_PLLSRC_XOSC_gc & OSC_PLLSRC_gm) | (OSC_PLLFAC_gm & 8));					//Set the external oscillator as the clock source for the pll and set to multiply by 8
	CCP = CCP_IOREG_gc;																		//Disable register security so we can update the pll control settings
	OSC.PLLCTRL = temp;																		//Write pll control settings to register
	OSC.CTRL |= OSC_PLLEN_bm;																//Enable the pll
	while(!(OSC.STATUS & OSC_PLLRDY_bm)){ERROR_SET();};										//While the pll is not ready, set the error led
	ERROR_CLR();																			//Disable the error led if successfully stabilized

	//Set system pll clock divisions and set up as source for all system clocks
	temp = ((CLK_PSADIV_gm & CLK_PSADIV_1_gc) | (CLK_PSBCDIV_gm & CLK_PSBCDIV_1_1_gc));		//Set system to use pll divided by 1 (no division)
	CCP = CCP_IOREG_gc;																		//Disable register security so we can update the clock source division setting
	CLK.CTRL = temp;																		//Write division settings to register

	temp = CLK_SCLKSEL_PLL_gc;																//Set pll as system clock source
	CCP = CCP_IOREG_gc;																		//Disable register security so we can update the system clock
	CLK.CTRL = temp;																		//Write clock source settings to register
	
}

void configure32MhzInternalOsc(){
	OSC_CTRL |= OSC_RC32MEN_bm; //Setup 32Mhz crystal
	
	while(!(OSC_STATUS & OSC_RC32MRDY_bm));
	
	CCP = CCP_IOREG_gc; //Trigger protection mechanism
	CLK_CTRL = CLK_SCLKSEL_RC32M_gc; //Enable internal  32Mhz crystal
}

void configureUSART(void){
	//Set TX (pin7) to be output
	PORTC.DIRSET = PIN7_bm;
	//Set RX (pin6) to be input
	PORTC.DIRCLR = PIN6_bm;
	
	//Enable alternate pin location for USART0 in PORTC
	PORTC.REMAP |= (1 << 4);
	
	USART_Format_Set(&COMP_USART, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, false);
	
	//Enable a 57600 baudrate
	USART_Baudrate_Set(&COMP_USART, 34, 0);
	
	// Enable both RX and TX.
	USART_Rx_Enable(&COMP_USART);
	USART_Tx_Enable(&COMP_USART);
}

void configureTimerCounter(){
	//Set the timer to run (with a prescaler)
	TCC4.CTRLA = TC_TC4_CLKSEL_DIV1024_gc;	//Configure a 1024 prescaler (we want very broad timing here, exact precision isn't required)
	TCC4.PER = TC_1024_500MS;               //500mS delay
	//Default delay value. Reference pre-calculated table up above for more information
	
	TCC4.CTRLB = TC_TC4_WGMODE_NORMAL_gc;   //Configure the timer for Normal mode operation
	
	TCC4.INTCTRLA = TC_TC4_OVFINTLVL_LO_gc;	//Set a low priority overflow interrupt
}

//Handles compare vector for T/C 4
ISR (TCC4_OVF_vect){
	TCC4.INTFLAGS |= 0b1;  //Reset overflow interrupt
	
	broadcastStatus = 1;
}

void configureRTC(){
	RTC.CTRL = RTC_CORREN_bm | RTC_PRESCALER_DIV1_gc;		//Enable the RTC correction process, and the RTC itself with no prescaler
	RTC.INTCTRL = RTC_COMPINTLVL_LO_gc | RTC_OVFINTLVL_LO_gc; //Enable the overflow and
	
	OSC.CTRL |= OSC_RC32KEN_bm;								//Enable the 32.768kHz internal oscillator
	
	_delay_us(400);											//Wait for the oscillator to stabalize.
	
	CLK.RTCCTRL = CLK_RTCSRC_RCOSC32_gc;					//Set the RTC input as the 32.768kHz internal oscillator
	CLK.RTCCTRL |= CLK_RTCEN_bm;							//Enable the clock input
	
	//Testing setup code
	RTC.COMP = 16384; //~1 second? Assuming 32.768 KHz
	RTC.PER = 0xFF00;  //No tengo nuguien idea

}

ISR(RTC_OVF_vect){
	
}

ISR(RTC_COMP_vect){
	if(globalToggle){
		STATUS_CLR();
		globalToggle = 0;
	}
	else{
		STATUS_SET();
		globalToggle = 1;
	}
	
	RTC.CNT = 0;
	RTC.INTFLAGS = 0x02;

}



/* Read NVM signature. From http://www.avrfreaks.net/forum/xmega-production-signature-row */
uint8_t ReadCalibrationByte( uint8_t index ){
	uint8_t result;

	/* Load the NVM Command register to read the calibration row. */
	NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
	result = pgm_read_byte(index);

	/* Clean up NVM Command register. */
	NVM_CMD = NVM_CMD_NO_OPERATION_gc;

	return( result );
}

uint8_t ReadSignatureByte(uint16_t Address)
{
	NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
	uint8_t Result;
	__asm__ ("lpm %0, Z\n" : "=r" (Result) : "z" (Address));
	//  __asm__ ("lpm \n  mov %0, r0 \n" : "=r" (Result) : "z" (Address) : "r0");
	NVM_CMD = NVM_CMD_NO_OPERATION_gc;
	return Result;
}

/*
Note: This function requires serial and should be called after configureUSART()
*/
void configureSerialNumber(){
	DeviceSignature[ 0] = ReadSignatureByte(LOTNUM0);
	DeviceSignature[ 1] = ReadSignatureByte(LOTNUM1);
	DeviceSignature[ 2] = ReadSignatureByte(LOTNUM2);
	DeviceSignature[ 3] = ReadSignatureByte(LOTNUM3);
	DeviceSignature[ 4] = ReadSignatureByte(LOTNUM4);
	DeviceSignature[ 5] = ReadSignatureByte(LOTNUM5);
	DeviceSignature[ 6] = ReadSignatureByte(WAFNUM );
	DeviceSignature[ 7] = ReadSignatureByte(COORDX0);
	DeviceSignature[ 8] = ReadSignatureByte(COORDX1);
	DeviceSignature[ 9] = ReadSignatureByte(COORDY0);
	DeviceSignature[10] = ReadSignatureByte(COORDY1);
	
	UC_LOT_NUMBER = DeviceSignature[0];
	
	for(int i = 1; i < 6; ++i){
		UC_LOT_NUMBER += DeviceSignature[i] * (100*i);//(DeviceSignature[i] << i*8);
	}
	
	UC_WAFER_ID = DeviceSignature[6];
	
	for(int i = 1; i < 4; ++i){  //Original stops at 5
		UC_WAFER_ID += DeviceSignature[i+6] * (100*i);
	}
	
	serialNumber = -1; //Set the default serial number to -1 ("Not assigned yet")
	
	//Hardcoded serial number lookups (don't judge)
	if(UC_LOT_NUMBER == 17117 && UC_WAFER_ID == 9715){
		serialNumber = 1;
		SendStringPC("\n\rIdentified board.");
	}
	
	#ifdef OUTPUT_LOT_AND_WAFER_INFO
		SendStringPC((char *)"#[Lot ID Number: ");
		SendNumPC(UC_LOT_NUMBER);
		SendStringPC((char *)"]");
		
		SendStringPC((char *)"[Wafer ID Number: ");
		SendNumPC(UC_WAFER_ID);
		SendStringPC((char *)"]");
	#endif
}