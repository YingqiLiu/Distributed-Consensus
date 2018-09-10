#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <ctime>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector> 
using namespace std; 

int main()
{
	int udpfd; 
    struct sockaddr_in servaddr; 
    char name[]="nunki.usc.edu";
    int n=0, ap;
	cout << "Enter number of acceptors: \n";
    cin >> ap; 

    struct sockaddr_storage their_addr;
    socklen_t sin_size;

    if( (udpfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1 )
    { 
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno); 
        exit(0); 
    } 
  
    struct hostent *he;
    struct in_addr **addr_list;
    struct sockaddr_in server;
    
    he = gethostbyname(name);
    addr_list = (struct in_addr **) he->h_addr_list;
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr= **addr_list; 
  
    servaddr.sin_port = htons(3300+875); 
  
    int on=1;  
    if((setsockopt(udpfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)  
    {  
        perror("setsockopt failed");  
        exit(EXIT_FAILURE);  
    }  
    if( bind(udpfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
    {
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno); 
        exit(0); 
    } 
    
    char buffer[20];
    string res[ap], str;
    int count[ap];
    for(int i=0; i<ap; i++) count[i]=1;
    for(int j=0; j<ap; j++)
    {
    	for(int i=0; i<20; i++) buffer[i]='\0';
    	recvfrom(udpfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&their_addr, &sin_size);
    	printf("Learner recv: %s\n", buffer);
    	res[j]=buffer;
    	str=res[j];
    	for(int k=0; k<j; k++)
    	{
    		if(res[k]==res[j])
			{
				count[k]++;
				if(count[k]>n) n=count[k];
			} 
		}
		if(n>=(ap/2+1)) break;
	}
    
    printf("Learner has result: %s\n", str.c_str());
    
	close(udpfd);
}
