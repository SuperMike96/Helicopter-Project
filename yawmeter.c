/* *****************************************************************************
 * yawmeter.c
 *
 * Yaw measurement support for Tiva kit. Based on quadrature decoding using two
 * input signals.
 *
 * Hangwen Hu and Marc Katzef
 * Last modified:  3.6.2017
 */

#include "yawmeter.h"

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"

static volatile bool g_stateA = false;
static volatile bool g_stateB = false;
static volatile int16_t g_pinChangeInterruptCount = 0;
static volatile yawDirection_t g_currentDirection = DIRECTION_CW;
static volatile bool g_yawCalibrated = false;

/* *****************************************************************************
 * yawCalibrated_p: returns true if the yaw reference signal has been detected,
 * otherwise false.
 */
bool
yawCalibrated_p (void)
{
    return g_yawCalibrated;
}


/* *****************************************************************************
 * yawDifference: returns the smallest (in magnitude) angle from between the
 * two given bearings. Measured from current to reference. Result will fall in
 * [-180, 180].
 */
int32_t
yawDifference(uint16_t reference, uint16_t current)
{
	int32_t yawError;
	yawError = ((int32_t)reference) - current;
	if (yawError < -180) {
		yawError += 360;
	} else if (yawError > 180) {
		yawError -= 360;
	}
	return yawError;
}


/* *****************************************************************************
 * pinChangeIntHandler: carries out quadrature decoding using the two input
 * signals A and B. Increments g_pinChangeInterruptCount when clockwise movement
 * is read, decrements it for anticlockwise.
 */
void
pinChangeIntHandler (void)
{
    GPIOIntClear (GPIO_PORTB_BASE, YAW_PIN_A);
    GPIOIntClear (GPIO_PORTB_BASE, YAW_PIN_B);

    bool newStateA = (GPIOPinRead(YAW_BASE_A, YAW_PIN_A) & YAW_PIN_A) == YAW_PIN_A;
    bool newStateB = (GPIOPinRead(YAW_BASE_B, YAW_PIN_B) & YAW_PIN_B) == YAW_PIN_B;

	int16_t pinChangeInterruptCount = g_pinChangeInterruptCount;
	yawDirection_t direction = DIRECTION_CW;

	if (newStateA != g_stateA) {
		if (newStateA == newStateB) {
			direction = DIRECTION_CW;
		} else {
			direction = DIRECTION_ANTI_CW;
		}

	} else if (newStateB != g_stateB) {
		if (newStateA != newStateB) {
			direction = DIRECTION_CW;
		} else {
			direction = DIRECTION_ANTI_CW;
		}
	}

	if (direction == DIRECTION_CW) {
		pinChangeInterruptCount++;
	} else if (direction == DIRECTION_ANTI_CW) {
		pinChangeInterruptCount--;
	}

	if (pinChangeInterruptCount >= INTERRUPTS_PER_REV) {
		pinChangeInterruptCount -= INTERRUPTS_PER_REV;
	} else if (pinChangeInterruptCount < 0) {
		pinChangeInterruptCount += INTERRUPTS_PER_REV;
	}

	g_stateA = newStateA;
	g_stateB = newStateB;
	g_currentDirection = direction;
	g_pinChangeInterruptCount = pinChangeInterruptCount;
}

/* *****************************************************************************
 * yawRefIntHandler: calibrates yaw readings by setting pinChangeInterruptCount
 * to zero. Should be called once - as soon as a reference signal is applied.
 */
void
yawRefIntHandler (void)
{
	GPIOIntClear (YAW_BASE_REF, YAW_PIN_REF);
	g_pinChangeInterruptCount = 0;
	g_yawCalibrated = true;
}

/* *****************************************************************************
 * initReferencePin: intitialises the peripheral and pin required to read the
 * yaw reference pin. Sets and enables interrupts for the relevant GPIO.
 */
void
initReferencePin (void)
{
    SysCtlPeripheralEnable (YAW_PERIPH_REF);
    GPIOIntRegister (YAW_BASE_REF, yawRefIntHandler);

    GPIOPinTypeGPIOInput (YAW_BASE_REF, YAW_PIN_REF);
    GPIOPadConfigSet (YAW_BASE_REF, YAW_PIN_REF, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPU);

    GPIOIntTypeSet (YAW_BASE_REF, YAW_PIN_REF, GPIO_BOTH_EDGES);
    GPIOIntEnable (YAW_BASE_REF, YAW_PIN_REF);
	IntEnable (YAW_GPIO_INT_REF);
}

/* *****************************************************************************
 * initYawPins: intitialises the peripherals and pins required to read the
 * quadrature signal. Sets and enables interrupts for the relevant GPIO.
 */
void
initYawPins (void)
{
    SysCtlPeripheralEnable (YAW_PERIPH_A);
    SysCtlPeripheralEnable (YAW_PERIPH_B);

    GPIOIntRegister (YAW_BASE_A, pinChangeIntHandler);
    GPIOIntRegister (YAW_BASE_B, pinChangeIntHandler);

    GPIOPinTypeGPIOInput (YAW_BASE_A, YAW_PIN_A);
	GPIOPinTypeGPIOInput (YAW_BASE_B, YAW_PIN_B);

    GPIOPadConfigSet (YAW_BASE_A, YAW_PIN_A, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPD);
	GPIOPadConfigSet (YAW_BASE_B, YAW_PIN_B, GPIO_STRENGTH_2MA,
	   GPIO_PIN_TYPE_STD_WPD);

    GPIOIntTypeSet (YAW_BASE_A, YAW_PIN_A, GPIO_BOTH_EDGES);
	GPIOIntTypeSet (YAW_BASE_B, YAW_PIN_B, GPIO_BOTH_EDGES);

    GPIOIntEnable (YAW_BASE_A, YAW_PIN_A);
    GPIOIntEnable (YAW_BASE_B, YAW_PIN_B);

    IntEnable (YAW_GPIO_INT_A);
	IntEnable (YAW_GPIO_INT_B);
}


/* *****************************************************************************
 * getCurrentYaw: returns the current yaw as measured by quadrature decoding.
 * Return value represents an angle clockwise from reference point or initial
 * position. This value will fall in [0, 360).
 */
uint16_t
getCurrentYaw (void)
{
	return (g_pinChangeInterruptCount * 360) / INTERRUPTS_PER_REV;
}


/* *****************************************************************************
 * disableYawRefInt: disables the yaw reference interrupt. May be used to
 * prevent unexpected behaviour where calculations are based on the yaw values
 * taken before calibration. Undone through a call to enableYawRefInt.
 */
void
disableYawRefInt (void)
{
    GPIOIntDisable (YAW_BASE_REF, YAW_PIN_REF);
}


/* *****************************************************************************
 * enableYawRefInt: enables the yaw reference interrupt. Called in initYawmeter.
 */
void
enableYawRefInt (void)
{
    GPIOIntEnable (YAW_BASE_REF, YAW_PIN_REF);
}


/* *****************************************************************************
 * initYawmeter: calls pin initialisation functions, records initial channel
 * states and sets the interrupt count to zero.
 */
void
initYawmeter (void)
{
	initReferencePin();
	initYawPins();
	g_pinChangeInterruptCount = 0;

	g_stateA = (GPIOPinRead (YAW_BASE_A, YAW_PIN_A) & YAW_PIN_A) == YAW_PIN_A;
    g_stateB = (GPIOPinRead (YAW_BASE_B, YAW_PIN_B) & YAW_PIN_B) == YAW_PIN_B;
}
