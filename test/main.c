#include <sdk/sdk.h>

void string_test() {
  console_log("STRING");
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
  console_log("!STRING");
}
void array_test() {
  console_log("ARRAY");
  array arr;
  array_constructor(&arr, u64);
  array_push(&arr, (u64)10);
  array_push(&arr, (u64)11);
  array_push(&arr, (u64)12);
  assert(((u64*)arr.data)[0] == 10);
  assert(((u64*)arr.data)[1] == 11);
  assert(((u64*)arr.data)[2] == 12);
  assert(arr.length == 3);
  assert(arr.capacity == 4);
  array_free(&arr);
  console_log("!ARRAY");
}
void socket_test() {
  console_log("SOCKET");
  error_code error = socket_startup();
  if (error)
    return;
  net_socket sock;
  socket_constructor(
    &sock, (socket_options) {
    .host = "localhost",
      .port = 8080,
      .timeout = 0
  });
  console_log("!SOCKET");
}

int main() {
  console_inicialize();
  // string_test();
  // array_test();
  socket_test();

  app_run();
  return 0;
}