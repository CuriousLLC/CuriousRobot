#ifndef ServoController_h
#define ServoController_h

#include <Servo/Servo.h>
#include <stdint.h>
#include <stdlib.h>
#include <TimerOne/TimerOne.h>
#include <TimerThree/TimerThree.h>
#include "CuriousServo.h"

class ServoController
{
    public:
        ServoController(uint8_t servoMax);
        ~ServoController();
        int8_t addServo(uint8_t type, uint8_t pwm);
        CuriousServo *getServo(uint8_t id);
        int8_t rotateServoType(uint8_t mask, uint16_t dutyCycle);
        int8_t rotateServoGpio(uint8_t mask, uint16_t dutyCycle);
        int8_t rotateServoTypeDuration(uint8_t mask, uint16_t pulseWidth, uint16_t milliSeconds);
        int8_t rotateServoGpioDuration(uint8_t mask, uint16_t pulseWidth, uint16_t milliSeconds);
        uint8_t get_count() const { return _servoCount; }
        void checkExpiration();
        /*static TimerOne timerOne;
        static TimerThree timerThree;
        static void timerOneISR() { timerOne.detachInterrupt(); timerOne.isrCallback = NULL; }
        static void timerThreeISR() { timerThree.detachInterrupt(); timerOne.isrCallback = NULL; }*/
    private:
        CuriousServo **_servoList;
        uint8_t _servoCount;
        uint8_t _servoMax;

};

#endif