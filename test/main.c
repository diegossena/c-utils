#include <sdk/sdk.h>

void string_test() {
  console_log("STRING");
  string str1 = {};
  string str2 = {};

  string_append_cstr(&str1, "test");
  assert(string_equal(str1.data, "test") == true);
  string_append_char(&str1, '1');
  assert(string_equal(str1.data, "test1") == true);
  string_append_cstr(&str2, "test2");
  assert(string_equal(str2.data, "test2") == true);
  assert(string_equal(str1.data, str2.data) == false);
  assert(string_compare(str1.data, str2.data) != 0);
  string_append_str(&str1, str2.data, str2.length);
  assert(string_equal(str2.data, "test1test2") == true);
  string_free(&str1);
  string_free(&str2);
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
}
void socket_test() {
  console_log("SOCKET");
  error_last = socket_startup();
  if (error_last)
    return;
  net_socket socket;
  error_last = socket_constructor(
    &socket, (socket_options) {
    .host = "google.com.br",
      .port = 80,
      .timeout = 0
  });
  if (error_last)
    return;
  socket_write_cstr(
    &socket,
    "GET / HTTP/1.1\r\n"
    "Host: google.com.br\r\n"
    "Connection: close\r\n"
    "\r\n"
  );
  u64 buffer_size = 25565;
  char buffer[buffer_size];
  u64 buffer_length = 0;
  while (true) {
    i32 received = socket_read(&socket, buffer + buffer_length, buffer_size);
    if (!received)
      break;
    if (received < 0) {
      error_last = error_last;
      return;
    }
    buffer_length += received;
  }
  if (!error_last) {
    console_log("%s", buffer);
  }
  socket_free(&socket);
}
void date_test() {
  console_log("DATE");
  assert(date_now() > 0);
}
void snowflake_test() {
  console_log("SNOWFLAKE");
  assert(snowflake_uid() < 0);
}

int main() {
  console_inicialize();

  // string_test();
  // array_test();
  // socket_test();
  // date_test();
  // snowflake_test();

  app_run();

  console_log_cstr(
    CONSOLE_FORE_GREEN
    "SUCCESS"
    CONSOLE_FORE_GREEN
    CONSOLE_RESET
  );
  return 0;
}