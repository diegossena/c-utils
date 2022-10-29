// https://github.com/maxim2266/str

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* const _c_empty_string = "";

// string type

typedef struct String {
	const char* ptr;
	size_t length;
} _String;

#define string_empty ((_String){ 0, 0 })

// string properties //

size_t string_length(const _String str) { return str.length; }
const char* string_ptr(const _String str) { return str.ptr ? str.ptr : _c_empty_string; }

// string memory control //

void _string_free(const _String* str) {
	printf("str_free");
	if (str->length)
		free((void*)str->ptr);
}
#define String	_String __attribute__((cleanup(_string_free)))

// string constructors //

_String _cstr_string_new(const char* str) {
	size_t str_length = strlen(str);

	if (!str_length)
		return string_empty;

	size_t size = str_length + 1;
	_String __str = {
		.ptr = (char*)memcpy(malloc(size), str, size),
		.length = str_length
	};

	return __str;
}

_String _string_new(_String str) {
	return str;
}

#define string_new(str)	\
	_Generic((str),	\
		char*:	_cstr_string_new,				\
		const char*:	_cstr_string_new,	\
		_String:				_string_new				\
	)(str)

// concatenate strings //

int _string_cat_range(_String* const dest, const _String* src, size_t count) {
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
