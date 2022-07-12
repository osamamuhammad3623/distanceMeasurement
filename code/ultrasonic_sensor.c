/*
 *	File name: ultrasonic_sensor.c
 *  Created on: Oct 11, 2021
 *  Author: osama
 */

#include "ultrasonic_sensor.h"
#include "gpio.h"
#include "icu.h"
#include <avr/delay.h>
#include "common_macros.h"

static uint8 g_edgeCounter = 0;
static uint32 highTime=0; /* number of clock cycles when Echo pin is high */

/* Functions implementation */
void ultrasonic_init(void){
	GPIO_setupPinDirection(ULTRASONIC_TRIGGER_PORT, TRIG_PIN, PIN_OUTPUT);
	GPIO_setupPinDirection(ULTRASONIC_ECHO_PORT, ECHO_PIN, PIN_INPUT);
	GPIO_writePin(ULTRASONIC_TRIGGER_PORT, TRIG_PIN, LOGIC_LOW);

	/* initialize ICU */
	Icu_ConfigType Icu_config = {F_CPU_8, RISING};
	Icu_init(&Icu_config);
	Icu_setCallBack(ultrasonic_edgeProcessing);
}

void ultrasonic_Trigger(void){
	/* send a pulse for 10us to Trigger pin */
	GPIO_writePin(ULTRASONIC_TRIGGER_PORT, TRIG_PIN, LOGIC_HIGH);
	_delay_us(TRIGGER_PULSE_DURATION);
	GPIO_writePin(ULTRASONIC_TRIGGER_PORT, TRIG_PIN, LOGIC_LOW);

	/* make echo pin high */
	GPIO_writePin(ULTRASONIC_ECHO_PORT, ECHO_PIN, LOGIC_HIGH);
}

uint16 ultrasonic_readDistance(void){
	/* send the pulse to the sensor and start counting high-time */
	ultrasonic_Trigger();
	Icu_clearTimerValue();

	/*
	 * sound speed = 340 m/s = 34000 cm/s
	 * total distance = speed * echo time
	 * distance = total distance /2 = 34000/2 * echo time
	 * 			= 17000 * echo time
	 *
	 * 	since using 8MHz freq & F_CPU/8 prescalar,
	 *  then, 1 count every 1us
	 *	then, echo time = 17000 * 10^-6 = 0.017
	 *
	 * */

	/* note: subtracting 2 from result to solve the fraction-error */
	return ((highTime *0.017)-2);
}

void ultrasonic_edgeProcessing(void){
	g_edgeCounter++;
	if (g_edgeCounter == 1){
		/* change edge detection to Falling edge at the Echo pin */
		Icu_setEdgeDetectionType(FALLING);

	}else if (g_edgeCounter ==2){
		/* get the high-time from ICU */
		highTime = Icu_getInputCaptureValue();
		Icu_setEdgeDetectionType(RISING);
		g_edgeCounter=0;
	}
}
