#include <nanvix/syscall.h>
#include <errno.h>
#include <nanvix/const.h>
#include <nanvix/fence.h>

/*
The semget function allows a process to use a semaphore associated with a key ( texttt key). If no semaphore is associated with key,
a new semaphore must be created.
If successful, the function will return the semaphore identifier
associated with the key. On error, −1 should be returned.
*/

int sys_fenceget(unsigned key){
    int i = 0;
    pfenceArray cell = getFenceCell(i);
    while(key != cell->key && cell != NULL){
        i++;
        cell = getFenceCell(i);
    }
    //have to create new one if cell == NULL
    if(cell == NULL){
        return create_fence(0, key); //Sem initialized at 0
    }
    return i; //return the value of cell conresponding to the key
}