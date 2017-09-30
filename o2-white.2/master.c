#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

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
		list.push_back(loc);
	}
	
	// Code for each child in palin.cpp
	
	

	
    return 0;
}