#include "syscall.h"

int main()
{
	char *sender = "../test/prog7";
        char *answer = "Answer 1 from ../test/prog8";
	char *messageptr;
	int buffer_id = -1;
	int result = -1;
	buffer_id = WaitMessage(sender,messageptr,buffer_id);

        result = SendAnswer(result,answer,buffer_id);

	answer = "Answer 2 from ../test/prog8";

	buffer_id = WaitMessage(sender,messageptr,buffer_id);

        result = SendAnswer(result,answer,buffer_id);
	Exit(0);
}
