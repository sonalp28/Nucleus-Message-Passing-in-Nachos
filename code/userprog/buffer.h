#ifndef BUFFER_H
#define BUFFER_H

class Thread;

#include "list.h"
#include <string.h>

class Buffer{
private:
  int buffer_id;
  Thread *sender;
  Thread *receiver;
  bool isActiveStatus;
  List<std::string> *listOfMessages;   //list of messages specific to buffer


public:
  Buffer(Thread *Tsender, Thread *Treceiver);
  Buffer(Thread *Tsender, Thread *Treceiver, int buffer_id);
  ~Buffer();

  int getBufferId() {return buffer_id;}
  Thread* getSender() {return sender;}
  Thread* getReceiver() {return receiver;}
  bool getActiveStatus() {return isActiveStatus;}
  List<std::string>* getMessagesList() {return listOfMessages;}

  void setBufferId(int bufferId) {buffer_id = bufferId ;}
  void setSender(Thread *sender) {sender = sender;}
  void setReceiver(Thread *receiver) {receiver = receiver;}
  void setActiveStatus(bool activeFlag) {isActiveStatus = activeFlag;}

};

#endif
