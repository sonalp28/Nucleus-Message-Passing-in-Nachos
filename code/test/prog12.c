#include "syscall.h"

int main()
{
char *receiver = "../test/prog13";
char *message = "Hello, message 1 from ../test/prog12";
int buffer_id = -1;
char *answer;
int result1 = -1, result2 = -1;

buffer_id = SendMessage(receiver, message, buffer_id);
result1 = WaitAnswer(result1, answer, buffer_id);

receiver = "../test/prog14";
message = "Hello, message 2 from ../test/prog12";
buffer_id = SendMessage(receiver, message, buffer_id);

result2 = WaitAnswer(result2, answer, buffer_id);
Exit(0);
}
