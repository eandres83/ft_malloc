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
void split_block(t_block *block, size_t size)
{
	t_block *new_block;

	// Comprobar si entra el size en el bloque
	// Puede ser que despues de muchos usos el bloque no tenga el espacio suficiente
	if (block->size < size + BLOCK_META_SIZE + ALIGN(1))
		return ;

	// Casteo el t_block a char * para que lo gestione como bytes y le sumo los metadatos y el size
	// Esto se hace para que el new_block este por delante del que yo quiero crear
	// si el bloque mide 1000 y yo quiero 10, el new_block son los 990, y el block anterior lo convierto en el tamano que me piden
	new_block = (t_block *)((char *)block + BLOCK_META_SIZE + size);

	// Calculo los valores del new_block
	new_block->size = block->size - size - BLOCK_META_SIZE;
	new_block->next = block->next;
	new_block->prev = block;
	new_block->free = 1;

	block->size = size;
	block->next = new_block;

	if (new_block->next)
		new_block->next->prev = new_block;
}

