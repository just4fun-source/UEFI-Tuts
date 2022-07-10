// UEFI From Scratch - ThatOSDev ( 2021 - 2022 )
// https://github.com/ThatOSDev/C_UEFI

#ifndef CLIBS_H
#define CLIBS_H

#define NULL (void*)0
#define DECIMAL 10
#define HEX     16

typedef unsigned long long     size_t;
typedef unsigned char          uint8_t;

void itoa(unsigned long int n, CHAR16* buffer, int basenumber)
{
	unsigned long int hold;
	int i, j;
	hold = n;
	i = 0;
	
	do{
		hold = n % basenumber;
		buffer[i++] = (hold < 10) ? (hold + '0') : (hold + 'a' - 10);
	} while(n /= basenumber);
	buffer[i--] = 0;
	
	for(j = 0; j < i; j++, i--)
	{
		hold = buffer[j];
		buffer[j] = buffer[i];
		buffer[i] = hold;
	}
}

// string.h
size_t strlen(const char* str)
{
	const char* strCount = str;

	while (*strCount++);
	return strCount - str - 1;
}

void* memcpy(void* dst, const void* src, size_t size)
{
    for ( size_t i = 0; i < size; i++ )
        ((uint8_t*) dst)[i] = ((const uint8_t*) src)[i];
    return dst;
}

int memcmp(const void* src1, const void* src2, size_t numberOfBytes)
{
  unsigned char const* str1 = src1;
  unsigned char const* str2 = src2;

  while (numberOfBytes--) {
    if (*str1 != *str2)
      return (int)*str1 - (int)*str2;
    str1++;
    str2++;
  }

  return 0;
}

void *memset(void* str, int chr, size_t numberOfBytes)
{
  unsigned char* dest = str;

  while (numberOfBytes--)
    *dest++ = (unsigned char)chr;

  return str;
}

void* memmove(void* destination, const void* source, size_t numberOfBytes)
{
  unsigned char* dest = destination;
  unsigned char const* src = source;

  if (dest < src) {
    while (numberOfBytes--)
      *dest++ = *src++;
  } else {
    dest += numberOfBytes;
    src += numberOfBytes;
    while (numberOfBytes--)
      *--dest = *--src;
  }

  return destination;
}

void* memchr(const void* string, int chr, size_t numberOfBytes)
{
	size_t			i;
	unsigned char* str = (unsigned char*)string;
	unsigned char asciiChar = (unsigned char)chr;
	i = 0;
	while (i < numberOfBytes)
	{
		if (*str == asciiChar)
		{
			return (str);
		}
		str++;
		i++;
	}
	return (NULL);
}

#endif // CLIBS_H