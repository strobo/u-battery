#include "lpc111x.h"
#include "LPC1100.h"
#include "timer32.h"

static volatile TIMER32_INTERVAL tmr32int;

void CT32B1_IRQHandler(void)
{
	if(TMR32B1IR & 0x01){
		TMR32B1IR = _BV(0);
		tmr32int.handler();
	}
}

/* setTimerInterval                             */
/* call specifiled function every timer32 match */
/* timerNum : timer32 channnel                  */
/* time     : call interval (ms)                */
/* handler  : function that will be called      */
/* data     : argument of handler function      */
void setTimer32Interval(uint8_t timerNum, uint32_t time, void (*handler))
{
	if(timerNum == 1){
		__set_SYSAHBCLKCTRL(PCCT32B1, 1);

		tmr32int.handler = handler;

		TMR32B1MR0 = time * 12000;

		TMR32B1MCR = 0x03; /* Enable MR0_RESET and MR0_INTERRUPT */

		TMR32B1TCR |= _BV(0); /* Enable timer32-1 */

	}
}

void initTimer32PWM(uint8_t timerNum, uint32_t period)
{
	//timer32Disable(timerNum);
	if(timerNum == 0){
		//SCB_SYSAHBCLKCTRL |= (SCB_SYSAHBCLKCTRL_CT32B0);
		__set_SYSAHBCLKCTRL(PCCT32B0, 1);

		TMR_TMR32B0EMR = TMR_TMR32B0EMR_EMC2_TOGGLE | TMR_TMR32B0EMR_EMC0_TOGGLE;
		
		IOCON_PIO0_1 &= ~IOCON_PIO0_1_FUNC_MASK;
		IOCON_PIO0_1 |= IOCON_PIO0_1_FUNC_CT32B0_MAT2;
		
		TMR_TMR32B0PWMC = TMR_TMR32B0PWMC_PWM2_ENABLED;
		
		TMR_TMR32B0MR0 = period;
		TMR_TMR32B0MR2 = period/2;
		
		TMR_TMR32B0MCR = TMR_TMR32B0MCR_MR0_RESET_ENABLED;
		
		TMR_TMR32B0PR = 2;

		TMR_TMR32B0TCR = TMR_TMR32B0TCR_COUNTERENABLE_ENABLED; /* Enable Timer32-0 */
	}
}
