#include "../include/ft_malloc.h"

t_heap g_heap = {NULL, NULL, NULL};

static	void	*request_memory(size_t size)
{
	void *ptr;

	ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (ptr == MAP_FAILED)
		return (NULL);
	return (ptr);
}

static	t_block *init_zone(size_t zone_size)
{
	void 	*zone_ptr;
	t_block *first_block;

	zone_ptr = request_memory(zone_size);
	if (!zone_ptr)
		return (NULL);

	first_block = (t_block *)zone_ptr;

	first_block->size = zone_size - BLOCK_META_SIZE;
	first_block->next = NULL;
	first_block->prev = NULL;
	first_block->free = 1;

	return (first_block);
}

static	size_t	calc_zone_size(size_t max_block_size)
{
	size_t page_size;
	size_t min_size_needed;
	size_t zones_size;

	page_size = getpagesize(); // Normaly 4096

	// We calculate how much space 100 blocks occupy (Data + Header)
	min_size_needed = (max_block_size + BLOCK_META_SIZE) * 100;

	// We round up to the next multiple of page_size
	zones_size = ((min_size_needed + page_size - 1) / page_size) * page_size;

	return (zones_size);
}

void	*malloc(size_t size)
{
	size_t	aligned_size;
	t_block *zone;
	
	if (size <= 0)
		return (NULL);

	// El size que me piden lo tengo que alinear a un multiplo de 8,
	// para tener la ram organizada, por lo que tengo que meter padding
	// si te piden 10 bytes y mi struct mide 28 bytes, tengo que pedir 38 pero como 
	// tiene que ser multiplo de 8 tengo que pedir 40 bytes para que los ultimos tres digitos sean 0
	aligned_size = ALIGN(size);

	// Compruebo el tamano que quieren reservar y chequeo si es tiny o small
	// Si es tiny o small, compruebo si ya he pre asignado un espacio para no tener que pedir todo el rato espacio al kernel
	// ahora de la zona pre asigned que es grande tengo que cojer solo el tamano que me han pedido,
	// la zona suele medir 16KB, si el user solo me pide 10 bytes tengo que splitearlo.
	if (aligned_size <= TINY_MAX_SIZE)
	{
		if (!g_heap.tiny_zone)
		{
			size_t	zone_size = calc_zone_size(TINY_MAX_SIZE);
			g_heap.tiny_zone = init_zone(zone_size);

			if (!g_heap.tiny_zone)
				return (NULL);
		}
		zone = find_free_block(&g_heap.tiny_zone, aligned_size);
		if (!zone)
		{
			t_block *cursor = g_heap.tiny_zone;
			while (cursor && cursor->next)
				cursor = cursor->next;

			zone = extend_zone(cursor, TINY_MAX_SIZE);
			if (!zone)
				return (NULL);
		}
	}
	else if (aligned_size <= SMALL_MAX_SIZE)
	{
		if (!g_heap.small_zone)
		{
			size_t zone_size = calc_zone_size(SMALL_MAX_SIZE);
			g_heap.small_zone = init_zone(zone_size);

			if (!g_heap.small_zone)
				return (NULL);
		}
		zone = find_free_block(&g_heap.small_zone, aligned_size);
	}
	else
		return (malloc_large(aligned_size));
	// Cuando ya tienes la zona la tienes que splitear para devolver solo el tamano que te han pedido.
	// una tiny zone puede ser 16KB y el user solo te ha pedido 10 bytes,
	// hacer el split y devuelves el puntero al payload de ese bloque saltandote los metadatos del principio.
	if (zone)
	{
		split_block(zone, aligned_size);
		zone->free = 0; // Poner free a 0 para indicar que este bloque esta ocupado, para el free

		// Tienes que apuntar al payload y para eso tienes que saltar lo que ocupa los metadatos.
		// Por lo que tienes que castear zone a char * para poder utilizar '+' y que salte byte a byte.
		return ((void *)((char *)zone + BLOCK_META_SIZE));
	}
	return (NULL);
}

