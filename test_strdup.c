#include <string.h>
#include <unistd.h>
#include "include/ft_malloc.h"

void	ft_putchar_fd(char c, int fd)
{
	write(fd, &c, 1);
}

void	ft_putstr_fd(char *s, int fd)
{
	if (s)
		write(fd, s, strlen(s));
}

void	ft_putnbr_hex(unsigned long n)
{
	char	*base = "0123456789ABCDEF";

	if (n >= 16)
		ft_putnbr_hex(n / 16);
	ft_putchar_fd(base[n % 16], 1);
}

void	ft_putbyte(unsigned char c)
{
	char	*base = "0123456789ABCDEF";
	
	write(1, "0x", 2);
	ft_putchar_fd(base[c / 16], 1);
	ft_putchar_fd(base[c % 16], 1);
}

void	ft_putaddr(void *ptr)
{
	write(1, "0x", 2);
	ft_putnbr_hex((unsigned long)ptr);
}

int	main(void)
{
	char *str;

	ft_putstr_fd("\n=== INICIO DEL TEST ===\n", 1);

	str = malloc(15);
	if (!str)
		return (1);

	// Test Poisoning (Deberia ser 0xAA si esta activo)
	ft_putstr_fd("Valor virgen (malloc): ", 1);
	ft_putbyte((unsigned char)str[0]); 
	ft_putstr_fd("\n", 1);

	strcpy(str, "Hola");
	ft_putstr_fd("String inicial: ", 1);
	ft_putstr_fd(str, 1);
	ft_putstr_fd(" (Direccion: ", 1);
	ft_putaddr(str);
	ft_putstr_fd(")\n", 1);

	str = realloc(str, 20);
	strcat(str, " Mundo!");
	
	ft_putstr_fd("String ampliado: ", 1);
	ft_putstr_fd(str, 1);
	ft_putstr_fd(" (Direccion: ", 1);
	ft_putaddr(str);
	ft_putstr_fd(")\n", 1);

	free(str);
	ft_putstr_fd("\n--- MEMORIA LIBERADA ---\n", 1);

	// Test Poisoning tras free (Deberia ser 0xBB si esta activo)
	ft_putstr_fd("Valor tras free: ", 1);
	ft_putbyte((unsigned char)str[0]);
	ft_putstr_fd("\n", 1);

	ft_putstr_fd("\n--- SHOW ALLOC MEM ---\n", 1);
	show_alloc_mem();

	// Para ver algo interesante, vamos a crear un bloque nuevo antes del dump
	char *secreto = malloc(32);
	strcpy(secreto, "Mensaje Secreto en el HexDump");
	
	ft_putstr_fd("\n--- SHOW ALLOC MEM EX (HEX DUMP) ---\n", 1);
	show_alloc_mem_ex();
	
	free(secreto);

	return (0);
}

