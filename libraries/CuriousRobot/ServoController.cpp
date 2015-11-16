#include "ServoController.h"

ServoController::ServoController(uint8_t servoMax)
{
    _servoList = (CuriousServo **) malloc (sizeof (CuriousServo*) * servoMax);
    _servoCount = 0;
    _servoMax = servoMax;
}

ServoController::~ServoController()
{
    for(uint8_t i=0; i<_servoCount; i++) {
        free(_servoList[i]);
    }

    free(_servoList);
}

int8_t  ServoController::addServo(uint8_t type, uint8_t pwm)
{
    if (_servoCount == _servoMax) {
        return -2;
    }

    CuriousServo *servo = new CuriousServo(type, _servoCount, pwm);
    if (servo == NULL) {
        return -1;
    }

    _servoList[_servoCount++] = servo;
    
    return servo->id;
}

CuriousServo * ServoController::getServo(uint8_t id)
{
    if (id >= _servoCount) {
        return NULL;
    }

    return _servoList[id];
}

int8_t ServoController::rotateServoType(uint8_t mask, uint16_t milliSeconds)
{
    for (uint8_t i=0; i<_servoCount; i++) {
        CuriousServo *servo = _servoList[i];
        
        if ((servo->type & 0x0f) == (mask & 0x0f)) {
            servo->set_dutyCycle(milliSeconds);
        }
    }

    return 1;
}

int8_t ServoController::rotateServoGpio(uint8_t gpio, uint16_t milliSeconds)
{
    for (uint8_t i=0; i<_servoCount; i++) {
        CuriousServo *servo = _servoList[i];
        
        if (servo->gpio == gpio) {
            servo->set_dutyCycle(milliSeconds);
        }
    }

    return 1;
}

int8_t ServoController::rotateServoTypeDuration(uint8_t mask, uint16_t pulseWidth, uint16_t milliSeconds)
{
    for (uint8_t i=0; i<_servoCount; i++) {
        CuriousServo *servo = _servoList[i];
        
        if ((servo->type & 0x0f) == (mask & 0x0f)) {
            servo->set_dutyCycle(pulseWidth);
            servo->setExpire(milliSeconds);
            /*
            if (timerOne.isrCallback == NULL) {
                unsigned long usecs = (unsigned long)millis * 1000;
                timerOne.initialize(usecs);
                timerOne.attachInterrupt(timerOneISR);
            } else if (timerThree.isrCallback == NULL) {
                unsigned long usecs = (unsigned long)millis * 1000;
                timerThree.initialize(usecs);
                timerThree.attachInterrupt(timerThreeISR);
            }*/

        }
    }

    return 1;
}

int8_t ServoController::rotateServoGpioDuration(uint8_t gpio, uint16_t pulseWidth, uint16_t milliSeconds)
{
    for (uint8_t i=0; i<_servoCount; i++) {
        CuriousServo *servo = _servoList[i];
        
        if (servo->gpio == gpio) {
            servo->set_dutyCycle(pulseWidth);
            servo->setExpire(milliSeconds);
            /*
            if (timerOne.isrCallback == NULL) {
                unsigned long usecs = (unsigned long)millis * 1000;
                timerOne.initialize(usecs);
                timerOne.attachInterrupt(timerOneISR);
            } else if (timerThree.isrCallback == NULL) {
                unsigned long usecs = (unsigned long)millis * 1000;
                timerThree.initialize(usecs);
                timerThree.attachInterrupt(timerThreeISR);
            }*/
        }
    }

    return 1;
}

void ServoController::checkExpiration()
{
    for (uint8_t i=0; i<_servoCount; i++) {
        CuriousServo *servo = _servoList[i];
        
        if (servo->expireTime > 0) {
            servo->checkExpired();
        }
    }
}