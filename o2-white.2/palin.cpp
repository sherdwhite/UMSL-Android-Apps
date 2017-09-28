#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

int main(int argc, char * argv[]) 
{
	// Code for each child
	// for ( i = 0; i < 5; i++ )
	// {
		// execute code to enter critical section;
		// /* Critical section */
		// sleep for random amount of time (between 0 and 2 seconds);sses and shared memory
		//    // Start from leftmost and rightmost corners of str
		int l = 0;
		int r = strlen(str) - 1;
	 
		// Keep comparing characters while they are same
		while (r > l)
		{
			if (str[l++] != str[r--])
			{
				printf("%s Not Palindrome", str);
				return;
			}
			l++;
			r--;
		}
		printf("%s Palindrome", str);
		// sleep for random amount of time (between 0 and 2 seconds);
		// execute code to exit from critical section;
	// }
	
	
	// Write palindromes and non-palindromes to their files. 
	// file1 = "palin.out";
	// file1 = "nopalin.out";
	// std::ofstream file(file1.c_str());
	// std::ofstream file(file2.c_str());
	// for (int i = 0; i < list.size(); i++)
	// {
		// if (palin) {
			// write to palin.out
		// }
		// else
			// write to nopalin.out
	// }
	// file.close();
	
    return 0;
}