Robot Message Controller
========================

* Teensy LC

This package will listen for incoming data on Serial1 (GPIO0), and parses bytes into valid messages.
Messages will tell the controller to configure a new servo, or rotate a servo (or servos) for a period
of time.

Each servo is configured with a mask. The lower 4 bits of the mask specify the type of servo:

* 0000 - Standard
* 0001 - Continuous

The upper 4 bits describe how to interpret rotation requests:

* 0000 - Standard
* 1000 - Invserse

An *inverse* servo will rotate in the opposite direction as requested. This is because two wheels
will be controlled by servos that face opposing directions.

There are several messages types:

* Add servo - Adds a new servo with a mask on a GPIO
* Rotate type - Rotates every servo of a given type. This prevents the user from sending multiple messages to move each wheel
* Rotate servo - Rotates a single servo. This allows the user to turn in one direction.
* Rotate type duration - Rotates every servo of a given type for a given period of time.
* Rotate servo duration - Rotates a single servo for a given period of time.
