#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in server,client;
    socklen_t len;
    struct hostent *ser,*cli;
    int num;
    char recvbuf[1024],sendbuf[1024];
    fd_set read_fds,write_fds;

    if(argc<3)
    {
	printf("Usage: %s <server ip> <server port> <client ip> <client port>\n",argv[0]);
	exit(0);
    }
    if((ser=gethostbyname(argv[1]))==NULL)
    {
	perror("gethostbyname error");
	exit(1);
    }
    if((cli=gethostbyname(argv[3]))==NULL)
    {
	perror("gethostbyname error");
	exit(1);
    }

    if((sockfd = socket(AF_INET, SOCK_DGRAM,0))==-1)
    {
	perror("socket failed\n");
	exit(1);
    }
    bzero(&server,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    server.sin_addr=*((struct in_addr*)ser->h_addr);   

    bzero(&client, sizeof(client));
    client.sin_family=AF_INET;
    client.sin_port =htons(atoi(argv[4]));
    client.sin_addr = *((struct in_addr*)cli->h_addr);

    len=sizeof(client);

    if(bind(sockfd,(struct sockaddr*)&server,sizeof(server))==-1)
    {
	perror("bind failed");
	exit(1);
    }
    

    int ret;
    while(1) 
    {
	FD_ZERO(&read_fds);
	FD_SET(0, &read_fds);
	FD_SET(sockfd,&read_fds);

	ret = select(sockfd+1,&read_fds,NULL,NULL,NULL);

	if(ret<0)
	{
	    perror("select failed");
	    exit(1);
	}
	if(FD_ISSET(sockfd, &read_fds))
	{
	    printf("enter recv\n");
	    num = recvfrom(sockfd, &recvbuf,1024,0,(struct sockaddr*)&client,&len);
	    recvbuf[num-1]='\x00';
	    printf("Recv from %s:%d %s\n",inet_ntoa(client.sin_addr),htons(client.sin_port),recvbuf);
	}
	if(FD_ISSET(0,&read_fds))
	{
	    fgets(sendbuf,1024,stdin);
	    printf("Send > %s",sendbuf);
	    if(sendto(sockfd,&sendbuf,strlen(sendbuf),0,(struct sockaddr*)&client,sizeof(client))<=0)
	    {
		printf("Send failed\n");
		exit(1);
	    }
	}
    }
    close(sockfd);
}




	
	
	





