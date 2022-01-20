#ifndef PEOPLE_H
#define PEOPLE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
struct peopleData
{
	struct sockaddr_in sin;
};
struct people_node
{
	struct peopleData data;
	struct people_node * pnext;
};
struct people_head
{
	int len;
    struct people_node * pfirst;
};
struct people_head * create_people_head();
int destroy_people_head(struct people_head * phead);

struct people_node * create_people_node(struct peopleData * data);
int destroy_people_node(struct people_node * pnode);

int insert_node_at_tail(struct people_head * phead,struct people_node * pnode);

struct people_node * search_people_node(struct people_head * phead,struct sockaddr_in sin);

int remove_people_node(struct people_head * phead,struct people_node * pnode);

#endif

