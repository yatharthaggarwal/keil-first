#include "myDelay.h"

volatile uint32_t _millis = 0;
volatile uint32_t _micros = 0;
volatile uint32_t _count = 0;
volatile uint32_t last_count = 0;


void SysTickIntHandler(void)
{
		_millis++;
}

void delay(int del)
{
	last_count = _millis;
	while((_millis - last_count) < del)
	{
	}
}

uint32_t millis(void)
{
	return _millis;
}

uint32_t micros(void)
{
	
	return _micros;
}

void initSysTck(void)
{
	SysTickPeriodSet(SysCtlClockGet()/1000-1);
	SysTickIntRegister(SysTickIntHandler);
	IntMasterEnable();
  SysTickIntEnable();
  SysTickEnable();
}
