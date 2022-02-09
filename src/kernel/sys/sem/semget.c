#include <nanvix/syscall.h>
#include <errno.h>
#include <nanvix/const.h>
#include <sys/sem.h>


int sys_semget(unsigned key){
    int i = 0;
    pSemaphoreChart cell = getCell(i);
    while(key != cell->key && cell != NULL){
        i++;
        cell = getCell(i);
    }
    //have to create new one if cell == NULL
    if(cell == NULL){
        return create(0, key); //Sem initialized at 0
    }
    return i; //return the value of cell conresponding to the key
}