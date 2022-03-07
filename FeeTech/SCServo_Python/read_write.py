#!/usr/bin/env python
#
# *********     Gen Write Example      *********
#
#
# Available SCServo model on this example : All models using Protocol SCS
# This example is tested with a SCServo(STS/SMS/SCS), and an URT
# Be sure that SCServo(STS/SMS/SCS) properties are already set as %% ID : 1 / Baudnum : 6 (Baudrate : 1000000)
#

import os

if os.name == 'nt':
    import msvcrt
    def getch():
        return msvcrt.getch().decode()
        
else:
    import sys, tty, termios
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    def getch():
        try:
            tty.setraw(sys.stdin.fileno())
            ch = sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        return ch

from scservo_sdk import *                    # Uses SCServo SDK library

# Control table address
ADDR_SCS_TORQUE_ENABLE     = 40                 #[扭矩开关]写0：关闭扭力输出； 写1：打开扭力输出
ADDR_SCS_GOAL_ACC          = 41                 #[加速度](100步/s^2)如设置为10 则按1000步每秒平方的加减速度变速
ADDR_SCS_GOAL_POSITION     = 42                 #[目标位置](步)
ADDR_SCS_GOAL_SPEED        = 46                 #[运行速度0-254]每秒内运动的步数,(50步/秒= 0.732 RPM)
ADDR_SCS_PRESENT_POSITION  = 56                 #[当前位置]反馈当前所处位置的步数，每步为一个最小分辨角度；绝对位置控制方式，最大值对应最大有效角度
ADDR_SCS_PRESENT_LOAD      = 60                 #[当前负载](0.1%)当前控制输出驱动电机的电压占空比
# ADDR_SCS_OVERLOAD_TORQUE   = 36                 #[过载扭矩]0-100%触发过载保护的输出扭矩
# ADDR_SCS_OVERLOAD_TIME     = 35                 #[保护时间](10ms)当前负载输出超过过载扭力并保持的计时时长，如设100表示1秒，最大可设2.5秒
# ADDR_SCS_OVERLOAD_TORQUE_SAFETY = 34            #[保护扭矩](1.0%)进入过载保护后输出扭矩，如设40表示40%的最大扭矩
ADDR_SCS_TORQUE_LIMIT      = 48                 #[转矩限制1-1000](0.1%)上电初始值会由最大扭矩（0x10）赋值，用户可程序修改此值来控制最大扭矩的输出


# Default setting
SCS_ID                      = 1                 # SCServo ID : 1
BAUDRATE                    = 115200            # SCServo default baudrate : 1000000
DEVICENAME                  = 'COM11'           # Check which port is being used on your controller
                                                # ex) Windows: "COM1"   Linux: "/dev/ttyUSB0" Mac: "/dev/tty.usbserial-*"

SCS_MINIMUM_POSITION_VALUE  = 0           # SCServo will rotate between this value
SCS_MAXIMUM_POSITION_VALUE  = 4096        # and this value (note that the SCServo would not move when the position value is out of movable range. Check e-manual about the range of the SCServo you use.)
SCS_MOVING_STATUS_THRESHOLD = 20          # SCServo moving status threshold
SCS_MOVING_SPEED            = 0           # SCServo moving speed 0-254
SCS_MOVING_ACC              = 0           # SCServo moving acc 0-254
protocol_end                = 0           # SCServo bit end(STS/SMS=0, SCS=1)
SCS_PRESENT_LOAD            = 0           #[当前负载](0.1%)当前控制输出驱动电机的电压占空比
# SCS_OVERLOAD_TORQUE         = 1           #[过载扭矩]0-100%触发过载保护的输出扭矩
# SCS_OVERLOAD_TIME           = 12          #[保护时间](10ms)当前负载输出超过过载扭力并保持的计时时长，如设100表示1秒，最大可设2.5秒
# SCS_OVERLOAD_TORQUE_SAFETY  = 1           #[保护扭矩](1.0%)进入过载保护后输出扭矩，如设40表示40%的最大扭矩(保护扭矩<过载扭矩)
SCS_TORQUE_LIMIT_REDUCED      = 100         #[转矩限制1-1000](0.1%)上电初始值会由最大扭矩（0x10）赋值，用户可程序修改此值来控制最大扭矩的输出
SCS_TORQUE_LIMIT              = 1000        #[转矩限制1-1000](0.1%)上电初始值会由最大扭矩（0x10）赋值，用户可程序修改此值来控制最大扭矩的输出

index = 0
scs_goal_position = [SCS_MINIMUM_POSITION_VALUE, SCS_MAXIMUM_POSITION_VALUE]         # Goal position


# Initialize PortHandler instance
# Set the port path
# Get methods and members of PortHandlerLinux or PortHandlerWindows
portHandler = PortHandler(DEVICENAME)

# Initialize PacketHandler instance
# Get methods and members of Protocol
packetHandler = PacketHandler(protocol_end)
    
# Open port
if portHandler.openPort():
    print("Succeeded to open the port")
else:
    print("Failed to open the port")
    print("Press any key to terminate...")
    getch()
    quit()

# Set port baudrate
if portHandler.setBaudRate(BAUDRATE):
    print("Succeeded to change the baudrate")
else:
    print("Failed to change the baudrate")
    print("Press any key to terminate...")
    getch()
    quit()

# Write SCServo acc
scs_comm_result, scs_error = packetHandler.write1ByteTxRx(portHandler, SCS_ID, ADDR_SCS_GOAL_ACC, SCS_MOVING_ACC)
if scs_comm_result != COMM_SUCCESS:
    print("%s" % packetHandler.getTxRxResult(scs_comm_result))
elif scs_error != 0:
    print("%s" % packetHandler.getRxPacketError(scs_error))

# # Overload control
# # Write SCServo overload torque
# scs_comm_result, scs_error = packetHandler.write1ByteTxRx(portHandler, SCS_ID, ADDR_SCS_OVERLOAD_TORQUE, SCS_OVERLOAD_TORQUE)
# if scs_comm_result != COMM_SUCCESS:
#     print("%s" % packetHandler.getTxRxResult(scs_comm_result))
# elif scs_error != 0:
#     print("%s" % packetHandler.getRxPacketError(scs_error))
# # Write SCServo overload torque
# scs_comm_result, scs_error = packetHandler.write1ByteTxRx(portHandler, SCS_ID, ADDR_SCS_OVERLOAD_TIME, SCS_OVERLOAD_TIME)
# if scs_comm_result != COMM_SUCCESS:
#     print("%s" % packetHandler.getTxRxResult(scs_comm_result))
# elif scs_error != 0:
#     print("%s" % packetHandler.getRxPacketError(scs_error))
# # Write SCServo overload torque
# scs_comm_result, scs_error = packetHandler.write1ByteTxRx(portHandler, SCS_ID, ADDR_SCS_OVERLOAD_TORQUE_SAFETY, SCS_OVERLOAD_TORQUE_SAFETY)
# if scs_comm_result != COMM_SUCCESS:
#     print("%s" % packetHandler.getTxRxResult(scs_comm_result))
# elif scs_error != 0:
#     print("%s" % packetHandler.getRxPacketError(scs_error))

# Write SCServo speed
scs_comm_result, scs_error = packetHandler.write2ByteTxRx(portHandler, SCS_ID, ADDR_SCS_GOAL_SPEED, SCS_MOVING_SPEED)
if scs_comm_result != COMM_SUCCESS:
    print("%s" % packetHandler.getTxRxResult(scs_comm_result))
elif scs_error != 0:
    print("%s" % packetHandler.getRxPacketError(scs_error))

while 1:
    print("Press any key to continue! (or press ESC to quit!)")
    if getch() == chr(0x1b):
        break

    # Set max torque output
    scs_comm_result, scs_error = packetHandler.write2ByteTxRx(portHandler, SCS_ID, ADDR_SCS_TORQUE_LIMIT, SCS_TORQUE_LIMIT)
    if scs_comm_result != COMM_SUCCESS:
        print("%s" % packetHandler.getTxRxResult(scs_comm_result))
    elif scs_error != 0:
        print("%s" % packetHandler.getRxPacketError(scs_error))

    # Write SCServo goal position
    scs_comm_result, scs_error = packetHandler.write2ByteTxRx(portHandler, SCS_ID, ADDR_SCS_GOAL_POSITION, scs_goal_position[index])
    if scs_comm_result != COMM_SUCCESS:
        print("%s" % packetHandler.getTxRxResult(scs_comm_result))
    elif scs_error != 0:
        print("%s" % packetHandler.getRxPacketError(scs_error))

    while 1:
        # Read SCServo present position (and speed since read4Byte)
        scs_present_position_speed, scs_comm_result, scs_error = packetHandler.read4ByteTxRx(portHandler, SCS_ID, ADDR_SCS_PRESENT_POSITION)
        if scs_comm_result != COMM_SUCCESS:
            print(packetHandler.getTxRxResult(scs_comm_result))
        elif scs_error != 0:
            print(packetHandler.getRxPacketError(scs_error))

        # Read SCServo present load
        scs_present_load, scs_comm_result, scs_error = packetHandler.read2ByteTxRx(portHandler, SCS_ID, ADDR_SCS_PRESENT_LOAD)
        if scs_comm_result != COMM_SUCCESS:
            print(packetHandler.getTxRxResult(scs_comm_result))
        elif scs_error != 0:
            print(packetHandler.getRxPacketError(scs_error))

        scs_present_position = SCS_LOWORD(scs_present_position_speed)
        scs_present_speed = SCS_HIWORD(scs_present_position_speed)
        # print("[ID:%03d] GoalPos:%03d PresPos:%03d PresSpd:%03d PresLoad:%03d"
        #       % (SCS_ID, scs_goal_position[index], scs_present_position, SCS_TOHOST(scs_present_speed, 15), SCS_TOHOST(scs_present_load, 10)))

        # if this can stop the motor?
        if abs(SCS_TOHOST(scs_present_load, 10)) > 950:
            print("[ID:%03d] Big load detected!" % (SCS_ID))
            # Reduce torque output
            scs_comm_result, scs_error = packetHandler.write2ByteTxRx(portHandler, SCS_ID, ADDR_SCS_TORQUE_LIMIT, SCS_TORQUE_LIMIT_REDUCED)
            if scs_comm_result != COMM_SUCCESS:
                print("%s" % packetHandler.getTxRxResult(scs_comm_result))
            elif scs_error != 0:
                print("%s" % packetHandler.getRxPacketError(scs_error))

        if not (abs(scs_goal_position[index] - scs_present_position_speed) > SCS_MOVING_STATUS_THRESHOLD):
            break


    # Change goal position
    if index == 0:
        index = 1
    else:
        index = 0    

scs_comm_result, scs_error = packetHandler.write1ByteTxRx(portHandler, SCS_ID, ADDR_SCS_TORQUE_ENABLE, 0)
if scs_comm_result != COMM_SUCCESS:
    print("%s" % packetHandler.getTxRxResult(scs_comm_result))
elif scs_error != 0:
    print("%s" % packetHandler.getRxPacketError(scs_error))
# Close port
portHandler.closePort()