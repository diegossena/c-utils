// https://github.com/maxim2266/str

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
  const char* buffer;
  size_t length;
} _string_t;

void _str_free(const _string_t* str) {
  printf("str_free");
  if (str->buffer)
    free((void*)str->buffer);
}

#define string_t	_string_t __attribute__((cleanup(_str_free)))

#define string_empty ((string_t){ 0, 0 })

#define str_cat_range(dest, src, count)	\
	_Generic((dest),	\
		str*:	_str_cat_range,	\
		int:	_str_cat_range_to_fd,	\
		FILE*:	_str_cat_range_to_stream	\
	)((dest), (src), (count))

#define str_cat(dest, ...)	\
({	\
	const str args[] = { __VA_ARGS__ };	\
	str_cat_range((dest), args, sizeof(args)/sizeof(args[0]));	\
})
