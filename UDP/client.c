#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define PORT 1234

int main(int argc, char* argv[])
{
    struct sockaddr_in server,client;
    int sockfd;
    int num;
    socklen_t len;
    char buf[1024],sendbuf[1024];
    struct hostent *he;
    
    if(argc <2)
    {
	printf("usage: %s <ip> \n",argv[0]);
	exit(0);
    }
    if((he=gethostbyname(argv[1]))==NULL)
    {
	perror("gethostbyname failed");
	exit(1);
    }

    if((sockfd=socket(AF_INET,SOCK_DGRAM,0))==-1)
    {
	perror("socket failed");
	exit(1);
    }

    bzero(&server,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr=*((struct in_addr*)he->h_addr);

    len=sizeof(client);
    while(1)
    {
	fgets(sendbuf,1024,stdin);
	if(sendto(sockfd, &sendbuf,strlen(sendbuf),0,(struct sockaddr*)&server,sizeof(server))<=0)
	{
	    perror("send failed");
	    exit(1);
	}
	num=recvfrom(sockfd,&buf,1024,0,(struct sockaddr*)&client,&len);
	if(num < 0)
	{
	    perror("recvfrom failed");
	    exit(1);
	}
	buf[num]='\x00';
	if(strcmp("quit",buf)==0)
	{
	    close(sockfd);
	    exit(0);
	}
	printf("Recvfrom server: %s \n",buf);
    }
}






