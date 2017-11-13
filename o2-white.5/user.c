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
	int shm_clock_id = shmget(key, sizeof(shared_clock), SHM_RDONLY | IPC_CREAT);
    if (shm_clock_id == -1) {
        perror("Failed to create shared memory segment. \n");
        return 1;
	}
	// printf("My user segment id for shared clock is %d\n", shm_clock_id);
	
	// attach shared memory segment
	shared_clock* shm_clock = (shared_clock*)shmat(shm_clock_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (shm_clock == (void*)-1) {
        perror("Failed to attach shared memory segment. \n");
        return 1;
    }
	// printf("My user shared clock address is %x\n", shm_clock);
	
	int rsrc_key = 91514;
	int shm_rsrc_id = shmget(rsrc_key, sizeof(shared_resources)*20, PERM | IPC_CREAT);
    if (shm_rsrc_id == -1) {
        perror("Failed to create shared resource segment. \n");
        return 1;
	}
	// printf("My user segment id for the resource share is %d\n", shm_rsrc_id);
	
	// attach shared memory segment
	shared_resources* shm_resources = (shared_resources*)shmat(shm_rsrc_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (shm_resources == (void*)-1) {
        perror("Failed to attach resources segment. \n");
        return 1;
    }
	// printf("My OS resources address is %x\n", shm_resources);
	
	printf("Child %d start at seconds: %d and nanoseconds: %ld.\n", pid, shm_clock->seconds, shm_clock->nanoseconds);
	 
	// strcpy(shm_resources->msgTest, "Hello!");  // for writing messages
	// printf("Child.\n");	
	
	srand(pid * time(NULL));
	long nano_end = 0;
	int sec_end = 0;
	long random_time = rand() % 1000000 + 1;
	if((shm_clock->nanoseconds + random_time)  < 1000000000){
			nano_end = shm_clock->nanoseconds + rand() % 1000000 + 1;
			sec_end = shm_clock->seconds;
		}
	else if((shm_clock->nanoseconds + random_time)  >= 1000000000){
		nano_end = (shm_clock->nanoseconds + random_time) - shm_clock->nanoseconds ;
		sec_end = shm_clock->seconds  + 1;
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
		if((sec_end < shm_clock->seconds && shm_resources->ready == 0) || (nano_end <= shm_clock->nanoseconds && sec_end <= shm_clock->seconds && shm_resources->ready == 0)){  
			shm_resources->pid = pid;
			sem_post(sem); // adds 1
			clear = 1;
			printf("Child: %d cleared sem at sec: %d, nano: %ld \n", pid, shm_clock->seconds, shm_clock->nanoseconds);
			shm_resources->release = 1;
			break;
		}
		else {
			sem_post(sem); // adds 1, cede CS, not ready to send msg.
			continue;
		}
	}
	
	sem_close(sem);  // disconnect from semaphore
	
	// detach from shared memory segment
	int detach = shmdt(shm_clock);
	if (detach == -1){
		perror("Failed to detach shared memory segment. \n");
		return 1;
	}
	
	// detach from msg memory segment
	detach = shmdt(shm_resources);
	if (detach == -1){
		perror("Failed to detach shared msg memory segment. \n");
		return 1;
	}
	
    return 0;
}