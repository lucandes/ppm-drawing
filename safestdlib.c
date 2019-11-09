#include "safestdlib.h"

/****************************************************
Função: 
Parâmetros: 
Retorno: 

Descrição:
*****************************************************/
void *safeMalloc(size_t size){
	void *p;
	p = malloc(size);

	if (p == NULL)
		exit(1);

	return p;
}

/****************************************************
Função: 
Parâmetros: 
Retorno: 

Descrição:
*****************************************************/
void *safeCalloc(int times, size_t size){
	void *p;
	p = calloc(times, size);

	if (p == NULL)
		exit(1);

	return p;
}