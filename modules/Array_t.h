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

#define string_ref(str)	((_String){ str, strlen(str) })

_String _cstr_copy(const char* str, size_t str_length) {
	if (!str_length)
		return string_empty;

	size_t size = str_length + 1;
	_String __str = {
		.ptr = (char*)memcpy(malloc(size), str, size),
		.length = str_length
	};

	return __str;
}

_String _cstr_string_new(const char* str) {
	size_t str_length = strlen(str);
	return _cstr_copy(str, str_length);
}

_String _string_new(_String str) {
	return _cstr_copy(str.ptr, str.length);
}

#define string_new(str)	_Generic((str),	\
	char*:				_cstr_string_new,				\
	const char*:	_cstr_string_new,				\
	_String:			_string_new							\
)(str)

// string composition //

size_t _strings_length_sum(const _String* src, size_t count) {
	size_t sum = 0;

	for (; count > 0; --count)
		sum += (*src++).length;

	return sum;
}

// concatenate strings //

int _string_append_range(_String* const dest, const _String* src_strings, size_t count) {
	const size_t total_length = _strings_length_sum(src_strings, count) + dest->length;
	if (!total_length)
		return 0;

	dest->ptr = (char*)realloc((void*)dest->ptr, total_length + 1);
	char* ptr = (char*)(dest->ptr + dest->length);
	for (; count > 0; --count) {
		const _String* src_string = src_strings++;
		memcpy(ptr, src_string->ptr, src_string->length);
		ptr += src_string->length;
	}
	*ptr = '\0';

	dest->length = total_length;

	return 0;
}

#define string_append_range(dest, src, count)	\
	_Generic((dest),	\
		_String*:	_string_append_range	\
	)((dest), (src), (count))

#define string_append(dest, ...) ({	\
	const _String args[] = { __VA_ARGS__ };	\
	string_append_range((dest), args, sizeof(args)/sizeof(args[0]));	\
})
