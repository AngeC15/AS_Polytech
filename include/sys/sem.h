/*
 * Copyright(C) 2011-2016 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * This file is part of Nanvix.
 * 
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SEM_H_
#define SEM_H_

#include <nanvix/pm.h>


    #define MAX_SEMA 64 //the maximum of semaphore created is 64

	/**
	 * @brief Comand values for semaphores.
	 */
	/**@{*/
	#define GETVAL   0 /**< Returns the value of a semaphore. */
	#define SETVAL   1 /**< Sets the value of a semaphore.    */
	#define IPC_RMID 3 /**< Destroys a semaphore.            */
	/**@}*/

	/* Forward definitions. */
	extern int semget(unsigned);
	extern int semctl(int, int, int);
	extern int semop(int, int);

    typedef struct sem {
        int value;
        struct process **blocked_proc;
    } semaphore, *pSemaphore;

    typedef struct semArray{
        int valide;
        pSemaphore semaphoreCell;
        unsigned int key;
        int inUse;
        int waitingProcess;
    }semaphoreChart, *pSemaphoreChart;

    //new functions
    void resetChart();
    int create(int n, unsigned int key);
    int down(int idSem);
    int up(int idSem);
    int destroy(int idSem);
    pSemaphoreChart getCell(int i);

#endif /* SEM_H_ */