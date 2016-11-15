#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

void ip2host(char *arg);
void host2ip(char *arg);

int main(int argc, char *argv[])
{
    int opt;
    opt = getopt(argc, argv, "h:i:");
    while( opt!= -1) 
    {
	switch( opt ) 
	{
	    case 'h':
		host2ip((char*)optarg);
		break;
	    case 'i':
		ip2host((char*)optarg);
		break;
	    default:
		printf("Usage: %s <-h host> or <-i ip>\n",argv[0]);
		break;
	}
    }
    printf("Usage: %s <-h host> or <-i ip>\n",argv[0]);
    return 0;
}
    
void host2ip(char *arg)
{
    struct hostent *info;
    if((info=gethostbyname(arg))==NULL)
    {
	perror("gethostbyname failed");
	exit(1);
    }
    printf("host name: %s\n",info->h_name);
    char **p;

    for(p=info->h_addr_list;*p!=0;p++)
    {
	struct in_addr in;
	(void)memcpy(&in.s_addr,*p,sizeof(in.s_addr));
	(void)printf("ip: %s\t%s\n",inet_ntoa(in),info->h_name);
    }
    char **q;
    for(q=info->h_aliases;*q!=0;q++)
    {
	(void)printf("%s\n",*q);
    }
    exit(0);
}

void ip2host(char *arg)
{
    struct hostent *info;
    if((info=gethostbyaddr((struct in_addr*)arg,sizeof(struct in_addr),AF_INET))!=NULL)
    {
	char **p;
	printf("host name: %s\n",info->h_name);
	for(p=info->h_addr_list;*p!=0;p++)
	{
	    struct in_addr in;
	    (void)memcpy(&in.s_addr, *p,sizeof(struct in_addr));
	    (void)printf("ip: %s\t%s\n",inet_ntoa(in),info->h_name);
	}
	char **q;
	for(q=info->h_aliases;*q!=0;q++)
	{
	    (void)printf("alias: %s\n",*q);
	}
	exit(0);
    }
    else 
    {
	perror("gethostbyaddr failed");
	exit(1);
    }
}



    




    
