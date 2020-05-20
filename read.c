#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

#include "read.h"

OI_FILE OI_open (const char * path, int acces)
{
	OI_FILE file;

	if (acces & O_CREAT)
		file.descripteur = open (path, acces, 0644);

	else
		file.descripteur = open (path, acces);

	if (file.descripteur == -1) 
	{
		printf ("Erreur à l'ouverture de %s : %s\n", path, strerror (errno) );
		return file;
	}

	file.path = malloc ((strlen (path) + 1) * sizeof (char));

	strcpy (file.path, path);
	file.acces = acces;

	return file;
}

int OI_close (OI_FILE file)
{
	if (close (file.descripteur) == -1)
	{
		printf ("Erreur à la fermeture de %s : %s\n", file.path, strerror (errno) );
		return -1;
	}

	free (file.path);

	return 0;
}

int OI_delete (const char * path)
{
	if (unlink (path) == -1)
	{
		printf ("Erreur à la suppression de %s : %s\n", path, strerror (errno) );
		return -1;
	}

	return 0;
}

int OI_readChar (OI_FILE file, char * caractere)
{
	int retour = 0;

	if (! (file.acces == O_RDONLY || file.acces == (O_CREAT | O_RDONLY)	|| file.acces & O_RDONLY || file.acces & O_RDWR) )
	{
		printf ("Erreur à la lecture de %s : le fichier n'a pas été ouvert avec un accès en lecture\n", file.path);
		return -1;
	}

	retour = read (file.descripteur, caractere, 1);

	if (retour == -1)
		printf ("Erreur à la lecture de %s : %s\n", file.path, strerror (errno) );

	return retour;
}

int OI_readStr (OI_FILE file, char * chaine, int maxSize) 
{
	int i      = 0;
	int reti   = 0;
	int retour = 0;

	if (! (file.acces == O_RDONLY || file.acces == (O_CREAT | O_RDONLY) || file.acces & O_RDONLY || file.acces & O_RDWR) ) 
	{
		printf ("Erreur à la lecture de %s : le fichier n'a pas été ouvert avec un accès en lecture\n", file.path);
		return -1;
	}

	do 
	{
		reti = read (file.descripteur, chaine + i, 1); 
		retour += reti;
		++i;
	} while (reti > 0 && i < maxSize);

	if (reti < 0)
	{
		printf ("Erreur à la lecture de %s : %s\n", file.path, strerror (errno) );
		return reti;
	}

	chaine [i] = '\0';

	return retour;
}