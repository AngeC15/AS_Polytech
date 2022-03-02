#include <nanvix/syscall.h>
#include <errno.h>
#include <nanvix/fence.h>

/*
The semop function allows to perform atomic operations incrementing
or decrementing the variable associated with the semaphore identified by semid
A negative value for op specifies the downw operation and a value
positive the up operation.
*/

int sys_fenceop(int idFen){

    pfenceArray cell = getFenceCell(idFen);
    //check if the cell has not a null semaphore and the semaphore is valid
    if(cell == NULL || !cell->inUse){
        return -1;
    }

    int res = -1;


//if the fence doesn't have enough space to block one more proc, release all proc, else blocked


    disable_interrupts();
    res = hit_fence(idFen);
    enable_interrupts();
  

    return res;
}