#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "people.h"
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
struct people_head * phead;
void * snd(void * arg)
{

	int sfd = *(int *)arg;
	struct people_node * pnode = phead->pfirst;
	while(1)
	{
		fgets(ag.text,sizeof(ag.text),stdin);
		ag.text[strlen(ag.text)-1]=0;
		while(pnode!=NULL)
		{
			struct sockaddr_in sin;
			sin = pnode->data.sin;
			if(sendto(sfd,ag.text,sizeof(ag.text),0,(struct sockaddr*)&sin,sizeof(sin))<0)
			{
				ERR_MSG("sendto");
				return NULL;
			}
			pnode = pnode->pnext;
		}
		if(pnode==NULL)
		{
			break;
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
	char str[128];
	int res;
	while(1)
	{
		bzero(buf,sizeof(buf));
		res = recvfrom(sfd,buf,sizeof(buf),0,(struct sockaddr *)&cin,&len);
		if(res<0)
		{
			ERR_MSG("recvfrom");
			return NULL;
		}
		if(buf[0]=='C')
		{
			struct people_node * pnode = phead->pfirst;
			bzero(str,sizeof(str));
			int s = sprintf(str,"%s%s%s%d%s%s","[",inet_ntoa(cin.sin_addr),":",ntohs(cin.sin_port),"]",buf+1);
			struct people_node * mynode = search_people_node(phead,cin);
			struct sockaddr_in sin;
			printf("%s\n",str);

			while(pnode!=NULL)
			{
				if(pnode==mynode)
				{
					pnode = pnode->pnext;
					continue;
				}
				sin = pnode->data.sin;
				if(sendto(sfd,buf,s,0,(struct sockaddr*)&sin,sizeof(sin))<0)
				{
					ERR_MSG("sendto");
					return NULL;
				}
				pnode = pnode->pnext;
			}
				}
		else if (buf[0]=='L')
		{
			struct people_node * pnode = phead->pfirst;
			bzero(str,sizeof(str));
			int s = sprintf(str,"%s%s",buf,"已上线");
			printf("%s\n",str+1);
			struct sockaddr_in sin;
			while(pnode!=NULL)
			{
				sin = pnode->data.sin;	
				if(sendto(sfd,str,sizeof(str),0,(struct sockaddr*)&sin,sizeof(sin))<0)
				{
					ERR_MSG("sendto");
					return NULL;
				}
				pnode = pnode->pnext;
			}
			struct peopleData data;
			data.sin = cin;
			struct people_node * pfirst = create_people_node(&data);
			insert_node_at_tail(phead,pfirst);
	
		}
		else if(buf[0]=='Q')
		{
			struct people_node * mynode = search_people_node(phead,cin);
			remove_people_node(phead,mynode);
			struct people_node * pnode = phead->pfirst;
			bzero(str,sizeof(str));
			int s = sprintf(str,"%s%s",buf,"已下线");
			printf("%s\n",str+1);
			struct sockaddr_in sin;
			while(pnode!=NULL)
			{
				sin = pnode->data.sin;
				if(sendto(sfd,str,s,0,(struct sockaddr*)&sin,sizeof(sin))<0)
				{
					ERR_MSG("sendto");
					return NULL;
				}
				pnode = pnode->pnext;
			}

		}
	}
	pthread_exit(NULL);
}

int main(int argc, const char *argv[])
{


	phead = create_people_head();
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
	if(bind(sfd,(struct sockaddr *)&sin,sizeof(sin))<0)
	{
		ERR_MSG("bind");
		return -1;
	}
	pthread_t pth1,pth2;
	pthread_create(&pth1,NULL,snd,&sfd);
	pthread_create(&pth2,NULL,rcv,&sfd);
	pthread_join(pth1,NULL);
	pthread_join(pth2,NULL);
	destroy_people_head(phead);
	phead = NULL;
	return 0;
}
