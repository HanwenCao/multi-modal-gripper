/********** ********** ********** ********** ********** ********** **********
 *                                 ( 0-0 )                                  *
 *                            (" \/       \/ ")                             *
 ********** ********** ********** ********** ********** ********** **********
 * Copyright (C) 2018 - 2022 CURI & HKCLR                                   *
 * File name   : servo_motor_test.cpp                                       *
 * Author      : CHEN Wei                                                   *
 * Version     : 1.0.0                                                      *
 * Date        : 2022-02-26                                                 *
 * Description : The test code and demo for servo motor library.            *
 * Others      : None                                                       *
 * History     : 2022-02-26 1st version.                                    *
 ********** ********** ********** ********** ********** ********** **********
 *                              (            )                              *
 *                               \ __ /\ __ /                               *
 ********** ********** ********** ********** ********** ********** **********/
#include<stdio.h> 
#include "servo_motor.h"

int main()
{
	printf("Starting..\n");
	servo_motor motor1;
	if (motor1.open_serial_port("/dev/ttyUSB0", 115200)) {  // for linux
		// position control
		for (int i = 0; i < 500; ++ i) {
			motor1.position_control(1, i * 2);
			usleep(2000); // wait at least 20 ms for each motor (speed control)
		}		
		for (int i = 500; i >= 0; -- i) {
			motor1.position_control(1, i * 2);
			usleep(2000); // wait at least 20 ms for each motor
		}

		// finish
		motor1.close_serial_port();
	}

	return 0;
}
