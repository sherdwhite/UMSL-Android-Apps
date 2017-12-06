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
#define MAX_PROCESSES 12

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

// populate resource
int process_queue[MAX_PROCESSES];
int front, rear;
int memory[256];
char page_table[257];
int max_children = 18;
int max_time = 2;
int available = 256;
FILE *file;
char *filename = "log";
int queue = 0;

void push(int child) {
    if (rear >= max_children - 1) {
        printf("\nQueue overflow no more elements can be inserted");
        return;
    }
    if ((front == -1) && (rear == -1)){
        front++;
        rear++;
        process_queue[rear] = child;
        return;
    }    
    else{
        int i,j;
 
		for (i = 0; i <= rear; i++){
			if (child >= process_queue[i]){
				for (j = rear + 1; j > i; j--){
					process_queue[j] = process_queue[j - 1];
				}
				process_queue[i] = child;
				return;
			}
		}
		process_queue[i] = child;
	}
    rear++;
}

void pop(int child) {
    int i;
 
    if ((front==-1) && (rear==-1)) {
        printf("Queue is empty no elements to delete. \n");
        return;
    }
 
    for (i = 0; i <= rear; i++){
        if (child == process_queue[i]){
            for (; i < rear; i++){
                process_queue[i] = process_queue[i + 1];
            }
 
        process_queue[i] = -99;
        rear--;
 
        if (rear == -1) 
            front = -1;
        return;
        }
    }
    printf("%d not found in queue to delete. \n", child);
	return;
}

// void signalHandler();

int main(int argc, char * argv[]) 
{
	
	// Signal Handler
	// signal(SIGINT, signalHandler);
	// signal(SIGSEGV, signalHandler);
	
	int c;
	clock_t begin = clock();
	clock_t end;
	double elapsed_secs;
	srand(time(NULL));

	while ((c = getopt (argc, argv, "hs:l:t:")) != -1)
    switch (c)
		  {
			case 'h':
				break;
			case 's':
				max_children = atoi(optarg);
				if (max_children <= 0 || max_children > 18) {
					fprintf (stderr, "Can only specify 1 to 18 children. \n");
					// perror("Can only specify 1 to 18 children. \n");
					return 1;
				}
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
				if (optopt == 's'){
					fprintf (stderr, "Option -%c requires an argument. \n", optopt);
					perror("No arguement value given! \n");
					return 1;
				}
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
	int shm_clock_id = shmget(key, sizeof(shared_clock), PERM | IPC_CREAT | IPC_EXCL);
    if (shm_clock_id == -1) {
        perror("Failed to create shared clock segment. \n");
        return 1;
	}
	// printf("My OS segment id for shared memory is %d\n", shm_clock_id);
	
	// attach shared memory segment
	shared_clock* shm_clock = (shared_clock*)shmat(shm_clock_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (shm_clock == (void*)-1) {
        perror("Failed to attach shared clock segment. \n");
        return 1;
    }
	// printf("My OS shared clock address is %x\n", shm_clock);
	
	int mem_key = 91514;
	int shm_id = shmget(mem_key, sizeof(shared_memory)*18, PERM | IPC_CREAT | IPC_EXCL);
    if (shm_id == -1) {
        perror("Failed to create shared resources segment. \n");
        return 1;
	}
	// printf("My OS segment id for the resource share is %d\n", shm_id);
	
	// attach shared memory segment
	shared_memory* sh_mem = (shared_memory*)shmat(shm_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (sh_mem == (void*)-1) {
        perror("Failed to attach shared resources segment. \n");
        return 1;
    }
	
	// Initialize named semaphore for shared processes.  Create it if it wasn't created, 
	// 0644 permission. 1 is the initial value of the semaphore
	sem_t *sem = sem_open("BellandJ", O_CREAT | O_EXCL, 0644, 1);
	if(sem == SEM_FAILED) {
        perror("Failed to sem_open. \n");
        return;
    }	
	
	// set clock to zero
	shm_clock->seconds  = 0;
	shm_clock->nanoseconds  = 0;
	
	int i;	
	// set resources to zero
	for(i = 0; i < 20; i++){
		sh_mem[i].pid = 0;
		sh_mem[i].request = 0;
		sh_mem[i].allocation = 0;
		sh_mem[i].release = 0;
		sh_mem[i].ready = 1;
	}
	
	// set memory and paging to . (empty) to start
	for(i = 0; i < 256; i++){
		memory[i] = -1;
		page_table[i] = '.';
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
	
	// printf("Total Children: %d. \n", i);
	pid_t childpid;
	char cpid[12];
	char shsec[2];
	char shnano[10];
	char msgsec[2];
	char msgnano[10];
	char msgtext[132];
	char msgtext2[132];
	int total_log_lines = 0;
	int active_children = 0;
	struct timespec next;
	int random_time;
	int random_size = 32;
	next.tv_sec = 0;
	next.tv_nsec = 0;
	int j = 0;
	int p;
	page_table[256] = '\0';
	
	do {
		if(elapsed_secs >= max_time || active_children > max_children || total_log_lines >= 100000){
			pid_t pid = getpgrp();  // gets process group
			printf("Terminating PID: %i due to limit met. Elapsed Seconds: %d. Active chilren: %d; Log lines: %d. \n", pid, elapsed_secs, active_children, total_log_lines);
			sem_close(sem);  // disconnect from semaphore
			sem_unlink("BellandJ"); // destroy if all closed.
			shmctl(shm_clock_id, IPC_RMID, NULL);
			shmctl(shm_id, IPC_RMID, NULL);
			shmdt(shm_clock);
			shmdt(sh_mem);
			killpg(pid, SIGINT);  // kills the process group
			exit(EXIT_SUCCESS);
			// break;
		}

		sem_wait(sem);  // wait until we can subtract 1
		if(shm_clock->nanoseconds  > 999990000){
			shm_clock->nanoseconds  = 0;
			shm_clock->seconds  += 1;
		}
		else{
			shm_clock->nanoseconds += 10000;
		}
		sem_post(sem); // adds 1

		for(i = 0; i < max_children; i++){
			if(queue > 0){
				pop(i);
				available = 0;
				for(j = 0; j < 256; j++){
					if(memory[j] == -1 && page_table[j] == '.'){
						available++;
					}
				}
				if(random_size >= available){
					sprintf(shsec, "%d", shm_clock->seconds);
					sprintf(shnano, "%ld", shm_clock->nanoseconds);
					sprintf(msgtext, "OSS: Child pid %d moved from queue to memory at my time ", i);
					fputs(msgtext, file);
					fputs(shsec, file);
					fputs(".", file);
					fputs(shnano, file);
					fputs(".\n", file);
					sprintf(msgtext2, "OSS: %c ", page_table);
					fputs(msgtext2, file);
					fputs(".\n", file);
					total_log_lines++;
					sh_mem[i].request = 0;
					sh_mem[i].allocation = 1;
					sh_mem[i].release = 0;
					while(available > 0){
						for(j = 0; j < 256; j++){
							if(memory[j] == -1 && page_table[j] == '.'){
								memory[j] = i;
								page_table[j] = 'D';
								available--;
							}
						}
					}	
				}
				else{
					push(i);
				}
			}
			
			if(sh_mem[i].release == 1){
				sprintf(shsec, "%d", shm_clock->seconds);
				sprintf(shnano, "%ld", shm_clock->nanoseconds);
				sprintf(msgtext, "OSS: Child pid %d is releasing memory at my time ", i);
				fputs(msgtext, file);
				fputs(shsec, file);
				fputs(".", file);
				fputs(shnano, file);
				fputs(".\n", file);
				total_log_lines++;
				sh_mem[i].request = 0;
				sh_mem[i].allocation = 0;
				sh_mem[i].release = 0;
				sh_mem[i].ready = 1;
				for(j = 0; j < 256; j++){
					memory[j] = -1;
					page_table[j] = '.';
				}
				active_children--;
			}
			
			if(sh_mem[i].request == 1 && sh_mem[i].allocation == 0){
				available = 0;
				for(j = 0; j < 256; j++){
					if(memory[j] == -1 && page_table[j] == '.'){
						available++;
					}
				}
				random_size = rand() % 32 + 1;
				if(random_size >= available){
					sprintf(shsec, "%d", shm_clock->seconds);
					sprintf(shnano, "%ld", shm_clock->nanoseconds);
					sprintf(msgtext, "OSS: Child pid %d is allocated to memory at my time ", i);
					fputs(msgtext, file);
					fputs(shsec, file);
					fputs(".", file);
					fputs(shnano, file);
					fputs(".\n", file);
					total_log_lines++;
					sh_mem[i].request = 0;
					sh_mem[i].allocation = 1;
					sh_mem[i].release = 0;
					sh_mem[i].size = random_size;
					while(available > 0){
						for(j = 0; j < 256; j++){
							if(memory[j] == -1 && page_table[j] == '.'){
								memory[j] = i;
								page_table[j] = 'D';
								available--;
							}
						}
					}	
				}
				else{
					push(i);
					sh_mem[i].size = random_size;
					sprintf(shsec, "%d", shm_clock->seconds);
					sprintf(shnano, "%ld", shm_clock->nanoseconds);
					sprintf(msgtext, "OSS: Child pid %d hit page fault at my time ", i);
					fputs(msgtext, file);
					fputs(shsec, file);
					fputs(".", file);
					fputs(shnano, file);
					fputs(" and putting process in the queue.\n", file);
					sprintf(msgtext2, "OSS: %c ", page_table);
					fputs(msgtext2, file);
					fputs(".\n", file);
					total_log_lines++;
				}
			}
		}
		
		if(active_children < max_children && next.tv_sec <= shm_clock->seconds && next.tv_nsec <=shm_clock->nanoseconds){  
			random_time = rand() % 50000000 + 1000000; // nano;
			if((random_time + shm_clock->nanoseconds) >= 1000000000){
				next.tv_sec = shm_clock->seconds + 1;
				next.tv_nsec = random_time - shm_clock->nanoseconds;
			}
			else {
				next.tv_sec = shm_clock->seconds;
				next.tv_nsec = random_time + shm_clock->nanoseconds;
			}

			if (sh_mem[j].ready == 1) {
				childpid = fork();
				if (childpid == -1) {
					perror("OSS: Failed to fork.");
					return 1;
				}
				if (childpid == 0) { 
					printf("OSS: Child pid %d is starting at my time %d:%ld. \n ", j, shm_clock->seconds, shm_clock->nanoseconds);
					sprintf(cpid, "%d", j); 
					sh_mem[j].pid = j;
					execlp("user", "user", cpid, NULL);  // lp for passing arguements
					active_children++;
					printf("Active Children: %d. \n", active_children);
					//perror("Child failed to execlp. \n");
				}
				
				if (childpid != 0 && sh_mem[j].ready == 1) {
					sh_mem[j].ready = 0;
					sprintf(shsec, "%d", shm_clock->seconds);
					sprintf(shnano, "%ld", shm_clock->nanoseconds);
					sprintf(msgtext, "OSS: Generating process with PID %d at time ", i);
					fputs(msgtext, file);
					fputs(shsec, file);
					fputs(":", file);
					fputs(shnano, file);
					fputs(". \n", file);
					total_log_lines++;
					j++;
				}
			}
		}
		
		end = clock();
		elapsed_secs = (double)(end - begin) / CLOCKS_PER_SEC;  //only reports in seconds.
	} while(elapsed_secs < max_time);
	
	// wait for children
	int k;
	for (k = 0; k <= i; k++){
		wait(NULL);
	}
	printf("All children returned. \n");
	printf("Total Children end: %d. \n", i);
	
	sem_close(sem);  // disconnect from semaphore
	sem_unlink("BellandJ"); // destroy if all closed.
	 
	// detach from shared memory segment
	int detach = shmdt(shm_clock);
	if (detach == -1){
		perror("Failed to detach clock memory segment. \n");
		return 1;
	}
	// delete shared memory segment
	int delete_mem = shmctl(shm_clock_id, IPC_RMID, NULL);
	if (delete_mem == -1){
		perror("Failed to remove shared memory segment. \n");
		return 1;
	}
	
	// detach from msg memory segment
	detach = shmdt(sh_mem);
	if (detach == -1){
		perror("Failed to detach msg memory segment. \n");
		return 1;
	}
	// delete msg memory segment
	delete_mem = shmctl(shm_id, IPC_RMID, NULL);
	if (delete_mem == -1){
		perror("Failed to remove msg memory segment. \n");
		return 1;
	}

    return 0;
}

// void signalHandler() {
			
// }