#include "syscall.h"

int main()
{
	char *sender = "../test/prog12";
        char *answer = "Answer from ../test/prog14";
	char *messageptr;
	int buffer_id = -1;
	int result = -1;
	buffer_id = WaitMessage(sender,messageptr,buffer_id);
        result = SendAnswer(result,answer,buffer_id);
	Exit(0);
}
