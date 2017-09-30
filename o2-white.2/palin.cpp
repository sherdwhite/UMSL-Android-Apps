#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <cstring>

int main(int argc, char * argv[]) 
{
	if (argc <= 1)
	{
		std::cerr << "Not enough arguments, filename needed!" << argv[1] << std::endl;
		return (1);
	}

	std::string word = argv[1];
	// Code for each child
	// for ( i = 0; i < 5; i++ )
	// {
			// execute code to enter critical section;
			// /* Critical section */
			// sleep for random amount of time (between 0 and 2 seconds);sses and shared memory
			//    // Start from leftmost and rightmost corners of str
			int l = 0;
			int r = strlen(word.c_str()) - 1;

			// Keep comparing characters while they are same
			while (r > l)
			{
					if (word[l] != word[r])
					{
							printf("%s Not Palindrome\n", word.c_str());
							return 0;
					}
					l++;
					r--;
			}
			printf("%s Palindrome\n", word.c_str());
			// sleep for random amount of time (between 0 and 2 seconds);
			// execute code to exit from critical section;
	// }

	
	
	// Write palindromes and non-palindromes to their files. 
	file1 = "palin.out";
	file2 = "nopalin.out";
	
	for (int i = 0; i < list.size(); i++)
	{
		if (palin) {
			// write to palin.out
			std::ofstream file(file1.c_str());
			file1 << word << std::endl;
		}
		else {
			// write to nopalin.out
			std::ofstream file(file2.c_str());
			file2 << word << std::endl;
		}

	}
	file.close();
	
    return 0;
}