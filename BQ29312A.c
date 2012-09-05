#include <stddef.h>
#include "BQ29312A.h"
#include "iic.h"
#include "timer32.h"

static volatile I2CCTRL I2cCtrl;	/* Control structure for I2C transaction */
static volatile int8_t I2cBuff[4];	/* I2C read/write buffer */
static volatile FET_STATUS fet;

/*-----------------------------------------------*/
/* writeBQ29312A                                 */

void writeBQ29312A (uint8_t reg, uint8_t data)
{
	I2cCtrl.sla = BQ29312A_ADDR >> 1;
	I2cCtrl.retry = 0;
	I2cCtrl.ncmd = 1; I2cCtrl.cmd[0] = reg;
	I2cCtrl.dir = I2C_WRITE;
	I2cCtrl.ndata = 1; I2cCtrl.data = &data;
	I2cCtrl.eotfunc = NULL;
	i2c0_start(&I2cCtrl);
	while (!I2cCtrl.stat) ;
}
/*-----------------------------------------------*/
/* readBQ29312A                                  */

uint8_t readBQ29312A (uint8_t reg)
{
	I2cCtrl.sla = BQ29312A_ADDR >> 1;
	I2cCtrl.retry = 0;
	I2cCtrl.ncmd = 1; I2cCtrl.cmd[0] = reg;
	I2cCtrl.dir = I2C_READ;
	I2cCtrl.ndata = 1; I2cCtrl.data = (uint8_t*)I2cBuff;
	I2cCtrl.eotfunc = NULL;
	i2c0_start(&I2cCtrl);
	while (!I2cCtrl.stat) ;
	return ((uint8_t)I2cBuff[0]);
}
void setChgFet(uint8_t state)
{
	uint8_t reg;

	reg = readBQ29312A(OUTPUT_CTL);
	if(state == 0){
		writeBQ29312A(OUTPUT_CTL, reg & ~(0b00000100));
	}
	else if(state == 1)
	{
		writeBQ29312A(OUTPUT_CTL, reg | 0b00000100);
	}
	fet.chg = state;
}
void setDsgFet(uint8_t state)
{
	uint8_t reg;
	
	reg = readBQ29312A(OUTPUT_CTL);
	if(state == 0){
		writeBQ29312A(OUTPUT_CTL, reg & ~(_BV(1)));
	}
	else if(state == 1)
	{
		writeBQ29312A(OUTPUT_CTL, reg | _BV(1));
	}
	fet.dsg = state;
}
void setFet(FET_CH_t ch, uint8_t stat)
{
	/* Todo replace setChgFet() and setDsgFet() */
}
uint8_t getFetState(FET_CH_t ch)
{
	if(ch == FET_DSG){
		return fet.dsg;
	}
	if(ch == FET_CHG){
		return fet.chg;
	}
}
void clearWdtFlag(void)
{
	writeBQ29312A(OUTPUT_CTL,0x00);
	writeBQ29312A(OUTPUT_CTL,0x01);
	writeBQ29312A(OUTPUT_CTL,0x00);
}
void initBQ29312A(void)
{
	fet.dsg = fet.chg = 0;
	/* Initialize I2C module */
	i2c0_init();

	initTimer32PWM(0, 122); /* Setup regs for 32kHz */
	clearWdtFlag();

	writeBQ29312A(SCD, 0b01000001); /* Set Discharge current limit */
}
