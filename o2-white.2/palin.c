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
        char *data[SIZE];
} shared_memory;

int main()
{
	int key = 9211191514;
	int shm_id = shmget(key, sizeof(shared_memory), PERM | IPC_CREAT);
	if (shm_id == -1) {
			perror("Failed to find shared memory segment");
							return 1;
	}
	printf("My palin segment id is %d\n", shm_id);

	// attach shared memory segment
	shared_memory* ptr = (shared_memory*)shmat(shm_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
	if (ptr == (void*)-1) {
			perror("Failed to attach existing shared memory segment");
			return 1;
	}
	printf("My palin ptr address is %x\n", ptr);

	int idr = ptr->id;
	int indexr = ptr->index;

	printf("Palin: The id is %d, the index is %d.\n", idr, indexr);

	int i = 0;
	char *word;
	// Testing array of strings for data.
	for(i = 0; i < sizeof(ptr->data); i++){
					word = (ptr->data[i]);
					printf("%s", word);
	}
	printf("Test");



	// // Start from leftmost and rightmost corners of str
	// int l = 0;
	// int r = strlen(word) - 1;
	// int palin = 1;
	// // Keep comparing characters while they are same
	// while (r > l)
	// {
			// if (word[l] != word[r]) {
					// printf("%s Not Palindrome\n", word);
					// palin = 0;
					// break;
					// }
			// l++;
			// r--;
	// }
	// if (palin == 1) {
			// printf("%s Palindrome\n", word);
	// }

	/* Critical section */
	// sleep for random amount of time (between 0 and 2 seconds);
	// Write palindromes and non-palindromes to their files.
	// FILE *file;

	// if (palin == 1) {
			// // write to palin.out
			// file = fopen("palin.out", "a");
			// fputs(word, file);
			// fputs("\n", file);
	// }
	// else {
			// // write to nopalin.out
			// file = fopen("nopalin.out", "a");
			// fputs(word, file);
			// fputs("\n", file);
	// }
	// fclose(file);
	
	// sleep for random amount of time (between 0 and 2 seconds);
	// execute code to exit from critical section;
	
	// detach from memory segment
	int detach = shmdt(ptr);
	if (detach == -1){
		perror("Failed to detach shared memory segment");
		return 1;
	}

    return 0;
}