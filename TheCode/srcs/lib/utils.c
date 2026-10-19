#include <stddef.h>
#include <stdint.h>

// copy n bytes from src to dest
void ft_memcpy(void *dest, const void *src, size_t n)
{
	uint8_t			*to = dest;
	const uint8_t	*from = src;

	for (size_t i = 0; i < n; i++)
	{
		to[i] = from[i];
	}
}
