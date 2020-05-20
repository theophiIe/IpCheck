#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "read.h"

#define SIZE_IP 15

typedef struct 
{
	char **tab_ip;
	int  nbr_ip;
	
} DATA;

typedef struct 
{
	char *ip;
	char *rqst;
	char *fullRqst;
	
} ARG_T_P;

typedef struct 
{
	pthread_mutex_t *mut;
	char *path;
	char *rqst;
	char *ip;
	
} ARG_T_R;

int initData(DATA *data, const char *path)
{
	char c;
	data -> nbr_ip = 0;

	OI_FILE file = OI_open (path, O_RDONLY);
	if (file.descripteur < 0) 
	{
		fprintf(stderr, "\nERR: opening config file\n");
		return 1;
	}

	while(OI_readChar (file, &c) > 0)
	{
		if(c == '\n')
			data -> nbr_ip += 1;
	}

	data -> tab_ip = malloc(data -> nbr_ip * sizeof(char*));
	for(int cntIp = 0; cntIp < data -> nbr_ip; cntIp++)
		data -> tab_ip[cntIp] = malloc((SIZE_IP + 2) * sizeof(char)); //Check size of malloc ERR : fail read in file config (ip + \n + \0) -> (15 + 1 + 1)
		
	OI_close(file);

	return 0;
}

int readFile(DATA *data, const char *path)
{
	int valRD;

	OI_FILE file = OI_open (path, O_RDONLY);
	if (file.descripteur < 0) 
	{
		fprintf(stderr, "\nERR: opening config file\n");
		return 1;
	}

	for(int cntIp = 0; cntIp < data->nbr_ip; cntIp++)
	{
		valRD = OI_readStr (file, data->tab_ip[cntIp], SIZE_IP + 1);
		if (valRD < 0) 
		{
			fprintf(stderr, "\nERR: read in data file\n");
			OI_close(file);
			return 1;
		}
	}

	OI_close(file);

	return 0;
}

char *initIP(char *ip)
{
	char *newIp = NULL;

	size_t size = strlen(ip) - 1;
	newIp = malloc(size * sizeof(char));
	if( !newIp ) 
	{
		printf("initIP: wrong alloc 'newIp'\n");
		return NULL;
	}

	strncpy( newIp, ip, size);

	return newIp;
}

char * initNameFile(char* ip)
{
	char *nameFile = NULL;
	char *extension = ".txt";

	size_t size = strlen(ip) + strlen(extension) + 1;
	nameFile = malloc(size * sizeof(char));
	if( !nameFile ) 
	{
		printf("initNameFile: wrong alloc 'nameFile'\n");
		return NULL;
	}

	strcat(strcpy( nameFile, ip), extension);

	return nameFile;
}

char * initrqst(char *ip)
{
	char *rqst  = NULL;
	char *param = "ping -c 4 ";
	char *rdctF = " > ";
	char *nameF = initNameFile(ip);

	size_t size = strlen(ip) + strlen(rdctF) + strlen(nameF) + strlen(param) + 1;
	rqst = malloc(size * sizeof(char));
	if( !rqst ) 
	{
		printf("initrqst: wrong alloc 'rqst'\n");
		return NULL;
	}

	strcat( strcat( strcat( strcpy( rqst, param), ip), rdctF), nameF);

	free(nameF);

	return rqst;
}

void * pingsParallel (void * arg)
{
	ARG_T_P *at = (ARG_T_P *) arg;
	
	int sysVal = -1;

	at->ip 		 = initIP( at->rqst );
	at->fullRqst = initrqst( at->ip );

	printf("%s\n", at->fullRqst);

	sysVal = system( at->fullRqst );
	if(sysVal < 0)
		printf("ERR : ping request fail\n");

	printf("%ld request : %s\n", pthread_self(), at->fullRqst);

	free(at->ip);
	free(at->fullRqst);

	return NULL;
}

int print (const char * path)
{
	char c;

	OI_FILE file = OI_open (path, O_RDONLY);
	if (file.descripteur < 0) 
	{
		fprintf(stderr, "\nERR: opening config ip file\n");
		return 1;
	}

	if (file.descripteur == -1)
		return -1;

	printf("\n");

	while (OI_readChar (file, &c) > 0)
		printf ("%c", c);

	printf("\n");
	
	OI_close (file);
	OI_delete (path);
	
	return 0;
}

void * printFile (void * arg)
{
	ARG_T_R *at = (ARG_T_R *) arg;
	
	at -> ip   = initIP( at->rqst );
	at -> path = initNameFile(at -> ip);

	pthread_mutex_lock(at -> mut);

	print(at -> path);

	free(at -> ip);
	free(at -> path);

	pthread_mutex_unlock(at -> mut);
	

	return NULL;
}

void ThreadCheckIP(DATA data)
{
	pthread_t * tid;
	
	ARG_T_P *at;
		
	at  = malloc(data.nbr_ip * sizeof(ARG_T_P));
	tid = malloc(data.nbr_ip * sizeof(pthread_t) );
	
	//Thread for ping
	for(int cnt = 0; cnt < data.nbr_ip; cnt++)
	{
		at[cnt].ip   	 = NULL;
		at[cnt].fullRqst = NULL;
		at[cnt].rqst 	 = data.tab_ip[cnt];
		pthread_create(tid + cnt, NULL, pingsParallel, at + cnt);
	}

	for(int cnt = 0; cnt < data.nbr_ip; cnt++)
		pthread_join(tid[cnt], NULL);

	free(at);
	free(tid);
}

void ThreadPrintFile(DATA data)
{
	pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
	pthread_t * tid;
	
	ARG_T_R *at;
		
	at  = malloc(data.nbr_ip * sizeof(ARG_T_R));
	tid = malloc(data.nbr_ip * sizeof(pthread_t) );

	for(int cnt = 0; cnt < data.nbr_ip; cnt++)
	{
		at[cnt].ip   = NULL;
		at[cnt].path = NULL;
		at[cnt].mut  = &mut;
		at[cnt].rqst = data.tab_ip[cnt];
		pthread_create(tid + cnt, NULL, printFile, at + cnt);
	}

	for(int cnt = 0; cnt < data.nbr_ip; cnt++)
		pthread_join(tid[cnt], NULL);

	free(at);
	free(tid);
}

int main(int argc, char ** argv)
{
	DATA data;

	initData(&data, "data.cfg");
	printf("Ip number : %d\n\n", data.nbr_ip);

	readFile(&data, "data.cfg");
	
	for(int cnt = 0; cnt < data.nbr_ip; cnt++)
		printf("ip : %s", data.tab_ip[cnt]);

	printf("\nPing rqst being processed... \n");

	ThreadCheckIP(data);
	ThreadPrintFile(data);
	
	for(int cnt = 0; cnt < data.nbr_ip; cnt++)
		free(data.tab_ip[cnt]);
	free(data.tab_ip);

	return 0;
}