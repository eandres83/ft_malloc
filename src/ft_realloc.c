#include "../include/ft_malloc.h"

// Standar memcpy implementation to avoid external library dependecies.
// Copies n bytes from src to dest.
static	void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t		i;
	unsigned char	*d;
	unsigned char	*s;

	if (!dest && !src)
		return (NULL);
	d = (unsigned char *)dest;
	s = (unsigned char *)src;
	i = 0;
	while (i < n)
	{
		d[i] = s[i];
		i++;
	}
	return (dest);
}

// Change the size of the memory block pointed to by ptr to size bytes.
// 1 If ptr is NULl -> malloc(size)
// 2 If size is 0 -> free(ptr)
// 3 Optimization: If current block holds enought space -> shrink it (split) & return.
// 4 Fallback: Malloc new space -> Copy data -> Free old space.
void	*realloc(void *ptr, size_t size)
{
	t_block *block;
	void	*new_ptr;
	size_t	copy_size;

	if (ptr == NULL)
		return (malloc(size));

	if (size == 0)
		return (free(ptr), NULL);

	block = (t_block *)((char *)ptr - BLOCK_META_SIZE);

	// If the current block is large enough, we don't need to move data.
	// We try to split the block to return unused memory to the system.
	if (block->size >= ALIGN(size) && block->size <= SMALL_MAX_SIZE)
	{
		split_block(block, ALIGN(size));
		return (ptr);
	}

	new_ptr = malloc(size);
	if (!new_ptr)
		return (NULL);

	// calculate safe copy size (min of old vs new size)
	if (block->size < size)
		copy_size = block->size;
	else
		copy_size = size;

	ft_memcpy(new_ptr, ptr, copy_size);
	free(ptr);

	return (new_ptr);
}

