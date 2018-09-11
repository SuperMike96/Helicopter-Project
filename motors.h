#ifndef MOTORS_H_
#define MOTORS_H_

/* *****************************************************************************
 * motors.h
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

#include <stdint.h>

/* *****************************************************************************
 * Peripheral definitions
 */
// Main motor PWM: M0PWM7, PC5, J4-05
#define PWM_MAIN_BASE	     PWM0_BASE
#define PWM_MAIN_GEN         PWM_GEN_3
#define PWM_MAIN_OUTNUM      PWM_OUT_7
#define PWM_MAIN_OUTBIT      PWM_OUT_7_BIT
#define PWM_MAIN_PERIPH_PWM	 SYSCTL_PERIPH_PWM0
#define PWM_MAIN_PERIPH_GPIO SYSCTL_PERIPH_GPIOC
#define PWM_MAIN_GPIO_BASE   GPIO_PORTC_BASE
#define PWM_MAIN_GPIO_CONFIG GPIO_PC5_M0PWM7
#define PWM_MAIN_GPIO_PIN    GPIO_PIN_5
#define DEFAULT_FREQUENCY_MAIN 150
#define DEFAULT_DUTY_CYCLE_MAIN 0
#define DUTY_MAX_MAIN 98
#define DUTY_MIN_MAIN 2

// Tail motor PWM: M1PWM5, PF1, J3-10
#define PWM_TAIL_BASE	     PWM1_BASE
#define PWM_TAIL_GEN         PWM_GEN_2
#define PWM_TAIL_OUTNUM      PWM_OUT_5
#define PWM_TAIL_OUTBIT      PWM_OUT_5_BIT
#define PWM_TAIL_PERIPH_PWM	 SYSCTL_PERIPH_PWM1
#define PWM_TAIL_PERIPH_GPIO SYSCTL_PERIPH_GPIOF
#define PWM_TAIL_GPIO_BASE   GPIO_PORTF_BASE
#define PWM_TAIL_GPIO_CONFIG GPIO_PF1_M1PWM5
#define PWM_TAIL_GPIO_PIN    GPIO_PIN_1
#define DEFAULT_FREQUENCY_TAIL 150
#define DEFAULT_DUTY_CYCLE_TAIL 0
#define DUTY_MAX_TAIL 98
#define DUTY_MIN_TAIL 2

// Shared PWM configuration
#define PWM_DIVIDER_CODE SYSCTL_PWMDIV_4
#define PWM_DIVIDER 4

// Macros
#define MIN(a,b) a>b?b:a
#define MAX(a,b) a>b?a:b

/* *****************************************************************************
 * getDCMain: returns the last duty cycle assigned to the main motor PWM signal,
 * or 0 if motor disabled. Returned value will fall in [0, 100].
 */
uint8_t
getDCMain (void);

/* *****************************************************************************
 * getDCTail: returns the last duty cycle assigned to the tail motor PWM signal,
 * or 0 if motor disabled. Returned value will fall in [0, 100].
 */
uint8_t
getDCTail (void);

/* *****************************************************************************
 * enableMainMotor: enables the PWM signal controlling the main motor.
 */
void
enableMainMotor (void);

/* *****************************************************************************
 * disableMainMotor: disables the PWM signal controlling the main motor.
 */
void
disableMainMotor (void);

/* *****************************************************************************
 * enableTailMotor: enables the PWM signal controlling the tail motor.
 */
void
enableTailMotor (void);

/* *****************************************************************************
 * disableTailMotor: disables the PWM signal controlling the tail motor.
 */
void
disableTailMotor (void);

/* *****************************************************************************
 * setPWMMain: changes the PWM signal controlling the main motor to have the
 * given frequency and duty cycle. Limits the given duty cycle to fall within
 * the allowed range.
 */
void
setPWMMain (uint32_t ui32Freq, uint32_t ui32Duty);

/* *****************************************************************************
 * setPWMTail: changes the PWM signal controlling the tail motor to have the
 * given frequency and duty cycle. Limits the given duty cycle to fall within
 * the allowed range.
 */
void
setPWMTail (uint32_t ui32Freq, uint32_t ui32Duty);

/* *****************************************************************************
 * initMotors: initialises the PWM modules required to control the main and tail
 * rotor motors. Leaves motors ready to be enabled by enableMainMotor and
 * enableTailMotor functions.
 */
void
initMotors (void);

#endif /* MOTORS_H_ */
