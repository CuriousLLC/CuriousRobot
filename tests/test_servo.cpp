#include "catch.hpp"
#include "catch_tests.h"
#include <string.h>
#include <sstream>
#include <iostream>
#include <exception>
#include <CuriousRobot/CuriousServo.h>

TEST_CASE( "Create a servo", "[Servo]") {
    CuriousServo servo(1, 1, 1);

    REQUIRE( servo.getDutyCycle() == 1500 );
    REQUIRE( servo.isRotating() == false );
}
