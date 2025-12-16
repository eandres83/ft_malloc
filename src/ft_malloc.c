#include "../include/ft_malloc.h"

t_heap g_heap = {NULL, NULL, NULL};

// Creates a single large free block covering the entire zone.
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

// Must accommodate at least 100 blocks of max_block_size to minimize mmap calls.
// Returns a multiple of getpagesize().
static	size_t	calc_zone_size(size_t max_block_size)
{
	size_t min_size_needed;
	size_t zones_size;

	// We calculate how much space 100 blocks occupy (Data + Header)
	min_size_needed = (max_block_size + BLOCK_META_SIZE) * 100;

	// We round up to the next multiple of page_size
	zones_size = ((min_size_needed + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;

	return (zones_size);
}

// Extends a zone chain by allocating a new zone and linking it to the last block.
static	t_block *extend_zone(t_block *last, size_t zone_type_size)
{
	size_t	zone_size;
	t_block	*new_zone;

	zone_size = calc_zone_size(zone_type_size);
	new_zone = init_zone(zone_size);
	if (!new_zone)
		return (NULL);
	if (last)
		last->next = new_zone;

	new_zone->prev = last;
	return (new_zone);
}

// Helper to handle TINY and SMALL allocations.
// 1. Checks if zone exists
// 2. Tries to find a free block
// 3. Extends zone if no space is found
static	t_block *handle_zone_allocation(t_block **head, size_t size, size_t max_size)
{
	t_block *block;
	t_block	*cursor;

	if (!*head)
	{
		*head = init_zone(calc_zone_size(max_size));
		if (!*head)
			return (NULL);
	}
	block = find_free_block(head, size);
	if (!block)
	{
		cursor = *head;
		while (cursor && cursor->next)
			cursor = cursor->next;
		block = extend_zone(cursor, max_size);
	}
	return (block);
}

// Main Malloc Function
// 1. Aligns the requested size
// 2. Routes the request to TINY, SMALL, or LARGE handlers
// 3. Split the block if it's too bit (for TINY/SMALL)
// 4. Return pointer to the payload
void	*malloc(size_t size)
{
	size_t	aligned_size;
	t_block *block;
	
	write(1, "Mi malloc este\n", 15);
	if (size <= 0)
		return (NULL);

	// El size que me piden lo tengo que alinear a un multiplo de 16,
	// para mantener la alineacion de memoria (system alignment) de 64 bits y evitar
	// fallos en CPU. Si me piden 10 lo redondeo a 16.
	aligned_size = ALIGN(size);
	block = NULL;

	// Si es tiny o small, compruebo si ya he pre asignado un espacio para no tener que pedir todo el rato espacio al kernel
	// ahora de la zona pre asigned que es grande tengo que cojer solo el tamano que me han pedido,
	// la zona suele medir 16KB, si el user solo me pide 10 bytes tengo que splitearlo.
	if (aligned_size <= TINY_MAX_SIZE)
		block = handle_zone_allocation(&g_heap.tiny_zone, aligned_size, TINY_MAX_SIZE);
	else if (aligned_size <= SMALL_MAX_SIZE)
		block = handle_zone_allocation(&g_heap.small_zone, aligned_size, SMALL_MAX_SIZE);
	else
		return (malloc_large(aligned_size));
	// Cuando ya tienes la zona la tienes que splitear para devolver solo el tamano que te han pedido.
	// una tiny zone puede ser 16KB y el user solo te ha pedido 10 bytes,
	// hacer el split y devuelves el puntero al payload de ese bloque saltandote los metadatos del principio.
	if (!block)
		return (NULL);

	split_block(block, aligned_size);
	block->free = 0;

	return ((void *)((char *)block + BLOCK_META_SIZE));
}

