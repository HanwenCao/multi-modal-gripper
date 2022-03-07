/********** ********** ********** ********** ********** ********** **********
 *                                 ( 0-0 )                                  *
 *                            (" \/       \/ ")                             *
 ********** ********** ********** ********** ********** ********** **********
 * Copyright (C) 2018 - 2022 CURI & HKCLR                                   *
 * File name   : servo_motor.cpp                                            *
 * Author      : CHEN Wei                                                   *
 * Version     : 1.0.0                                                      *
 * Date        : 2022-02-26                                                 *
 * Description : Serial bus servo motor control library in C code, which    *
 *             : can control the motor position through serial port.        *
 * Others      : None                                                       *
 * History     : 2022-02-26 1st version.                                    *
 ********** ********** ********** ********** ********** ********** **********
 *                              (            )                              *
 *                               \ __ /\ __ /                               *
 ********** ********** ********** ********** ********** ********** **********/

#include "servo_motor.h"

/*
 * function: open_serial_port
 *     open the serial port to control the motor
 * input: 
 *     com_id[int]: the com id of the serial port to control the motor
 *     baud_rate[int]: the baud rate of the serial port communication
 * output:
 *     state[bool]: success return true
 */
bool servo_motor::open_serial_port(const char com_id[], int baud_rate)
{
#ifdef WIN32
    hCom = CreateFile(LPCSTR(com_id),
                      GENERIC_READ | GENERIC_WRITE, // open mode: read & write  
                      0, //独占方式  
                      NULL,
                      OPEN_EXISTING, //打开而不是创建  
                      0, //同步方式  
                      NULL);
    if (hCom == (HANDLE)-1)
    {
      	printf("Error: open serial port fail!");
        return false;
    }

    // set buffer size
    SetupComm(hCom, SERVO_MOTOR_READ_BUFFER_SIZE, SERVO_MOTOR_WRITE_BUFFER_SIZE);

    // set timeout
    COMMTIMEOUTS TimeOuts; 
    TimeOuts.ReadIntervalTimeout = 1000;
    TimeOuts.ReadTotalTimeoutMultiplier = 0;
    TimeOuts.ReadTotalTimeoutConstant = 0;
    TimeOuts.WriteTotalTimeoutMultiplier = 0;
    TimeOuts.WriteTotalTimeoutConstant = 500;
    SetCommTimeouts(hCom, &TimeOuts); 

    DCB dcb;
    GetCommState(hCom, &dcb);
    dcb.BaudRate = baud_rate;
    dcb.ByteSize = 8; 
    dcb.Parity = NOPARITY; 
    dcb.StopBits = ONESTOPBIT; 
    if (!SetCommState(hCom, &dcb))
    {
        return false;
    }

    // clear the cache
    PurgeComm(hCom, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
#else
    hCom = open(com_id, O_RDWR | O_NOCTTY);
  	// Read in existing settings, and handle any error
    struct termios tty;
  	if (tcgetattr(hCom, &tty) != 0) {
      	printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
      	return false;
  	}
  	tty.c_cflag &= ~PARENB;        // Clear parity bit, disabling parity (most common)
  	tty.c_cflag &= ~CSTOPB;        // Clear stop field, only one stop bit used in communication (most common)
	tty.c_cflag &= ~CSIZE;         // Clear all bits that set the data size 
	tty.c_cflag |= CS8;            // 8 bits per byte (most common)
	tty.c_cflag &= ~CRTSCTS;       // Disable RTS/CTS hardware flow control (most common)
	tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO;          // Disable echo
	tty.c_lflag &= ~ECHOE;         // Disable erasure
	tty.c_lflag &= ~ECHONL;        // Disable new-line echo
	tty.c_lflag &= ~ISIG;          // Disable interpretation of INTR, QUIT and SUSP
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
	tty.c_oflag &= ~OPOST;         // Prevent special interpretation of output bytes (e.g. newline chars)

	tty.c_cc[VTIME] = 0;           // Wait for up to s (10 deciseconds), returning as soon as any data is received.
	tty.c_cc[VMIN] = 0;

	tcflush(hCom, TCIFLUSH);

	// other baud rate need to add
    switch (baud_rate) {
        case 9600:
            cfsetispeed(&tty, B9600);
            cfsetospeed(&tty, B9600);
            break;
        case 115200:
            cfsetispeed(&tty, B115200);
            cfsetospeed(&tty, B115200);
            break;
        default:
            return 0;
    }

	// Save tty settings, also checking for error
	if (tcsetattr(hCom, TCSANOW, &tty) != 0) {
		printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
		return false;
	}
#endif

   return true;
}

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
bool servo_motor::position_control(unsigned char id, int targert_positin, int period)
{ 
    if (lock) {
        return false;
    }

    lock = 1; // lock the com 

#ifdef WIN32
    COMSTAT ComStat; 
    DWORD dwErrorFlags; 
    ClearCommError(hCom, &dwErrorFlags, &ComStat); 
#endif

    BYTE write_buffer[11] = { 
        0x55, 0x55, 
        id, 
        7, 
        1, 
        BYTE(targert_positin % 256), BYTE(targert_positin / 256),
        BYTE(period % 256), BYTE(period / 256),
        0, '\n'}; 
    // check bit 
    int sum = 0; 
    for (int i = 2; i < 9; ++ i) { 
        sum += write_buffer[i]; 
    } 
    write_buffer[9] = 255 - sum % 256;
    DWORD dwBytesWrite = 10;

#ifdef WIN32
    if (!WriteFile(
#else
    if (!write(
#endif
                hCom, 
                write_buffer, 
                dwBytesWrite
#ifdef WIN32
                ,
                &dwBytesWrite, 
                NULL
#endif
        )) {
        lock = 0; 
        return false; 
    } 

    lock = 0; 
    return true; 
} 

/*
 * function: close_serial_port
 *     close the serial port
 * input:
 * output: 
 *     state[int]
 */
bool servo_motor::close_serial_port()
{
#ifdef WIN32
    return CloseHandle(hCom);
#else
	return close(hCom);
#endif
}
