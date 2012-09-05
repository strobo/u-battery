#ifndef _TIMER32_H
#define _TIMER32_H

typedef struct{
	void (*handler)(void);
} TIMER32_INTERVAL;

void setTimer32Interval(uint8_t timerNum, uint32_t time, void (*handler));
void initTimer32PWM(uint8_t timerNum, uint32_t period);

#endif
