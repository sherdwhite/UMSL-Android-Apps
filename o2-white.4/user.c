// Sherd White
// cs4760 Assignment 4
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
#define HIPRIORITY 10000					
#define MEDIUMPRIORITY 1000000			
#define LOWPRIORITY 100000000
#define HI 0
#define MEDIUM 1
#define LOW 2
#define QUANTUM 50000

typedef struct {
	long total_CPU_time_sec;
	long total_CPU_time_ns;
	long total_time_sec;
	long total_time_ns;
	long last_burst_sec;
	long last_burst_ns;
	int priority;
	int scheduled;
	int quantum;
	pid_t pid;
	int complete;
} pcb;

typedef struct {
	unsigned int seconds;
	unsigned int nanoseconds;
} timer;


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
	int pcb_id = shmget(key, sizeof(pcb)*18, SHM_RDONLY | IPC_CREAT);
    if (pcb_id == -1) {
        perror("Failed to create shared memory segment. \n");
        return 1;
	}
	// printf("My user segment id for shared PCB is %d\n", pcb_id);
	
	// attach shared memory segment
	pcb* PCB = (pcb*)shmat(pcb_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (PCB == (void*)-1) {
        perror("Failed to attach shared PCB segment. \n");
        return 1;
    }
	// printf("My user shared address is %x\n", PCB);
	
	int clock_key = 91514;
	int timer_id = shmget(clock_key, sizeof(timer), PERM | IPC_CREAT);
    if (timer_id == -1) {
        perror("Failed to create shared memory segment. \n");
        return 1;
	}
	// printf("My user segment id for the time share is %d\n", timer_id);
	
	// attach shared memory segment
	timer* shmTime = (timer*)shmat(timer_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (shmTime == (void*)-1) {
        perror("Failed to attach message segment. \n");
        return 1;
    }
	// printf("My OS time address is %x\n", shmTime);
	
	// Initialize named semaphore for shared processes.  Create it if it wasn't created, 
	// 0644 permission. 1 is the initial value of the semaphore
	sem_t *sem = sem_open("BellandJ", 1);
	if(sem == SEM_FAILED) {
        perror("Child Failed to sem_open. \n");
        return;
    }
	
	printf("Child %d start at seconds: %d and nanoseconds: %ld.\n", pid, shmTime->seconds, shmTime->nanoseconds);
	 
	// strcpy(shmMsg->msgTest, "Hello!");  // for writing messages
	
	srand(pid * time(NULL));
	
	// printf("Child: %d end time is %d sec and %ld nanoseconds. \n", pid, sec_end, nano_end);
	
	// int sem_value;
	// sem_getvalue(sem, &sem_value);
	// printf("Child: %d, Semaphore value is %d. \n", pid, sem_value);
	int clear = 0;
	while(clear == 0){
		sem_wait(sem);  // wait until we can subtract 1
		// printf("Child: %d cleared sem_wait. \n", pid);
		// Critical Section
		if(PCB[pid].scheduled == 1){  
			//shmTime->seconds = PCB->seconds;
			sem_post(sem); // adds 1
			clear = 1;
			printf("Child: %d cleared sem at sec: %d, nano: %ld \n", pid, shmTime->seconds, shmTime->nanoseconds);
			break;
		}
		else {
			sem_post(sem); // adds 1, cede CS, not ready to send msg.
			continue;
		}
	}
	
	sem_close(sem);  // disconnect from semaphore
	
	// detach from shared memory segment
	int detach = shmdt(PCB);
	if (detach == -1){
		perror("Failed to detach shared memory segment. \n");
		return 1;
	}
	
	// detach from msg memory segment
	detach = shmdt(shmTime);
	if (detach == -1){
		perror("Failed to detach shared msg memory segment. \n");
		return 1;
	}
	
    return 0;
}