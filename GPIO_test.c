#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"



//*****************************************************************************
//
// Blink the on-board LED.
//
//*****************************************************************************
int main(void)
{
    volatile uint32_t ui32Loop;

    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //
    // Check if the peripheral access is enabled.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }

    //
    // Enable the GPIO pin for the LED (PF3).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

    //
    // Loop forever.
    //
    while(1)
    {
        //
        // Turn on the LED.
        //
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x4);

        //
        // Delay for a bit.
        //
        for(ui32Loop = 0; ui32Loop < 400000; ui32Loop++)
        {
        }

       
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x0);
				for(ui32Loop = 0; ui32Loop < 40000; ui32Loop++)
        {
        }
				
				
    }
}
