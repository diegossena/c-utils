// https://github.com/maxim2266/str

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// string type

typedef struct string_t
{
	const char* ptr;
	size_t length;
} _string_t;

#define string_null ((string_t){ 0, 0 })
#define string_empty ((string_t){ "", 0 })

// string properties //

size_t string_length(const _string_t str) { return str.length; }
const char* string_ptr(const _string_t str) { return str.ptr; }

// string memory control //

void _string_free(const _string_t* str) {
	printf("str_free");
	if (str->length)
		free((void*)str->ptr);
}

#define string_t	_string_t __attribute__((cleanup(_string_free)))


// concatenate strings
int _string_cat_range(_string_t* const dest, const _string_t* src, size_t count) {
	printf("_string_cat_range");
	// if (!src)
	// {
	// 	str_clear(dest);
	// 	return 0;
	// }

	// // calculate total length
	// const size_t num = total_length(src, count);

	// if (num == 0)
	// {
	// 	str_clear(dest);
	// 	return 0;
	// }

	// // allocate
	// char* const buff = ALLOC(num + 1);

	// // copy bytes
	// char* p = buff;

	// for (; count > 0; --count)
	// 	p = append_str(p, *src++);

	// // null-terminate and assign
	// *p = 0;
	// str_assign(dest, str_acquire_chars(buff, num));
	return 0;
}

#define string_cat_range(dest, src, count)	\
	_Generic((dest),	\
		str*:	_string_cat_range	\
	)((dest), (src), (count))

#define string_cat(dest, ...)	\
({	\
	const str args[] = { __VA_ARGS__ };	\
	string_cat_range((dest), args, sizeof(args)/sizeof(args[0]));	\
})
