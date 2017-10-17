// Sherd White
// cs4760 Assignment 3
// 10/04/2017

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

#define PERM (S_IRUSR | S_IWUSR)
#define LENGTH 132

typedef struct {
	int seconds;
	long nanoseconds;
} shared_memory;

typedef struct {
	pid_t pid;
	int seconds;
	long nanoseconds;
} messaging;

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
	int shm_id = shmget(key, sizeof(shared_memory), SHM_RDONLY | IPC_CREAT);
    if (shm_id == -1) {
        perror("Failed to create shared memory segment. \n");
        return 1;
	}
	// printf("My user segment id for shared memory is %d\n", shm_id);
	
	// attach shared memory segment
	shared_memory* shared = (shared_memory*)shmat(shm_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (shared == (void*)-1) {
        perror("Failed to attach shared memory segment. \n");
        return 1;
    }
	// printf("My user shared address is %x\n", shared);
	
	int msgkey = 91514;
	int msg_id = shmget(msgkey, sizeof(messaging), PERM | IPC_CREAT);
    if (msg_id == -1) {
        perror("Failed to create shared memory segment. \n");
        return 1;
	}
	// printf("My user segment id for the msg share is %d\n", msg_id);
	
	// attach shared memory segment
	messaging* shmMsg = (messaging*)shmat(msg_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (shmMsg == (void*)-1) {
        perror("Failed to attach message segment. \n");
        return 1;
    }
	// printf("My OS message address is %x\n", shared);
	
	printf("Child %d start at seconds: %d\n", pid, shared->seconds);
	printf("Child %d start at nanoseconds: %ld\n", pid, shared->nanoseconds);
	 
	// strcpy(shmMsg->msgTest, "Hello!");  // for writing messages
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
	
	// Initialize named semaphore for shared processes.  Create it if it wasn't created, 
	// 0644 permission. 1 is the initial value of the semaphore
	sem_t *sem = sem_open("BellandJ", 1);
	if(sem == SEM_FAILED) {
        perror("Child Failed to sem_open. \n");
        return;
    }
	
	int sem_value;
	sem_getvalue(sem, &sem_value);
	printf("Child: %d, Semaphore value is %d. \n", pid, sem_value);
	int clear = 0;
	while(clear = 0){
		sem_wait(sem);  // wait until we can subtract 1
		printf("Child: %d sem_wait. \n", pid);
		// Critical Section
		if(nano_end < shared->nanoseconds && sec_end < shared->seconds){
			if(shmMsg->pid == 0 && shmMsg->seconds == 0 && shmMsg->nanoseconds == 0){
				shmMsg->pid = pid;
				shmMsg->seconds = shared->seconds;
				shmMsg->nanoseconds = shared->nanoseconds;
				sem_post(sem); // adds 1
				clear = 1;
				printf("Child: %d cleared sem at sec: %d, nano: %ld \n", pid, shmMsg->seconds, shmMsg->nanoseconds);
			}
			else{
				sem_post(sem); // adds 1 to wait until shmMsg clear.
			}
		}
		else {
			sem_post(sem); // adds 1, cede CS, not ready to send msg.
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
	detach = shmdt(shmMsg);
	if (detach == -1){
		perror("Failed to detach shared msg memory segment. \n");
		return 1;
	}
	
    return 0;
}