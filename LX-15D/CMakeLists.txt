cmake_minimum_required(VERSION 3.1.0)
project(servo_motor)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Architecture
if (CMAKE_CL_64) 
    set(ADDRESS_MODEL 64)
    set(NODE_TARGET x64)
else()
    set(ADDRESS_MODEL 32)
    set(NODE_TARGET x86)
endif()

# OS
if (WIN32)
    message("OS: Windows")
    add_executable(${PROJECT_NAME}
        servo_motor.h
        servo_motor.cpp
        servo_motor_test_on_windows.cpp)
elseif (UNIX AND NOT APPLE) 
    message("OS: Linux")
    add_executable(${PROJECT_NAME}
        servo_motor.h
        servo_motor.cpp
        servo_motor_test_on_linux.cpp)
endif()


target_link_libraries(${PROJECT_NAME})
