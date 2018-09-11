/* *****************************************************************************
 * motors.c
 *
 * Support for two motors controlled by the Tiva board.
 *
 * Intended for use controlling a main and tail rotor motor. The motors are
 * controlled by independent PWM signals, each of which required to remain
 * within a safe range.
 *
 * Hangwen Hu and Marc Katzef
 * Last modified:  3.6.2017
 */

#include "motors.h"

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"

/* *****************************************************************************
 * Globals to module.
 */
static uint8_t g_mainDC = 0;
static uint8_t g_tailDC = 0;
static bool g_mainEnabled = false;
static bool g_tailEnabled = false;


/* *****************************************************************************
 * getDCMain: returns the last duty cycle assigned to the main motor PWM signal,
 * or 0 if motor disabled. Returned value will fall in [0, 100].
 */
uint8_t
getDCMain (void)
{
    if (g_mainEnabled) {
        return g_mainDC;
    } else {
        return 0;
    }
}


/* *****************************************************************************
 * getDCTail: returns the last duty cycle assigned to the tail motor PWM signal,
 * or 0 if motor disabled. Returned value will fall in [0, 100].
 */
uint8_t
getDCTail (void)
{
    if (g_tailEnabled) {
        return g_tailDC;
    } else {
        return 0;
    }
}


/* *****************************************************************************
 * enableMainMotor: enables the PWM signal controlling the main motor.
 */
void
enableMainMotor (void)
{
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, true);
    g_mainEnabled = true;
}


/* *****************************************************************************
 * disableMainMotor: disables the PWM signal controlling the main motor.
 */
void
disableMainMotor (void)
{
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, false);
    g_mainEnabled = false;
}


/* *****************************************************************************
 * enableTailMotor: enables the PWM signal controlling the tail motor.
 */
void
enableTailMotor (void)
{
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, true);
    g_tailEnabled = true;
}


/* *****************************************************************************
 * disableTailMotor: disables the PWM signal controlling the tail motor.
 */
void
disableTailMotor (void)
{
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, false);
    g_tailEnabled = false;
}



/* *****************************************************************************
 * setPWMMain: changes the PWM signal controlling the main motor to have the
 * given frequency and duty cycle. Limits the given duty cycle to fall within
 * the allowed range.
 */
void
setPWMMain (uint32_t ui32Freq, uint32_t ui32Duty)
{
    ui32Duty = MAX(DUTY_MIN_MAIN, ui32Duty);
    ui32Duty = MIN(DUTY_MAX_MAIN, ui32Duty);
    g_mainDC = ui32Duty;

    uint32_t ui32Period = SysCtlClockGet() / PWM_DIVIDER / ui32Freq;

    PWMGenPeriodSet(PWM_MAIN_BASE, PWM_MAIN_GEN, ui32Period);
    PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM,
        ui32Period * ui32Duty / 100);
}


/* *****************************************************************************
 * setPWMTail: changes the PWM signal controlling the tail motor to have the
 * given frequency and duty cycle. Limits the given duty cycle to fall within
 * the allowed range.
 */
void
setPWMTail (uint32_t ui32Freq, uint32_t ui32Duty)
{
	ui32Duty = MAX(DUTY_MIN_TAIL, ui32Duty);
	ui32Duty = MIN(DUTY_MAX_TAIL, ui32Duty);
	g_tailDC = ui32Duty;

    uint32_t ui32Period = SysCtlClockGet() / PWM_DIVIDER / ui32Freq;

    PWMGenPeriodSet(PWM_TAIL_BASE, PWM_TAIL_GEN, ui32Period);
    PWMPulseWidthSet(PWM_TAIL_BASE, PWM_TAIL_OUTNUM,
        ui32Period * ui32Duty / 100);
}


/* *****************************************************************************
 * initMotors: initialises the PWM modules required to control the main and tail
 * rotor motors. Leaves motors ready to be enabled by enableMainMotor and
 * enableTailMotor functions.
 */
void
initMotors (void)
{
	// Main motor
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_GPIO);

    GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_MAIN_GPIO_BASE, PWM_MAIN_GPIO_PIN);

    PWMGenConfigure(PWM_MAIN_BASE, PWM_MAIN_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    // Set the initial PWM parameters
    setPWMMain (DEFAULT_FREQUENCY_MAIN, DEFAULT_DUTY_CYCLE_MAIN);

    PWMGenEnable(PWM_MAIN_BASE, PWM_MAIN_GEN);
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, false);

    // Tail motor
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_PWM);
	SysCtlPeripheralEnable(PWM_TAIL_PERIPH_GPIO);

	GPIOPinConfigure(PWM_TAIL_GPIO_CONFIG);
	GPIOPinTypePWM(PWM_TAIL_GPIO_BASE, PWM_TAIL_GPIO_PIN);

	PWMGenConfigure(PWM_TAIL_BASE, PWM_TAIL_GEN,
					PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
	// Set the initial PWM parameters
	setPWMTail (DEFAULT_FREQUENCY_TAIL, DEFAULT_DUTY_CYCLE_TAIL);

	PWMGenEnable(PWM_TAIL_BASE, PWM_TAIL_GEN);
	PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, false);

	SysCtlPWMClockSet(PWM_DIVIDER_CODE);
}
