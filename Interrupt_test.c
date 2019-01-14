#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"

long value = 0;

void PortFIntHandler()
{
	if (GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_PIN_4) 
		{	
			GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);
			value ++ ;
			if(value == 400)
			{
				GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, 0x00);
			}
		}
		
} 

int main(void)
{
 	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

  GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_4);
	
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, 0x00);
  //GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	
	GPIOIntDisable(GPIO_PORTF_BASE, GPIO_PIN_4);
	GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);
  GPIOIntTypeSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_RISING_EDGE);
  GPIOIntRegister(GPIO_PORTF_BASE,PortFIntHandler);
	IntPrioritySet(INT_GPIOE, 0);
  GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4);
	
	
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_PIN_4);
  while(1)
	{
		
	}
}
