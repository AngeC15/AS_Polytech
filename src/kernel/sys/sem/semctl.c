#include <nanvix/syscall.h>
#include <errno.h>


int sys_semctl(int idSem, int cmd, int val){
    idSem++;
    cmd++;
    val++;



    return 0;
}
