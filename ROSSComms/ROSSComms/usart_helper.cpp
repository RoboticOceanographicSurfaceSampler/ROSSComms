/*
 * usartROSS.cpp
 *
 * Created: 12/30/2015 1:48:15 PM
 *  Author: Nick McComb [nickmccomb.net]
 */ 

#include "usart_helper.h"
#include <stdio.h>

//Sends a string to the computer
void SendStringPC(char *stufftosend){
	for(int i = 0 ; stufftosend[i] != '\0' ; i++){
		while(!USART_IsTXDataRegisterEmpty(&COMP_USART));
		USART_PutChar(&COMP_USART, stufftosend[i]);
	}
}

//Can only send 16 bit numbers... TODO: Should this change?
void SendNumPC(uint16_t numToSend){
	char buffer[20];
	itoa(numToSend, buffer, 10);
	SendStringPC(buffer);
}

/*
For this function, we need to split the 64 bit integer into two separate
32 bit integers because %llx and %lld are not implemented in this version
of sprintf/
*/
void SendNumPC(uint64_t numToSend){
	char buffer[50];
	uint32_t tempLSB;
	uint32_t tempMSB;
	
	tempLSB = numToSend & 0xFFFFFFFF;	//Least significant four bytes
	tempMSB = (uint32_t) ((numToSend & 0xFFFFFFFF00000000) >> 32);		//Most significant four bytes
	
	if(tempMSB)
	sprintf(buffer,"%lx%lx", tempMSB, tempLSB);
	else
	sprintf(buffer,"%lx", tempLSB);
	
	SendStringPC(buffer);
}

void SendFloatPC(double numToSend){
	char buffer[100];
	
	int d1 = numToSend;
	float f2 = numToSend - d1;
	int d2 = trunc(f2 * 10000);
	
	sprintf(buffer, "%d.%04d", d1, abs(d2));
	
	SendStringPC(buffer);
} 
