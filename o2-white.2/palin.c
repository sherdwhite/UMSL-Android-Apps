#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char * argv[])
{
        if (argc != 2) {
                fprintf(stderr, "Usage: %s string\n", argv[0]);
                return 1;
        }
        char* word;
        word = argv[1];
		
        // /* Critical section */
        // sleep for random amount of time (between 0 and 2 seconds);sses and shared memory
		
        // Start from leftmost and rightmost corners of str
        int l = 0;
        int r = strlen(word) - 1;
        int palin = 1;
        // Keep comparing characters while they are same
        while (r > l)
        {
                if (word[l] != word[r]) {
                        printf("%s Not Palindrome\n", word);
                        palin = 0;
                        break;
                        }
                l++;
                r--;
        }
        if (palin == 1) {
                printf("%s Palindrome\n", word);
        }

        // Write palindromes and non-palindromes to their files.
        FILE *file;

        if (palin == 1) {
                // write to palin.out
                file = fopen("palin.out", "a");
                fputs(word, file);
                fputs("\n", file);
        }
        else {
                // write to nopalin.out
                file = fopen("nopalin.out", "a");
                fputs(word, file);
                fputs("\n", file);
        }
        fclose(file);
		
		// sleep for random amount of time (between 0 and 2 seconds);
        // execute code to exit from critical section;

    return 0;
}