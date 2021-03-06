#include "WProgram.h"
#include <stdint.h>

#include <CuriousRobot/MessageReader.h>
#include <CuriousRobot/ServoController.h>
#include <CuriousRobot/Message.h>

#define LED_PIN   13

#define ADD_SERVO               0x01
#define ROTATE_TYPE             0x02
#define ROTATE_ONE              0x03
#define ROTATE_TYPE_DURATION    0x04
#define ROTATE_ONE_DURATION     0x05
#define LIST                    0x10


static ServoController ctrl(3);
static unsigned int flashDelay;

/**
 * This function is our message processor. Any complete message can
 * be passed to this function, and we will preform the requested task.
 * @param message Pointer to message
 */
void processMessage(unsigned char *message) {
    uint8_t type = message[1];

    switch (type) {
        case ADD_SERVO:
        {
            // Add new servo
            uint8_t mask = (uint8_t) message[2];
            uint8_t gpio = (uint8_t) message[3];

            ctrl.addServo(mask, gpio);
            break;
        }
        case ROTATE_TYPE:
        {
            // Rotate all servos of a certain type. Useful for
            // moving straight forward or backward in a single
            // message.
            uint8_t mask = message[2];
            uint16_t pulseWidth = (message[3] << 8) + message[4];

            ctrl.rotateServoType(mask, pulseWidth);
            break;
        }
        case ROTATE_ONE:
        {
            // Rotate a single servo at _pulseWidth_. This would be
            // used for turning left or right.
            uint8_t gpio = message[2];
            uint16_t pulseWidth = (message[3] << 8) + message[4];

            ctrl.rotateServoGpio(gpio, pulseWidth);
        }
        case ROTATE_TYPE_DURATION:
        {
            // Rotate a type of servo at _pulseWidth_ for _milliSeconds_.
            // This let's the user send a single command which will move
            // a servo type for a certain period of time. This is much more
            // accurate than the user having to account for network delay.
            uint8_t mask = message[2];
            uint16_t pulseWidth = (message[3] << 8) + message[4];
            uint16_t milliSeconds = (message[5] << 8) + message[6];

            ctrl.rotateServoTypeDuration(mask, pulseWidth, milliSeconds);
        }
        case ROTATE_ONE_DURATION:
        {
            // Rotate a single servo at _pulseWidth_ for _milliSeconds_.
            // Again, we handle the duration internally. The user doesn't
            // have to stop us manually.
            uint8_t gpio = message[2];
            uint16_t pulseWidth = (message[3] << 8) + message[4];
            uint16_t milliSeconds = (message[5] << 8) + message[6];

            ctrl.rotateServoGpioDuration(gpio, pulseWidth, milliSeconds);
        }
        case LIST:
        {
            Message msg;
            for (uint8_t i = 0; i < ctrl.get_count(); i++) {
                CuriousServo *servo = ctrl.getServo(i);
                uint16_t dc = servo->getDutyCycle();

                msg.AddByte(i);
                msg.AddByte(servo->type);
                msg.AddByte(servo->gpio);
                msg.AddByte((dc >> 8) & 0x0ff);
                msg.AddByte((dc) & 0x0ff);
                Serial1.write(msg.Serialize(), msg.WireSize());
                msg.Reset();
            }
            break;
        }
    }
}

/**
 * Simple function to toggle the LED if a certain duration has passed
 */
void flashLed() {
    static uint8_t toggle = 0;
    static unsigned long lastUpdate = 0;

    if ((millis() - lastUpdate) > flashDelay) {
            digitalWrite(LED_PIN, toggle ^= 1);
            lastUpdate = millis();
        }
}

void setup() {
    Serial1.begin(115200, SERIAL_8N1);
    pinMode(LED_PIN, OUTPUT);
    flashDelay = 1000;
    Message msg;
    msg.AddBytes((unsigned char*)"CuriousRobot>", 13);
    Serial1.write(msg.Serialize(), msg.WireSize());
}

extern "C" int main(void) {
    unsigned char *message;
    MessageReader reader;

    setup();

    while (1) {
        switch (reader.GetState()) {
            case WAITING:
                // Flash once per second when waiting for a command
                flashDelay = 1000;
                break;
            default:
                // If we get stuck inside a command, flash faster
                flashDelay = 500;
                break;
        }

        // Find out if we have a completed message sitting in our buffer.
        // If so, process it. The returned message is pointing to the
        // actual buffer that ProcessQueue uses. So we have to process
        // the message, or it will be overwritten.
        // This works because there is no OS to preempt us, and no interrupts
        // will mess with our buffer.
        message = reader.ProcessQueue();
        if (message != NULL) {
            processMessage(message);
        }

        // Display the system status
        flashLed();

        // Let the servo controller clear any expired movements
        ctrl.checkExpiration();

        while (Serial1.available()) {
            if (reader.AddChar(Serial1.read()) < 0) {
                // If there was an error adding the character,
                // go into error mode.
                flashDelay = 250;
            }
        }
    }
    
    // Never reached
    return 0;
}
