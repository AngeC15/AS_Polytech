#include <nanvix/syscall.h>
#include <errno.h>
#include <nanvix/const.h>
#include <sys/sem.h>



int sys_semctl(int idSem, int cmd, int value){
    pSemaphoreChart cell = getCell(idSem);
   
   if( cell->valide != 1 || cell == NULL || cell->semaphoreCell.value > 0){
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
