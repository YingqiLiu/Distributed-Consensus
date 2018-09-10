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
	int udpfd, connfd; 
    struct sockaddr_in servaddr; 
    char name[]="nunki.usc.edu";
    int n=0, v, agent, ap, pp; 
    cout << "Enter number of acceptors: \n";
    cin >> ap;
    cout << "Enter number of proposers: \n";
    cin >> pp;
    
    pid_t pid[ap];

    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    int k=0;
    for(int i=0; i<ap; i++) pid[i]=0;
    while((pid[k]=fork())==0)
	{
		k++;
		if(k==ap) exit(0);
	}
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
  
    for(int i=0; i<ap; i++)
    {
    	if(pid[i]>0) servaddr.sin_port = htons(11400+875+i*100), agent=i;
	} 
  
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
    for(int i=0; i<20; i++) buffer[i]='\0';
    int sel, prop=0;
    
    while(1)
    {
    	for(int i=0; i<20; i++) buffer[i]='\0';
		socklen_t sin_size;
		sin_size = sizeof their_addr;
		recvfrom(udpfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&their_addr, &sin_size);
		printf("Acceptor%d recv %s\n", agent, buffer);
		istringstream sin(buffer);
		string num, cha;
		stringstream ss;
		int res1, res2, pora;
        int j=0;
        while (getline(sin, num,'.'))
        {
            j++;
            if(j==1)
            {
            	if(num[0]=='p') pora=0;
            	else pora=1;
			}
            else if(j==2)
            {
				ss << num;
		        ss >> res1;
				ss.clear();
				ss.str("");
			}
            else if(j==3)
            {
				ss << num;
		        ss >> res2;
				ss.clear();
				ss.str("");
			}
		}
		for(int i=0; i<20; i++) buffer[i]='\0';
		if(pora==0)
		{
		if(res1<n)
		{
			strcat(buffer,"error");
			sendto(udpfd, buffer, strlen(buffer), 0, (struct sockaddr*)&their_addr, sin_size);
		}
		else if(res1>n && n==0)
		{
			strcat(buffer,"no previous");
			sendto(udpfd, buffer, strlen(buffer), 0, (struct sockaddr*)&their_addr, sin_size);
			n=res1;
			v=res2;
			prop++;
		}
		else if(res1>=n)
		{
			ss << n;
			ss >> cha;
		    ss.clear();
			ss.str("");
			strcat(buffer,cha.c_str());
			strcat(buffer,".");	
			ss << v;
			ss >> cha;
			ss.clear();
			ss.str("");
			strcat(buffer,cha.c_str());
			sendto(udpfd, buffer, strlen(buffer), 0, (struct sockaddr*)&their_addr, sin_size);
			n=res1;
		}
		}
		else
		{
			prop++;
		}
        if(prop==pp) break;
	}
	servaddr.sin_port = htons(3300+875);
	stringstream ss;
	string cha;
	ss << v;
	ss >> cha;
	ss.clear();
	ss.str("");
	sendto(udpfd, cha.c_str(), strlen(cha.c_str()), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
	printf("Acceptor%d send %s to Learner\n", agent, cha.c_str());
	close(udpfd);
}
