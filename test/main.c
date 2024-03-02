#include <sdk/sdk.h>

void string_test() {
  console_log_cstr(CONSOLE_FORE_LIGHTBLUE "STRING" CONSOLE_RESET);
  string* str1 = string_new();
  string* str2 = string_new();

  string_append_cstr(str1, "test");
  assert(string_equal_cstr(str1, "test") == true);
  string_append_char(str1, '1');
  assert(string_equal_cstr(str1, "test1") == true);
  string_append_cstr(str2, "test2");
  assert(string_equal_cstr(str2, "test2") == true);
  assert(string_equal_string(str1, str2) == false);
  assert(string_compare(str1, str2) != 0);
  string_append_string(str1, str2);
  assert(string_equal_cstr(str1, "test1test2") == true);
  string_free(str1);
  string_free(str2);
}
void array_test() {
  console_log_cstr(CONSOLE_FORE_LIGHTBLUE "ARRAY" CONSOLE_RESET);
  array* arr = array_new(u64);
  array_push(arr, (u64)10);
  array_push(arr, (u64)11);
  array_push(arr, (u64)12);
  assert(array_at(arr, u64, 0) == 10);
  assert(array_at(arr, u64, 1) == 11);
  assert(array_at(arr, u64, 2) == 12);
  assert(array_length(arr) == 3);
  assert(array_capacity(arr) == 4);
  array_free(arr);
}
void socket_test() {
  console_log_cstr(CONSOLE_FORE_LIGHTBLUE "SOCKET" CONSOLE_RESET);
  error_last = socket_startup();
  if (error_last)
    return;
  net_socket* socket = socket_new();
  socket_host_set(socket, "google.com.br");
  socket_port_set(socket, 80);
  error_last = socket_connect(socket);
  if (error_last)
    return;
  socket_write_cstr(
    socket,
    "GET / HTTP/1.1\r\n"
    "Host: google.com.br\r\n"
    "Connection: close\r\n"
    "\r\n"
  );
  u16 buffer_size = 65535;
  char buffer[buffer_size];
  u16 buffer_length = 0;
  while (true) {
    i32 received = socket_read(socket, buffer + buffer_length, buffer_size);
    if (!received)
      break;
    if (received < 0) {
      error_last = error_last;
      return;
    }
    buffer_length += received;
  }
  if (!error_last) {
    console_log_str(buffer, buffer_length);
    assert(buffer_length > 0);
  }
  socket_free(socket);
}
void date_test() {
  console_log_cstr(CONSOLE_FORE_LIGHTBLUE "DATE" CONSOLE_RESET);
  assert(date_now() > 0);
}
void snowflake_test() {
  console_log_cstr(CONSOLE_FORE_LIGHTBLUE "SNOWFLAKE" CONSOLE_RESET);
  assert(snowflake_uid() > 0);
}
void map_test() {
  console_log_cstr(CONSOLE_FORE_LIGHTBLUE "MAP" CONSOLE_RESET);
  map* map_u64 = map_new(u64);
  map_set_cstr(map_u64, "key1", (u64)10);
  assert(map_get_cstr(map_u64, u64, "key1") == 10);
  map_delete_cstr(map_u64, "key1");
  assert(map_get_cstr(map_u64, u64, "key1") == 0);
  map_free(map_u64);
}
void http_test() {

}

int main() {
  console_inicialize();

  // string_test();
  // array_test();
  // socket_test();
  // date_test();
  // snowflake_test();
  // map_test();

  app_run();

  console_log_cstr(
    CONSOLE_FORE_GREEN
    "SUCCESS"
    CONSOLE_FORE_GREEN
    CONSOLE_RESET
  );
  return 0;
}