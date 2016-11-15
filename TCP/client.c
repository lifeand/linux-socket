/* client */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 1234
#define MAXDATASIZE 100

int main(int argc, char *argv[])
{
    int sockfd, num; /* files descriptors */
    char buf[MAXDATASIZE];	/* buf will store received text */
    struct hostent *he; /* structure that will get information about remote host */
    struct sockaddr_in server; 
    char senddata[MAXDATASIZE];
    char recvdata[MAXDATASIZE]; 
    if(argc!=2)
    {
	printf("Usage: %s <IP Address>\n",argv[0]);
	exit(1);
    }

    if((he = gethostbyname(argv[1])) == NULL)
    {
	printf("gethostbyname() error\n");
	exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
	printf("socket() error\n");
	exit(1);
    }

    bzero(&server,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port= htons(PORT);
    server.sin_addr= *((struct in_addr *)he->h_addr);
    if(connect(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
	printf("connect() error\n");
	exit(1);
    }
    while(1) {
	fgets(senddata,MAXDATASIZE,stdin);
	if(send(sockfd, senddata,strlen(senddata),0)==-1)
	{
	    perror("send failed\n");
	    exit(1);
	}
    if((num = recv(sockfd, buf, MAXDATASIZE,0))== -1)
    {
	printf("recv() error\n");
	exit(1);
    }
    buf[num-1] = '\x00';
    if(strcmp("quit",buf)==0)
    {
	close(sockfd);
	exit(0);
    }
    printf("server messagae: %s\n", buf);

    }
    close(sockfd);
}
