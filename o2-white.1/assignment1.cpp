#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main (int argc, char *argv[]) {
    pid_t childpid = 0;
    int i, n;
	int nchars;'
	char character;
		
    if (argc != 3){ /* check for valid number of command-line arguments */
        fprintf(stderr, "Usage: %s processes\n", argv[0]);
    return 1;
    }

    n = atoi(argv[1]);
	nchars = atoi(argv[2]);
	char mybuf[nchars];
	
    for (i = 1; i < n; i++)
        if (childpid = fork())
        break;
	
	for (i = 1; i <= nchars; i++){
        std::cout << "Please enter a single character: ";
		std::cin >> character;
		mybuf[nchars-1] = character;
	}
	mybuf[nchars] = '\0';
	
    fprintf(stderr, "process ID:%ld mybuf:%s \n", (long)getpid(), mybuf);
    return 0;
}