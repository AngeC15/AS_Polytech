#include <nanvix/syscall.h>
#include <nanvix/const.h>
#include <sys/sem.h>



//the tab of semaphore:
semaphoreChart tabSema[MAX_SEMA];
//number of semaphore currently living
int livingSemaphore = 0;

/*
for initialization of the tab
*/
void resetChart() {
  for(int i=0; i<MAX_SEMA; i++) {
    tabSema[i].inUse = 0;
  }
}



/*
Create a semaphore initialized with value n
*/
int create(int n, unsigned int key) {
    //check if the number max of semaphore is not reach
    if(livingSemaphore <= MAX_SEMA){
        //Create a new sempahore:
        int i=0;
        while(tabSema[i].inUse == 1) {
            i++;
        }
        semaphore s;
        s.value = n;
        s.blocked_proc = NULL; //no process blocked at the beginning

        tabSema[i].valide = 1;
        tabSema[i].semaphoreCell = &s;
        tabSema[i].key = key;
        tabSema[i].inUse = 1;
        tabSema[i].waitingProcess = 0;

        livingSemaphore++;
        return i;
    }else{
        return -1;
    }
}


/*
Take a ressource on the semaphore idSem, could be blocking if semaphore's value is less than 0 (substract 1)
see also up
*/
int down(int idSem) {
    if(tabSema[idSem].inUse == 1){
        tabSema[idSem].waitingProcess++;
        tabSema[idSem].semaphoreCell->value--;
        while(tabSema[idSem].semaphoreCell->value <= 0){
            sleep(tabSema[idSem].semaphoreCell->blocked_proc , curr_proc->priority);
        }
        tabSema[idSem].waitingProcess--;
        return 0;
    }
    return -1;
}

/*
Free a ressource on the semaphore (add 1)
*/
int up(int idSem) {
  if(tabSema[idSem].inUse == 1) {
    tabSema[idSem].semaphoreCell->value++;
    if(tabSema[idSem].waitingProcess > 0 && tabSema[idSem].semaphoreCell->value>=0) {
      wakeup(tabSema[idSem].semaphoreCell->blocked_proc);
    }
    return 0;
  }
  return -1;
}

/*
Destroy the semaphore idSem, return 0 if done, -1 if a problem happened
*/
int destroy(int idSem){
//destroy means saying that the cell is not used:
    if(tabSema[idSem].inUse == 1){
        tabSema[idSem].inUse = 0;
        return 0;
    }
    return -1;
}