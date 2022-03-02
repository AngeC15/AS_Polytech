#ifndef FENCE_H_
#define FENCE_H_

#include <nanvix/pm.h>

    #define MAX_FENCE 64 //the maximum of fence created is 64


    /**@{*/
	#define GETVAL   0 /**< Returns the value of a semaphore. */
	#define SETVAL   1 /**< Sets the value of a semaphore.    */
	#define IPC_RMID 3 /**< Destroys a semaphore.            */
	/**@}*/

   /*
    A fence as a value and a list of blocked processus
    */
    typedef struct fen {
        int value; //Max proc blocked on the fence
        struct process *blocked_proc;
    } fence, *pfence;

    /*
    a cell of fence contains all needed to work on it
    */
    typedef struct fArray{
        fence fenceCell; //do not use a pointer because of the local adress used in the creation of the fence
        unsigned int key;
        int inUse;
        int waitingProcess;
    }fenceArray, *pfenceArray;

    //new functions
    void initFenceChart(); 
    int create_fence(int n, unsigned int key);
    int hit_fence(int idSem);
    int barrier_delete (int key);
    int barrier_get (int key);
    int barrier_wait (int barid);
    pfenceArray getFenceCell(int i);
    int getCellFence(int key);

#endif /* FENCE_H_ */