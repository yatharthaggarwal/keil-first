#ifndef _DELAY_
#define _DELAY_

#include <stdbool.h>
#include <stdint.h>
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "inc/hw_nvic.h"

void SysTickIntHandler(void);
void delay(int del);
void initSysTck(void);

extern volatile uint32_t _millis;
extern volatile uint32_t _micros;

uint32_t millis(void);
uint32_t micros(void);
#endif
