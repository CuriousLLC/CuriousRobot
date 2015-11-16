#include "MessageReader.h"


MessageReader::MessageReader()
{
    _currentState = WAITING;
    _currentRead = 0;
    _currentMessageSize = 0x0;
}

// Called by external function to add character to the
// processing queue.
int MessageReader::AddChar(char ch)
{
    return _queue.enqueue(ch);
}

unsigned char MessageReader::GetState()
{
    return _currentState;
}

void MessageReader::_resetState()
{
    _currentRead = 0;
    _currentMessageSize = 0;
    _currentState = WAITING;
}

// Process all data in the queue. If a valid message is found, break early and return
// a pointer to the message. Otherwise return null.
// NOTE: This message will be overwritten once additional data is processed, so you
// must act on the message before continuing to process the queue.
unsigned char *MessageReader::ProcessQueue()
{
  while (!_queue.isEmpty()) {
    char ch = _queue.dequeue();

    switch(_currentState) {
      case WAITING:
        // If we are waiting for a message, discard everything but the SOM
        if (ch == SOM) {
          _currentState = GOT_SOM;
        }
        break;
      case GOT_SOM:
        // If we received an SOM character, this next value is the length
        _currentMessageSize = (unsigned int)ch;
        if (_currentMessageSize > MAX_SIZE) {
            _resetState();
        } else {
            _currentRead = 0;
            _message[_currentRead++] = ch; // We need to store the length in the message
            _currentState = READING_MESSAGE;
        }
        break;
      case READING_MESSAGE:
      	// Just add everything to the message until we've read the given length
      	// of the message
      	_message[_currentRead++] = ch;

      	if (_currentRead > _currentMessageSize) {
      		_currentState = MESSAGE_READ;
      	}
      	break;
      case MESSAGE_READ:
      	// If we read the given message size, but didn't get an EOM, then
      	// something went wrong. Start over.
      	if (ch == EOM) {
      		_currentState = VALID_MESSAGE;
      	} else {
      		_resetState();
      	}
      	break;
    }

    if (_currentState == VALID_MESSAGE) {
        _resetState();
    	return _message;
    }
  }

  return NULL;
}
