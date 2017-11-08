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
#include <fcntl.h>
#include <time.h>

#define PERM (S_IRUSR | S_IWUSR)
#define LENGTH 132
#define HIPRIORITY 10000					
#define MEDIUMPRIORITY 1000000			
#define LOWPRIORITY 100000000
#define QUANTUM 50000

typedef struct {
	long total_CPU_time_sec;
	long total_CPU_time_ns;
	long total_time_sec;
	long total_time_ns;
	long last_burst_sec;
	long last_burst_ns;
	long start_wait;
	long end_wait;
	long wait_total;
	pid_t pid;
	int scheduled;
	int complete;
	int ready;
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
	PCB[i].start_wait = clock();
	
	srand(pid * time(NULL));
	
	struct timespec delay;
	
	// int sem_value;
	// sem_getvalue(sem, &sem_value);
	// printf("Child: %d, Semaphore value is %d. \n", pid, sem_value);
	int clear = 0;
	int quantum_check, completed;
	unsigned int run_time = QUANTUM;
	while(clear == 0){
		sem_wait(sem);  // wait until we can subtract 1
		PCB[i].end_wait = clock();
		PCB[i].wait_total += (((PCB[i].end_wait - PCB[i].start_wait) / CLOCKS_PER_SEC) * 1000000000);
		printf("Child: %d cleared sem_wait. \n", pid);
		// Critical Section
		if(PCB[pid].scheduled == 1){  
			//shmTime->seconds = PCB->seconds;
			quantum_check = rand() % 2;
			if(quantum_check == 0){
				run_time = rand() % QUANTUM;
				delay.tv_sec = 0; // sec;
				delay.tv_nsec = run_time; // nano;
				nanosleep(&delay, NULL);
				PCB[pid].last_burst_ns = run_time;
				PCB[pid].total_CPU_time_ns += run_time;
				completed = rand() % 2;
				if(completed == 0 && PCB[pid].total_time_ns < 50000000){
					sem_post(sem); // adds 1
					PCB[i].start_wait = clock();
					printf("Child(.1): %d Wait. \n", pid);
					continue;	
				}
				if(completed == 1 || PCB[pid].total_time_ns >= 50000000){
					sem_post(sem); // adds 1
					clear = 1;
					PCB[pid].complete = 1;
					printf("Child(.1): %d cleared sem at sec: %d, nano: %ld \n", pid, shmTime->seconds, shmTime->nanoseconds);
					break;
				}
			}
			else {
				PCB[pid].last_burst_ns = QUANTUM;
				PCB[pid].total_CPU_time_ns += QUANTUM;
				run_time = QUANTUM;
				delay.tv_sec = 0; // sec;
				delay.tv_nsec = run_time; // nano;
				completed = rand() % 2;
				if(completed == 0 && PCB[pid].total_time_ns < 50000000){
					sem_post(sem); // adds 1
					printf("Child(.2): %d Wait. \n", pid);
					PCB[i].start_wait = clock();
					continue;	
				}
				if(completed == 1 || PCB[pid].total_time_ns >= 50000000){
					sem_post(sem); // adds 1
					clear = 1;
					PCB[pid].complete = 1;
					printf("Child(.2): %d cleared sem at sec: %d, nano: %ld \n", pid, shmTime->seconds, shmTime->nanoseconds);
					break;
				}
			}
		}
		else {
			sem_post(sem); // adds 1, cede CS, not scheduled.
			printf("Child: %d Not scheduled. \n", pid);
			PCB[i].start_wait = clock();
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