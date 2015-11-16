/* 
 * File:   Message.h
 * Author: rday
 *
 * Created on November 9, 2015, 6:03 AM
 */

#ifndef MESSAGE_H
#define	MESSAGE_H

#include <string.h>
#include <stdint.h>

// Controls
#define SOM                     0xf0
#define EOM                     0xf1

// States for machine
#define WAITING                 0x00
#define GOT_SOM                 0x01
#define READING_MESSAGE         0x02
#define MESSAGE_READ            0x03
#define VALID_MESSAGE           0x04

// Message data can't be more than MAX_SIZE
#define DATA_SIZE                0x10
#define MAX_SIZE                DATA_SIZE + 1   // SOM needs space too

class Message
{
	public:
            Message();
            int8_t AddByte(unsigned char ch);
            int8_t AddBytes(unsigned char *bytes, unsigned int size);
            unsigned char *Serialize();
            uint8_t WireSize();
            void Reset();
	private:
            unsigned char _messageData[MAX_SIZE + 1];   // EOM will be the last byte
            uint8_t _dataPtr = 2;                       // Start of data segment
};

#endif	/* MESSAGE_H */
