#ifndef H_READ
#define H_READ

typedef struct {
	int descripteur;
	int acces;
	char *path;
} OI_FILE;

OI_FILE OI_open (const char * path, int acces);

int OI_close (OI_FILE file);

int OI_delete (const char * path);

int OI_readChar (OI_FILE file, char * caractere);

int OI_readStr (OI_FILE file, char * chaine, int maxSize);

#endif