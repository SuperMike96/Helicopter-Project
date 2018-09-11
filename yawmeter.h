#ifndef YAWMETER_H_
#define YAWMETER_H_

/* *****************************************************************************
 * yawmeter.h
 *
 * Yaw measurement support for Tiva kit. Based on quadrature decoding using two
 * input signals.
 *
 * Hangwen Hu and Marc Katzef
 * Last modified:  3.6.2017
 */

#include <stdint.h>
#include <stdbool.h>

/* *****************************************************************************
 * Peripheral definitions
 */
// Quadrature channel A - PB0
#define YAW_PIN_A GPIO_PIN_0
#define YAW_PERIPH_A SYSCTL_PERIPH_GPIOB
#define YAW_BASE_A GPIO_PORTB_BASE
#define YAW_GPIO_INT_A INT_GPIOB

// Quadrature channel B - PB1
#define YAW_PIN_B GPIO_PIN_1
#define YAW_PERIPH_B SYSCTL_PERIPH_GPIOB
#define YAW_BASE_B GPIO_PORTB_BASE
#define YAW_GPIO_INT_B INT_GPIOB

// Yaw reference signal - PC4
#define YAW_PIN_REF GPIO_PIN_4
#define YAW_PERIPH_REF SYSCTL_PERIPH_GPIOC
#define YAW_BASE_REF GPIO_PORTC_BASE
#define YAW_GPIO_INT_REF INT_GPIOC

/* *****************************************************************************
 * Optical encoder calculations
 */
#define QUADRATURE_SENSOR_COUNT 2
#define SLOTS_PER_REV 112
#define INTERRUPTS_PER_SLOT 2
#define INTERRUPTS_PER_REV (INTERRUPTS_PER_SLOT * SLOTS_PER_REV * QUADRATURE_SENSOR_COUNT)

/* *****************************************************************************
 * Constants
 */
typedef enum yawDirection {DIRECTION_CW = 0, DIRECTION_ANTI_CW} yawDirection_t;

/* *****************************************************************************
 * yawCalibrated_p: returns true if the yaw reference signal has been detected,
 * otherwise false.
 */
bool
yawCalibrated_p (void);

/* *****************************************************************************
 * yawDifference: returns the smallest (in magnitude) angle from between the
 * two given bearings. Measured from current to reference. Result will fall in
 * [-180, 180].
 */
int32_t
yawDifference (uint16_t reference, uint16_t current);

/* *****************************************************************************
 * getCurrentYaw: returns the current yaw as measured by quadrature decoding.
 * Return value represents an angle clockwise from reference point or initial
 * position. This value will fall in [0, 360).
 */
uint16_t
getCurrentYaw (void);

/* *****************************************************************************
 * disableYawRefInt: disables the yaw reference interrupt. May be used to
 * prevent unexpected behaviour where calculations are based on the yaw values
 * taken before calibration. Undone through a call to enableYawRefInt.
 */
void
disableYawRefInt (void);

/* *****************************************************************************
 * enableYawRefInt: enables the yaw reference interrupt. Called in initYawmeter.
 */
void
enableYawRefInt (void);

/* *****************************************************************************
 * initYawmeter: calls pin initialisation functions, records initial channel
 * states and sets the interrupt count to zero.
 */
void
initYawmeter (void);

#endif /* YAWMETER_H_ */
