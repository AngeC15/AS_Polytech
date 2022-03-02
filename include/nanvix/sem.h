
#ifndef SEM_H_
#define SEM_H_


#include <nanvix/pm.h>

    #define MAX_SEMA 64 //the maximum of semaphore created is 64

    /**@{*/
	#define GETVAL   0 /**< Returns the value of a semaphore. */
	#define SETVAL   1 /**< Sets the value of a semaphore.    */
	#define IPC_RMID 3 /**< Destroys a semaphore.            */
	/**@}*/

    /*
    A semaphore as a value and a list of blocked processus
    */
    typedef struct sem {
        int value;
        struct process *blocked_proc;
    } semaphore, *pSemaphore;

    /*
    a cell of semaphore contains all needed to work on it
    */
    typedef struct semArray{
        semaphore semaphoreCell; //do not use a pointer because of the local adress used in the creation of the semaphore
        unsigned int key;
        int inUse;
        int waitingProcess;
    }semaphoreChart, *pSemaphoreChart;

    //new functions
    void initChart(); 
    int create(int n, unsigned int key);
    int down(int idSem);
    int up(int idSem);
    int destroy(int idSem);
    pSemaphoreChart getCell(int i);

#endif /* SEM_H_ */