#include "syscall.h"

int main()
{
char *receiver = "../test/prog16";
char *message = "Hello, message 1 from ../test/prog15";
int buffer_id = -1;

buffer_id = SendMessage(receiver, message, buffer_id);

message = "Hello, message 2 from ../test/prog15";

buffer_id = SendMessage(receiver, message, buffer_id);

message = "Hello, message 3 from ../test/prog15";

buffer_id = SendMessage(receiver, message, buffer_id);

message = "Hello, message 4 from ../test/prog15";

buffer_id = SendMessage(receiver, message, buffer_id);

message = "Hello, message 5 from ../test/prog15";

buffer_id = SendMessage(receiver, message, buffer_id);

message = "Hello, message 6 from ../test/prog15";

buffer_id = SendMessage(receiver, message, buffer_id);

message = "Hello, message 7 from ../test/prog15";

buffer_id = SendMessage(receiver, message, buffer_id);

message = "Hello, message 8 from ../test/prog15";

buffer_id = SendMessage(receiver, message, buffer_id);

message = "Hello, message 9 from ../test/prog15";

buffer_id = SendMessage(receiver, message, buffer_id);

message = "Hello, message 10 from ../test/prog15";

buffer_id = SendMessage(receiver, message, buffer_id);

message = "Hello, message 11 from ../test/prog15";
buffer_id = SendMessage(receiver, message, buffer_id);




Exit(0);
}
