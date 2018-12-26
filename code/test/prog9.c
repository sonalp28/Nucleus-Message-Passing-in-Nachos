#include "syscall.h"

int main()
{
char *receiver = "../test/prog10";
char *message = "Hello, message 1 from ../prog9";
int buffer_id1 = -1, buffer_id2 = -1;
char *answer;
int result1 = -1, result2 = -1;

buffer_id1 = SendMessage(receiver, message, buffer_id1);

receiver = "../test/prog11";
message = "Hello, message 2 from ../test/prog9";
buffer_id2 = SendMessage(receiver, message, buffer_id2);
result1 = WaitAnswer(result1, answer, buffer_id1);
result2 = WaitAnswer(result2, answer, buffer_id2);
Exit(0);
}
