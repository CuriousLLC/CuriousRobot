/* 
 * File:   Servo.h
 * Author: rday
 *
 * Created on November 2, 2015, 11:03 AM
 */

#ifndef CURIOUSSERVO_H
#define	CURIOUSSERVO_H

#include "core_pins.h"
#include <Servo/Servo.h>
#include <stdlib.h>
#include <stdint.h>

#define INVERSE     0x80        // Set the inverse bit if the wheel
                                // faces the other direction

class CuriousServo
{
    public:
        uint8_t type;
        uint8_t id;
        uint8_t gpio;

        CuriousServo(uint8_t _type, uint8_t _id, uint8_t _gpio)
        {
            type = _type;
            id = _id;
            gpio = _gpio;
            _dutyCycle = 1500;  // Use a safe init value
            _rotationEndTime = 0;
            _rotationStartTime = 0;
            _servo.attach(_gpio);
        }

        uint8_t isRotating() const { return _rotationEndTime > 0; }
        uint16_t getDutyCycle() const { return _dutyCycle; }

        /**
         * Set the duty cycle of this servo to mid-range if
         * there was a rotation which has now expired.
         */
        void clearExpiredRotation()
        {
            if (millis() >= _rotationEndTime) {
                setDutyCycle(1500);
                _rotationEndTime = 0;
            }
        }

        /**
         * Set the duration for a current rotation to _duration_
         * @param duration Milliseconds to rotate
         */
        void setRotateDuration(unsigned long duration)
        {
            _rotationStartTime = millis();
            _rotationEndTime = _rotationStartTime + duration;
        }

        /**
         * Set this servos duty cycle to _pulseWidth_
         * @param pulseWidth Microsecond pulse width
         * @return -1 if width outside acceptable range, 0 if success
         */
        int8_t setDutyCycle(uint16_t pulseWidth)
        {
            // If this servo is an inverse servo, then it should rotate
            // in the opposite direction.
            if (type & INVERSE) {
                pulseWidth = (1500 - pulseWidth) + 1500;
            }

            // Try to keep the requested pulse width within normal range
            if (900 <= pulseWidth && pulseWidth <= 2100) {
                _dutyCycle = pulseWidth;
                _servo.writeMicroseconds(pulseWidth);
                return 0;
            }
            
            return -1;
        }
            
    private:
        Servo _servo;
        uint16_t _dutyCycle;
        unsigned long _rotationStartTime;
        unsigned long _rotationEndTime;

};


#endif	/* CURIOUSSERVO_H */

