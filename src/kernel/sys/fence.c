#include <nanvix/syscall.h>
#include <nanvix/const.h>
#include <sys/fence.h>



//the tab of fence:
fenceArray tabFence[MAX_FENCE];
//number of fence currently living
int livingFence = 0;


/*
for initialization of the tab, to make all cell not used
*/
void initFenceChart() {
  for(int i=0; i<MAX_FENCE; i++) {
    tabFence[i].inUse = 0;
  }
}



/*
Create a fence initialized with value n
*/
int create_fence(int n, unsigned int key) {
    //check if the number max of fence is not reach
    if(livingFence == 0){
        initFenceChart(); //call the init of chart only for the first creation of a fence
    }
    if(livingFence < MAX_FENCE){
        //Create a new sempahore:
        int i=0;
        while(tabFence[i].inUse == 1) {
            i++;
        }
        //fulfill all the field of a fenceCells
        tabFence[i].valide = 1;
        if(n > MAX_PROC_BLOCKED){
               tabFence[i].fenceCell.value = MAX_PROC_BLOCKED;
        }else{
               tabFence[i].fenceCell.value = n;
        }
     
        tabFence[i].key = key;
        tabFence[i].inUse = 1;
        tabFence[i].waitingProcess = 0;
        //there is a other fence in use
        livingFence++;
        return i;
    }else{
        return -1;
    }
}


/*
Take a ressource on the fence idFen, could be blocking if fence's value is less than 0 (substract 1)
see also up
*/
int down_fence(int idFen) {
    if(tabFence[idFen].inUse == 1){
        
        while(tabFence[idFen].fenceCell.value > tabFence[idFen].waitingProcess){ //while the fence has not a sufficient value he keeps being blocked
            //sleep(&tabFence[idFen].fenceCell.blocked_proc , curr_proc->priority);
            tabFence[idFen].waitingProcess++;
        }
        return 0;
    }
    return -1;
}

/*
Free all ressource on the fence (add 1)
*/
int up_fence(int idFen) {
  if(tabFence[idFen].inUse == 1) {

    if(tabFence[idFen].waitingProcess >= tabFence[idFen].fenceCell.value ) { //if the value is higher than 0 and there is process wich are blocked, wake up them
    int i = 0;
        while(i < tabFence[idFen].waitingProcess){
            //wakeup(tabFence[idFen].fenceCell.blocked_proc[i]);
            i++;

        }
        tabFence[idFen].waitingProcess = 0;
    }   
    return 0;
  }
  return -1;
}

/*
Destroy the fence idFen, return 0 if done, -1 if a problem happened
*/

int barrier_delete(int key){
    int i = 0;
    while(i < MAX_FENCE){
        if(tabFence[i].key == (unsigned int) key){
           if(tabFence[key].inUse == 1){
            tabFence[key].inUse = 0;
            livingFence--;
            return 0;
            }
        }
        i++;
    }
//destroy means saying that the cell is not used:
    
    return -1;
}


int barrier_get (int key){
    int i = 0;
    while(i < MAX_FENCE){
        if(tabFence[i].key == (unsigned int) key){
            return i;
        }
        i++;
    }
    return -1;
}

int barrier_wait (int barid){
    barid++;
    return -1;

}

/*
return the fence at the ceell i
*/
pfenceArray getFenceCell(int i){
    if ( i > MAX_FENCE || i < 0){
        return NULL;
    }
    return &tabFence[i];
}
/*
return the int in the tab linked with the key
*/
int getCellFence(int key){
    int i = 0;
    while(i < MAX_FENCE && tabFence[i].key != (unsigned int) key){
        i++;
    }
    return i;
}