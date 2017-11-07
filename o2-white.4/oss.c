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
#define MAXCHILDREN 5 // 18

int hi_queue[MAXCHILDREN];
int front, rear;

typedef struct {
	long total_CPU_time_sec;
	long total_CPU_time_ns;
	long total_time_sec;
	long total_time_ns;
	long last_burst_sec;
	long last_burst_ns;
	long wait_total;
	clock_t begin;
	clock_t end;
	pid_t pid;
	int scheduled;
	int complete;
	int ready;
} pcb;

typedef struct {
	unsigned int seconds;
	unsigned int nanoseconds;
} timer;

int max_time = 60;
FILE *file;
char *filename = "log";

void push(int child) {
    if (rear >= MAXCHILDREN - 1)
    {
        printf("\nQueue overflow no more elements can be inserted");
        return;
    }
    if ((front == -1) && (rear == -1)){
        front++;
        rear++;
        hi_queue[rear] = child;
        return;
    }    
    else{
        int i,j;
 
		for (i = 0; i <= rear; i++){
			if (child >= hi_queue[i]){
				for (j = rear + 1; j > i; j--)
				{
					hi_queue[j] = hi_queue[j - 1];
				}
				hi_queue[i] = child;
				return;
			}
		}
		hi_queue[i] = child;
	}
    rear++;
}

void pop(int child) {
    int i;
 
    if ((front==-1) && (rear==-1))
    {
        printf("Queue is empty no elements to delete. \n");
        return;
    }
 
    for (i = 0; i <= rear; i++)
    {
        if (child == hi_queue[i])
        {
            for (; i < rear; i++)
            {
                hi_queue[i] = hi_queue[i + 1];
            }
 
        hi_queue[i] = -99;
        rear--;
 
        if (rear == -1) 
            front = -1;
        return;
        }
    }
    printf("%d not found in queue to delete. \n", child);
	return;
}

void print_list() {
    if ((front == -1) && (rear == -1))
    {
        printf("\nQueue is empty");
        return;
    }
 
    for (front = 0; front <= rear; front++)
    {
        printf(" %d ", hi_queue[front]);
    }
 
    front = 0;
	return;
}

int main(int argc, char * argv[]) 
{
	int c;

	while ((c = getopt (argc, argv, "hl:t:")) != -1)
    switch (c)
		  {
			case 'h':
				break;
			case 'l':
				filename = strdup(optarg);
				break;
			case 't':
				max_time = atoi(optarg);
				if (max_time <= 0 || max_time > 60) {
					fprintf (stderr, "Can only specify time between 1 and 60 seconds. \n");
					// perror("Can only specify time between 1 and 60 seconds. \n");
					return 1;
				}
				break;
			case '?':
				if (optopt == 'l'){
					fprintf (stderr, "Option -%c requires an argument. \n", optopt);
					perror("No arguement value given! \n");
					return 1;
				}
				if (optopt == 't'){
					fprintf (stderr, "Option -%c requires an argument. \n", optopt);
					perror("No arguement value given! \n");
					return 1;
				}
				else if (isprint (optopt)){
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
					perror("Incorrect arguement given! \n");
					return 1;
				}
				else {
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
					perror("Unknown arguement given! \n");
					return 1;
				}
			default:
				exit(1);
		  }
		  
	// open log file
	file = fopen(filename, "w+");

	// create shared memory segment and get the segment id
	// IPC_PRIVATE, child process, created after the parent has obtained the
	// shared memory, so that the private key value can be passed to the child
	// when it is created.  Key could be arbitrary integer also.
	// IPC_CREAT says to create, but don't fail if it is already there
	// IPC_CREAT | IPC_EXCL says to create and fail if it already exists
	// PERM is read write, could also be number, say 0755 like chmod command
	int key = 92111;
	int pcb_id = shmget(key, sizeof(pcb)*18, PERM | IPC_CREAT | IPC_EXCL);
    if (pcb_id == -1) {
        perror("Failed to create shared memory segment. \n");
        return 1;
	}
	// printf("My OS segment id for shared memory is %d\n", pcb_id);
	
	// attach shared memory segment
	pcb* PCB = (pcb*)shmat(pcb_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (PCB == (void*)-1) {
        perror("Failed to attach shared memory segment. \n");
        return 1;
    }
	// printf("My OS shared address is %x\n", shared);
	
	int clock_key = 91514;  
	int timer_id = shmget(clock_key, sizeof(timer), PERM | IPC_CREAT | IPC_EXCL);
    if (timer_id == -1) {
        perror("Failed to create shared memory segment. \n");
        return 1;
	}
	// printf("My OS segment id for the msg share is %d\n", timer_id);
	
	// attach shared memory segment
	timer* shmTime = (timer*)shmat(timer_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (shmTime == (void*)-1) {
        perror("Failed to attach shared message segment. \n");
        return 1;
    }
	// printf("My OS message address is %x\n", PCB);
	
	// Initialize named semaphore for shared processes.  Create it if it wasn't created, 
	// 0644 permission. 1 is the initial value of the semaphore.  Named semaphore stored at /dev/shm
	sem_t *sem = sem_open("BellandJ", O_CREAT | O_EXCL, 0644, 1);
	if(sem == SEM_FAILED) {
        perror("Failed to sem_open clock. \n");
        return;
    }

	// set shmTime to zero.
	shmTime->seconds = 0;
	shmTime->nanoseconds = 0;
	
	int i, k;
	// initialize all PCB blocks
	for(i = 0; i < MAXCHILDREN; i++){
		PCB[i].total_CPU_time_sec = 0;
		PCB[i].total_CPU_time_ns = 0;
		PCB[i].total_time_sec = 0;
		PCB[i].total_time_ns = 0;
		PCB[i].last_burst_sec = 0;
		PCB[i].last_burst_ns = 0;
		PCB[i].pid = 0;
		PCB[i].scheduled = 0;
		PCB[i].ready = 1;
		PCB[i].complete = 0;
		PCB[i].wait_total = 0;
		PCB[i].begin = 0;
		PCB[i].end = 0;
	}	
	
	// pid_t childpid;
	// char cpid[12];
	// int i;
	// for (i = 0; i < max_children; i++) {
		// childpid = fork();
		// if (childpid == -1) {
			// perror("Failed to fork. \n");
			// return 1;
		// }
		// if (childpid == 0) { /* child code */
			// sprintf(cpid, "%d", i);
			// execlp("user", "user", cpid, NULL);  // lp for passing arguements
			// perror("Child failed to execlp. \n");
			// return 1;
		// }
	// }
	
	char shsec[2];
	char shnano[10];
	char msgtext[132];
	pid_t childpid;
	char cpid[12];
	int active_children = 0;
	long nano = 0;
	int sec = 0;
	long random_time = 0;
	int random_number;
	struct timespec delay;
	int total_log_lines = 0;
	front = -1, rear = -1;
	srand(time(NULL));
	
	do {
		random_time = rand() % 1000 + 1;
		if((random_time + shmTime->nanoseconds)  > 999999000){
			shmTime->nanoseconds += 0;
			shmTime->seconds  += 2;
		}
		else {
			shmTime->nanoseconds += random_time;
			shmTime->seconds += 1;
		}
		
		if(shmTime->seconds >= max_time || active_children > 18 || total_log_lines >= 10000){
			pid_t pid = getpgrp();  // gets process group
			printf("Terminating PID: %i due to limit met. \n", pid);
			sem_close(sem);  // disconnect from semaphore
			sem_unlink("BellandJ"); // destroy if all closed.
			shmctl(pcb_id, IPC_RMID, NULL);
			shmctl(timer_id, IPC_RMID, NULL);
			shmdt(PCB);
			shmdt(shmTime);
			killpg(pid, SIGINT);  // kills the process group
			exit(EXIT_SUCCESS);
			// break;
		}
		
		// nano = 0;
		// sec = 0;
		// random_time = rand() % 1999999999 + 1;
		// if((shmTime->nanoseconds + random_time)  < 1000000000){
				// nano = random_time;
			// }
		// else if((shmTime->nanoseconds + random_time)  >= 1000000000){
			// nano = random_time - 1000000000;
			// sec = 1;
		// }
		
		for (i = 0; i < MAXCHILDREN; i++) {
			if (PCB[i].scheduled == 1 && PCB[i].complete == 1){
				sprintf(shsec, "%d", shmTime->seconds);
				sprintf(shnano, "%ld", shmTime->nanoseconds);
				sprintf(msgtext, "Master: Child pid %d is terminating at my time %d:%ld. \n ", PCB[i].pid, shmTime->seconds, shmTime->nanoseconds);
				fputs(msgtext, file);
				fputs(shsec, file);
				fputs(".", file);
				fputs(shnano, file);
				fputs(". \n", file);
				total_log_lines += 1;
				pop(i);
				PCB[i].complete = 0;
				PCB[i].ready = 1;
				PCB[i].scheduled = 0;
				// active_children -= 1;
			}
			
			if(active_children < MAXCHILDREN && PCB[i].ready == 1){
				childpid = fork();
				if (childpid == -1) {
					perror("Master: Failed to fork.");
					return 1;
				}
				if (childpid == 0) { 
					delay.tv_sec = 1; // sec;
					delay.tv_nsec = 0; // nano;
					nanosleep(&delay, NULL);
					shmTime->seconds += 1;
					active_children++;
					printf("Master: Child pid %d is starting at my time %d:%ld. \n ", i, shmTime->seconds, shmTime->nanoseconds);
					
					// if((shmTime->nanoseconds + nano) < 1000000000){
							// shmTime->nanoseconds += nano;
						// }
					// else if((shmTime->nanoseconds + nano) >= 1000000000){
						// shmTime->nanoseconds = ((shmTime->nanoseconds + nano) - 1000000000);
						// shmTime->seconds += 1;
					// }
				}
				
				if (childpid != 0) {
					push(i);
					print_list();
					PCB[i].pid = i;    
					PCB[i].total_CPU_time_sec = 0;
					PCB[i].total_CPU_time_ns = 0;
					PCB[i].total_time_sec = 0;
					PCB[i].total_time_ns = 0;
					PCB[i].last_burst_sec = 0;
					PCB[i].last_burst_ns = 0;
					PCB[i].scheduled = 1;
					PCB[i].complete = 0;
					PCB[i].ready = 0;
					PCB[i].wait_total = 0;
					PCB[i].begin = clock();
					sprintf(cpid, "%d", i); 
					execlp("user", "user", cpid, NULL);  // lp for passing arguements
					//perror("Child failed to execlp. \n");
					sprintf(shsec, "%d", shmTime->seconds);
					sprintf(shnano, "%ld", shmTime->nanoseconds);
					sprintf(msgtext, "OSS: Generating process with PID %d at time ", PCB[i].pid);
					fputs(msgtext, file);
					fputs(shsec, file);
					fputs(":", file);
					fputs(shnano, file);
					fputs(". \n", file);
					printf("Active Children: %d. \n", active_children);
					// continue;
					random_time = rand() % 1000 + 1;
					if((random_time + shmTime->nanoseconds)  > 999999000){
						shmTime->nanoseconds += 0;
						shmTime->seconds  += 2;
					}
					else {
						shmTime->nanoseconds += random_time;
						shmTime->seconds += 1;
					}
				}
			}
			// code here for scheduling
			// for (i = 0; i < MAXCHILDREN; i++) {
				// if(PCB[i].scheduled == 1 && PCB[i].complete == 0 && PCB[i].queue == HI) {
					// random_number = rand() % 4;
					// if(random_number == 0){
						// kill PID
					// }
				// }
				
				
				// if a process isn't scheduled, need to keep track of wait time.
				// if(PCB[i].scheduled == 0) {
					// PCB[i].end = clock();
					// PCB[i].wait_total += (((PCB[i].end - PCB[i].begin) / CLOCKS_PER_SEC) * 1000000000);
					// PCB[i].begin = clock();
				// }
				
				
				// if(PCB[i].wait_total < HIPRIORITY) {
					// PCB[i].priority = HI;

				// }
				// else if(PCB[i].wait_total >= HIPRIORITY  && PCB[i].wait_total < LOWPRIORITY) {
					// PCB[i].priority = MEDIUM;

				// }
				// else if(PCB[i].wait_total >= MEDIUMPRIORITY) {
					// PCB[i].priority = LOW;

				// }
			// }
			
		}
	}while (active_children > 0);
	
	// wait for children
	int j;
	for (j = 0; j <= MAXCHILDREN; j++){
		wait(NULL);
	}
	printf("All children returned. \n");
	printf("Total Children end: %d. \n", active_children);
	
    // printf("Msg: %s\n", shmTime->msg);
	
	sem_close(sem);  // disconnect from semaphore
	sem_unlink("BellandJ"); // destroy if all closed.
	 
	// detach from shared memory segment
	int detach = shmdt(PCB);
	if (detach == -1){
		perror("Failed to detach shared memory segment. \n");
		return 1;
	}
	// delete shared memory segment
	int delete_mem = shmctl(pcb_id, IPC_RMID, NULL);
	if (delete_mem == -1){
		perror("Failed to remove shared memory segment. \n");
		return 1;
	}
	
	// detach from msg memory segment
	detach = shmdt(shmTime);
	if (detach == -1){
		perror("Failed to detach msg memory segment. \n");
		return 1;
	}
	// delete msg memory segment
	delete_mem = shmctl(timer_id, IPC_RMID, NULL);
	if (delete_mem == -1){
		perror("Failed to remove msg memory segment. \n");
		return 1;
	}

    return 0;
}

