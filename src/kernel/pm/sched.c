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

Seems to be the basic queue

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
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip non-ready process. */
		if (p->state != PROC_READY)
			continue;
		
		/*
		 * Process with higher
		 * waiting time found.
		 */

		if (p->nice + p->priority < next->priority + next->nice || 
			((p->nice + p->priority == next->priority + next->nice) && 
			(p->counter > next->counter)))
		{
			next->counter++;
			next = p;
		}else{
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

This is the implementation of the lottery queue

*/

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
	/* Get the winner ticket */
	int winner =  (ticks % (MAX_TICKET - (restant))); //is it really random?
	next = tickets[winner];
	//the process choosen could be not ready 
	while( next->state != PROC_READY){
		/* Get a new winner ticket */
		winner = (ticks % (MAX_TICKET - (restant)));

		next = tickets[winner];
		kprintf("num: %d  || next est: %s || etat vaut: %d \n", winner, next->name, next->state);
	}
	//kprintf("winner: %d ||   next proc = %d || current = %s  || restant vaut: %d || ticks vaut %d \n",winner, next->pid, curr_proc->name, restant, ticks );

	/* Switch to next process. */
	next->priority = PRIO_USER;
	next->state = PROC_RUNNING;
	next->counter = PROC_QUANTUM;
	if (curr_proc != next)
		switch_to(next);
}

void swap(int n){
	switch (n)
	{
	case 1:
	yield1();
		break;
	
	case 2:
	yield2();
		break;

	case 4:
	yield4();
		break;

	default:
		break;
	}

	return;
}

/**
 * @brief Yields the processor.
 */
PUBLIC void yield(void)
{
	swap(4);
}