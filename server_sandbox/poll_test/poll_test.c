#include <stdio.h>
#include <poll.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    char name[255];
    int counter = 0;

    struct pollfd mypoll;

    //init mypoll
    memset(&mypoll, 0, sizeof(mypoll));
    mypoll.fd = 0;
    mypoll.events = POLLIN;

    printf("Type in your name:\n");

    while (1)
    {
        if (poll(&mypoll, 1, 100) == 1)
        {
            printf("Event POLLIN detected\n");
            read(0, name, sizeof(name));
            printf("Hello %s\n", name);
            break;
        }
        else
            counter++;
    }

    printf("It tok you %d ms to type in your name\n", 100*counter);
    
    return 0;
}
