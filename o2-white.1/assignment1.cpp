#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

int main (int argc, char *argv[]) {
    pid_t childpid = 0;
    int i, n;
	int nchars;
	char character;
		
    if (argc != 3){ /* check for valid number of command-line arguments */
        fprintf(stderr, "Usage: %s processes\n", argv[0]);
    return 1;
    }

    n = atoi(argv[1]);
	// k = atoi(argv[2]);  // 3.8 #5
	// m = atoi(argv[3]);  // 3.8 #5
	nchars = atoi(argv[2]);
	char mybuf[nchars];
	
    for (i = 1; i < n; i++)
        if (childpid = fork())
        break;
	
	for (i = 1; i <= nchars; i++){
        std::cout << "Please enter a single character: ";
		std::cin >> character;
		mybuf[nchars-1] = char(character);
	}
	mybuf[nchars] = '\0';
	
	// sleep(10);  // 3.8 #4
	// childpid = wait(NULL);  // 3.8 #6
    fprintf(stderr, "process ID:%ld; mybuf:%s \n", (long)getpid(), mybuf);
	
	// 3.8 #5
	/* for (i = 1; i < k; i++) {
		fprintf(stderr, "i:%d process ID:%ld parent ID:%ld child ID:%ld\n", i, (long)getpid(), (long)getppid(), (long)childpid);
		sleep(m);
	} */
	
	// 3.8 #7
	/* fprintf(stderr, "i:%d ", i);
 	fprintf(stderr, "process ID:%ld ", (long)getpid());
 	fprintf(stderr, "parent ID:%ld ", (long)getppid());
 	fprintf(stderr, "child ID:%ld\n", (long)childpid); */
	
    return 0;
}