#include <stdio.h>
#include <stdlib.h>
#include <stropts.h>
#include <unistd.h>
#include <string.h>
#include <sys/utsname.h>
#include <nanvix/accounts.h>
#include <nanvix/config.h>
#include <dev/tty.h>

void crash (void)
{   
    /*la ram*/
	int disk = open("/dev/ramdisk", O_WRONLY); 

	
	if(disk < 0) 
	{
		printf("Echec ! impossible d'ouvrir le fichier /dev/ramdisk");
		exit(EXIT_FAILURE);
	}

	char *buffer = malloc(sizeof(char)*2147483647); //Allocate a huge array
	write(disk,buffer,2147483647);
    close(disk);
	free(buffer);
}

int main(int argc, char *const argv[])
{
    ((void)argc);
	((void)argv);
	crash();
	return (EXIT_SUCCESS);
}