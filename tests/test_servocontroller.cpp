#include "catch.hpp"
#include <string.h>
#include <sstream>
#include <iostream>
#include <exception>
#include <kinetis.h>
#include <TimerOne/config/known_16bit_timers.h>
#include <CuriousRobot/ServoController.h>

TEST_CASE( "Limit servo count", "[ServoController]" ) {
    ServoController ctrl(3);

    ctrl.addServo(0x01, 1);
    ctrl.addServo(0x81, 2);
    ctrl.addServo(0x01, 3);

    REQUIRE( ctrl.addServo(0x01, 4) == -2 );
}

TEST_CASE( "Verify indexing", "[ServoController]" ) {
    ServoController ctrl(3);

    ctrl.addServo(0x01, 1);
    ctrl.addServo(0x81, 2);
    ctrl.addServo(0x01, 3);

    REQUIRE( ctrl.getServo(1)->type == 0x81 );
}

TEST_CASE( "Rotate servo", "[ServoController]" ) {
    ServoController ctrl(3);

    ctrl.addServo(0x01, 1);
    ctrl.addServo(0x81, 2);
    ctrl.addServo(0x00, 3);

    ctrl.rotateServoType(0x01, 2000);
    
    // Normal rotation
    REQUIRE( ctrl.getServo(0)->getDutyCycle() == 2000 );
    
    // Inverse rotation
    REQUIRE( ctrl.getServo(1)->getDutyCycle() == 1000 );

    // Different type
    REQUIRE( ctrl.getServo(2)->getDutyCycle() == 1500 );
}

TEST_CASE( "Rotate servo too far", "[ServoController]" ) {
    ServoController ctrl(3);

    ctrl.addServo(0x01, 1);
    ctrl.addServo(0x81, 2);
    ctrl.addServo(0x01, 3);

    ctrl.rotateServoType(0x01, 2200);
    
    REQUIRE( ctrl.getServo(0)->getDutyCycle() == 1500 );
    REQUIRE( ctrl.getServo(1)->getDutyCycle() == 1500 );
    REQUIRE( ctrl.getServo(2)->getDutyCycle() == 1500 );
}
