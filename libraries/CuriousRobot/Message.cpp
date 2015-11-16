#include "Message.h"

Message::Message()
{
    memset(_messageData, 0, sizeof(_messageData));
    _messageData[0] = SOM;
}

int8_t Message::AddByte(unsigned char ch)
{
    if (_dataPtr != MAX_SIZE) {
        _messageData[_dataPtr++] = ch;
        return 0;
    }
    
    return -1;
}

int8_t Message::AddBytes(unsigned char *bytes, unsigned int size)
{
    if (_dataPtr + size < (MAX_SIZE)) {
        for (unsigned int i=0; i<size; i++) {
            _messageData[_dataPtr++] = bytes[i];
        }
        
        return 0;
    }
    
    return -1;
}

unsigned char *Message::Serialize()
{
    // The last byte will be set as the EOM.
    _messageData[_dataPtr] = EOM;

    // Set the message size to the size of the data.
    // Size, SOM and EOM are not included in the count.
    _messageData[1] = _dataPtr - 2;

    return _messageData;
}

/*
 * This is the size of the message on the wite on the wite
 */
uint8_t Message::WireSize()
{
    return _dataPtr + 1;    // Makes sure EOM has room
}

/*
 * Reset this message back to the default state.
 * Useful in loops where you want to save allocations.
 */
void Message::Reset()
{
    memset(_messageData, 0, sizeof(_messageData));
    _messageData[0] = SOM;
    _dataPtr = 2;
}