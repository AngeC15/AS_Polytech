/*
 * Copyright(C) 2011-2016 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis <davidsondfgl@hotmail.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <nanvix/clock.h>
#include <nanvix/const.h>
#include <nanvix/hal.h>
#include <nanvix/pm.h>
#include <signal.h>
#include <nanvix/klib.h>

#define PRIORITY_MAX 140
#define NICE_MAX 40
#define PRIORITY_COEFF 1.0
#define NICE_COEFF 1.0
#define QUEUES 4
#define MAX_PPQ 100
#define SELECTED_ALGO 5

int pow(int x, int y);

#include <nanvix/klib.h>


#define MAX_TICKET 1024
struct process *tickets[MAX_TICKET]; //link a process for each ticket
int restant = MAX_TICKET; //number of tickets not linked

PUBLIC void niceToTicket(struct process *p){
	int nbTicketsCorrespondant = -1*(p->nice - 21); //nice minimum priority is 20
	if(nbTicketsCorrespondant == 0){ //in case of
		nbTicketsCorrespondant = 1;
	}
	p->nbTickets = (nbTicketsCorrespondant>restant) ? restant : nbTicketsCorrespondant;
	kprintf("process %d a %d tickets, nice vallait, %d; restant vaut: %d nbTicket vaut: %d", p->pid, p->nbTickets, p->nice, restant, nbTicketsCorrespondant);
}


PUBLIC void changeTicket(struct process *p){
	//int i;
	//kprintf("Process: %s need %d tickets", p->name, p->nbTickets);
	if( p->state == PROC_DEAD || p == NULL || p->state == PROC_ZOMBIE){ //the process don't deserve tickets
		//we delete tickets linked with this process
		for(int i = 0; i < MAX_TICKET; i++){ //course of all  tickets
			if( tickets[i] == p){ //ticket i is possessed by p
				kprintf("Cellule %d libéré de %d \n", i, p->pid);
				tickets[i] = NULL;
			}
		}
	} else {
		int i = MAX_TICKET-restant;

		while(p->ticketAllocated < p->nbTickets){ 
			kprintf("ticket voulu de p: %d et tickets reelement alloue: %d ", p->nbTickets, p->ticketAllocated);
			tickets[i] = p; //assign the ticket | to get the next element of the process list (the for loop terminate in k the last process, the next ticket is at k+1 )
			tickets[i]->ticketAllocated += 1;
			i++;
			restant--; 
			kprintf("Cellule %d assigne a %d || tickerCounter a %d et nbTicker voulu: %d \n", i-1, tickets[i-1]->pid, tickets[i-1]->ticketAllocated, p->nbTickets);

		}
	}

	//for 2 cells, if the first is empty and not the second, the process get a lower index
	int k = 0;
	for (int i = 0 ; i<MAX_TICKET ; i++){
		if (tickets[k] == NULL){
			if (tickets[i] != NULL){
				kprintf("Changement des case %d et %d",i, k);
				tickets[k] = tickets[i];
				tickets[i] = NULL;
				k++;
			}
		}
		else{
			k++;
		}
		//kprintf("Tableau cellule: %d possède un ticket pour %s", i, tickets[i]->name);
	}
	
}




/**
 * @brief Schedules a process to execution.
 * 
 * @param proc Process to be scheduled.
 */
PUBLIC void sched(struct process *proc)
{
	proc->state = PROC_READY;
	proc->counter = 0;
	//need to change tickets
	changeTicket(proc);
}

/**
 * @brief Stops the current running process.
 */
PUBLIC void stop(void)
{
	curr_proc->state = PROC_STOPPED;
	sndsig(curr_proc->father, SIGCHLD);
	//need to change tickets
	changeTicket(curr_proc);
	yield();
}

/**
 * @brief Resumes a process.
 * 
 * @param proc Process to be resumed.
 * 
 * @note The process must stopped to be resumed.
 */
PUBLIC void resume(struct process *proc)
{	
	/* Resume only if process has stopped. */
	if (proc->state == PROC_STOPPED)
		sched(proc);
}
/*

// LRU
*/
PUBLIC void yield1(void)
{
	struct process *p;    /* Working process.     */
	struct process *next; /* Next process to run. */

	/* Re-schedule process for execution. */
	if (curr_proc->state == PROC_RUNNING)
		sched(curr_proc);

	/* Remember this process. */
	last_proc = curr_proc;

	/* Check alarm. */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip invalid processes. */
		if (!IS_VALID(p))
			continue;
		
		/* Alarm has expired. */
		if ((p->alarm) && (p->alarm < ticks))
			p->alarm = 0, sndsig(p, SIGALRM);
	}

	/* Choose a process to run next. */
	next = IDLE;
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip non-ready process. */
		if (p->state != PROC_READY)
			continue;
		
		/*
		 * Process with higher
		 * waiting time found.
		 */

		if (p->counter > next->counter){
			next->counter++;
			next = p;
		}else {
			p->counter++;
		}
		/*
		 * Increment waiting
		 * time of process.
		 */
	}
	
	/* Switch to next process. */
	next->priority = PRIO_USER;
	next->state = PROC_RUNNING;
	next->counter = PROC_QUANTUM;
	if (curr_proc != next)
		switch_to(next);
}
/*

Implementation for nice priority

*/
PUBLIC void yield2(void)
{
	struct process *p;    /* Working process.     */
	struct process *next; /* Next process to run. */

	/* Re-schedule process for execution. */
	if (curr_proc->state == PROC_RUNNING)
		sched(curr_proc);

	/* Remember this process. */
	last_proc = curr_proc;

	/* Check alarm. */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip invalid processes. */
		if (!IS_VALID(p))
			continue;
		
		/* Alarm has expired. */
		if ((p->alarm) && (p->alarm < ticks))
			p->alarm = 0, sndsig(p, SIGALRM);
	}

	/* Choose a process to run next. */
	next = IDLE;
	next->counter = 0;
	float ratio = 0.0;
	float nextratio = 0.0;

	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip non-ready process. */
		if (p->state != PROC_READY)
			continue;
		
		ratio = ((float) PRIORITY_COEFF * ((p->priority - 40) / -PRIORITY_MAX) - (float) NICE_COEFF * (p->nice / NICE_MAX)) / -2;
		nextratio = ((float) PRIORITY_COEFF *((next->priority - 40) / -PRIORITY_MAX) - (float) NICE_COEFF * (p->nice / NICE_MAX)) / -2;

		/*process with highest ratio will be the next*/
		if (ratio < nextratio)
		{
			if (next != IDLE)
				next->counter++;
			next = p;
		}

		/*(if ratio are both the same),
		the process that has waited the longest will be the next.*/
		else if (ratio == nextratio)
		{
			if (p->counter > next->counter)
			{
				if (next != IDLE){
					next->counter++;
				}
				next = p;
			}

			/*
         	* Increment waiting
         	* time of process.
         	*/
			else
			{
				if (p != IDLE)
					p->counter++;
			}
		}

		else
		{
			if (p != IDLE)
				p->counter++;
		}
	}
	
	/* Switch to next process. */
	next->priority = PRIO_USER;
	next->state = PROC_RUNNING;
	next->counter = PROC_QUANTUM;
	if (curr_proc != next)
		switch_to(next);
}

// Multiple Queues

PUBLIC void yield3(void)
{
	struct process *p;	  /* Working process.     */
	struct process *next; /* Next process to run. */

	/* Re-schedule process for execution. */
	if (curr_proc->state == PROC_RUNNING)
		sched(curr_proc);

	/* Remember this process. */
	last_proc = curr_proc;

	/* Check alarm. */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip invalid processes. */
		if (!IS_VALID(p))
			continue;

		/* Alarm has expired. */
		if ((p->alarm) && (p->alarm < ticks))
			p->alarm = 0, sndsig(p, SIGALRM);
	}

	struct process *queues[QUEUES][MAX_PPQ];
	int procPerQueue[QUEUES]; /* Number of processes per queue */

	/* Flush the queues */
	for (int i = 0; i < QUEUES; i++)
		procPerQueue[i] = 0;

	/* Choosing the appropriate queue for each process using nice*/
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip non-ready process. */
		if (p->state != PROC_READY)
			continue;

		/* Choose desired queue q for process p */
		int q = (p->nice / (NICE_MAX / QUEUES));
		if (q >= QUEUES)
			q = QUEUES - 1;

		/* Put the process in the chosen queue */
		queues[q][procPerQueue[q]++] = p;
	}

	/* Choose a process to run next (Fixed priority preemptive scheduling) */
	int isEmpty = 1;
	next = IDLE;
	for (int i = 0; i < QUEUES; i++)
	{
		for (int j = 0; j < procPerQueue[i]; j++)
		{
			isEmpty = 0;
			struct process *p = queues[i][j];

			/*
			* Process with higher
			* waiting time found.
			*/
			if (p->counter > next->counter)
			{
				next->counter++;
				next = p;
			}

			/*
			* Increment waiting
			* time of process.
			*/
			else
				p->counter++;
		}
		if (!isEmpty)
			break;
	}

	/* Switch to next process. */
	next->priority = PRIO_USER;
	next->state = PROC_RUNNING;
	next->counter = PROC_QUANTUM;
	if (curr_proc != next)
		switch_to(next);
}

// Lottery

PUBLIC void yield4(void)
{
	struct process *p;    /* Working process.     */
	struct process *next; /* Next process to run. */


	/* Re-schedule process for execution. */
	if (curr_proc->state == PROC_RUNNING)
		sched(curr_proc);

	/* Remember this process. */
	last_proc = curr_proc;

	/* Check alarm. */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip invalid processes. */
		if (!IS_VALID(p))
			continue;
		
		/* Alarm has expired. */
		if ((p->alarm) && (p->alarm < ticks))
			p->alarm = 0, sndsig(p, SIGALRM);
	}

	/* Choose a process to run next. */
	next = IDLE;
	int nbTicket=1;
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip non-ready process. */
		if (p->state != PROC_READY)
			continue;
		nbTicket+=40-p->nice;
	}


	/* choose the lottery winner */
	int winner=CURRENT_TIME % nbTicket;
	
	int nbSeenTickets=0;
	for (p= FIRST_PROC; p<= LAST_PROC; p++)
	{
		/* Skip non-ready process */
		if (p->state != PROC_READY)
			continue;
		/* Filling tickets array */
		int priority=p->nice;
		int nTickets=40-priority;
		nbSeenTickets+=nTickets;
		if(nbSeenTickets>=winner){
			next=p;
			break;
		}
	}
	


	/* Switch to next process. */
	next->priority = PRIO_USER;
	next->state = PROC_RUNNING;
	next->counter = PROC_QUANTUM;
	if (curr_proc != next)
		switch_to(next);
}

// Fair Share

PUBLIC void yield5(void)
{
    struct process *p;      /* Working process.     */
    struct process *next; /* Next process to run. */

    /* Re-schedule process for execution. */
    if (curr_proc->state == PROC_RUNNING)
        sched(curr_proc);

    /* Remember this process. */
    last_proc = curr_proc;

    /* Check alarm. */
    for (p = FIRST_PROC; p <= LAST_PROC; p++)
    {
        /* Skip invalid processes. */
        if (!IS_VALID(p))
            continue;

        /* Alarm has expired. */
        if ((p->alarm) && (p->alarm < ticks))
            p->alarm = 0, sndsig(p, SIGALRM);
    }

    /* Choose a process to run next */
    next = IDLE;


    /* loop for the end of the table */ 
    for (p = curr_proc; p <= LAST_PROC; p++)
    {
        /* skip first process */
        if ((p->state != PROC_READY) || (p == curr_proc))
        {
            continue;
        }
        else
        {
            next = p;
            break;
        }
    }
    
    if (next == IDLE) /* if we didn't find any next process */ 
    {
        /* check the first part of the list */ 
        for (p = FIRST_PROC; p < curr_proc; p++)
        {

            if (p->state != PROC_READY)
            {
                continue;
            }
            
            else
            {
                next = p;
                break;
            }
        }
    }

    next->state = PROC_RUNNING;

    if (curr_proc != next)
    {
        switch_to(next);
    }
}


/**
 * @brief Yields the processor.
 */
PUBLIC void yield(void)
{	
	switch (SELECTED_ALGO)
	{
	case 1:
	yield1();
		break;
	
	case 2:
	yield2();
		break;
	
	case 3:
	yield3();
		break;
	
	case 4:
	yield4();
		break;
	
	case 5:
	yield5();
		break;

	default:
		break;
	}

	return;
}
