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

enum ServoType {
    Positional = (uint8_t)0,
    Continuous
};

class CuriousServo
{
    public:
        CuriousServo(uint8_t _type, uint8_t _id, uint8_t _gpio)
        {
            type = _type;
            id = _id;
            gpio = _gpio;
            _dutyCycle = 1500;  // Use a safe init value
            expireTime = 0;
            _setTime = 0;
            _servo.attach(_gpio);
        }

        uint8_t type;
        uint8_t id;
        uint8_t gpio;
        unsigned long expireTime;

        void checkExpired()
        {
            unsigned long msecs = millis();
            
            if (msecs - _setTime > expireTime) {
                set_dutyCycle(1500);
                expireTime = 0;
            }
        }

        void setExpire(unsigned long duration) { expireTime = duration; _setTime = millis(); }
        uint16_t get_dutyCycle() const { return _dutyCycle; }
        int8_t set_dutyCycle(uint16_t dt)
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
        unsigned long _setTime;
};


#endif	/* CURIOUSSERVO_H */

