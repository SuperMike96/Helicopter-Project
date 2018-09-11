#ifndef BUTTONS_H_
#define BUTTONS_H_

/* *****************************************************************************
 * buttons.h
 *
 * Debounce support for buttons and a slide switch on the Tiva/Orbit.
 *
 * The buttons are:  UP, DOWN, LEFT, RIGHT, and RESET.
 * The slide switch is SLIDE_RIGHT.
 *
 * Debounce algorithm: A state machine is associated with each button. A state
 * change occurs only after NUM_BUT_POLLS consecutive polls have read the pin in
 * the opposite condition, before the state changes and a flag is set. Set
 * NUM_BUT_POLLS according to the polling rate.
 *
 * P.J. Bones UCECE, Modified by Hangwen Hu and Marc Katzef
 * Last modified:  3.6.2017
 */

#include <stdint.h>
#include <stdbool.h>

/* *****************************************************************************
 * Peripheral definitions
 */
// UP button
#define UP_BUT_PERIPH  SYSCTL_PERIPH_GPIOE
#define UP_BUT_PORT_BASE  GPIO_PORTE_BASE
#define UP_BUT_PIN  GPIO_PIN_0
#define UP_BUT_NORMAL  false

// DOWN button
#define DOWN_BUT_PERIPH  SYSCTL_PERIPH_GPIOD
#define DOWN_BUT_PORT_BASE  GPIO_PORTD_BASE
#define DOWN_BUT_PIN  GPIO_PIN_2
#define DOWN_BUT_NORMAL  false

// LEFT button
#define LEFT_BUT_PERIPH  SYSCTL_PERIPH_GPIOF
#define LEFT_BUT_PORT_BASE  GPIO_PORTF_BASE
#define LEFT_BUT_PIN  GPIO_PIN_4
#define LEFT_BUT_NORMAL  true

// RIGHT button
#define RIGHT_BUT_PERIPH  SYSCTL_PERIPH_GPIOF
#define RIGHT_BUT_PORT_BASE  GPIO_PORTF_BASE
#define RIGHT_BUT_PIN  GPIO_PIN_0
#define RIGHT_BUT_NORMAL  true

// RESET button
#define RESET_BUT_PERIPH  SYSCTL_PERIPH_GPIOA
#define RESET_BUT_PORT_BASE  GPIO_PORTA_BASE
#define RESET_BUT_PIN  GPIO_PIN_6
#define RESET_BUT_NORMAL  true

// SLIDE_RIGHT switch
#define SLIDER_RIGHT_PERIPH  SYSCTL_PERIPH_GPIOA
#define SLIDER_RIGHT_PORT_BASE  GPIO_PORTA_BASE
#define SLIDER_RIGHT_PIN  GPIO_PIN_7
#define SLIDER_RIGHT_NORMAL  false

#define NUM_BUT_POLLS 3

/* *****************************************************************************
 * Constants
 */
enum butNames {UP = 0, DOWN, LEFT, RIGHT, RESET, SLIDE_RIGHT, NUM_BUTS};
enum butStates {RELEASED = 0, PUSHED, NO_CHANGE};

/* *****************************************************************************
 * initButtons: initialise the variables associated with the set of buttons
 * defined by the constants above.
 */
void
initButtons (void);

/* *****************************************************************************
 * updateButtons: polls all buttons once and updates variables associated with
 * the buttons if necessary. Designed to be called regularly and efficient
 * enough to be called from an ISR.
 */
void
updateButtons (void);

/* *****************************************************************************
 * checkButton: returns the new button state if the button state (PUSHED or
 * RELEASED) has changed since the last call, otherwise returns NO_CHANGE.  The
 * argument butName should be one of constants in the enumeration butNames,
 * excluding 'NUM_BUTS'. Safe under interrupt.
 */
uint8_t
checkButton (uint8_t butName);

#endif /*BUTTONS_H_*/
