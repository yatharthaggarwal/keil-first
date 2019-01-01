#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"

int main(void)
{
	uint32_t uiADC0Value[4];
		
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))
	{		
	}
	ADCSequenceDisable(ADC0_BASE, 1);
	
	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH1);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH2);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_CH3 | ADC_CTL_IE | ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 1);
	
	while(1)
	{
		ADCIntClear(ADC0_BASE, 1);
		ADCProcessorTrigger(ADC0_BASE, 1);
		while(!ADCIntStatus(ADC0_BASE, 1, false))
		{
		}

		ADCSequenceDataGet(ADC0_BASE, 1, uiADC0Value);
	}
}
