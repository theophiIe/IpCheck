#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "se_fichier.h"

#define SIZE_IP 15

typedef struct 
{
	char **tab_ip;
	int  nbr_ip;
	
} DATA;

typedef struct 
{
	pthread_mutex_t *mut;
	char *request;
	
} ARG_T;

int initData(DATA *data, const char *path)
{
	char c;
	data -> nbr_ip = 0;

	SE_FICHIER file = SE_ouverture (path, O_RDONLY);
	if (file.descripteur < 0) 
	{
		fprintf(stderr, "\nERR: opening config file\n");
		return 1;
	}

	while(SE_lectureCaractere (file, &c) > 0)
	{
		if(c == '\n')
			data -> nbr_ip += 1;
	}

	data -> tab_ip = malloc(data -> nbr_ip * sizeof(char*));
	for(int cntIp = 0; cntIp < data -> nbr_ip; cntIp++)
		data -> tab_ip[cntIp] = malloc(SIZE_IP * sizeof(char));
		
	SE_fermeture(file);

	return 0;
}

int readFile(DATA *data, const char *path)
{
	SE_FICHIER file = SE_ouverture (path, O_RDONLY);
	if (file.descripteur < 0) 
	{
		fprintf(stderr, "\nERR: opening config file\n");
		return 1;
	}

	for(int cntIp = 0; cntIp < data->nbr_ip; cntIp++)
	{
		SE_lectureChaine (file, data->tab_ip[cntIp], SIZE_IP+1);
		printf("ip lecture: %s", data->tab_ip[cntIp]);
	}

	SE_fermeture(file);

	return 0;
}

char *initIP(char *ip)
{
	size_t size = strlen(ip) - 1;
	char *NewIp = (char *) malloc(size);

	strncpy( NewIp, ip, strlen(ip) - 1);

	return NewIp;
}

char * initNameFile(char* ip)
{
	char *extension = ".txt";

	size_t size = strlen(ip) + strlen(extension);
	char *nameFile = (char *) malloc(size);

	strcat(strcpy( nameFile, ip), extension);

	return nameFile;
}

char * initRequest(char *ip)
{
	char *param = "ping -c 4 ";
	char *rdctF = " > ";
	char *nameF = initNameFile(ip);

	size_t size = strlen(ip) + strlen(rdctF) + strlen(nameF);
	char *request = (char *) malloc(size);

	strcat( strcat( strcat( strcpy( request, param), ip), rdctF), nameF);

	return request;
}

void * pingsParallel (void * arg)
{
	ARG_T *at = (ARG_T *) arg;
	
	system( initRequest( initIP( at->request ) ) );

	return NULL;
}

void initThread(DATA data)
{
	pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
	pthread_t * tid;
	
	ARG_T *at;
		
	at  = malloc(data.nbr_ip * sizeof(ARG_T));
	tid = malloc(data.nbr_ip * sizeof(pthread_t) );
	
	for(int cnt = 0; cnt < data.nbr_ip; cnt++)
	{
		at[cnt].mut = &mut;
		at[cnt].request = data.tab_ip[cnt];
		pthread_create(tid + cnt, NULL, pingsParallel, at + cnt);
	}

	for(int cnt = 0; cnt < data.nbr_ip; cnt++)
		pthread_join(tid[cnt], NULL);
}

int main(int argc, char ** argv)
{
	DATA data;

	initData(&data, "data.cfg");
	printf("nombre de ligne : %d\n", data.nbr_ip);

	readFile(&data, "data.cfg");
	
	for(int cnt = 0; cnt < data.nbr_ip; cnt++)
		printf("ip : %s", data.tab_ip[cnt]);

	initThread(data);

	return 0;
}