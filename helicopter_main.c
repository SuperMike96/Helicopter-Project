/* *****************************************************************************
 * helicopter_main.c
 *
 * Controls the altitude and yaw of a helicopter based on user input.
 *
 * Uses supporting modules to read information about the helicopter's current
 * and desired position. Applies a suitable signal to the main and tail
 * helicopter motor to minimise the difference between the two.
 *
 * Also features logic for procedures such as take-off and landing for ease of
 * use.
 *
 * Hangwen Hu and Marc Katzef
 * Last modified:  3.6.2017
 */

#include "helicopter_main.h"
#include "pidController.h"
#include "motors.h"
#include "altimeter.h"
#include "buttons.h"
#include "yawmeter.h"

#include "OrbitOLEDInterface.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "stdio.h"
#include "stdlib.h"

/* *****************************************************************************
 * Globals to module
 */
// Position variables
static uint16_t g_targetYaw;
static uint8_t g_targetAlt;
static uint16_t g_currentYaw;
static int16_t g_currentAlt;

// Altitude and yaw controllers
static pidController_t altController;
static pidController_t yawController;
static const double g_deltaT = 1.0 / CONTROL_UPDATE_FREQUENCY;

// State variables
static heliState_t g_state = IDLE;
static bool g_flightModeActive = false;
static uint8_t g_yawDebounce = 0;
static uint8_t g_altDebounce = 0;

/* *****************************************************************************
 * pollingIntHandler: polls altitude and buttons. Immediately identifies if
 * reset button has been pushed, for faster and more reliable response.
 */
void
pollingIntHandler (void)
{
	updateAltitude();

	updateButtons();
	if (checkButton(RESET) == PUSHED) {
		SysCtlReset();
	}
}


/* *****************************************************************************
 * controllerIntHandler: calculates and adds altitude and yaw errors to
 * corresponding controllers. Sets the new control values to motors.
 */
void
controllerIntHandler (void)
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	if (g_flightModeActive) {
	    int16_t yawError = yawDifference(g_targetYaw, g_currentYaw);

		double altControl = pidUpdate(&altController, g_targetAlt - g_currentAlt, g_deltaT);
		double yawControl = pidUpdate(&yawController, yawError, g_deltaT);

		setPWMMain(DEFAULT_FREQUENCY_MAIN, altControl);
		setPWMTail(DEFAULT_FREQUENCY_TAIL, yawControl);
	}
}


/* *****************************************************************************
 * initClock: initialises system clock.
 */
void
initClock (void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
}


/* *****************************************************************************
 * initPollingInterrupt: initialises a SysTick interrupt set to call the
 * pollingIntHandler.
 */
void
initPollingInterrupt (void)
{
    SysTickEnable();
    SysTickPeriodSet(SysCtlClockGet() / POLLING_FREQUENCY);
    SysTickIntRegister(pollingIntHandler);
    SysTickIntEnable();
}


/* *****************************************************************************
 * initControllerInterrupt: initialises a timer interrupt set to call the
 * controllerIntHandler.
 */
void
initControllerInterrupt (void)
{
    SysCtlPeripheralEnable(CONTROLLER_TIMER_PERIPH);
    TimerConfigure(CONTROLLER_TIMER_BASE, TIMER_CFG_PERIODIC);
    TimerIntRegister(CONTROLLER_TIMER_BASE, CONTROLLER_TIMER_MODULE, controllerIntHandler);
    TimerEnable(CONTROLLER_TIMER_BASE, CONTROLLER_TIMER_MODULE);
    IntEnable(CONTROLLER_TIMER_INT);
    TimerIntEnable(CONTROLLER_TIMER_BASE, CONTROLLER_TIMER_INT_MODE);
    TimerLoadSet(CONTROLLER_TIMER_BASE, CONTROLLER_TIMER_MODULE, SysCtlClockGet() / CONTROL_UPDATE_FREQUENCY);
}


/* *****************************************************************************
 * initConsole: initialises UART communication.
 * TODO: abstract the pins and peripherals using #defines.
 */
void
initConsole(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioConfig(0, BAUD_RATE, UART_CLK_FREQ);
}


/* *****************************************************************************
 * displayPosition: prints current information about the helicopter to the
 * Orbit's OLED display and through UART.
 */
void
displayPosition (int yaw, int altitude)
{
	char uartString[50];
    char oledString[17];

	sprintf (oledString, "ALT: %3d [%3d]", g_currentAlt, g_targetAlt);
	sprintf (uartString, "Altitude: %3d [%3d]\n", g_currentAlt, g_targetAlt);
	OLEDStringDraw (oledString, 0, 0);
	UARTprintf(uartString);

	sprintf (oledString, "YAW: %3d [%3d]", g_currentYaw, g_targetYaw);
	sprintf (uartString, "Yaw: %3d [%3d]\n", g_currentYaw, g_targetYaw);
	OLEDStringDraw (oledString, 0, 1);
	UARTprintf(uartString);

	uint8_t currentMainDC = getDCMain();
	uint8_t currentTailDC = getDCTail();
	sprintf (oledString, "MAIN DC: %3d", currentMainDC);
	OLEDStringDraw (oledString, 0, 2);
	sprintf (oledString, "TAIL DC: %3d", currentTailDC);
	OLEDStringDraw (oledString, 0, 3);
    sprintf (uartString, "DC: Main: %3d Tail: %3d\n", currentMainDC, currentTailDC);
    UARTprintf(uartString);

	switch (g_state) {
	case IDLE:
		sprintf (uartString, "Mode: idle\n");
		break;
	case TAKING_OFF:
		sprintf (uartString, "Mode: taking off\n");
		break;
	case FLYING:
		sprintf (uartString, "Mode: flying\n");
		break;
	case LANDING:
		sprintf (uartString, "Mode: landing\n");
		break;
	}
	UARTprintf(uartString);

	UARTprintf("----------\n");
}


/* *****************************************************************************
 * updateStateIdle: disables the main and tail motor upon state entry, reads
 * input updates. Returns the next state to assume - IDLE or TAKING_OFF.
 */
heliState_t
updateStateIdle (bool justChangedState)
{
    heliState_t resultState = IDLE;

    if (justChangedState) {
        disableMainMotor();
        disableTailMotor();
        g_flightModeActive = false;
    }

    if (checkButton(SLIDE_RIGHT) == PUSHED) {
        initPidController(&altController, KP_ALT, KI_ALT, KD_ALT);
        initPidController(&yawController, KP_YAW, KI_YAW, KD_YAW);
        g_targetAlt = YAW_CORRECTION_ALT;
        resultState = TAKING_OFF;
    }

    return resultState;
}


/* *****************************************************************************
 * updateStateTakingOff: rotates the helicopter to find the calibration point.
 * Initialises the yaw after detecting the calibration point and returns
 * state FLYING. Until then, TAKING_OFF or IDLE (if mode switch flipped).
 */
heliState_t
updateStateTakingOff (bool justChangedState)
{
    heliState_t resultState = TAKING_OFF;

    if (justChangedState) {
        calibrateAltimeter();
        enableMainMotor();
        enableTailMotor();
        g_flightModeActive = true;
        g_targetAlt = YAW_CORRECTION_ALT;
        g_yawDebounce = 0;
    }

    // Check if flight disabled
    if (checkButton(SLIDE_RIGHT) == RELEASED) {
        resultState = IDLE;
    } else {
        if (yawCalibrated_p()) {
            g_targetYaw = 0;
            disableYawRefInt(); // Prevent undesired re-calibration
            resultState = FLYING;
        } else {
            int32_t yawError = abs(yawDifference(g_targetYaw, g_currentYaw));
            int32_t altError = abs((int32_t)g_targetAlt - g_currentAlt);

            if ((yawError < YAW_TAKEOFF_TOLERANCE) && (altError < ALT_TOLERANCE)) {
                g_yawDebounce++;
                if (g_yawDebounce > YAW_MIN_POLLS) {
                    int32_t newTargetYaw = g_targetYaw + YAW_REF_INCREMENT;
                    if (newTargetYaw >= 360) {
                        newTargetYaw -= 360;
                    }
                    g_targetYaw = newTargetYaw;
                }
            } else {
                g_yawDebounce = 0;
            }
        }
    }

    return resultState;
}


/* *****************************************************************************
 * updateStateFlying: uses direction buttons to change the helicopter target
 * position. Reads mode switch to enter LANDING state.
 */
heliState_t
updateStateFlying (bool justChangedState)
{
    heliState_t resultState = FLYING;

    if (checkButton(SLIDE_RIGHT) == RELEASED) {
        resultState = LANDING;
    } else {
        if (checkButton(UP) == PUSHED) {
            g_targetAlt = MIN(100, g_targetAlt + INCREMENT_ALT);
        }
        if (checkButton(DOWN) == PUSHED) {
            g_targetAlt = MAX(0, g_targetAlt - INCREMENT_ALT);
        }
        if (checkButton(RIGHT) == PUSHED) {
            g_targetYaw = (g_targetYaw + INCREMENT_YAW) % 360;
        }
        if (checkButton(LEFT) == PUSHED) {
            if (g_targetYaw < INCREMENT_YAW) {
                g_targetYaw = g_targetYaw - INCREMENT_YAW + 360;
            } else {
                g_targetYaw -= INCREMENT_YAW;
            }
        }
    }

    return resultState;
}


/* *****************************************************************************
 * updateStateLanding: rotates helicopter to reference point and descends.
 * Returns state IDLE if altitude is 0%. 
 */
heliState_t updateStateLanding (bool justChangedState)
{
    heliState_t resultState = LANDING;

    if (justChangedState) {
        g_yawDebounce = 0;
        g_altDebounce = 0;
    }

    if (checkButton(SLIDE_RIGHT) == PUSHED) {
        resultState = FLYING;
    } else {
        int32_t yawError = abs(yawDifference(g_targetYaw, g_currentYaw));

        if (yawError < YAW_LANDING_TOLERANCE) {
            g_yawDebounce++;
            if (g_yawDebounce > YAW_MIN_POLLS) {
                if (g_targetYaw != 0) {
                    int32_t newTargetYaw;
                    if (yawDifference(0, g_targetYaw) < 0) {
                        newTargetYaw = MAX(0, ((int32_t)g_targetYaw) - YAW_REF_INCREMENT);
                    } else {
                        newTargetYaw = g_targetYaw + YAW_REF_INCREMENT;
                        if (newTargetYaw >= 360) {
                            newTargetYaw = 0;
                        }
                    }
                    g_targetYaw = newTargetYaw;
                }
            }
        } else {
            g_yawDebounce = 0;
        }

        yawError = abs(yawDifference(0, g_currentYaw));
        if (yawError < YAW_LANDING_TOLERANCE) {
            if (g_currentAlt <= LANDING_MAX_ALT) {
                resultState = IDLE;
            } else {
                int32_t altError = abs((int32_t)g_targetAlt - g_currentAlt);

                if (altError < ALT_TOLERANCE) {
                    g_altDebounce++;
                    if (g_altDebounce > ALT_MIN_POLLS) {
                        g_targetAlt = MAX(0, ((int32_t)g_targetAlt) - ALT_LANDING_INCREMENT);
                    }
                } else {
                    g_altDebounce = 0;
                }
            }
        }
    }

    return resultState;
}


/* *****************************************************************************
 * main: initialises required modules before entering an infinite loop which
 * polls altitude and yaw, displays current information, and updates a finite
 * state machine.
 */
int
main(void)
{
	initClock();
	OLEDInitialise ();
	initAltimeter();
	initYawmeter();
	initMotors();
	initControllerInterrupt();
	initConsole();
	initButtons();
	initPollingInterrupt();

	IntMasterEnable ();

    bool justChangedState = true;
    heliState_t newState;

	while (1)
	{
		g_currentAlt = getCurrentAltitude();
		g_currentYaw = getCurrentYaw();
		displayPosition (g_currentYaw, g_currentAlt);

		switch (g_state) {
		case IDLE:
			newState = updateStateIdle(justChangedState);
			break;

		case TAKING_OFF:
		    newState = updateStateTakingOff(justChangedState);
			break;

		case FLYING:
		    newState = updateStateFlying(justChangedState);
			break;

		case LANDING:
		    newState = updateStateLanding(justChangedState);
		    break;
		}

		if (newState == g_state) {
		    justChangedState = false;
		} else {
		    justChangedState = true;
            g_state = newState;
		}
	}
}
