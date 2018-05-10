/*
 * project_header.h
 *
 * Created: 3/30/2018 12:35:05 PM
 *  Author: Nick
 */ 


#ifndef PROJECT_HEADER_H_
#define PROJECT_HEADER_H_


#ifndef F_CPU
#define F_CPU 32000000  //Define clock speed for _delay
//#define F_CPU 2000000
#endif

#define COMP_USART USARTC0

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include "macros.h"
#include "config.h"
#include "usart_helper.h"

#ifdef __cplusplus
extern "C" {
	#endif
	#include "usart_driver.h"
	#ifdef __cplusplus
}
#endif

//The following are debugging options available

//#define OUTPUT_LOT_AND_WAFER_INFO



//Used for generating a serial number
enum {
	LOTNUM0=8,  // Lot Number Byte 0, ASCII
	LOTNUM1,    // Lot Number Byte 1, ASCII
	LOTNUM2,    // Lot Number Byte 2, ASCII
	LOTNUM3,    // Lot Number Byte 3, ASCII
	LOTNUM4,    // Lot Number Byte 4, ASCII
	LOTNUM5,    // Lot Number Byte 5, ASCII
	WAFNUM =16, // Wafer Number
	COORDX0=18, // Wafer Coordinate X Byte 0
	COORDX1,    // Wafer Coordinate X Byte 1
	COORDY0,    // Wafer Coordinate Y Byte 0
	COORDY1,    // Wafer Coordinate Y Byte 1
};



#endif /* PROJECT_HEADER_H_ */