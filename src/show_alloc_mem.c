#include "../include/ft_malloc.h"

static	int	ft_isprint(int c)
{
	return (c >= 32 && c <= 126);
}

static	void	ft_putchar(char c)
{
	write(1, &c, 1);
}

static	void	ft_putstr(char *str)
{
	int i;

	i = 0;
	while (str[i])
		i++;
	write(1, str, i);
}

static	void	ft_putnbr(size_t n)
{
	if (n >= 10)
		ft_putnbr(n / 10);
	ft_putchar((n % 10) + '0');
}

static	void	ft_puthex(size_t n)
{
	char *base;

	base = "0123456789ABCDEF";
	if (n >= 16)
		ft_puthex(n / 16);
	ft_putchar(base[n % 16]);
}

static	void	ft_putaddr(void *ptr)
{
	write(1, "0x", 2);
	ft_puthex((size_t)ptr);
}

// Iterates throught a zone list and prints only occupied blocks.
// Returns the total bytes accoupied in this zone
static	size_t	print_zone(char *name, t_block *block)
{
	size_t	total_bytes;
	void	*start;
	void	*end;

	total_bytes = 0;
	ft_putstr(name);
	ft_putstr(" : ");
	ft_putaddr(block);
	ft_putchar('\n');

	while (block)
	{
		if (block->free == 0)
		{
			start = (void *)((char *)block + BLOCK_META_SIZE);
			end = (void *)((char *)start + block->size);

			ft_putaddr(start);
			ft_putstr(" - ");
			ft_putaddr(end);
			ft_putstr(" : ");
			ft_putnbr(block->size);
			ft_putstr(" bytes\n");

			total_bytes += block->size;
		}
		block = block->next;
	}
	return (total_bytes);
}

static	void	print_hex_line(void *addr, size_t len)
{
	size_t		i;
	unsigned char	*ptr;

	ptr = (unsigned char *)addr;
	ft_putaddr(addr);
	ft_putstr("  ");

	i = 0;
	while (i < 16)
	{
		if (i < len)
		{
			if (ptr[i] < 16)
				ft_putchar('0');
			ft_puthex(ptr[i]);
		}
		else
			ft_putstr("  ");
		ft_putstr(" ");
		i++;
	}
	ft_putstr(" |");

	i = 0;
	while (i < 16 && i < len)
	{
		if (ft_isprint(ptr[i]))
			ft_putchar(ptr[i]);
		else
			ft_putchar('.');
		i++;
	}
	ft_putstr("|\n");
}

static	void	dump_memory(void *ptr, size_t size)
{
	size_t i;

	i = 0;
	while (i < size)
	{
		print_hex_line((char *)ptr + i, size - i);
		i += 16;
	}
}

static	void	dump_zone(char *name, t_block *block)
{
	void	*start;

	ft_putstr("\n[ DUMPING ");
	ft_putstr(name);
	ft_putstr(" ZONE ]\n");

	while (block != NULL)
	{
		if (block->free == 0)
		{
			start = (void *)((char *)block + BLOCK_META_SIZE);
			ft_putstr("BLOCK: ");
			ft_putaddr(start);
			ft_putstr(" (");
			ft_putnbr(block->size);
			ft_putstr(" bytes)\n");
			dump_memory(start, block->size);
			ft_putstr("\n");
		}
		block = block->next;
	}
}

// Display extended info
void	show_alloc_mem_ex(void)
{
	if (g_heap.tiny_zone)
		dump_zone("TINY", g_heap.tiny_zone);
	if (g_heap.small_zone)
		dump_zone("SMALL", g_heap.small_zone);
	if (g_heap.large_zone)
		dump_zone("LARGE", g_heap.large_zone);
}

// Displays the state of all allocated memory zones
void	show_alloc_mem(void)
{
	size_t total;

	total = 0;

	if (g_heap.tiny_zone)
		total += print_zone("TINY", g_heap.tiny_zone);

	if (g_heap.small_zone)
		total += print_zone("SMALL", g_heap.small_zone);

	if (g_heap.large_zone)
		total += print_zone("LARGE", g_heap.large_zone);

	ft_putstr("Total : ");
	ft_putnbr(total);
	ft_putstr(" bytes\n");

	if (g_heap.debug_mode)
	{
		ft_putstr("=== BONUS INFO ===\n");
		ft_putstr("Malloc calls : ");
		ft_putnbr(g_heap.malloc_calls);
		ft_putchar('\n');
		ft_putstr("Free calls : ");
		ft_putnbr(g_heap.free_calls);
		ft_putchar('\n');
		ft_putstr("Total Alloc : ");
		ft_putnbr(g_heap.total_allocated);
		ft_putchar('\n');
		ft_putstr("Poisoning : ACTIVE (0xAA/0xBB)\n");
	}
}

