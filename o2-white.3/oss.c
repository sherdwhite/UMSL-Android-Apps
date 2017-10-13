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

#define PERM (S_IRUSR | S_IWUSR)
#define LENGTH 132

typedef struct {
	int seconds;
	int nanoseconds;
} shared_memory;

typedef struct {
	char msg[LENGTH];
} messaging;

int max_time = 20;
int max_children = 5;
FILE *file;

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
				if (max_children <= 0 || max_children > 18) {
					fprintf (stderr, "Can only specify 1 to 19 children.");
					perror("Can only specify 1 to 19 children.");
					return -1;
				}
				break;
			case 'l':
				file = fopen(optarg, "w+");
				break;
			case 't':
				max_time = atoi(optarg);
				if (max_time <= 0 || max_time > 60) {
					fprintf (stderr, "Can only specify time between 1 and 60 seconds.");
					perror("Can only specify time between 1 and 60 seconds.");
					return -1;
				}
				break;
			case '?':
				if (optopt == 's'){
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
					perror("No arguement value given!");
					return -1;
				}
				if (optopt == 'l'){
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
					perror("No arguement value given!");
					return -1;
				}
				if (optopt == 't'){
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
					perror("No arguement value given!");
					return -1;
				}
				else if (isprint (optopt)){
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
					perror("Incorrect arguement given!");
					return -1;
				}
				else {
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
					perror("Unknown arguement given!");
					return -1;
				}
			default:
				exit(-1);
		  }

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
        perror("Failed to create shared memory segment");
        return 1;
	}
	// printf("My OS segment id for shared memory is %d\n", shm_id);
	
	// attach shared memory segment
	shared_memory* shared = (shared_memory*)shmat(shm_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (shared == (void*)-1) {
        perror("Failed to attach shared memory segment");
        return 1;
        }
	// printf("My OS shared address is %x\n", shared);
	
	int msgkey = 91514;
	int msg_id = shmget(msgkey, sizeof(messaging), PERM | IPC_CREAT | IPC_EXCL);
    if (msg_id == -1) {
        perror("Failed to create shared memory segment");
        return 1;
	}
	// printf("My OS segment id for the msg share is %d\n", msg_id);
	
	// attach shared memory segment
	messaging* shmMsg = (messaging*)shmat(msg_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (shmMsg == (void*)-1) {
        perror("Failed to attach shared message segment");
        return 1;
    }
	// printf("My OS message address is %x\n", shared);
	
	shared->seconds  = 0;
	shared->nanoseconds  = 0;
	
	pid_t childpid;
	childpid = fork();
		if (childpid == -1) {
		perror("Failed to fork");
		return 1;
	}
	if (childpid == 0) { /* child code */
		// char cpid[12];
		// sprintf(cpid, "%ld", (long)childpid);
		// execlp("user", "user", cpid, NULL);  // lp for passing arguements
		execl("user", "user", NULL);
		perror("Child failed to execlp.\n");
		return 1;
	}
	if (childpid != wait(NULL)) { /* parent code */
		perror("Parent failed to wait due to signal or error");
		return 1;
	}
	
    // printf("Msg: %s\n", shmMsg->msg);
	 
	// detach from shared memory segment
	int detach = shmdt(shared);
	if (detach == -1){
		perror("Failed to detach shared memory segment");
		return 1;
	}
	// delete shared memory segment
	int delete_mem = shmctl(shm_id, IPC_RMID, NULL);
	if (delete_mem == -1){
		perror("Failed to remove shared memory segment");
		return 1;
	}
	
	// detach from msg memory segment
	detach = shmdt(shmMsg);
	if (detach == -1){
		perror("Failed to detach msg memory segment");
		return 1;
	}
	// delete msg memory segment
	delete_mem = shmctl(msg_id, IPC_RMID, NULL);
	if (delete_mem == -1){
		perror("Failed to remove msg memory segment");
		return 1;
	}

    return 0;
}