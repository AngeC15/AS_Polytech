#include <nanvix/const.h>
#include <nanvix/sem.h>


/*
Create a semaphore initialized with value n
*/
PUBLIC int create(int n, unsigned int key) {

    return 0;
}


/*
Take a ressource on the semaphore idSem, could be blocking if semaphore's value is less than 0 (substract 1)
see also up
*/
PUBLIC int down(int idSem) {



    return 0;
}

/*
Free a ressource on the semaphore (add 1)
*/
PUBLIC int up(int idSem) {



    return 0;
}

/*
Destroy the semaphore idSem, return 0 if done, -1 if a problem happened
*/
public int destroy(int idSem){


    return -1;
}