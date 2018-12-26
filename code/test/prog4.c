#include "syscall.h"

int main()
{
	char *sender = "../test/prog3";
        char *answer = "Answer 1 from ../test/prog4";
	char *messageptr;
	int buffer_id = -1;
	int result1 = -1, result2 = -1;
	
	buffer_id = WaitMessage(sender,messageptr,buffer_id);
	buffer_id = WaitMessage(sender,messageptr,buffer_id);
        
	result1 = SendAnswer(result1,answer,buffer_id);

	answer = "Answer 2 from ../test/prog4";
	result2 = SendAnswer(result2,answer,buffer_id);
	Exit(0);
}
