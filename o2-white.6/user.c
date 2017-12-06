// Sherd White
// cs4760 Assignment 6
// 11/21/2017

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
	int read_write;
	int size;
} shared_memory;

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
        perror("Failed to create shared clock segment. \n");
        return 1;
	}
	// printf("My OS segment id for shared clock is %d\n", shm_clock_id);
	
	// attach shared memory segment
	shared_clock* shm_clock = (shared_clock*)shmat(shm_clock_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (shm_clock == (void*)-1) {
        perror("Failed to attach shared clock segment. \n");
        return 1;
    }
	// printf("My OS shared clock address is %x\n", shm_clock);
	
	int mem_key = 91514;
	int shm_id = shmget(mem_key, sizeof(shared_memory)*18, PERM | IPC_CREAT);
    if (shm_id == -1) {
        perror("Failed to create shared memory segment. \n");
        return 1;
	}
	// printf("My OS segment id for the resource share is %d\n", shm_id);
	
	// attach shared memory segment
	shared_memory* sh_mem = (shared_memory*)shmat(shm_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (sh_mem == (void*)-1) {
        perror("Failed to attach shared memory segment. \n");
        return 1;
    }
	
	printf("Child %d start at seconds: %d and nanoseconds: %ld.\n", pid, shm_clock->seconds, shm_clock->nanoseconds);
	 
	// strcpy(sh_mem->msgTest, "Hello!");  // for writing messages
	// printf("Child.\n");	
	
	srand(time(NULL));
	long nano_end = 0;
	int sec_end = 0;
	long random_time = rand() % 250000000;
	random_time = rand() % 250000000;
	if((shm_clock->nanoseconds + random_time)  < 1000000000){
			nano_end = shm_clock->nanoseconds + random_time;
			sec_end = shm_clock->seconds;
	}
	else if((shm_clock->nanoseconds + random_time)  >= 1000000000){
		nano_end = (shm_clock->nanoseconds + random_time) - shm_clock->nanoseconds ;
		sec_end = shm_clock->seconds  + 1;
	}
	
	srand(time(NULL) * random_time);
	
	printf("Child: %d end time is %d sec and %ld nanoseconds. \n", pid, sec_end, nano_end);
	
	int clear = 0;
	while(clear == 0){
		if(rand() % 2 == 1){
			sh_mem[pid].read_write = 1;
			clear = 1;
			printf("Child: %d read memory at sec: %d, nano: %ld \n", pid, shm_clock->seconds, shm_clock->nanoseconds);
		}
		else{
			sh_mem[pid].read_write = 0;
			clear = 1;
			printf("Child: %d wrote memory at sec: %d, nano: %ld \n", pid, shm_clock->seconds, shm_clock->nanoseconds);
		}
		
		if(nano_end <= shm_clock->nanoseconds && sec_end <= shm_clock->seconds){
			if(rand() % 10 <= 1){
				sh_mem[pid].request = 0;
				sh_mem[pid].allocation = 0;
				sh_mem[pid].release = 1;
				clear = 1;
				printf("Child: %d releasing resources at sec: %d, nano: %ld \n", pid, shm_clock->seconds, shm_clock->nanoseconds);
			}
			else{
				random_time = rand() % 250000000;
				if((shm_clock->nanoseconds + random_time)  < 1000000000){
						nano_end = shm_clock->nanoseconds + random_time;
						sec_end = shm_clock->seconds;
				}
				else if((shm_clock->nanoseconds + random_time)  >= 1000000000){
					nano_end = (shm_clock->nanoseconds + random_time) - shm_clock->nanoseconds ;
					sec_end = shm_clock->seconds  + 1;
				}
			}
		}
		else{
			if(rand() % 10 <= 1){
				if(sh_mem[pid].request == 0 && sh_mem[pid].allocation == 0){
					sh_mem[pid].request = 1;
					sh_mem[pid].allocation == 0;
					sh_mem[pid].release = 0;
				}
				else if(sh_mem[pid].request == 0 && sh_mem[pid].allocation == 1){
					sh_mem[pid].request = 0;
					sh_mem[pid].allocation == 0;
					sh_mem[pid].release = 1;
				}
			}
		}
	}
	
	// detach from shared memory segment
	int detach = shmdt(shm_clock);
	if (detach == -1){
		perror("Failed to detach shared memory segment. \n");
		return 1;
	}
	
	// detach from msg memory segment
	detach = shmdt(sh_mem);
	if (detach == -1){
		perror("Failed to detach shared msg memory segment. \n");
		return 1;
	}
	
    return 0;
}