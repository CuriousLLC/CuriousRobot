/* 
 * File:   Servo.h
 * Author: rday
 *
 * Created on November 2, 2015, 11:03 AM
 */

#ifndef CURIOUSSERVO_H
#define	CURIOUSSERVO_H

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

        void clearExpiredRotation()
        {
            if (millis() >= _rotationEndTime) {
                setDutyCycle(1500);
                _rotationEndTime = 0;
            }
        }

        void setRotateDuration(unsigned long duration)
        {
            _rotationStartTime = millis();
            _rotationEndTime = _rotationStartTime + duration;
        }

        int8_t setDutyCycle(uint16_t dt)
        { 
            if (type & INVERSE) {
                dt = (1500 - dt) + 1500;
            }

            if (900 <= dt && dt <= 2100) {
                _dutyCycle = dt;
                _servo.writeMicroseconds(dt);
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

