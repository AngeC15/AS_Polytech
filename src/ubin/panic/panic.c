#include <stdio.h>
#include <stdlib.h>
#include <stropts.h>
#include <unistd.h>
#include <string.h>
#include <sys/utsname.h>
#include <nanvix/accounts.h>
#include <nanvix/config.h>
#include <dev/tty.h>

/**
 *  crash via "infinite fork"
 */

static void infinite_fork(void)
{
    while(1)
	{
        pid_t pid = fork(); // création de nouveau processus
        printf("processus %d lance\n", pid); 
    }
}


int main(int argc, char *const argv[])
{
    ((void)argc);
	((void)argv);
	infinite_fork();
	return (EXIT_SUCCESS);
}