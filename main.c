/*------------------------------------------------------------------------/
/  Mobile USB Battery Unit test program                                   /
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2012, strobo, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------*/

#include "LPC1100.h"
#include "lpc111x.h"
#include "adc.h"
//#include "gpio.h"
#include "integer.h"
#include "timer32.h"
#include "BQ29312A.h"

#define F_CPU	12000000




typedef struct {
	uint8_t checkBattery;
	float adcf[4];			/* Command bytes follows SLA+W */
	float vcellf[4];
	double kact;
	double vosact;
	float voutr;
	float vref;
	float res;
	float vo4_5;
	uint8_t batlow;
} BATTERY_STATUS;

typedef struct {
	uint8_t stat;			/* DCin Power status  0: DISCONNECT 1:CONNECT */
	uint8_t count;			/* counter to determinate stat */
} DC_STATUS;


static volatile BATTERY_STATUS battery;
static volatile DC_STATUS dc;
static volatile uint8_t switch_flag;


volatile uint32_t systickTicks = 0; /* 1kHz? increment timer */




/*--------------------------------------------------------------*/
/* 1000Hz? interval timer                                       */
/*--------------------------------------------------------------*/

void SysTick_Handler (void)
{
	systickTicks++;
}

static
void setChgPin(uint8_t state)
{
	if(state == 0)
	{
		GPIO3DATA &= ~(_BV(1));
		//gpioSetValue(3,1,0);
	}
	else if(state == 1)
	{
		GPIO3DATA |= _BV(1);
		//gpioSetValue(3,1,1);
	}
}
void setLowChgPin(uint8_t state)
{
	if(state == 0)
	{
		GPIO3DATA &= ~(_BV(2));
		//gpioSetValue(3,2,0);
	}
	else if(state == 1)
	{
		GPIO3DATA |= _BV(2);
		//gpioSetValue(3,2,1);
	}
}

/* -------------------------------------------- */
/* portNum: port number (0..3)                  */
/* bitPos:  bit position (0..11)                */
/* dir:     pin direction (0:Input 1:Output)    */
/* -------------------------------------------- */
static
void gpioSetDir(uint8_t portNum, uint8_t bitPos, uint8_t dir)
{
	volatile uint32_t *gpiodir;
	switch(portNum)
	{
		case 0:
			gpiodir = &GPIO0DIR;
			break;
		case 1:
			gpiodir = &GPIO1DIR;
			break;
		case 2:
			gpiodir = &GPIO2DIR;
			break;
		case 3:
			gpiodir = &GPIO3DIR;
			break;
	}
	switch(dir)
	{
		case 0:
			*gpiodir &= ~(_BV(bitPos));
			break;
		case 1:
			*gpiodir |= _BV(bitPos);
			break;
	}
}
static
void setDCDC(uint8_t state)
{
	if(state == 0){
		GPIO3DATA &= ~(_BV(0));
	}else if(state == 1){
		GPIO3DATA |= _BV(0);
	}
}
static
void systickDelay (uint32_t delayTicks) 
{
  uint32_t curTicks;
  curTicks = systickTicks;

  // Make sure delay is at least 1 tick in case of division, etc.
  if (delayTicks == 0) delayTicks = 1;

  if (curTicks > 0xFFFFFFFF - delayTicks)
  {
    // Rollover will occur during delay
    while (systickTicks >= curTicks)
    {
      while (systickTicks < (delayTicks - (0xFFFFFFFF - curTicks)));
    }
  }
  else
  {
    while ((systickTicks - curTicks) < delayTicks);
  }
}

/*---------------------------------------------*/
/* Show Battery Level interrupt                */

void PIO_1_IRQHandler (void)
{
	GPIO1IC = _BV(11);	/* Clear P1.11 irq */
	switch_flag = 1;
}

static
void showBatteryLevel(void)
{
	uint8_t i,batLevel;
	float vbat;

	vbat = battery.vcellf[0] + battery.vcellf[1] + battery.vcellf[2];
	if(vbat <= 10.0){
		batLevel = 0;
	}else{
		batLevel = (uint8_t)((vbat - 9) * 10/3.6);
	}
	/* Show battery level like MacBookPro's Battery level gauge */
	GPIO2DATA = 0xffffff; /* all leds off */
	if( batLevel == 0){
		for(i = 0; i <= 7; i++)
		{
			GPIO2DATA ^= _BV(0);
			systickDelay(100);
		}
	}else{
		for(i = 0; i < batLevel; i++)
		{
			GPIO2DATA &= ~(_BV(i));
			systickDelay(200);
		}
		systickDelay(1500);
		GPIO2DATA = 0xffffff; /* all leds off */
	}
	switch_flag = 0;
}

/* Read Battery voltage. Check DCin status */
/* Controll low voltage alert or low voltage shutdown */
void setBatteryFlag(void)
{
	battery.checkBattery = 1;
}
static
void checkBatteryStatus(void){
	GPIO2DATA ^= _BV(0); /* led0 on */
	writeBQ29312A(FUNCTION_CTL, VMEN); /* enable voltage monitoring */
	/* -------------------------------------------- */
	//writeBQ29312A(CELL_SEL, 0b00001100);
	//systickDelay(1);
	//battery.vref = (float)((float)adcRead(1) * (float)(3.3/1024));
	//writeBQ29312A(CELL_SEL, VC4_5);
	//systickDelay(1);
	//battery.vo4_5 = (float)((float)adcRead(1) * (float)(3.3/1024));
	//writeBQ29312A(CELL_SEL, 0b00001000);
	//systickDelay(1);
	//battery.voutr = (float)((float)adcRead(1) * (float)(3.3/1024));
	/* -------------------------------------------- */

	/* Measure Cell Voltage */
	writeBQ29312A(CELL_SEL, VC4_5);
	systickDelay(1);
	battery.adcf[0] = (float)adcRead(1) * (float)(3.3/1024);
	writeBQ29312A(CELL_SEL, VC4_3);
	systickDelay(1);
	battery.adcf[1] = (float)adcRead(1) * (float)(3.3/1024);
	writeBQ29312A(CELL_SEL, VC3_2);
	systickDelay(1);
	battery.adcf[2] = (float)adcRead(1) * (float)(3.3/1024);

	writeBQ29312A(FUNCTION_CTL, VMEN | PACKOUT);
	//uint8_t chgFet = getFetState(FET_CHG);
	//setChgFet(0);
	//setChgPin(0); // chg circuit on
	systickDelay(8);
	battery.adcf[3] = (float)adcRead(1) * (float)(3.3/1024);
	//setChgPin(1); // chg curcuit off
	//setChgFet(chgFet);

	/* Covert Cell Voltage from adc value *but this value is little inaccuracy */
	battery.vcellf[0] = (0.975 - battery.adcf[0]) / 0.15;
	battery.vcellf[1] = (0.975 - battery.adcf[1]) / 0.15;
	battery.vcellf[2] = (0.975 - battery.adcf[2]) / 0.15;
	battery.vcellf[3] = battery.adcf[3] * 25;

	//battery.vcellf[0] = (float)(-0.15 * battery.adcf[0] + 0.975);

	//battery.kact = (battery.vo4_5 - battery.voutr) / battery.vref;
	//battery.vosact = (battery.vo4_5 - battery.vref) / (1 + battery.kact);

	//battery.res = (battery.adcf[0] - battery.vcellf[0]) / battery.kact;
	//float res = (vref + ( 1 + kact ) * vosact - adcf[0] ) / kact - ( vo4_5 - adcf[0] ) / kact;
	//float res = (-adcf[0] + vref ) / kact;


	/* Check DC_in status */
	if(battery.vcellf[3] > 12.6){
		/* DC connected */
		dc.stat = 1;
		GPIO2DATA &= ~(_BV(9)); /* led9 on */
	}else{
		/* DC disconnected */
		dc.stat = 0;
		GPIO2DATA |= _BV(9); /* led9 off */
	}

	/* When DCin is disconnect. CHG FET set to ON for reduce the voltage drop */
	if(dc.stat == 0) setChgFet(1);
	if(dc.stat == 1) setChgFet(0);

	//if(dc.stat == 1){
	//	setDsgFet(0);
	//}

	/* Low voltage alert */
	if(battery.vcellf[0] <= 3.6)
	{
		battery.batlow = 1;
		GPIO2DATA |= _BV(0); // led0 off
	}
	else if(battery.vcellf[1] <= 3.6)
	{
		battery.batlow = 1;
		GPIO2DATA |= _BV(0); // led0 off
	}
	else if(battery.vcellf[2] <= 3.6)
	{
		battery.batlow = 1;
		GPIO2DATA |= _BV(0); // led0 off
	}
	else
	{
		battery.batlow = 0;
	}

	/* Low voltage shutdown */
	if(battery.vcellf[0] <= 3.4)
	{
		setDCDC(0);
		setDsgFet(0);
		/* Todo Enter deep sleep */
	}
	else if(battery.vcellf[1] <= 3.4)
	{
		setDCDC(0);
		setDsgFet(0);
		/* Todo Enter deep sleep */
	}
	else if(battery.vcellf[2] <= 3.4)
	{
		setDCDC(0);
		setDsgFet(0);
		/* Todo Enter deep sleep */
	}
	else
	{
		setDCDC(1);
		setDsgFet(1);
	}

	battery.checkBattery = 0;
}

/*--------------------------------------------------------------*/
/* Initialization and main processing loop                      */
/*--------------------------------------------------------------*/

int main (void)
{
	uint8_t i;

	dc.stat = 0;
	dc.count = 0;
	switch_flag = 0;

	/* misc I/Os Direction regs */
	gpioSetDir(3, 0, 1); // DC-DC Convertor EN
	gpioSetDir(3, 1, 1); // Chg Disable pin
	gpioSetDir(3, 2, 1); // Low Chg pin
	gpioSetDir(1, 11, 0); // Switch

	/* LED Array Direction regs set to output */
	gpioSetDir(2, 0, 1); // led no.0 the most left led.
	gpioSetDir(2, 1, 1); // led no.1
	gpioSetDir(2, 2, 1); // led no.2
	gpioSetDir(2, 3, 1); // led no.3
	gpioSetDir(2, 4, 1); // led no.4
	gpioSetDir(2, 5, 1); // led no.5
	gpioSetDir(2, 6, 1); // led no.6
	gpioSetDir(2, 7, 1); // led no.7
	gpioSetDir(2, 8, 1); // led no.8
	gpioSetDir(2, 9, 1); // led no.9

	setChgPin(1); 		/* charge circuit off */
	setLowChgPin(0); 	/* low charge off */

	/* Enable SysTick timer in interval of 1 ms */
	SYST_RVR = F_CPU / 1000 - 1;
	SYST_CSR = 0x07;

	initBQ29312A(); /* Initialize BQ29312A */

	/* DON'T DSG FET TURN TO OFF WHEN DCIN DOESN'T CONNECT */
	setDsgFet(1); // DSG FET turn to ON
	/* DON'T CHG FET TURN TO ON WHEN CONNECTING BATTERY */
	setChgFet(0); // CHG FET turn to OFF

	GPIO2DATA = 0xffffff; /* all leds off */

	setDCDC(1); /* Start DC5V output */

	/* flash all LEDs */
	for(i = 0; i <= 5; i++)
	{
		GPIO2DATA ^= ( _BV(0) | _BV(1) | _BV(2) | _BV(3) | _BV(4) |
						_BV(5) | _BV(6) | _BV(7) | _BV(8) | _BV(9) );
		systickDelay(100);
	}

	/* Initialize ADC module */
	adcInit();
	
	checkBatteryStatus();
	/* Call setBatteryFlag() 2000ms interval */
	setTimer32Interval(1, 2000, setBatteryFlag);
	__enable_irqn(CT32B1_IRQn);

	/* Enale GPIO1-11 interrupt for battery check switch */
	GPIO1IS &= ~(_BV(11));		/* Interrupt is Edge Sense */
	GPIO1IBE &= ~(_BV(11));		/* Interrupt is Single Triger */
	GPIO1IEV &= ~(_BV(11));		/* Select interrupt source (falling edge of P1.11 pin) */
	GPIO1IE = _BV(11);			/* Unmask interrupt of P1.11 pin */
	__enable_irqn(PIO_1_IRQn);	/* Enable PIO1 interrupt */

	for (;;) {
		if(battery.checkBattery) checkBatteryStatus();
		if(switch_flag) showBatteryLevel();
		__WFI(); /* Wait for interrupt */
	}
}
