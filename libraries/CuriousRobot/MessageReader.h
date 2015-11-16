#ifndef MessageReader_h
#define MessageReader_h

#include "Message.h"
#include "../QueueArray/QueueArray.h"

template <class T>
class QueueArray;

class MessageReader
{
	public:
            MessageReader();
            int AddChar(char ch);
            unsigned char *ProcessQueue();
            unsigned char GetState();
	private:
            void _resetState();
            QueueArray <char> _queue;
            unsigned char _currentState;
            unsigned char _currentRead;
            unsigned char _currentMessageSize;
            unsigned char _message[MAX_SIZE + 1];   // +1 for EOM
};

#endif
