#include <nanvix/syscall.h>
#include <errno.h>



int sys_semop(int idSem, int op){
    idSem++;
    op++;




    return 0;
}