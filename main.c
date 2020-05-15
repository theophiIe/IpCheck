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
	int   nbr_ip;
	
} DATA;

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
	for(int cmptIp = 0; cmptIp < data -> nbr_ip; cmptIp++)
		data -> tab_ip[cmptIp] = malloc(SIZE_IP * sizeof(char));
		
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

	for(int cmptIp = 0; cmptIp < data -> nbr_ip; cmptIp++)
	{
		SE_lectureChaine (file, data -> tab_ip[cmptIp], SIZE_IP+1);
		printf("ip lecture: %s", data -> tab_ip[cmptIp]);
	}

	SE_fermeture(file);

	return 0;
}

int main(int argc, char ** argv)
{
	DATA data;

	initData(&data, "data.cfg");
	printf("nombre de ligne : %d\n", data.nbr_ip);

	readFile(&data, "data.cfg");
	
	for(int cmpt = 0; cmpt < data.nbr_ip; cmpt++)
		printf("ip : %s", data.tab_ip[cmpt]);

	char *param = "ping -c 4";

	size_t fullSize = strlen( param ) + 1 +  strlen( data.tab_ip[0] ) + 1;;

	char *config = (char *) malloc( fullSize );

	strcat( strcat( strcpy( config, param ), " " ), data.tab_ip[0] );

	printf("requete : %s", config);
	system(config);


	return 0;
}