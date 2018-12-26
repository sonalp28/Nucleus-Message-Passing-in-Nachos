// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include "buffer.h"
#include <string.h>
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

//utility function - convert char* to string
std::string convertToString(int startAddress)
{
  std::string resultString;
  int charIndex = startAddress;
  char character;

  while(true){
    bool flag = kernel->machine->ReadMem(startAddress, 1, &charIndex);
    character = (char)charIndex;
    if(flag && character != '\0'){
      resultString += character;
      startAddress++;
    }
    else{
      break;
    }
  }
  return resultString;
}

//utility function to get the thread by its name
Thread* getThreadByName(std::string name)
{
  Thread *matchingThread = NULL;

  ListIterator<Thread*> *iterator = new ListIterator<Thread*>(kernel->listOfForkedThreads);
  for (; !iterator->IsDone(); iterator->Next()) {
    if(iterator->Item()->getName() == name){
       matchingThread = iterator->Item();
       break;
    }
  }
  return matchingThread;
}

//get the buffer entry from the list of kernel buffer pool

Buffer* getExistingBuffer(int buffer_id, std::string receiverThreadName)
{
  std::map<int, List<Buffer*>* >::iterator it;
  it = kernel->bufferPool.find(buffer_id);

  if(it != kernel->bufferPool.end())  //the entry is found
  {
    Buffer *newBuffer = it->second->Front();
    Thread *receiverThread = it->second->Front()->getReceiver();

    if(newBuffer != NULL && (newBuffer->getReceiver()->getName() == receiverThreadName)){
       return newBuffer;
    }
  }
  return NULL;

}

//print contents of kernel buffer pool map
void printContentsOfMap()
{
  for(map<int, List<Buffer*>* >::const_iterator it = kernel->bufferPool.begin();
    it != kernel->bufferPool.end(); ++it)
  {
    std::cout << it->first << " " << it->second->Front()->getBufferId() << " " << "\n";
  }

}

//get the thread buffer entry from current thread's queue by passing buffer id and the message sender

Buffer* getThreadBuffer(int buffer_id, std::string senderName)
{
  std::map<int, Buffer*>::iterator it;
  it = kernel->currentThread->tBufferPool.find(buffer_id);

  Buffer *newBuffer = NULL;
  if(it != kernel->currentThread->tBufferPool.end())  //the entry is found
  {
    newBuffer = it->second;
    return newBuffer;
  }
  else{
    for(map<int, Buffer* >::const_iterator it = kernel->currentThread->tBufferPool.begin();
      it != kernel->currentThread->tBufferPool.end(); ++it){
      if(it->second->getSender()->getName() == senderName){
        newBuffer = it->second;
        return newBuffer;
      }
    }
  }
  return newBuffer;
}

//write the message into the memory

void copyMessageToMem(std::string message, int messagePtr)
{
    for(int i = messagePtr, j = 0; i < messagePtr + message.length(); i++, j++)
    {
        kernel->machine->WriteMem(i, 1, (int)message.at(j));
    }
}

//update program counter

void updateProgramCounter()
{
    /* set previous programm counter (debugging only)*/
    kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

    /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
    kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

    /* set next programm counter for brach execution */
    kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
}


void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
    case SyscallException:
      switch(type) {
      case SC_Halt:
	DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

	SysHalt();

	ASSERTNOTREACHED();
	break;

      case SC_Add:
	DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

	/* Process SysAdd Systemcall*/
	int result;
	result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
			/* int op2 */(int)kernel->machine->ReadRegister(5));

	DEBUG(dbgSys, "Add returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);

	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;

	ASSERTNOTREACHED();

	break;
      case SC_Exit:
      {
      //main thread wants to finish
        kernel->listOfForkedThreads->Remove(kernel->currentThread);

        cout<<"Exit Called by :"<<kernel->currentThread->getName()<<endl;

        kernel->currentThread->Finish();
        updateProgramCounter();
    }
      return;
      break;

      case SC_SendMessage:
      {
        IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
        std::string receiver, message;

        int receiverIndex = (int)kernel->machine->ReadRegister(4);
        int messageIndex = (int) kernel->machine->ReadRegister(5);
        int buffer_id = (int)kernel->machine->ReadRegister(6);

        receiver = convertToString(receiverIndex);
        message = convertToString(messageIndex);

        Thread *receiverThread = getThreadByName(receiver);

        if(receiverThread != NULL)
        {
          if(kernel->currentThread->getMessageCount() < kernel->MessageLimit)
          {
              if(buffer_id == -1){
                Buffer *buffer = new Buffer(kernel->currentThread, receiverThread);

                  if(buffer->getBufferId() == -1){
                    kernel->machine->WriteRegister(2, (int)buffer->getBufferId());
                    updateProgramCounter();
                    (void) kernel->interrupt->SetLevel(oldLevel);
                    break;
                  }
                std::cout<<"-------------------------------------------\n";

                std::cout<<"Buffer ID: " << buffer->getBufferId() << " created between Threads: " << kernel->currentThread->getName() << " and " << receiverThread->getName() << endl;

                  std::cout<<"-------------------------------------------\n";
                buffer->getMessagesList()->Append(message);
                List<Buffer*> *kernelBufferList = new List<Buffer*>();
                kernelBufferList->Append(buffer);

                kernel->bufferPool.insert(std::pair<int, List<Buffer*>* > (buffer->getBufferId(), kernelBufferList));
                receiverThread->tBufferPool.insert(std::pair<int, Buffer*> (buffer->getBufferId(), buffer));

                kernel->machine->WriteRegister(2, (int)buffer->getBufferId());

                std::cout<<"Message sent from " << kernel->currentThread->getName() << " to " << receiverThread->getName() << " , Message: " << message << endl;


                kernel->currentThread->incrementMessageCount();
              }
              else{  //user program has passed some buffer id
                Buffer *validateBuffer = getExistingBuffer(buffer_id, receiver);

                if(validateBuffer != NULL){
                  validateBuffer->getMessagesList()->Append(message);
                  receiverThread->tBufferPool.insert(std::pair<int, Buffer*> (buffer_id, validateBuffer));

                  kernel->machine->WriteRegister(2, buffer_id);

                  kernel->currentThread->incrementMessageCount();
                  std::cout<<"Message sent from " << kernel->currentThread->getName() << " to " << receiverThread->getName() << " , Message: " << message << endl;
                }
                else{
                  kernel->machine->WriteRegister(2, -1);
                  std::cout<<"Security error while sending to " << receiverThread->getName() << endl;
                }
              }

          }
          else{  // since the message limit is reached, kernel is sending dummy messages
            Buffer *validateBuffer = getExistingBuffer(buffer_id, receiver);

            if(kernel->currentThread->getMessageCount() < kernel->MessageLimit)
            {
              if(validateBuffer != NULL){
                validateBuffer->getMessagesList()->Append(kernel->DummyMessage);
                receiverThread->tBufferPool.insert(std::pair<int, Buffer*> (buffer_id, validateBuffer));

                kernel->machine->WriteRegister(2, buffer_id);
              }

              else{
                  std::cout<<"Dummy message sent to " << receiverThread->getName() << endl;
              }
            }
            else{
              std::cout<<"Maximum message limit reached for " << kernel->currentThread->getName();
              std::cout<<", so Dummy message sent to " << receiverThread->getName() << endl;
            }

          }
        }
        else{
          kernel->deleteBufferEntryFromKernel(buffer_id);
        }

        updateProgramCounter();
        (void)kernel-> interrupt->SetLevel(oldLevel);


      }
      return;
      //
      // ASSERTNOTREACHED();

    	break;
      case SC_WaitMessage:
      {
        IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
        std::string sender, message;

        int senderIndex = (int)kernel->machine->ReadRegister(4);
        int messageIndex = (int) kernel->machine->ReadRegister(5);
        int buffer_id = (int)kernel->machine->ReadRegister(6);

        sender = convertToString(senderIndex);
        message = convertToString(messageIndex);

        Thread *senderThread = getThreadByName(sender);

        while(TRUE)
        {
            if(kernel->listOfForkedThreads->IsInList(senderThread)) //sender thread is alive
            {
                Buffer *buffer = getThreadBuffer(buffer_id, sender);
                if(buffer != NULL && !buffer->getMessagesList()->IsEmpty())
                {
                    std::string message = buffer->getMessagesList()->RemoveFront();
                    copyMessageToMem(message,messageIndex);
                    kernel->machine->WriteRegister(2, (int)buffer->getBufferId());
                    cout << "Message received by " << kernel->currentThread->getName() << " sent from " << senderThread->getName() << " , Message : " << message << endl;
                    break;
                }
                else{
                  kernel->currentThread->Yield();
                }
            }
            else{ //sender thread dead
              Buffer *buffer = getThreadBuffer(buffer_id, sender);// get current thread pool
              if(buffer != NULL && !buffer->getMessagesList()->IsEmpty())
              {
                  std::string message = buffer->getMessagesList()->RemoveFront();
                  copyMessageToMem(message,messageIndex);
                  kernel->machine->WriteRegister(2, (int)buffer->getBufferId());
                  cout << "Message received by " << kernel->currentThread->getName() << " sent from " << sender << " , Message : " << message << endl;
              }
              else{
                kernel->deleteBufferEntryFromKernel(buffer_id);
                copyMessageToMem(kernel->DummyMessage,messageIndex);
                kernel->machine->WriteRegister(2,-1);
                cout << "Sender Thread finished without sending message to " << kernel->currentThread->getName();
                cout << ", so Dummy message sent to " << kernel->currentThread->getName() << endl;
              }
              break;
            }
        }
        updateProgramCounter();
        (void)kernel-> interrupt->SetLevel(oldLevel);
      }

      return;
      //
      // ASSERTNOTREACHED();

    	break;

      case SC_SendAnswer:
      {
        IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
        std::string answer;

        int result = (int)kernel->machine->ReadRegister(4);
        int answerIndex = (int) kernel->machine->ReadRegister(5);
        int buffer_id = (int)kernel->machine->ReadRegister(6);

        answer = convertToString(answerIndex);

        std::map<int, Buffer*>::iterator it1;
        it1 = kernel->currentThread->tBufferPool.find(buffer_id);
        Buffer *currentThreadBuffer = NULL;

        if(it1 != kernel->currentThread->tBufferPool.end()){  //the entry is found
            currentThreadBuffer = it1->second;
            Thread* receiverThread = currentThreadBuffer->getSender();
            //std::cout << "receiverThread " << receiverThread->getName() << endl;
            if(kernel->listOfForkedThreads->IsInList(receiverThread)){
                std::map<int, Buffer*>::iterator it2;
                it2 = receiverThread->tBufferPool.find(buffer_id);

                if(it2 != receiverThread->tBufferPool.end()){ //if receiver has the buffer entry append answer
                  it2->second->getMessagesList()->Append(answer);
                }

                else{ //receiver doesn't have buffer entry create a new buffer instance in receiver bufferpool with same buffer id
                  Buffer *newBuffer = new Buffer(kernel->currentThread, receiverThread, buffer_id);
                  newBuffer->getMessagesList()->Append(answer);

                  std::map<int, List<Buffer*>* >::iterator it3;
                  it3 = kernel->bufferPool.find(buffer_id);
                  if(it3 != kernel->bufferPool.end()){
                    it3->second->Append(newBuffer);
                  }

                  receiverThread->tBufferPool.insert(std::pair<int, Buffer*> (newBuffer->getBufferId(), newBuffer));
                }
                kernel->machine->WriteRegister(2, (int)currentThreadBuffer->getBufferId());
                cout << "Answer sent from " << kernel->currentThread->getName() << " to " << receiverThread->getName() << ", Answer :" << answer << endl;
            }
            else{
              kernel->deleteBufferEntryFromKernel(buffer_id);
              kernel->machine->WriteRegister(2, -1);
              cout << "Dummy response sent in case of receiver being dead or null" << endl;
            }
        }
        updateProgramCounter();
        (void)kernel-> interrupt->SetLevel(oldLevel);
      }

      return;
      //
      // ASSERTNOTREACHED();

      break;

      case SC_WaitAnswer:
      {
        IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
        int result = (int)kernel->machine->ReadRegister(4);
        int answerIndex = (int) kernel->machine->ReadRegister(5);
        int buffer_id = (int)kernel->machine->ReadRegister(6);
        std::string sender;

        std::map<int, Buffer*>::iterator it;
        it = kernel->currentThread->tBufferPool.find(buffer_id);
        if(it != kernel->currentThread->tBufferPool.end()) {// if entry exists
          sender = it->second->getSender()->getName();
        }
        else
        {
          std::map<int, List<Buffer*>* >::iterator it;
          it = kernel->bufferPool.find(buffer_id);
          if(it != kernel->bufferPool.end()){
            sender = it->second->Front()->getReceiver()->getName();
          }
        }

        Thread *senderThread = getThreadByName(sender);
        while(TRUE) {
          if(kernel->listOfForkedThreads->IsInList(senderThread)){
            Buffer *buffer = getThreadBuffer(buffer_id, sender);
            if(buffer != NULL && !buffer->getMessagesList()->IsEmpty())
            {
              std::string message = buffer->getMessagesList()->RemoveFront();
              copyMessageToMem(message,answerIndex);
              kernel->machine->WriteRegister(2, (int)buffer->getBufferId());

              std::size_t found = message.find_last_of(" ");

	             std::string senderExtracted = message.substr(found+1);

              cout << "Answer received by " << kernel->currentThread->getName() << " sent from " << senderExtracted << " , Answer : " << message << endl;
              break;
            }
            else
            {
              kernel->currentThread->Yield();
            }

          }
          else
          {
            //std::cout<<"BUffer id : " << buffer_id << " , sender : "<< sender << endl;
            Buffer *buffer = getThreadBuffer(buffer_id, sender);// get current thread pool

            //std::cout<<"BUffer entry : " << buffer << endl;

            if(buffer != NULL && !buffer->getMessagesList()->IsEmpty())
            {
              std::string message = buffer->getMessagesList()->RemoveFront();
              copyMessageToMem(message,answerIndex);
              kernel->machine->WriteRegister(2, (int)buffer->getBufferId());

              std::size_t found = message.find_last_of(" ");

	             std::string senderExtracted = message.substr(found+1);

              cout << "Answer received by " << kernel->currentThread->getName() << " sent from " << senderExtracted << " , Answer : " << message << endl;
            }
            else
            {
              kernel->deleteBufferEntryFromKernel(buffer_id);
              copyMessageToMem(kernel->DummyMessage,0);
              kernel->machine->WriteRegister(2,0);
              cout << "Dummy answer sent to " << kernel->currentThread->getName() << " due to security error by sending on wrong buffer id or sender thread exited" << endl;
            }
            break;
            }
        }
        updateProgramCounter();
        (void)kernel-> interrupt->SetLevel(oldLevel);
      }

      return;
      //
      // ASSERTNOTREACHED();

      break;


      default:
//	cerr << "Unexpected system call " << type << "\n";
	break;
      }
      break;
    default:
      cerr << "Unexpected user mode exception" << (int)which << "\n";
      break;
    }
    //ASSERTNOTREACHED();
}
