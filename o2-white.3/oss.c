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
#define SIZE 50 
#define LENGTH 132

typedef struct {
	int id;
	int index;  //key_t key;
	char data[SIZE][LENGTH];
} shared_memory;

int main(int argc, char * argv[]) 
{
	if (argc <= 1)
	{
		fprintf(stderr, "A missing or incorrect file specified.\n");
		return 1;
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
	// printf("My master segment id for shared memory is %d\n", shm_id);
	
	int msgkey = 91514;
	int msg_id = shmget(msgkey, sizeof(shared_memory), PERM | IPC_CREAT | IPC_EXCL);
    if (msg_id == -1) {
        perror("Failed to create shared memory segment");
        return 1;
	// printf("My master segment id for the msg share is %d\n", msg_id);

	// attach shared memory segment
	shared_memory* shared = (shared_memory*)shmat(shm_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (shared == (void*)-1) {
        perror("Failed to attach shared memory segment");
        return 1;
        }
	// printf("My master shared address is %x\n", shared);
	
	// test saving data
	// shared->id  = 1;
	// shared->data[0] = "test";
	// shared->index  = 2;
	// We can use the shared_memory shared to get access to shared memory.
	// Could also be shared[0].index or shared[0].flag, etc.
	// printf("The id is %d, the index is %d, the 1st string of the array is %s.\n", shared->id, shared->index, shared->data[0]);
	// printf("Master: The id is %d, the index is %d.\n", shared->id, shared->index);
	
	FILE *fp = fopen(argv[1], "r");
	if (fp == 0)
    {
        fprintf(stderr,"File %s not found\n",argv[1]);
        return 1;
    }
	
	int i = 0;
	char line[LENGTH];
	while (fgets(line, sizeof(line), fp)) {
			line[strlen(line) - 1] = '\0';
			strcpy(shared->data[i], line);
			//printf("%s ", shared->data[i]);
			i++;
	  }

	pid_t childpid;
	childpid = fork();
		if (childpid == -1) {
		perror("Failed to fork");
	return 1;
	}
	if (childpid == 0) { /* child code */
		// shared->id  = 0;
		shared->index  = 2;
		char indx[2];
		char cpid[12];
		sprintf(cpid, "%ld", (long)childpid);
		sprintf(indx, "%d", shared->index);
		execlp("palin", "palin", cpid, indx, NULL);
		perror("Child failed to execv");

	return 1;
	}
	if (childpid != wait(NULL)) { /* parent code */
		perror("Parent failed to wait due to signal or error");
		return 1;
	}
	
	// Testing array of strings for data.
	// for(i=0; i<50; i++){
		// printf(shared->data[i]);
    // }
	 
	// detach from memory segment
	int detach = shmdt(shared);
	if (detach == -1){
		perror("Failed to detach shared memory segment");
		return 1;
	}
	
	int delete_mem = shmctl(shm_id, IPC_RMID, NULL);
	if (delete_mem == -1){
		perror("Failed to remove shared memory segment");
		return 1;
	}

    return 0;
}