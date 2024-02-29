#include <SDK/SDK.h>
#include <stdio.h> // TODO: remove this line

void string_test() {
  printf("STRING\n");
  string str1 = {};
  string str2 = {};

  string_append_cstr(&str1, "test");
  string_append_char(&str1, '1');
  string_append_cstr(&str2, "test2");
  assert(string_equal(str1.data, "test1") == true);
  assert(string_equal(str2.data, "test2") == true);
  assert(string_equal(str1.data, str2.data) == false);
  assert(string_compare(str1.data, str2.data) != 0);

  string_free(&str1);
  string_free(&str2);
  printf("!STRING\n");
}

int main() {
  string_test();
  return 0;
}