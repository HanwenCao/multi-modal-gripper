/********** ********** ********** ********** ********** ********** **********
 *                                 ( 0-0 )                                  *
 *                            (" \/       \/ ")                             *
 ********** ********** ********** ********** ********** ********** **********
 * Copyright (C) 2018 - 2022 CURI & HKCLR                                   *
 * File name   : servo_motor.h                                              *
 * Author      : CHEN Wei                                                   *
 * Version     : 1.0.0                                                      *
 * Date        : 2022-02-26                                                 *
 * Description : Serial bus servo motor control library in c++ code, which  *
 *             : can control the motor position through serial port.        *
 * Others      : None                                                       *
 * History     : 2022-02-26 1st version.                                    *
 ********** ********** ********** ********** ********** ********** **********
 *                              (            )                              *
 *                               \ __ /\ __ /                               *
 ********** ********** ********** ********** ********** ********** **********/

#ifndef SERVO_MOTOR_H
#define SERVO_MOTOR_H

#include <stdio.h>
#ifdef WIN32
    #include "windows.h"
#else
    #include <fcntl.h>   // Contains file controls like O_RDWR
    #include <errno.h>   // Error integer and strerror() function
    #include <termios.h> // Contains POSIX terminal control definitions
    #include <unistd.h>  // write(), read(), close()
    #include <stdint.h>
    #include <string.h>

    typedef unsigned char BYTE;
    typedef unsigned int  DWORD;
#endif

// configure parameters
#define SERVO_MOTOR_READ_BUFFER_SIZE  128
#define SERVO_MOTOR_WRITE_BUFFER_SIZE 128

class servo_motor
{
private:

#ifdef WIN32
    HANDLE hCom;  // the handl of the serial port
#else
    int hCom;
#endif

    int lock = 0; // Wait until the transfer is complete before making the next transfer
public:

    /*
     * function: open_serial_port
     *     open the serial port to control the motor
     * input:
     *     com_id[int]: the com id of the serial port to control the motor
     * output:
     *     state[bool]: success return true
     */
    bool open_serial_port(const char com_id[], int rate);

    /*
     * function: control the servo motor with the id to targert_positin in time_t
     *     The motor of id takes period (ms) to travel to the target_position (cnt)
     * input:
     *     id[unsigned char]: the motor id (0 to 255)
     *     targert_positin[int]: the target position in cnt (0 to 1000)
     *     period[int]: time cost to go to the target position in ms (at least 20)
     * output:
     *     state[bool]: success return true
     */
    bool position_control(unsigned char id, int targert_positin, int period = 20);

    /*
     * function: close_serial_port
     *     close the serial port
     * input:
     * output:
     *     state[int]
     */
    bool close_serial_port();
};

#endif