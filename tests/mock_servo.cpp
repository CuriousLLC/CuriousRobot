#include "catch_tests.h"
#include <CuriousRobot/CuriousServo.h>

// Servo.h will only provide an implementation when we are compiling
// on an AVR or ARM architecture. This mock implementation will fill
// in when linking our tests.

Servo::Servo()
{
	servoIndex = 0;
}

uint8_t Servo::attach(int pin)
{
	return servoIndex++;
}

void Servo::detach()  
{
}

void Servo::write(int value)
{
    writeMicroseconds(value);
}

void Servo::writeMicroseconds(int value)
{
    return;
}

int Servo::read()
{
    return 0;
}

int Servo::readMicroseconds()
{
    return 0;
}

bool Servo::attached()
{
    return true;
}
