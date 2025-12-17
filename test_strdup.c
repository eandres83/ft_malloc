#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
	char *str;

	str = malloc(15);
	strcpy(str, "Hola");
	printf("String inicial: %s (Direccion: %p)\n", str, str);

	str = realloc(str, 20);

	if (!str)
		return (1);

	strcat(str, " Mundo!");
	printf("String ampliado: %s (Direccion: %p)\n", str, str);

	str = realloc(str, 20);
	printf("String reducido: %s (Direccion: %p)\n", str, str);

	free(str);

	return (0);
}

