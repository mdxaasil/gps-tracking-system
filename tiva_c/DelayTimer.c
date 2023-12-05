#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_ints.h"
#include "inc/hw_timer.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "inc/hw_nvic.h"

static unsigned long milliseconds = 0;

void timerInit(void)
{

	ROM_SysTickPeriodSet(0x00FFFFFF);
    ROM_SysTickEnable();

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);
    ROM_TimerConfigure(TIMER5_BASE, TIMER_CFG_PERIODIC_UP);

    ROM_TimerLoadSet(TIMER5_BASE, TIMER_A, ROM_SysCtlClockGet()/1000);

    ROM_IntEnable(INT_TIMER5A);
    ROM_TimerIntEnable(TIMER5_BASE, TIMER_TIMA_TIMEOUT);

    ROM_TimerEnable(TIMER5_BASE, TIMER_A);

    ROM_IntMasterEnable();

}

unsigned long micros(void)
{
	return (milliseconds * 1000) + (HWREG(TIMER5_BASE + TIMER_O_TAV) / 80);
}

unsigned long millis(void)
{
	return milliseconds;
}

void delayMicroseconds(unsigned int us)
{
	volatile unsigned long elapsedTime;
	unsigned long startTime = HWREG(NVIC_ST_CURRENT);
	do{
		elapsedTime = startTime-(HWREG(NVIC_ST_CURRENT) & 0x00FFFFFF);
	}
	while(elapsedTime <= us*80);
}

void delay(uint32_t milliseconds)
{
		unsigned long i;
		for(i=0; i<milliseconds; i++){
			delayMicroseconds(1000);
		}
}

void Timer5IntHandler(void)
{
    ROM_TimerIntClear(TIMER5_BASE, TIMER_TIMA_TIMEOUT);

	milliseconds++;
}


