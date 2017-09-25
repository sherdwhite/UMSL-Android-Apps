#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

int main(int argc, char * argv[]) 
{
	if (argc <= 1)
	{
		std::cerr << "A file was not found or is not accessable." << std::endl;
		return (1);
	}

	vector<int> list1;
	int loc;

	ifstream fin(argv[1]);

	while (fin >> loc)
	{
		list1.push_back(loc);
	}
	
	// Code for each child in palin.cpp
	
	
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