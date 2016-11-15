#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct ARG
{
    int connfd;
    struct sockaddr_in client;
};
void *handler(void *arg);
void process(int connfd, struct sockaddr_in client);
int main()
{
    struct sockaddr_in server,client;
    int connfd, listenfd;
    int num;
    socklen_t len;
    pthread_t tid;
    

    if((listenfd=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
	perror("socket failed");
	exit(1);
    }

    bzero(&server, sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port = htons(1234);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,&opt,sizeof(opt));

    if(bind(listenfd,(struct sockaddr*)&server,sizeof(server))==-1)
    {
	perror("bind failed");
	exit(1);
    }
    if(listen(listenfd, 5)==-1)
    {
	perror("listen failed");
	exit(1);
    }
    len=sizeof(client);
    while(1)
    {
	connfd=accept(listenfd, (struct sockaddr*)&client,&len);
	if(connfd==-1)
	{
	    perror("accept failed");
	    exit(1);
	}
	printf("Accept from %s:%d connfd:%d\n",inet_ntoa(client.sin_addr),htons(client.sin_port),connfd);

	struct ARG *arg=(struct ARG*)malloc(sizeof(struct ARG));
	arg->connfd = connfd;
	memcpy((void*)&arg->client,&client,sizeof(client));
	if(pthread_create(&tid,NULL,handler,(void*)arg))
	{
	    perror("thread failed");
	    exit(1);
	}
    }
    close(listenfd);
}

void *handler(void* arg)
{
    struct ARG *info;
    info=(struct ARG*)arg;
    printf("enter thread \n");
    process(info->connfd, info->client);
    free(arg);
    pthread_exit(NULL);
}
void process(int connfd, struct sockaddr_in client)
{
    char name[10];
    int num;
    char recvbuf[1024],sendbuf[1024];
    send(connfd, "Input you name:",16,0);
    int n=recv(connfd, &name, 10,0);
    if(n>0)
    {
	name[n-1]='\x00';
	printf("Access client:%s\n",name);
	send(connfd,name,n,0);
	while(1)
	{
	    if((num=recv(connfd,&recvbuf,1024,0))>0)
	    {
		int i;
		recvbuf[num-1]='\x00';
		if(strcmp(recvbuf,"quit")==0)
		{
		    send(connfd,"quit",5,0);
		    close(connfd);
		    break;
		}
		printf("Recv from %s: %s  ",name,recvbuf);
		for(i=0;i<num-1;i++)
		{
		    sendbuf[i]=recvbuf[num-2-i];
		}
		sendbuf[num-1]='\x00';
		if(send(connfd, &sendbuf,num,0)<=0)
		{
		    perror("send failed");
		    exit(1);
		}
		printf("Send %s: %s \n",name,sendbuf);
	    }
	}
    }
    else 
    {
	perror("recv failed");
	exit(1);
    }
}

	
    


    


