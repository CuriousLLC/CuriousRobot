#include "catch.hpp"
#include <string.h>
#include <sstream>
#include <iostream>
#include <exception>
#include <CuriousRobot/Message.h>

TEST_CASE( "Serialize a message", "[Serialize]" ) {
    Message msg;
    
    msg.AddByte(0x01);
    msg.AddByte(0x02);
    msg.AddBytes((unsigned char *)"\x04\x04\x05", 3);
    msg.AddByte(0x06);
    
    REQUIRE( msg.WireSize() == 9 );
    REQUIRE( strcmp((const char *)msg.Serialize(), (const char*)"\xf0\x06\x01\x02\x04\x04\x05\x06\xf1") == 0);
}