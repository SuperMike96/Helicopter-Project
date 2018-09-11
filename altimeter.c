#include "altimeter.h"
#include "circBufT.h"

/* *****************************************************************************
 * altimeter.c
 *
 * Altitude measurement support for Tiva kit.
 *
 * Based on a single analogue input signal.
 *
 * Hangwen Hu and Marc Katzef
 * Last modified:  3.6.2017
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"

/* *****************************************************************************
 * Globals to module
 */
static circBuf_t g_altitudeBuffer;
static uint8_t g_sampleCount;
static uint32_t g_minAltADCValue;
static uint32_t g_maxAltADCValue;


/* *****************************************************************************
 * getCurrentAltitude: calculates and returns the current altitude based on the
 * average of the circular buffer. The returned value is a percentage based on
 * the expected input voltage range - 0% representing the lowest altitude, 100%
 * the highest. Returned value may exceed 100 (the result of equipment
 * variation).
 */
uint16_t
getCurrentAltitude (void)
{
    uint32_t currentAltADCValue = circBufMean(&g_altitudeBuffer);
    int32_t altPercNum = (currentAltADCValue - g_maxAltADCValue) * 100;
    int32_t altPercDenom = g_maxAltADCValue - g_minAltADCValue;

    uint16_t currentAlt = MAX(0, (2 * altPercNum - altPercDenom) / (2 * altPercDenom) + 100);
    return currentAlt;
}


/* *****************************************************************************
 * updateAltitude: reads an ADC value (where available), stores it in a circular
 * buffer, and triggers the next ADC conversion.
 * NOTE: a single ADC conversion must be triggered externally for this function
 * to operate.
 */
void
updateAltitude (void)
{
	if (ADCIntStatus(ALTITUDE_ADC_BASE, 3, false)) { // ADC has value ready
		ADCIntClear(ALTITUDE_ADC_BASE, 3);

		uint32_t ADCSampleBuffer[1];
		ADCSequenceDataGet(ALTITUDE_ADC_BASE, 3, ADCSampleBuffer);

		writeCircBuf(&g_altitudeBuffer, ADCSampleBuffer[0]);

		g_sampleCount++;
		if (g_sampleCount > BUF_SIZE) {
			g_sampleCount = BUF_SIZE;
		}

		ADCProcessorTrigger(ALTITUDE_ADC_BASE, 3);
	}
}


/* *****************************************************************************
 * calibrateAltimeter: waits for sufficient ADC samples to be taken for the
 * circular buffer to be filled. After the buffer is filled, the global minimum
 * and maximum ADC values are (re)calculated.
 * NOTE: waiting method is blocking.
 */
void
calibrateAltimeter (void)
{
    while (g_sampleCount < BUF_SIZE) {
        updateAltitude(); // Wait for buffer to be filled, for calibration
        SysCtlDelay(SysCtlClockGet() / (3 * ALT_INIT_POLL_RATE));
    }

    g_minAltADCValue = circBufMean(&g_altitudeBuffer);
    g_maxAltADCValue = g_minAltADCValue - (ALTITUDE_RANGE_VOLTS * (1 << ADC_WIDTH_BITS)) / ADC_RANGE_VOLTS;
}


/* *****************************************************************************
 * initAltimeter: initialises the pin required for altitude readings and the
 * buffer in which read values are stored. Calibrates readings.
 */
void
initAltimeter (void)
{
	SysCtlPeripheralEnable(ALTITUDE_ADC_PERIPH);
	SysCtlPeripheralEnable(ALTITUDE_PIN_PERIPH);

	GPIOPinTypeADC(ALTITUDE_PIN_BASE, ALTITUDE_PIN);
	ADCSequenceConfigure(ALTITUDE_ADC_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ALTITUDE_ADC_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE | ADC_CTL_END);

	ADCSequenceEnable(ALTITUDE_ADC_BASE, 3);
	ADCIntClear(ALTITUDE_ADC_BASE, 3);

	ADCProcessorTrigger(ALTITUDE_ADC_BASE, 3); // Get first value.

	initCircBuf(&g_altitudeBuffer, BUF_SIZE);
	g_sampleCount = 0;
	calibrateAltimeter();
}
