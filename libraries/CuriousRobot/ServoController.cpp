#include "ServoController.h"

/**
 * Allocate enough memory to handle _servoMax_ servos all at once.
 * @param servoMax
 */
ServoController::ServoController(uint8_t servoMax)
{
    _servoList = (CuriousServo **) malloc (sizeof (CuriousServo*) * servoMax);
    _servoCount = 0;
    _servoMax = servoMax;
}

/**
 * Free all the servos, then free the list
 */
ServoController::~ServoController()
{
    for(uint8_t i=0; i<_servoCount; i++) {
        free(_servoList[i]);
    }

    free(_servoList);
}

/**
 * Add a servo using _mask_ at _gpio_
 * @param mask Upper 4 bits tell whether this is an inverse servo, lower 4 bits
 * tell the type of servo
 * @param gpio GPIO of servo
 * @return -2 if maximum servo count is exceeded. 
 *          -1 If servo could not be allocated
 *          > 0 index of the servo in the list
 */
int8_t  ServoController::addServo(uint8_t mask, uint8_t gpio)
{
    if (_servoCount == _servoMax) {
        return -2;
    }

    CuriousServo *servo = new CuriousServo(mask, _servoCount, gpio);
    if (servo == NULL) {
        return -1;
    }

    _servoList[_servoCount++] = servo;
    
    return servo->id;
}

/**
 * Return the servo object located at index _id_
 * @param id Index of requested servo
 * @return CuriousServo pointer
 */
CuriousServo * ServoController::getServo(uint8_t id) const
{
    if (id >= _servoCount) {
        return NULL;
    }

    return _servoList[id];
}

/**
 * Set the duty cycle of the servo at _gpio_ to _pulseWidth_
 * @param mask The type of servos to adjust (lower 4 bits)
 * @param pulseWidth Microsecond pulseWidth. Should be safe for your servo type
 * @return 1 for success
 */
int8_t ServoController::rotateServoType(uint8_t mask, uint16_t pulseWidth)
{
    for (uint8_t i=0; i<_servoCount; i++) {
        CuriousServo *servo = _servoList[i];
        
        if ((servo->type & 0x0f) == (mask & 0x0f)) {
            servo->setDutyCycle(pulseWidth);
        }
    }

    return 1;
}

/**
 * Set the duty cycle of the servo at _gpio_ to _pulseWidth_
 * @param gpio GPIO of the servo
 * @param pulseWidth Microsecond pulseWidth. Should be safe for your servo type
 * @return 1 for success
 */
int8_t ServoController::rotateServoGpio(uint8_t gpio, uint16_t pulseWidth)
{
    for (uint8_t i=0; i<_servoCount; i++) {
        CuriousServo *servo = _servoList[i];
        
        if (servo->gpio == gpio) {
            servo->setDutyCycle(pulseWidth);
        }
    }

    return 1;
}

/**
 * Set the duty cycle of each servo matching _mask_ to _pulseWidth_. Also set 
 * the duration to _milliSeconds_ so this rotation will stop automatically.
 * @param mask The type of servos to adjust (lower 4 bits)
 * @param pulseWidth Microsecond pulseWidth. Should be safe for your servo type
 * @param milliSeconds Duration of rotation in milliSeconds
 * @return 1 for success
 */
int8_t ServoController::rotateServoTypeDuration(uint8_t mask, uint16_t pulseWidth, uint16_t milliSeconds)
{
    for (uint8_t i=0; i<_servoCount; i++) {
        CuriousServo *servo = _servoList[i];
        
        if ((servo->type & 0x0f) == (mask & 0x0f)) {
            servo->setDutyCycle(pulseWidth);
            servo->setRotateDuration(milliSeconds);
        }
    }

    return 1;
}

/**
 * Set the duty cycle of _gpio_ to _pulseWidth_. Also set the duration to
 * _milliSeconds_ so this rotation will stop automatically.
 * @param gpio GPIO of the servo
 * @param pulseWidth Microsecond pulseWidth. Should be safe for your servo type
 * @param milliSeconds Duration of rotation in milliSeconds
 * @return 1 for success
 */
int8_t ServoController::rotateServoGpioDuration(uint8_t gpio, uint16_t pulseWidth, uint16_t milliSeconds)
{
    for (uint8_t i=0; i<_servoCount; i++) {
        CuriousServo *servo = _servoList[i];
        
        if (servo->gpio == gpio) {
            servo->setDutyCycle(pulseWidth);
            servo->setRotateDuration(milliSeconds);
        }
    }

    return 1;
}

/**
 * Loop through the servo list and clear out any expired rotations.
 * These rotations will have been started from the ROTATE_TYPE_DURATION
 * or ROTATE_SERVO_DURATION messages.
 * 
 * In the next revision, we should set a system timer to handle this. We 
 * only have two servos, so we have enough timers to control rotations.
 */
void ServoController::checkExpiration() const
{
    for (uint8_t i=0; i<_servoCount; i++) {
        CuriousServo *servo = _servoList[i];
        
        if (servo->isRotating()) {
            servo->clearExpiredRotation();
        }
    }
}