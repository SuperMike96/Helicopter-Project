# ENCE361 2017
## Helicopter Project - GROUP 16
### Hangwen Hu, Marc Katzef

## About
An interrupt-driven C program for the Tiva kit and Orbit BoosterPack to control a toy helicopter.

## Modules
The project is divided into a single main module and several supporting modules (some of which given).

### New
`altimeter.c` - measures altitude based on an input analogue signal.  
`altimeter.h` - important values for altitude measurement module.  
`helicopter_main.c` - the main module of the project, uses all others.  
`helicopter_main.h` - important values for main module.  
`motors.c` - controls helicopter motors.  
`motors.h` - important values for motors module.  
`pidController.c` - abstract data type and functions for PID control.  
`pidController.h` - important values for PID control module.  
`yawmeter.c` - measures yaw through quadrature decoding.  
`yawmeter.h` - important values for yaw measurement module.  

### Given
`buttons.c` - debounces input buttons.  
`buttons.h` - important values for buttons module.  
`circBufT.c` - abstract data type and functions for circular buffers.  
`circBufT.h` - important values for circular buffer module.  
