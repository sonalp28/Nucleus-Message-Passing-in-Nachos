#include "syscall.h"

int main()
{
	char *sender = "../test/prog5";
        char *answer = "Answer 1 from ../test/prog6";
	char *messageptr;
	int buffer_id = -1;
	int result1 = -1, result2 = -1;
	
	buffer_id = WaitMessage(sender,messageptr,buffer_id);
        
	Exit(0);
}
