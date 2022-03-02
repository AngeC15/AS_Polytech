#ifndef FENCE_H_USER
#define FENCE_H_USER

#include <nanvix/pm.h>

    #define MAX_FENCE 64 //the maximum of fence created is 64

	/**
	 * @brief Comand values for fences.
	 */
	/**@{*/
	#define GETVAL   0 /**< Returns the value of a fence. */
	#define SETVAL   1 /**< Sets the value of a fence.    */
	#define IPC_RMID 3 /**< Destroys a fence.            */
	/**@}*/


	extern int fenceget(unsigned);
	extern int fencectl(int, int, int);
	extern int fenceop(int, int);

 
#endif /* FENCE_H_ */