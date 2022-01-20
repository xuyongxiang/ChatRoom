#include "people.h"
struct people_head * create_people_head()
{
	struct people_head * phead = (struct people_head *)malloc(sizeof(struct people_head));
	if(phead==NULL)
	{
		printf("people_head create failed");
		return NULL;
	}
	memset(phead,0,sizeof(struct people_head));
	phead->len = 0;
	return phead;
}
int destroy_people_head(struct people_head * phead)
{
	struct people_node * pnode = phead->pfirst;
	struct people_node * ptemp = NULL;
	while(pnode!=NULL)
	{
		ptemp = pnode->pnext;
		free(pnode);
		pnode = NULL;
		pnode = ptemp;
	}
	free(phead);
	phead = NULL;
	return 0;

}

struct people_node * create_people_node(struct peopleData * data)
{
	struct people_node * pnode =(struct people_node *)malloc(sizeof(struct people_node *));
	if(NULL==pnode)
	{
		printf("people_node create failed");
		return NULL;
	}
	memset(pnode,0,sizeof(struct people_node));
	memcpy(&pnode->data,data,sizeof(struct peopleData));
	return pnode;
}
int destroy_people_node(struct people_node * pnode)
{
	if(pnode!=NULL)
	{
		free(pnode);
		pnode = NULL;
	}
	return 0;
}

int insert_node_at_tail(struct people_head * phead,struct people_node * pnode)
{
	struct people_node * pfirst = phead->pfirst;
	while(pfirst!=NULL)
	{
		if(pfirst->pnext==NULL)
		{
			break;
		}
		pfirst = pfirst->pnext;
	}
	if(pfirst!=NULL)
	{
		pfirst->pnext = pnode;
	}
	else
	{
		phead->pfirst = pnode;
	}
	pnode->pnext = NULL;
	phead->len++;
	return 0;

}

struct people_node * search_people_node(struct people_head * phead,struct sockaddr_in sin)
{
	struct people_node * pnode = phead->pfirst;
	while(pnode!=NULL)
	{
		printf("%d\n",pnode->data.sin.sin_port);
		if(ntohs(pnode->data.sin.sin_port)==ntohs(sin.sin_port))
		{
			break;
		}
		pnode = pnode->pnext;
	}
	return pnode;
}
struct people_node * get_prev_node(struct people_head * phead,struct people_node * pnode)
{
	struct people_node * pprev = phead->pfirst;
	if(pprev==NULL)
	{
		return NULL;
	}
	else
	{
		if(pnode==pprev)
		{
			return NULL;
		}
		else
		{
			while(pprev!=NULL&&pprev->pnext!=pnode)
			{
				pprev = pprev->pnext;
			}
			return pprev;
		}
	}
	return pprev;
}
int remove_people_node(struct people_head * phead,struct people_node * pnode)
{
	struct people_node * pprev = get_prev_node(phead,pnode);
	if(pprev!=NULL)
	{
		pprev->pnext = pnode->pnext;
		pnode->pnext = NULL;
		destroy_people_node(pnode);
		pnode = NULL;
		phead->len--;
		return 1;
	}
	else if (pnode==phead->pfirst)
	{
		phead->pfirst = pnode->pnext;
		pnode->pnext = NULL;
		destroy_people_node(pnode);
		pnode = NULL;
		phead->len--;
		return 1;
	}
	return 0;
}


