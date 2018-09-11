#ifndef ALTIMETER_H_
#define ALTIMETER_H_

/* *****************************************************************************
 * altimeter.h
 *
 * Altitude measurement support for Tiva kit.
 *
 * Based on a single analogue input signal.
 *
 * Hangwen Hu and Marc Katzef
 * Last modified:  3.6.2017
 */

#include <stdint.h>

/* *****************************************************************************
 * Altitude peripheral definition
 */
#define ALTITUDE_PIN GPIO_PIN_4 // PE4
#define ALTITUDE_PIN_BASE GPIO_PORTE_BASE
#define ALTITUDE_PIN_PERIPH SYSCTL_PERIPH_GPIOE

#define ALTITUDE_ADC_BASE ADC0_BASE
#define ALTITUDE_ADC_PERIPH SYSCTL_PERIPH_ADC0
#define ALTITUDE_RANGE_VOLTS 0.8
#define ADC_WIDTH_BITS 12
#define ADC_RANGE_VOLTS 3.3

/* *****************************************************************************
 * General
 */
#define BUF_SIZE 25 // size of the circular buffer for altitude ADC values
#define ALT_INIT_POLL_RATE 200 // Rate at which initial altitude samples are taken for initialization

// Macros
#define MIN(a,b) a>b?b:a
#define MAX(a,b) a>b?a:b

/* *****************************************************************************
 * getCurrentAltitude: calculates and returns the current altitude based on the
 * average of the circular buffer. The returned value is a percentage based on
 * the expected input voltage range - 0% representing the lowest altitude, 100%
 * the highest. Returned value may exceed 100 (the result of equipment
 * variation).
 */
uint16_t
getCurrentAltitude (void);

/* *****************************************************************************
 * updateAltitude: reads an ADC value (where available), stores it in a circular
 * buffer, and triggers the next ADC conversion.
 * NOTE: a single ADC conversion must be triggered externally for this function
 * to operate.
 */
void
updateAltitude (void);

/* *****************************************************************************
 * calibrateAltimeter: waits for sufficient ADC samples to be taken for the
 * circular buffer to be filled. After the buffer is filled, the global minimum
 * and maximum ADC values are (re)calculated.
 * NOTE: waiting method is blocking.
 */
void
calibrateAltimeter (void);

/* *****************************************************************************
 * initAltimeter: initialises the pin required for altitude readings and the
 * buffer in which read values are stored. Calibrates readings.
 */
void
initAltimeter (void);

#endif /* ALTIMETER_H_ */
