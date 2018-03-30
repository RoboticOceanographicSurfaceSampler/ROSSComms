/*
 * usartROSS.h
 *
 * Created: 12/30/2015 1:47:28 PM
 *  Author: Nick McComb [nickmccomb.net]
 */ 


#ifndef USARTROSS_H_
#define USARTROSS_H_

#include "project_header.h"

void SendStringPC(char *stufftosend);
void SendNumPC(uint16_t numToSend);
void SendNumPC(uint64_t numToSend);
void SendFloatPC(double numToSend);

#endif /* USARTROSS_H_ */