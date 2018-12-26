#include "syscall.h"

int main()
{
	char *sender = "../test/prog17";
	char *messageptr;
	int buffer_id = -1;
	buffer_id = WaitMessage(sender,messageptr,buffer_id);
	Exit(0);
}
