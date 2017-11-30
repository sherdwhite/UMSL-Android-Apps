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
	int resource_descriptor;
	int release;
	int ready;
} shared_memory;

// populate resource
int process_queue[MAX_PROCESSES];
int memory[256];
char page_table[256];
int max_children = 18;
int max_time = 2;
FILE *file;
char *filename = "log";

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
	int shm_id = shmget(mem_key, sizeof(shared_memory)*20, PERM | IPC_CREAT | IPC_EXCL);
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
	
	int page_key = 91514;
	int page_id = shmget(page_key, sizeof(page_table), PERM | IPC_CREAT | IPC_EXCL);
    if (page_id == -1) {
        perror("Failed to create shared resources segment. \n");
        return 1;
	}
	// printf("My OS segment id for the resource share is %d\n", page_id);
	
	// attach shared memory segment
	page_table* paging = (page_table*)shmat(page_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (paging == (void*)-1) {
        perror("Failed to attach shared resources segment. \n");
        return 1;
    }
	
	int m_key = 91514;
	int mem_id = shmget(m_key, sizeof(memory), PERM | IPC_CREAT | IPC_EXCL);
    if (mem_id == -1) {
        perror("Failed to create shared resources segment. \n");
        return 1;
	}
	// printf("My OS segment id for the resource share is %d\n", mem_id);
	
	// attach shared memory segment
	memory* mem = (memory*)shmat(mem_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (mem == (void*)-1) {
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
		sh_mem[i].resource_descriptor = 99;
		sh_mem[i].release = 0;
		sh_mem[i].ready = 1;
	}
	
	// set memory and paging to . (empty) to start
	for(i = 0; i < 256; i++){
		paging[i] = ".";
		mem[i] = ".";
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
	int total_log_lines = 0;
	int active_children = 0;
	struct timespec next;
	int random_time;
	int random_resource;
	next.tv_sec = 0;
	next.tv_nsec = 0;
	int j = 0;
	int p;
	
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
			if(sh_mem[i].release == 1){
				sprintf(shsec, "%d", shm_clock->seconds);
				sprintf(shnano, "%ld", shm_clock->nanoseconds);
				sprintf(msgtext, "OSS: Child pid %d is releasing resources %d at my time ", i, sh_mem[i].resource_descriptor);
				fputs(msgtext, file);
				fputs(shsec, file);
				fputs(".", file);
				fputs(shnano, file);
				fputs(".\n", file);
				total_log_lines++;
				sh_mem[i].request = 0;
				sh_mem[i].allocation = 0;
				resource_queue[sh_mem[i].resource_descriptor]--;
				sh_mem[i].resource_descriptor = 99;
				sh_mem[i].release = 0;
				sh_mem[i].ready = 1;
				active_children--;
			}
			
			if(sh_mem[i].request == 1 && sh_mem[i].allocation == 0){
				random_resource = rand() % 20;
				sprintf(shsec, "%d", shm_clock->seconds);
				sprintf(shnano, "%ld", shm_clock->nanoseconds);
				sh_mem[i].resource_descriptor = random_resource;
				sprintf(msgtext, "OSS: Child pid %d is allocated to resource %d at my time ", i, random_resource);
				fputs(msgtext, file);
				fputs(shsec, file);
				fputs(".", file);
				fputs(shnano, file);
				fputs(".\n", file);
				total_log_lines++;
				sh_mem[i].request = 0;
				sh_mem[i].allocation = 1;
				sh_mem[i].release = 0;
				resource_queue[random_resource]++;
			}
		}
		
		if(j >= max_children){
			j = 0;
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
					sprintf(msgtext, "OSS: Generating process with PID %d at time ", sh_mem[i].pid);
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
		
		// Deadlock Detection
		for(i = 0; i < 20; i++){
			if(resource_queue[i] >=10){
				for(p = 0; p < max_children; p++){
					if(sh_mem[p].resource_descriptor == i){
						sprintf(shsec, "%d", shm_clock->seconds);
						sprintf(shnano, "%ld", shm_clock->nanoseconds);
						sprintf(msgtext, "OSS: Deadlock on resource queue %i. Child pid %d is releasing resources at my time ", i, p);
						// printf("OSS: Deadlock on resource %i. Child pid %d is releasing resources at my time %d:%ld. \n", i, p, sh_mem[i].resource_descriptor, shm_clock->seconds, shm_clock->nanoseconds);
						fputs(msgtext, file);
						fputs(shsec, file);
						fputs(".", file);
						fputs(shnano, file);
						fputs(".\n", file);
						total_log_lines++;
						sh_mem[p].request = 0;
						sh_mem[p].allocation = 0;
						resource_queue[sh_mem[p].resource_descriptor]--;
						sh_mem[p].resource_descriptor = 99;
						sh_mem[p].release = 0;
						sh_mem[p].ready = 1;
						active_children--;
					}
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