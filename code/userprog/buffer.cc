#include "main.h"
#include "buffer.h"

Buffer::Buffer(Thread *Tsender, Thread *Treceiver)
{
  buffer_id = kernel->bitmapForBuffer->FindAndSet();
  sender = Tsender;
  receiver = Treceiver;
  isActiveStatus = true;
  listOfMessages = new List<std::string>();
}

Buffer::Buffer(Thread *Tsender, Thread *Treceiver, int new_buffer_id)
{
  buffer_id = new_buffer_id;
  sender = Tsender;
  receiver = Treceiver;
  isActiveStatus = true;
  listOfMessages = new List<std::string>();
}

Buffer::~Buffer()
{
  delete listOfMessages;
}
