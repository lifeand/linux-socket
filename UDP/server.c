#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 1234

int main()
{
    struct sockaddr_in server,client;
    int listenfd;
    int num;
    socklen_t len;
    char buf[1024];

    if((listenfd=socket(AF_INET, SOCK_DGRAM,0))==-1)
    {
	perror("socket failed");
	exit(1);
    }
    bzero(&server,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr=htonl(INADDR_ANY);
    
    int opt=SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,&opt,sizeof(opt));

    if(bind(listenfd, (struct sockaddr*)&server,sizeof(server))==-1)
    {
	perror("bind failed");
	exit(1);
    }

    len=sizeof(client);
    while(1)
    {
	num=recvfrom(listenfd, buf, 1024,0,(struct sockaddr*)&client,&len);
	if(num<0)
	{
	    perror("recvfrom failed");
	    exit(1);
	}
	buf[num-1]='\x00';
	if(strcmp("quit",buf)==0)
	{
	    sendto(listenfd,"quit",5,0,(struct sockaddr*)&client,sizeof(client));
	}
	printf("Recv from %s:%d %s %d\n", inet_ntoa(client.sin_addr),htons(client.sin_port),buf,num);
	int i=0;
	char sendbuf[1024];
	for(i=0;i<num-1;i++)
	{
	    sendbuf[i]=buf[num-2-i];
	}
	sendbuf[num-1]='\x00';
	sendto(listenfd,&sendbuf,num,0,(struct sockaddr*)&client,sizeof(client));
    }
    close(listenfd);
}

