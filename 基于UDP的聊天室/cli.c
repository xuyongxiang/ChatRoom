#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#define ERR_MSG(msg) do{\
	printf("%d",__LINE__);\
	perror(msg);\
}while(0)

#define IP "192.168.1.92"
#define port 8888

struct agreeMent
{
	char type;
	char name[20];
	char text[20];
};
struct agreeMent ag;

void * snd(void * arg)
{
	int sfd = *(int *)arg;
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = inet_addr(IP);
	while(1)
	{
		bzero(ag.text,sizeof(ag.text));
		fgets(ag.text,sizeof(ag.text),stdin);
		ag.text[strlen(ag.text)-1] = 0;
		char buf[128];
		int len;
		bzero(buf,sizeof(buf));
		if(strcasecmp(ag.text,"quit")==0)
		{
			ag.type = 'Q';
			len	 = sprintf(buf,"%c%s",ag.type,ag.name);
		}
		else
		{
			ag.type = 'C';
			len = sprintf(buf,"%c%s%s%s%s%s",ag.type,"[",ag.name,"]","说:",ag.text);
		}
		if(sendto(sfd,buf,len,0,(struct sockaddr*)&sin,sizeof(sin))<0)
		{
			ERR_MSG("sendto");
			return NULL;
		}
		if(strcasecmp(ag.text,"quit")==0)
		{
			exit(0);
		}
	}
	pthread_exit(NULL);

}

void * rcv(void * arg)
{
	int sfd = *(int *)arg;
	struct sockaddr_in cin;
	int len = sizeof(cin);
	char buf[128];
	while(1)
	{
		bzero(buf,sizeof(buf));
		if(recvfrom(sfd,buf,sizeof(buf),0,(struct sockaddr *)&cin,&len)<0)
		{
			ERR_MSG("recvfrom");
			return NULL;
		}
		if(buf[0]=='C')
		{
			printf("%s\n",buf+1);
		}
		else
		{

        	printf("%s\n",buf);
		}
	}
	pthread_exit(NULL);
}

int main(int argc, const char *argv[])
{
	int sfd = socket(AF_INET,SOCK_DGRAM,0);
	if(sfd<0)
	{
		ERR_MSG("socket");
		return -1;
	}
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = inet_addr(IP);
	printf("请输入姓名>>>");
	fgets(ag.name,sizeof(ag.name),stdin);
	ag.name[strlen(ag.name)-1] = 0;
	ag.type = 'L';
	char buf[128];
	int len = sprintf(buf,"%c%s",ag.type,ag.name);
	if(sendto(sfd,buf,len,0,(struct sockaddr*)&sin,sizeof(sin))<0)
	{
		ERR_MSG("sendto");
		return -1;
	}
	pthread_t pth1,pth2;
	pthread_create(&pth1,NULL,snd,&sfd);
	pthread_create(&pth2,NULL,rcv,&sfd);
	pthread_join(pth1,NULL);
	pthread_join(pth2,NULL);
	return 0;
}
