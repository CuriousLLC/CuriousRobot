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

/**
 * When adding multiple bytes at once, this is faster than calling
 * AddByte() over and over.
 * @param bytes Pointer to array of bytes
 * @param size Number of bytes to add
 * @return -1 if we would have exceeded our buffer. 0 for success
 */
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

/**
 * A serialize message looks like
 *    SOM + Size + Data + EOM
 * 
 * @return Pointer to serialized message
 */
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
 * Reset this message back to the default state.
 * Useful in loops where you want to save allocations.
 */
void Message::Reset()
{
    memset(_messageData, 0, sizeof(_messageData));
    _messageData[0] = SOM;
    _dataPtr = 2;
}