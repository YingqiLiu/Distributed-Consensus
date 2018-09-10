#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <ctime>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector> 
using namespace std; 

int main()
{
	pid_t pid[3];
	for(int i=0; i<3; i++) pid[i]=0;
	int k=0;
	while((pid[k]=fork())==0)
	{
		k++;
		if(k==3) exit(0);
	}
	printf("%d  %d  %d\n",pid[0], pid[1], pid[2]);
}
