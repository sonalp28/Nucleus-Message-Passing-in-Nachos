#include "syscall.h"

int main()
{
char *receiver = "../test/prog2";
char *message = "Hello from ../test/prog1";
int buffer_id = -1;
char *answer;
int result = -1;

buffer_id = SendMessage(receiver, message, buffer_id);

result = WaitAnswer(result, answer, buffer_id);
Exit(0);
}
