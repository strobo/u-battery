#ifndef _BQ29312A_H
#define _BQ29312A_H

#include <stdint.h>

#define BQ29312A_ADDR		0b01000000

#define VMEN 				0b00000001
#define PACKOUT				0b00000010

#define VC4_5				0b00000000
#define VC4_3				0b00000001
#define VC3_2				0b00000010
#define VC1_2				0b00000011
#define CELL_TRANSLATION	0b00000000
#define OFFSET_MEASUR		0b00000100
#define MONITOR_VRE			0b00001000
#define MONITOR_VREF_DIRECT	0b00001100

typedef enum BQ29312A_REG {
	STATUS,
	OUTPUT_CTL,
	STATE_CTL,
	FUNCTION_CTL,
	CELL_SEL,
	OLV,
	OLT,
	SCC,
	SCD
} BQ29312A_REG_t;

typedef enum FET_CH {
	FET_DSG,
	FET_CHG
} FET_CH_t;

typedef struct {
	uint8_t dsg;			/* status of DSG FET */
	uint8_t chg;			/* status of CHG FET */
} FET_STATUS;

typedef struct {
	uint8_t sleep;			/*  */
	uint8_t ship;
} BQ29312_STATUS;

void writeBQ29312A(uint8_t reg, uint8_t data);
uint8_t readBQ29312A(uint8_t reg);
void sleepBq29312a(void);
void wakeBq29312a(void);
uint8_t getMonitorStatus(void);
void setChgFet(uint8_t state);
void setDsgFet(uint8_t state);
void setFet(FET_CH_t ch, uint8_t stat);
uint8_t getFetState(FET_CH_t ch);
void clearWdtFlag(void);
void initBQ29312A(void);
#endif
