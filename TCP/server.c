/* server.c */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define PORT 1234
#define BACKLOG 1
#define MAXDATASIZE 1024
#define FINISH "quit"

   
int main()
{
    struct sockaddr_in server,client;
    int listenfd, connfd;
    socklen_t len;
    char* recvdata;
    char* senddata;
    int num;
    int opt;
    int i;

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0))==-1)
    {
	perror("socket() failed\n");
	exit(EXIT_FAILURE);
    }
    
    bzero(&server,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr=inet_addr("127.0.0.1");

    opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

    
    if((bind(listenfd, (struct sockaddr*)&server,sizeof(server)))==-1)
    {
	perror("bind() failed\n");
	exit(EXIT_FAILURE);
    }

    len=sizeof(client);
    while((listen(listenfd, BACKLOG))!=-1)
    {
	connfd=accept(listenfd,(struct sockaddr*)&client,&len);
	if(connfd == -1)
	{
	    perror("accept() failed\n");
	    exit(EXIT_FAILURE);
	}
	printf("[*] Accept client from [ %s ]:%d \n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));

	recvdata = (char*)malloc(sizeof(char*)*MAXDATASIZE);

	bzero(recvdata,sizeof(recvdata));

	senddata=(char*)malloc(sizeof(char)*MAXDATASIZE);

	while(1)
	{
	    if((num=recv(connfd, recvdata, MAXDATASIZE,0))<=0)
	    {
		perror("recv failed\n");
		exit(EXIT_FAILURE);
	    }
	    recvdata[num-1]='\0';
	    if(strcmp(recvdata,"quit")==0)
	    {
		printf("Bye~\n");
		send(connfd,"quit",5,0);
		close(connfd);
		break;
	    }
	    
	    printf("[*]Recv: %s\n",recvdata);

	    for(i=0;i<num-1;i++)
	    {
		senddata[i]=recvdata[num-2-i];
	    }
	    senddata[num-1]='\x00';

	    if((send(connfd, senddata,num,0))<=0)
	    {
		perror("send failed\n");
		exit(EXIT_FAILURE);
	    }


	}
    }
		
}
