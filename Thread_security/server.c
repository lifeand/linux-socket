#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>

struct ARG
{
    int connfd;
    struct sockaddr_in client;
};
pthread_key_t key;
pthread_once_t once=PTHREAD_ONCE_INIT;
static void destructor(void *arg)
{
    free(arg);
}
static void createkey_once(void)
{
    pthread_key_create(&key,destructor);
}
struct ST_DATA {
    int index;
};


void process(int connfd, struct sockaddr_in client);
void *handler(void *arg);
void savedata(char *buf, int num,char *save);

int  main()
{
    struct sockaddr_in client,server;
    int connfd, listenfd;
    socklen_t len;
    int num;
    int n;
    
    if((listenfd=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
	perror("socket failed");
	exit(1);
    }
    bzero(&server,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port = htons(1234);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt=SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,&opt,sizeof(opt));

    if(bind(listenfd, (struct sockaddr*)&server, sizeof(server))==-1)
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
	if((connfd = accept(listenfd, (struct sockaddr*)&client,&len))==-1)
	{
	    perror("accept failed");
	    exit(1);
	}
	printf("Accept from %s:%d \n", inet_ntoa(client.sin_addr),htons(client.sin_port));

	struct ARG *arg;
	arg = (struct ARG*)malloc(sizeof(struct ARG));
	arg->connfd = connfd;
	memcpy((void*)&arg->client,&client,sizeof(client));
	pthread_t tid;
	if(pthread_create(&tid, NULL,handler,(void*)arg))
	{
	    perror("pthread_create failed");
	    exit(1);
	}
    }
    close(listenfd);
}

void *handler(void *arg)
{
    printf("enter thread\n");
    struct ARG *info;
    info=(struct ARG*)arg;
    process(info->connfd, info->client);
    free(arg);
    pthread_exit(NULL);
}

void process(int connfd, struct sockaddr_in client)
{
    printf("enter process\n");
    int num;
    int n;
    char savebuf[1024];
    char recvbuf[1024],sendbuf[1024];
    char name[10];

    send(connfd, "Input you name:",16,0);
    n=recv(connfd,&name,10,0);
    if(n>0)
    {
	name[n-1]='\x00';
	send(connfd,name,n,0);
    }
    else {
	perror("recv failed");
	return;
    }
    while(1)
    {
	if((num=recv(connfd, &recvbuf,1024,0))>0)
	{
	    recvbuf[num-1]='\x00';
	    if(strcmp("quit",recvbuf)==0)
	    {
		send(connfd, "quit",5,0);
		printf("bye %s, all Rave data: %s\n",name,savebuf);
		close(connfd);
		break;
	    }
	    printf("Recv from %s: %s\n",name,recvbuf);
	    savedata(recvbuf, num, savebuf);
	    int i;
	    for(i=0;i<num-1;i++)
	    {
		sendbuf[i]=recvbuf[num-2-i];
	    }
	    sendbuf[num-1]='\x00';
	    send(connfd, &sendbuf,num,0);
	}
    }
}

void savedata(char *buf, int num, char *save)
{
    struct ST_DATA *data;
    pthread_once(&once, createkey_once);
    if((data = (struct ST_DATA*)pthread_getspecific(key))==NULL)
    {
	data=(struct ST_DATA*)malloc(sizeof(struct ST_DATA));
	pthread_setspecific(key,data);
	data->index=0;
	printf("[*]Init data\n");
    }
    int i=0;
    for(;i<num-1;i++)
    {
	save[data->index++] = buf[i];
    }
    save[data->index]=' ';
    save[data->index+1]='\x00';
}


