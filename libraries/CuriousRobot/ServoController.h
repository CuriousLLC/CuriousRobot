#ifndef ServoController_h
#define ServoController_h

#include <Servo/Servo.h>
#include <stdint.h>
#include <stdlib.h>
#include "CuriousServo.h"

class ServoController
{
    public:
        ServoController(uint8_t servoMax);
        ~ServoController();
        int8_t addServo(uint8_t type, uint8_t gpio);
        CuriousServo *getServo(uint8_t id) const;
        int8_t rotateServoType(uint8_t mask, uint16_t pulseWidth);
        int8_t rotateServoGpio(uint8_t mask, uint16_t pulseWidth);
        int8_t rotateServoTypeDuration(uint8_t mask, uint16_t pulseWidth, uint16_t milliSeconds);
        int8_t rotateServoGpioDuration(uint8_t mask, uint16_t pulseWidth, uint16_t milliSeconds);
        uint8_t get_count() const { return _servoCount; }
        void checkExpiration() const;
    private:
        CuriousServo **_servoList;
        uint8_t _servoCount;
        uint8_t _servoMax;

};

#endif
