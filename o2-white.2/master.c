#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#define PERM (S_IRUSR | S_IWUSR)
#define SIZE 5000 /* size of shared memory segment in K */

int main(int argc, char * argv[]) 
{
	if (argc <= 1)
	{
		fprintf(stderr, "A missing or incorrect file specified.\n");
		return 1;
	}

	// vector<int> list1;
	// int loc;

	// ifstream fin(argv[1]);

	// while (fin >> loc)
	// {
		// list.push_back(loc);
	// }
	typedef struct {
		int shm_id, index, flag;
		//key_t key;
		char data[SIZE];
	} shared_memory;

	// create shared memory segment and get the segment id
	// IPC_PRIVATE, child process, created after the parent has obtained the 
	// shared memory, so that the private key value can be passed to the child 
	// when it is created.  Key could be arbitrary integer also.
	// IPC_CREAT says to create, but don't fail if it is already there
	// IPC_CREAT | IPC_EXCL says to create and fail if it already exists
	// PERM is read write, could also be number, say 0755 like chmod command
	int shm_id = shmget(IPC_PRIVATE, sizeof(shared_memory), PERM | IPC_CREAT | IPC_EXCL);
    if (shm_id == -1) {
        perror("Failed to create shared memory segment");
		return 1;
	}
	
	printf("My segment id is %d\n", shm_id);

	// attach shared memory segment
	shared_memory* ptr = (shared_memory*)shmat(shm_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
    if (ptr == (void*)-1) {
        perror("Failed to attach shared memory segment");
		return 1;
	}
	
	printf("My ptr address is %x\n", ptr);
	
	ptr->index = 0;
	ptr->flag = 0;
	ptr->data = 0;
	
	// We can use the shared_memory ptr to get access to shared memory.
	// Could also be ptr[0].index or ptr[0].flag, etc.
	printf("The index is %n, the flag is %n, the array is %s.", ptr->index, ptr->flag, ptr->data);  

	// could add to data via scanf("%
	
	// for(i = 0; i < 5; i++) {
	// critical seciton
	// Code for each child in palin.cpp
			//wait(1);
	// }
    /*
     * Wait until the other process
     * changes the first character of our memory
     * to '*', indicating that it has read what
     * we put there.
     */
	 
	// detach from memory segment
	int detach = shmdt(ptr);
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