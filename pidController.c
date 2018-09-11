/* *****************************************************************************
 * pidController.c
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

#include "pidController.h"

#include <stdint.h>
#include "stdlib.h"


/* *****************************************************************************
 * initPidController: initialises the given pidController instance by setting
 * all error members to zero, and storing the given control gains.
 */
void
initPidController (pidController_t *controller, double Kp, double Ki, double Kd) {
	controller->errorIntegrated = 0;
	controller->errorPrevious = 0;
	controller->gainProportional = Kp;
	controller->gainIntegral = Ki;
	controller->gainDerivative = Kd;
}


/* *****************************************************************************
 * pidUpdate: adds the new error value to the controller. Calculates and returns
 * the new control value.
 */
double
pidUpdate (pidController_t *controller, double error, double deltaT) {
	double errorDerivative;
	double control;

	controller->errorIntegrated += error * deltaT;
	errorDerivative = (error - controller->errorPrevious) / deltaT;

	control = error * controller->gainProportional +
			controller->errorIntegrated * controller->gainIntegral +
			errorDerivative * controller->gainDerivative;

	controller->errorPrevious = error;
	return control;
}
