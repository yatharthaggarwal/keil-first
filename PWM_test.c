#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pwm.h"

volatile uint32_t PWM_BASE = PWM1_BASE;

int main()
{
	//
	// Enable the PWM0 peripheral
	//
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	
	GPIOPinConfigure(GPIO_PF2_M1PWM6);
	GPIOPinConfigure(GPIO_PF3_M1PWM7);
	
	GPIOPinTypePWM(GPIO_PORTF_BASE,GPIO_PIN_2);
	GPIOPinTypePWM(GPIO_PORTF_BASE,GPIO_PIN_3);
	
	PWMGenConfigure(PWM_BASE, PWM_GEN_3,	PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
	//
	// Set the period. For a 50 KHz frequency, the period = 1/50,000, or 20
	// microseconds. For a 20 MHz clock, this translates to 400 clock ticks.
	// Use this value to set the period.
	//
	PWMGenPeriodSet(PWM_BASE, PWM_GEN_3, 400);

	PWMPulseWidthSet(PWM_BASE, PWM_OUT_6, 10);
//	PWMPulseWidthSet(PWM_BASE, PWM_OUT_7, 200);

	//
	PWMGenEnable(PWM_BASE, PWM_GEN_3);
	//
	// Enable the outputs.
	//
	PWMOutputState(PWM_BASE, (PWM_OUT_6_BIT | PWM_OUT_7_BIT), true);
}

