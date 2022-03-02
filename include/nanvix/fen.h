#ifndef FENCE_H_
#define FENCE_H_

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
    int down_fence(int idSem);
    int up_fence(int idSem);
    int barrier_delete (int key);
    int barrier_get (int key);
    int barrier_wait (int barid);
    pfenceArray getFenceCell(int i);
    int getCellFence(int key);

#endif /* FENCE_H_ */