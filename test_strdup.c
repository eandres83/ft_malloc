#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "include/ft_malloc.h"

int main(void)
{
	char *str;

	str = malloc(15);

	unsigned char c = (unsigned char)str[0];
	printf("Valor en memoria :0x%X\n", c);

	strcpy(str, "Hola");

	unsigned char s = (unsigned char)str[0];
	printf("Valor en memoria 2 :0x%X\n", s);

	printf("String inicial: %s (Direccion: %p)\n", str, str);

	str = realloc(str, 20);

	if (!str)
		return (1);

	strcat(str, " Mundo!");
	printf("String ampliado: %s (Direccion: %p)\n", str, str);

	str = realloc(str, 20);
	printf("String reducido: %s (Direccion: %p)\n", str, str);

	show_alloc_mem();

	unsigned char q = (unsigned char)str[0];
	printf("Valor en memoria 2 :0x%X\n", q);

	printf("\nBONUS INFO\n\n");

	show_alloc_mem_ex();
	free(str);

	return (0);
}

