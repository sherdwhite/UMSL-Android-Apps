// Sherd White
// cs4760 Assignment 5
// 11/10/2017

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>

#define PERM (S_IRUSR | S_IWUSR)
#define LENGTH 132

typedef struct {
	unsigned int seconds;
	unsigned int nanoseconds;
} shared_clock;

typedef struct {
	pid_t pid;
	int request;
	int allocation;
	int release;
	int ready;
} shared_resources;

int main(int argc, char * argv[]) 
{
	if (argc <= 1)
	{
		fprintf(stderr, "A missing or incorrect file specified.\n");
		return 1;
	}
	
	int pid = atoi(argv[1]);
	// printf("Child: %d started. \n", pid);

	// create shared memory segment and get the segment id
	// IPC_PRIVATE, child process, created after the parent has obtained the
	// shared memory, so that the private key value can be passed to the child
	// when it is created.  Key could be arbitrary integer also.
	// IPC_CREAT says to create, but don't fail if it is already there
	// IPC_CREAT | IPC_EXCL says to create and fail if it already exists
	// PERM is read write, could also be number, say 0755 like chmod command
	int key = 92111;
	int shm_id = shmget(key, sizeof(shared_clock), SHM_RDONLY | IPC_CREAT);
    if (shm_id == -1) {
        perror("Failed to create shared memory segment. \n");
        return 1;
	}
	// printf("My user segment id for shared clock is %d\n", shm_id);
	
	// attach shared memory segment
	shared_clock* shared = (shared_clock*)shmat(shm_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (shared == (void*)-1) {
        perror("Failed to attach shared memory segment. \n");
        return 1;
    }
	// printf("My user shared clock address is %x\n", shared);
	
	int rsrckey = 91514;
	int rsrc_id = shmget(rsrckey, sizeof(shared_resources), PERM | IPC_CREAT);
    if (rsrc_id == -1) {
        perror("Failed to create shared resource segment. \n");
        return 1;
	}
	// printf("My user segment id for the resource share is %d\n", rsrc_id);
	
	// attach shared memory segment
	shared_resources* resources = (shared_resources*)shmat(rsrc_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (resources == (void*)-1) {
        perror("Failed to attach resources segment. \n");
        return 1;
    }
	// printf("My OS resources address is %x\n", shared);
	
	printf("Child %d start at seconds: %d and nanoseconds: %ld.\n", pid, shared->seconds, shared->nanoseconds);
	 
	// strcpy(resources->msgTest, "Hello!");  // for writing messages
	// printf("Child.\n");	
	
	srand(pid * time(NULL));
	long nano_end = 0;
	int sec_end = 0;
	long random_time = rand() % 1000000 + 1;
	if((shared->nanoseconds + random_time)  < 1000000000){
			nano_end = shared->nanoseconds + rand() % 1000000 + 1;
			sec_end = shared->seconds;
		}
	else if((shared->nanoseconds + random_time)  >= 1000000000){
		nano_end = (shared->nanoseconds + random_time) - shared->nanoseconds ;
		sec_end = shared->seconds  + 1;
	}
	
	printf("Child: %d end time is %d sec and %ld nanoseconds. \n", pid, sec_end, nano_end);
	
	// Initialize named semaphore for shared processes.  Create it if it wasn't created, 
	// 0644 permission. 1 is the initial value of the semaphore
	sem_t *sem = sem_open("BellandJ", 1);
	if(sem == SEM_FAILED) {
        perror("Child Failed to sem_open. \n");
        return;
    }
	
	// int sem_value;
	// sem_getvalue(sem, &sem_value);
	// printf("Child: %d, Semaphore value is %d. \n", pid, sem_value);
	int clear = 0;
	while(clear == 0){
		sem_wait(sem);  // wait until we can subtract 1
		// printf("Child: %d cleared sem_wait. \n", pid);
		// Critical Section
		if((sec_end < shared->seconds && resources->ready == 0) || (nano_end <= shared->nanoseconds && sec_end <= shared->seconds && resources->ready == 0)){  
			resources->pid = pid;
			resources->seconds = shared->seconds;
			resources->nanoseconds = shared->nanoseconds;
			sem_post(sem); // adds 1
			clear = 1;
			printf("Child: %d cleared sem at sec: %d, nano: %ld \n", pid, shared->seconds, shared->nanoseconds);
			resources->ready = 1;
			break;
		}
		else {
			sem_post(sem); // adds 1, cede CS, not ready to send msg.
			continue;
		}
	}
	
	sem_close(sem);  // disconnect from semaphore
	
	// detach from shared memory segment
	int detach = shmdt(shared);
	if (detach == -1){
		perror("Failed to detach shared memory segment. \n");
		return 1;
	}
	
	// detach from msg memory segment
	detach = shmdt(resources);
	if (detach == -1){
		perror("Failed to detach shared msg memory segment. \n");
		return 1;
	}
	
    return 0;
}