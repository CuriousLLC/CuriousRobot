/*
 *  Interrupt and PWM utilities for 16 bit Timer1 on ATmega168/328
 *  Original code by Jesse Tane for http://labs.ideo.com August 2008
 *  Modified March 2009 by Jérôme Despatis and Jesse Tane for ATmega328 support
 *  Modified June 2009 by Michael Polli and Jesse Tane to fix a bug in setPeriod() which caused the timer to stop
 *  Modified April 2012 by Paul Stoffregen - portable to other AVR chips, use inline functions
 *  Modified again, June 2014 by Paul Stoffregen - support Teensy 3.x & even more AVR chips
 *  
 *
 *  This is free software. You can redistribute it and/or modify it under
 *  the terms of Creative Commons Attribution 3.0 United States License. 
 *  To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/us/ 
 *  or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 */

#ifndef TimerOne_h_
#define TimerOne_h_

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "config/known_16bit_timers.h"

#define TIMER1_RESOLUTION 65536UL  // Timer1 is 16 bit


// Placing nearly all the code in this .h file allows the functions to be
// inlined by the compiler.  In the very common case with constant values
// the compiler will perform all calculations and simply write constants
// to the hardware registers (for example, setPeriod).


class TimerOne
{
#define F_TIMER (F_PLL/2)

  public:
    //****************************
    //  Configuration
    //****************************
    void initialize(unsigned long microseconds=1000000) __attribute__((always_inline)) {
	setPeriod(microseconds);
    }
    void setPeriod(unsigned long microseconds) __attribute__((always_inline)) {
	const unsigned long cycles = (F_TIMER / 2000000) * microseconds;
	if (cycles < TIMER1_RESOLUTION) {
		clockSelectBits = 0;
		pwmPeriod = cycles;
	} else
	if (cycles < TIMER1_RESOLUTION * 2) {
		clockSelectBits = 1;
		pwmPeriod = cycles >> 1;
	} else
	if (cycles < TIMER1_RESOLUTION * 4) {
		clockSelectBits = 2;
		pwmPeriod = cycles >> 2;
	} else
	if (cycles < TIMER1_RESOLUTION * 8) {
		clockSelectBits = 3;
		pwmPeriod = cycles >> 3;
	} else
	if (cycles < TIMER1_RESOLUTION * 16) {
		clockSelectBits = 4;
		pwmPeriod = cycles >> 4;
	} else
	if (cycles < TIMER1_RESOLUTION * 32) {
		clockSelectBits = 5;
		pwmPeriod = cycles >> 5;
	} else
	if (cycles < TIMER1_RESOLUTION * 64) {
		clockSelectBits = 6;
		pwmPeriod = cycles >> 6;
	} else
	if (cycles < TIMER1_RESOLUTION * 128) {
		clockSelectBits = 7;
		pwmPeriod = cycles >> 7;
	} else {
		clockSelectBits = 7;
		pwmPeriod = TIMER1_RESOLUTION - 1;
	}
	uint32_t sc = FTM1_SC;
	FTM1_SC = 0;
	FTM1_MOD = pwmPeriod;
	FTM1_SC = FTM_SC_CLKS(1) | FTM_SC_CPWMS | clockSelectBits | (sc & FTM_SC_TOIE);
    }

    //****************************
    //  Run Control
    //****************************
    void start() __attribute__((always_inline)) {
	stop();
	FTM1_CNT = 0;
	resume();
    }
    void stop() __attribute__((always_inline)) {
	FTM1_SC = FTM1_SC & (FTM_SC_TOIE | FTM_SC_CPWMS | FTM_SC_PS(7));
    }
    void restart() __attribute__((always_inline)) {
	start();
    }
    void resume() __attribute__((always_inline)) {
	FTM1_SC = (FTM1_SC & (FTM_SC_TOIE | FTM_SC_PS(7))) | FTM_SC_CPWMS | FTM_SC_CLKS(1);
    }

    //****************************
    //  PWM outputs
    //****************************
    void setPwmDuty(char pin, unsigned int duty) __attribute__((always_inline)) {
	unsigned long dutyCycle = pwmPeriod;
	dutyCycle *= duty;
	dutyCycle >>= 10;
	if (pin == TIMER1_A_PIN) {
		FTM1_C0V = dutyCycle;
	} else if (pin == TIMER1_B_PIN) {
		FTM1_C1V = dutyCycle;
	}
    }
    void pwm(char pin, unsigned int duty) __attribute__((always_inline)) {
	setPwmDuty(pin, duty);
	if (pin == TIMER1_A_PIN) {
		*portConfigRegister(TIMER1_A_PIN) = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;
	} else if (pin == TIMER1_B_PIN) {
		*portConfigRegister(TIMER1_B_PIN) = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;
	}
    }
    void pwm(char pin, unsigned int duty, unsigned long microseconds) __attribute__((always_inline)) {
	if (microseconds > 0) setPeriod(microseconds);
	pwm(pin, duty);
    }
    void disablePwm(char pin) __attribute__((always_inline)) {
	if (pin == TIMER1_A_PIN) {
		*portConfigRegister(TIMER1_A_PIN) = 0;
	} else if (pin == TIMER1_B_PIN) {
		*portConfigRegister(TIMER1_B_PIN) = 0;
	}
    }

    //****************************
    //  Interrupt Function
    //****************************
    void attachInterrupt(void (*isr)()) __attribute__((always_inline)) {
	isrCallback = isr;
	FTM1_SC |= FTM_SC_TOIE;
	NVIC_ENABLE_IRQ(IRQ_FTM1);
    }
    void attachInterrupt(void (*isr)(), unsigned long microseconds) __attribute__((always_inline)) {
	if(microseconds > 0) setPeriod(microseconds);
	attachInterrupt(isr);
    }
    void detachInterrupt() __attribute__((always_inline)) {
	FTM1_SC &= ~FTM_SC_TOIE;
	NVIC_DISABLE_IRQ(IRQ_FTM1);
    }
    static void (*isrCallback)();

  private:
    // properties
    static unsigned short pwmPeriod;
    static unsigned char clockSelectBits;

#undef F_TIMER
};

extern TimerOne Timer1;

#endif

