#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
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
    int n=1, agent, ap, pp; 
    cout << "Enter number of acceptors: \n";
    cin >> ap;
    cout << "Enter number of proposers: \n";
    cin >> pp;
    
    pid_t pid[ap];

    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    int k=0;
    for(int i=0; i<pp; i++) pid[i]=0;
    while((pid[k]=fork())==0)
	{
		k++;
		if(k==pp) exit(0);
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
  
    for(int i=0; i<pp; i++)
    {
    	if(pid[i]>0) servaddr.sin_port = htons(3700+875+i*100), agent=i;
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
    
    fd_set fds;
    struct timeval timeout={5,0};
    char buffer[20];
    for(int i=0; i<20; i++) buffer[i]='\0';
    int sel;
    
    while(1)
    {
    	for(int i=0; i<20; i++) buffer[i]='\0';
		srand(time(NULL)+pid[agent]);
		int m=rand()%1000;
		string cha;
		stringstream ss;
		ss << n;
		ss >> cha;
		ss.clear();
		ss.str("");
		strcat(buffer,"prepare.");
		strcat(buffer,cha.c_str());
		strcat(buffer,".");	
		ss << m;
		ss >> cha;
		ss.clear();
		ss.str("");
		strcat(buffer,cha.c_str());
		printf("Proposer%d send %s\n", agent, buffer);
		for(int i=0; i<ap; i++)
		{
			servaddr.sin_port = htons(11400+875+i*100);
			sendto(udpfd, buffer, strlen(buffer), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
		}

		FD_ZERO(&fds); 
		FD_SET(udpfd,&fds);
		timeout.tv_sec=5;
		timeout.tv_usec=0;
		sel=select(udpfd+1,&fds,NULL,NULL,&timeout);
		if(sel<0)
		{
			printf("select failed\n");
			exit(0);
		}
		else if(sel==0)
		{
			n++;
			continue;
		}
		if(FD_ISSET(udpfd,&fds))
		{
			socklen_t sin_size;
			sin_size = sizeof their_addr;
			int error=0, np=0, rs=0;
			string res[ap], str;
			int count[ap];
            for(int i=0; i<ap; i++) count[i]=1;
			for(int k=0; k<ap; k++)
			{
			for(int i=0; i<20; i++) buffer[i]='\0';
			recvfrom(udpfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&their_addr, &sin_size);
			printf("Proposer%d recv %s\n", agent, buffer);
			if(buffer[0]=='e') error++;
			else if(buffer[0]=='n') np++; 
			else if(buffer[0]!='n')
			{
                istringstream sin(buffer);
				string v;
                int j=0;
                while (getline(sin, v,'.'))
                {
                	j++;
                	if(j==2)
                	{
                		res[k]=v;
                		for(int l=0; l<k; l++)
                    	{
                      		if(res[k]==res[l])
	                		{
			                	count[l]++;
	                 			if(count[l]>rs) 
								{
									rs=count[l];
									str=res[k];
								}
		                  	} 
	                	}
					}
				}
				
			}	
			}
			if(np>=(ap/2+1)) break;
			else if(rs>=(ap/2+1))
			{
				for(int i=0; i<20; i++) buffer[i]='\0';
				ss << n;
				ss >> cha;
		        ss.clear();
				ss.str("");
				strcat(buffer,"accept.");
				strcat(buffer,cha.c_str());
				strcat(buffer,".");	
				strcat(buffer,str.c_str());
				for(int i=0; i<ap; i++)
	        	{
	        		servaddr.sin_port = htons(11400+875+i*100);
	        	   	sendto(udpfd, buffer, strlen(buffer), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
	        	   	printf("Proposer%d send %s\n", agent, buffer);
        		}
        		break;
			}
			else 
			{
				n++;
	    		continue;
			}
		}
	}
	close(udpfd);
}
