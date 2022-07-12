/*
 *	File name: maic.c
 *  Created on: Oct 15, 2021
 *  Author: osama
 */
#include "lcd.h"
#include "ultrasonic_sensor.h"
#include <avr/io.h>


int main(void){
	SREG |= (1<<SREG_I); /* enable I-BIT */
	LCD_init();
	LCD_displayString("Distance in cm");
	LCD_displayStringRowColumn(1, 0, "=");
	ultrasonic_init();

	while(1){
		LCD_moveCursor(1, 1);
		LCD_intgerToString(ultrasonic_readDistance());

		/* To overwrite the 3rd digit [in case of going down from 100 to 99]
		 * and also solved a display problem in Proteus on my machine :( */
		LCD_displayString("      ");
	}
}
