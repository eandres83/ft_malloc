#include <string.h>
#include <stdio.h>
#include <unistd.h>

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
	printf("Original: %s\n", original);
	printf("Copia   : %s\n", copia);

	return (0);
}
