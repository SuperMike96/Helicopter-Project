#ifndef HELICOPTER_MAIN_H_
#define HELICOPTER_MAIN_H_

/* *****************************************************************************
 * helicopter_main.h
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

/* *****************************************************************************
 * Controller gains
 */
// Altitude
#define KP_ALT 1
#define KI_ALT 0.5
#define KD_ALT 0.2

// Yaw
#define KP_YAW 1
#define KI_YAW 0.2
#define KD_YAW 0.2

/* *****************************************************************************
 * Position parameters
 */
// Button responses
#define INCREMENT_ALT 10
#define INCREMENT_YAW 15

// Parameters for take off and landing
#define YAW_TAKEOFF_TOLERANCE 15
#define YAW_LANDING_TOLERANCE 3
#define YAW_REF_INCREMENT 30
#define YAW_MIN_POLLS 15

#define ALT_TOLERANCE 8
#define YAW_CORRECTION_ALT 10
#define LANDING_MAX_ALT 3
#define ALT_LANDING_INCREMENT 10
#define ALT_MIN_POLLS 10

/* *****************************************************************************
 * Timing and peripherals
 */
// Interrupt frequencies
#define CONTROL_UPDATE_FREQUENCY 100
#define POLLING_FREQUENCY 200 // for buttons and altitude

// Controller interrupt peripheral
#define CONTROLLER_TIMER_PERIPH SYSCTL_PERIPH_TIMER0
#define CONTROLLER_TIMER_MODULE TIMER_A
#define CONTROLLER_TIMER_BASE TIMER0_BASE
#define CONTROLLER_TIMER_INT INT_TIMER0A
#define CONTROLLER_TIMER_INT_MODE TIMER_TIMA_TIMEOUT

// UART
#define BAUD_RATE 9600
#define UART_CLK_FREQ 16000000

// Macros
#define MIN(a,b) a>b?b:a
#define MAX(a,b) a>b?a:b

/* *****************************************************************************
 * Constants
 */
typedef enum heliState {IDLE = 0, TAKING_OFF, FLYING, LANDING, NUM_HELI_STATES} heliState_t;

#endif /* HELICOPTER_MAIN_H_ */
