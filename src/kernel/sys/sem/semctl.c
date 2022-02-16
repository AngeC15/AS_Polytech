#include <nanvix/syscall.h>
#include <errno.h>
#include <nanvix/const.h>
#include <sys/sem.h>

/*
The semctl function allows a series of control operations on the
semaphore identified by semid. These operations are specified by cmd
and can be:
- GETVAL, returns the current value of the semaphore;
- SETVAL, sets the semaphore value to val;
- IPC RMID, removes the semaphore and destroys it if it is no longer used.
The return value of the function depends on cmd. If it is GETVAL, the
current value of the semaphore is returned
*/


int sys_semctl(int idSem, int cmd, int value){
    pSemaphoreChart cell = getCell(idSem);

   if( cell->valide != 1 || cell == NULL || cell->semaphoreCell.value < 0){
       return -1;
   }
   if(cmd == GETVAL){
       return cell->semaphoreCell.value;
   }else if(cmd == SETVAL){
        cell->semaphoreCell.value = value;
        return 0;
   }else if(cmd == IPC_RMID){
      return destroy(idSem);
   }
    return -1;



}
