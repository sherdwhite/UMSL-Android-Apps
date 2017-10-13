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
	int shm_id = shmget(key, sizeof(shared_memory), PERM | IPC_CREAT);
    if (shm_id == -1) {
        perror("Failed to create shared memory segment");
        return 1;
	}
	// printf("My user segment id for shared memory is %d\n", shm_id);
	
	// attach shared memory segment
	shared_memory* shared = (shared_memory*)shmat(shm_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (shared == (void*)-1) {
        perror("Failed to attach shared memory segment");
        return 1;
    }
	// printf("My user shared address is %x\n", shared);
	
	int msgkey = 91514;
	int msg_id = shmget(msgkey, sizeof(messaging), PERM | IPC_CREAT);
    if (msg_id == -1) {
        perror("Failed to create shared memory segment");
        return 1;
	}
	// printf("My user segment id for the msg share is %d\n", msg_id);
	
	// attach shared memory segment
	messaging* shmMsg = (messaging*)shmat(msg_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (shmMsg == (void*)-1) {
        perror("Failed to attach message segment");
        return 1;
    }
	// printf("My OS message address is %x\n", shared);
	
	printf("seconds: %d\n", shared->seconds);
	printf("nanoseconds: %ld\n", shared->nanoseconds);
	 
	strcpy(messaging->shmMsg, "Hello!");

	// int i = 0;
	// Testing array of strings for data.
	// for(i = 0; i < 50; i++){
		// printf("%s", shared->data[i]);
	// }

		
	// detach from shared memory segment
	int detach = shmdt(shared);
	if (detach == -1){
		perror("Failed to detach shared memory segment");
		return 1;
	}
	
	// detach from msg memory segment
	detach = shmdt(shmMsg);
	if (detach == -1){
		perror("Failed to detach shared msg memory segment");
		return 1;
	}
	
    return 0;
}