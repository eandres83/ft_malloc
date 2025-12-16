#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
	char *original = "Hola, malloc!";
	char *copia;

	copia = strdup(original); 
	if (!copia)
	{
		printf("Error: strdup falló (malloc devolvió NULL)\n");
		return (1);
	}
	write(1, copia, 13);
	write(1, "\n", 1);

	free(copia);
	return (0);
}

