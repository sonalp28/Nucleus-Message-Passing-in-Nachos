#include "syscall.h"

int main()
{
	char *sender = "../test/prog9";
        char *answer = "Answer from ../test/prog10";
	char *messageptr;
	int buffer_id = -1;
	int result = -1;
	buffer_id = WaitMessage(sender,messageptr,buffer_id);
        result = SendAnswer(result,answer,buffer_id);
	Exit(0);
}
