#include "../include/ft_malloc.h"

// Busca el primer bloque que encuentra que este libre y me entre el payload alineado
t_block	*find_free_block(t_block **list, size_t size)
{
	t_block	*current;

	current = *list;

	while (current != NULL)
	{
		if (current->free && current->size >= size)
			return (current);

		current = current->next;
	}
	return (NULL);
}

// Splitea el bloque para el temano que me piden
// t_block *block es el puntero al inicio del bloque donde tengo que
// meter el size
// tengo que meter el nuevo bloque entre el Bloque A y Bloque B
// [ Bloque A ] <-----(prev/next)-----> [ Bloque B ]
void split_block(t_block *block, size_t size)
{
	t_block *new_block;

	// Comprobar si entra el size en el bloque
	// Puede ser que despues de muchos usos el bloque no tenga el espacio suficiente
	if (block->size < size + BLOCK_META_SIZE + ALIGN(1))
		return ;

	// Casteo el t_block a char * para que lo gestione como bytes y le sumo los metadatos y el size
	// Esto se hace para que el new_block este por delante del que yo quiero crear
	// si el bloque mide 1000 y yo quiero 10, el new_block son los 958, y el block anterior lo convierto en el tamano que me piden
	new_block = (t_block *)((char *)block + BLOCK_META_SIZE + size);

	// Calculo los valores del new_block
	new_block->size = block->size - size - BLOCK_META_SIZE;
	new_block->next = block->next;
	new_block->prev = block;
	new_block->free = 1;

	block->size = size;
	block->next = new_block;

	// Si no estamos al final de la lista tenemos que actualizarlo
	if (new_block->next)
		new_block->next->prev = new_block;
}

void	*malloc_large(size_t size)
{
	t_block *new_block;
	size_t	page_size;
	size_t	total_size;

	page_size = getpagesize();

	total_size = size + BLOCK_META_SIZE;

	// Redondear al siguiente multiplo de pagina
	// ((total + 4095) / 4096) * 4096
	total_size = ((total_size + page_size -1) / page_size) * page_size;

	// Pedir memoria al sistema
	new_block = (t_block *)request_memory(total_size);

	new_block->size = total_size - size - BLOCK_META_SIZE;
	new_block->free = 0; // Ocupado
	new_block->next = g_heap.large_zone; // Insertamos al principio
	new_block->prev = NULL;

	// Si existe tenemos que actualizar su ->prev ahora tiene que ser new_block;
	// [ Bloque A ] <-----(prev)----- [ Bloque B]
	if (g_heap.large_zone)
		g_heap.large_zone->prev = new_block;

	// Y tambien actualizamos para que ahora la variable global apunte al nuevo bloque jefe.
	g_heap.large_zone = new_block;

	// devolver el puntero al payload
	return ((void *)((char *)new_block + BLOCK_META_SIZE));
}

void	*request_memory(size_t size)
{
	void *ptr;

	ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (ptr == MAP_FAILED)
		return (NULL);
	return (ptr);
}

