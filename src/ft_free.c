#include "../include/ft_malloc.h"

static	void	free_large(t_block *block)
{
	size_t total_size;
	size_t size;

	if (block->prev)
		block->prev->next = block->next;
	else
		g_heap.large_zone = block->next;

	if (block->next)
		block->next->prev = block->prev;

	// Calculate total size
	total_size = block->size + BLOCK_META_SIZE;
	size = ((total_size + PAGE_SIZE -1) / PAGE_SIZE) * PAGE_SIZE;

	if (munmap((void *)block, size) == -1)
		return ;
}

static	void	merge_block(t_block *block)
{
	block->size = block->size + BLOCK_META_SIZE + block->next->size;
	block->next = block->next->next;
	if (block->next)
		block->next->prev = block;
}

static	void	coalising(t_block *block)
{
	if (block->next && block->next->free == 1)
		merge_block(block);
	if (block->prev && block->prev->free == 1)
		merge_block(block->prev);
}

void	free(void *ptr)
{
	t_block *block;

	write(1, "Mi free aqui", 12);
	if (ptr == NULL)
		return ;

	block = (t_block *)((char *)ptr - BLOCK_META_SIZE);

	if (block->size <= SMALL_MAX_SIZE)
	{
		block->free = 1;
		coalising(block);
	}
	else
		free_large(block);
}
