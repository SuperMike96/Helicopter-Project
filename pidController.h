#ifndef PID_CONTROLLER_H_
#define PID_CONTROLLER_H_

/* *****************************************************************************
 * pidController.h
 *
 * Product, integral, derivative (PID) controller support for the Tiva/Orbit.
 *
 * Calculates an appropriate value for an actuator input based on regular error
 * updates. Error should be calculated as the difference between a reference
 * value and current value.
 *
 * Hangwen Hu and Marc Katzef
 * Last modified:  3.6.2017
 */

#include <stdint.h>

/* *****************************************************************************
 * Controller structure
 */
typedef struct {
	double errorIntegrated;
	double errorPrevious;
	double gainProportional;
	double gainIntegral;
	double gainDerivative;
} pidController_t;

/* *****************************************************************************
 * initPidController: initialises the given pidController instance by setting
 * all error members to zero, and storing the given control gains.
 */
void
initPidController (pidController_t *controller, double Kp, double Ki, double Kd);


/* *****************************************************************************
 * pidUpdate: adds the new error value to the controller. Calculates and returns
 * the new control value.
 */
double
pidUpdate (pidController_t *controller, double error, double deltaT);

#endif /*PID_CONTROLLER_H_*/
