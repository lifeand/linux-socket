#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 1234

void handler(int connfd, struct sockaddr_in client);

int main()
{
    int listenfd, connfd;
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t len;

    if((listenfd=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
	perror("socket failed\n");
	exit(1);
    }
    
    bzero(&server, sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr=htonl(INADDR_ANY);

    int opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    len=sizeof(client);

    if((bind(listenfd, (struct sockaddr *)&server, sizeof(server)))==-1)
    {
	perror("bind failed");
	exit(1);
    }
    if((listen(listenfd, 5))==-1)
    {
	perror("listenfd failed\n");
	exit(2);
    }
    while (1)
    {
	connfd = accept(listenfd, (struct sockaddr*)&client, &len);
	if(connfd==-1)
	{
	    perror("accept failed");
	    exit(1);
	}
	printf("Accept from %s:%d\n",inet_ntoa(client.sin_addr),htons(client.sin_port));
	pid_t pid;
	if((pid=fork())==0)
	{
	    handler(connfd, client);
	    close(listenfd);
	    exit(0);
	}
	else if(pid>0)
	{
	    close(connfd);
	    continue;
}
	else
	{
	    perror("fork failed");
	    exit(1);
	}
    }
    close(listenfd);
}

void handler(int connfd, struct sockaddr_in client)
{
    int num,n;
    char name[10];
    char sendbuf[1024],recvbuf[1024];
    bzero(&sendbuf,sizeof(sendbuf));
    bzero(&recvbuf,sizeof(recvbuf));

    send(connfd, "Please input you name:",21,0);
    n=recv(connfd,&name,10,0);
    name[n-1]='\x00';
    send(connfd,name,n,0);
    while(1)
    {
	if((num = recv(connfd, &recvbuf,1024,0))>0)
	{
	    recvbuf[num-1]='\x00';
	    if(strcmp(recvbuf,"quit\0")==0)
	    {
		send(connfd,"quit",5,0);
		close(connfd);
		break;
	    }
	    int i=0;
	    printf("from %s Recv: %s num:%d \n",name, recvbuf,num);
	    for(i=0;i<num-1;i++)
	    {
		sendbuf[i] = recvbuf[num-2-i];
	    }
	    sendbuf[num-1]='\x00';
	    printf("send %s \n",sendbuf);
	    if(send(connfd,&sendbuf, num,0)<=0)
	    {
		perror("send failed\n");
		exit(1);
	    }
	}
    }
}
