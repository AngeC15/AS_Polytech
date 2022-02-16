#include <nanvix/syscall.h>
#include <errno.h>
#include <sys/sem.h>



int sys_semop(int idSem, int op){
    //TODO when up and down are called, need to mask interruptions

    pSemaphoreChart cell = getCell(idSem);
    //check if the cell has not a null semaphore and the semaphore is valid
    if(cell == NULL || !cell->valide){
        return -1;
    }

    int res = -1;

    if(op < 0){
        //Call down
        disable_interrupts();
        res = down(idSem);
        enable_interrupts();
    }else if(op == 0){
        return -1;
    }else if(op > 0){
        //Call up
        disable_interrupts();
        res = up(idSem);
        enable_interrupts();
    }

    return res;
}