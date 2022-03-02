#include <nanvix/syscall.h>
#include <errno.h>
#include <nanvix/const.h>
#include <nanvix/fence.h>



int sys_fencectl(int idFen, int cmd, int value){
    pfenceArray cell = getFenceCell(idFen);

   if( cell->inUse != 1 || cell == NULL || cell->fenceCell.value < 0){
       return -1;
   }
   if(cmd == GETVAL){
       return cell->fenceCell.value;
   }else if(cmd == SETVAL){
        cell->fenceCell.value = value;
        return 0;
   }else if(cmd == IPC_RMID){
      return barrier_delete(idFen);
   }
    return -1;



}
