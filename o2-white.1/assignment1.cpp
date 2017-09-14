#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

int main (int argc, char *argv[]) {
    pid_t childpid = 0;
    int i, n, c;
	int nchars;
	char character;
	char *nvalue = NULL;
	char *cvalue = NULL;
		
    //if (argc != 3){ /* check for valid number of command-line arguments */
    //    fprintf(stderr, "Usage: %s processes\n", argv[0]);
	//	perror("Wrong number of arguements given!");
    //return 1;
    }
	
	while ((c = getopt (argc, argv, "n:c:")) != -1)
    switch (c)
		  {
			case 'n':
				nvalue = optarg;
				break;
			case 'c':
				cvalue = optarg;
				break;
			case '?':
				if (optopt == 'n'){
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
					perror("No arguement value given!");
				}
				if (optopt == 'c'){
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
					perror("No arguement value given!");
				}
				else if (isprint (optopt)){
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
					perror("Incorrect arguement given!");
				}
				else {
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
					perror("Unknown arguement given!");
					return 1;
				}
			default:
				abort ();
		  }

    n = atoi(nvalue);
	// k = atoi(argv[2]);  // 3.8 #5
	// m = atoi(argv[3]);  // 3.8 #5
	nchars = atoi(cvalue);
	char mybuf[nchars];

    std::cout <<  "Please enter " << nchars << " characters:";
    for (i = 1; i < n; i++)
        if (childpid = fork())
			break;

	for (i = 1; i <= nchars; i++){
		scanf(" %c", &character);
		mybuf[i-1] = character;
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