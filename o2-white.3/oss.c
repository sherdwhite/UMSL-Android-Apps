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
#include <fcntl.h>

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

int max_time = 20;
int max_children = 5;
FILE *file;
char *filename = "log";

int main(int argc, char * argv[]) 
{
	int c;
	while ((c = getopt (argc, argv, "hs:l:t:")) != -1)
    switch (c)
		  {
			case 'h':
				break;
			case 's':
				max_children = atoi(optarg);
				if (max_children <= 0 || max_children > 16) {
					fprintf (stderr, "Can only specify 1 to 16 children. \n");
					// perror("Can only specify 1 to 16 children. \n");
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
	int shm_id = shmget(key, sizeof(shared_memory), PERM | IPC_CREAT | IPC_EXCL);
    if (shm_id == -1) {
        perror("Failed to create shared memory segment. \n");
        return 1;
	}
	// printf("My OS segment id for shared memory is %d\n", shm_id);
	
	// attach shared memory segment
	shared_memory* shared = (shared_memory*)shmat(shm_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (shared == (void*)-1) {
        perror("Failed to attach shared memory segment. \n");
        return 1;
        }
	// printf("My OS shared address is %x\n", shared);
	
	int msgkey = 91514;
	int msg_id = shmget(msgkey, sizeof(messaging), PERM | IPC_CREAT | IPC_EXCL);
    if (msg_id == -1) {
        perror("Failed to create shared memory segment. \n");
        return 1;
	}
	// printf("My OS segment id for the msg share is %d\n", msg_id);
	
	// attach shared memory segment
	messaging* shmMsg = (messaging*)shmat(msg_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (shmMsg == (void*)-1) {
        perror("Failed to attach shared message segment. \n");
        return 1;
    }
	// printf("My OS message address is %x\n", shared);
	// set shared to zero
	shared->seconds  = 0;
	shared->nanoseconds  = 0;
	
	// set shmMsg to zero.
	shmMsg->pid = 0;
	shmMsg->seconds = 0;
	shmMsg->nanoseconds = 0;
	
	// Initialize named semaphore for shared processes.  Create it if it wasn't created, 
	// 0644 permission. 1 is the initial value of the semaphore
	sem_t *sem = sem_open("BellandJ", O_CREAT | O_EXCL, 0644, 1);
	if(sem == SEM_FAILED) {
        perror("Failed to sem_open. \n");
        return;
    }	
	
	pid_t childpid;
	int i;
	int total_children = 0;
	for (i = 0; i < max_children; i++) {
		childpid = fork();
		total_children++;
		if (childpid == -1) {
			perror("Failed to fork. \n");
			return 1;
		}
		if (childpid == 0) { /* child code */
			char cpid[12];
			sprintf(cpid, "%d", i);
			execlp("user", "user", cpid, NULL);  // lp for passing arguements
			perror("Child failed to execlp. \n");
			return 1;
		}
	}
	
	printf("Total Children: %d. \n", total_children);
	
	char shsec[2];
	char shnano[10];
	char msgsec[2];
	char msgnano[10];
	char msgtext[132];
	while (total_children > 0){
		if(shared->nanoseconds  <= 999000000){
			shared->nanoseconds += 100000;
		}
		if(shared->nanoseconds  > 999000000){
			shared->nanoseconds  = 0;
			shared->seconds  += 1;
		}
		if(shmMsg->seconds != 0 || shmMsg->nanoseconds != 0){
			sprintf(shsec, "%d", shared->seconds);
			sprintf(shnano, "%ld", shared->nanoseconds);
			sprintf(msgsec, "%d", shmMsg->seconds);
			sprintf(msgnano, "%ld", shmMsg->nanoseconds);
			sprintf(msgtext, "Master: Child pid %d is terminating at my time ", shmMsg->pid);
			fputs(msgtext, file);
			fputs(shsec, file);
			fputs(".", file);
			fputs(shnano, file);
			fputs(" because it reached ", file);
			fputs(msgsec, file);
			fputs(".", file);
			fputs(msgnano, file);
			fputs(" in slave. \n", file);
			shmMsg->pid = 0;
			shmMsg->seconds = 0;
			shmMsg->nanoseconds = 0;
			total_children--;
		}
		if(shared->seconds >= 100){
			break;
		}
		else {
			shared->nanoseconds += 100000;
		}
	}
	
	// wait for children
	int j;
	for (j = 0; j < max_children; j++){
		wait(NULL);
	}
	printf("All children returned. \n");
	
    // printf("Msg: %s\n", shmMsg->msg);
	
	sem_close(sem);  // disconnect from semaphore
	sem_unlink("BellandJ"); // destroy if all closed.
	 
	// detach from shared memory segment
	int detach = shmdt(shared);
	if (detach == -1){
		perror("Failed to detach shared memory segment. \n");
		return 1;
	}
	// delete shared memory segment
	int delete_mem = shmctl(shm_id, IPC_RMID, NULL);
	if (delete_mem == -1){
		perror("Failed to remove shared memory segment. \n");
		return 1;
	}
	
	// detach from msg memory segment
	detach = shmdt(shmMsg);
	if (detach == -1){
		perror("Failed to detach msg memory segment. \n");
		return 1;
	}
	// delete msg memory segment
	delete_mem = shmctl(msg_id, IPC_RMID, NULL);
	if (delete_mem == -1){
		perror("Failed to remove msg memory segment. \n");
		return 1;
	}

    return 0;
}