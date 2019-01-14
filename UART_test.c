#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

int main(void)
{
	char c;
	
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	
	GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	
	UARTCharPut(UART0_BASE,'L');
	UARTCharPut(UART0_BASE,'e');
	UARTCharPut(UART0_BASE,'t');
	UARTCharPut(UART0_BASE,'s');
	UARTCharPut(UART0_BASE,' ');
	UARTCharPut(UART0_BASE,'B');
	UARTCharPut(UART0_BASE,'e');
	UARTCharPut(UART0_BASE,'g');
	UARTCharPut(UART0_BASE,'i');
	UARTCharPut(UART0_BASE,'n');
	UARTCharPut(UART0_BASE,'\n');
	
	
	while (1)
	{
		if (UARTCharsAvail(UART0_BASE))
		{			
			c = UARTCharGet(UART0_BASE);
			UARTCharPut(UART0_BASE,c);
			
		}
	}
	
}
