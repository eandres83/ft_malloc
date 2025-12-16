#include "../include/ft_malloc.h"

static	size_t	*ft_memcpy(void *dest, const void *src, size_t n)
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

void	*realloc(void *ptr, size_t size)
{
	t_block *block;
	void	*new_ptr;
	size_t	copy_size;

	if (ptr == NULL)
		return (malloc(size));

	if (size == 0)
		return (free(size), NULL);

	block = (t_block *)((char *)ptr - BLOCK_META_SIZE);

	// Si el bloque ya es suficientement grande lo devolvemos
	if (block->size >= ALIGN(size))
		return (ptr);

	new_ptr = malloc(size);
	if (!new_ptr)
		return (NULL);

	if (block->size < size)
		copy_size = block->size
	else
		copy_size = size;

	ft_memcpy(new_ptr, ptr, copy_size);
	free(ptr);

	return (new_ptr);
}

