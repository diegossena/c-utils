#include <sdk/sdk.h>

void string_test() {
  console_log_cstr(CONSOLE_FORE_LIGHTBLUE "STRING" CONSOLE_RESET);
  string_t* str1 = string_new();
  string_t* str2 = string_new();
  string_append_cstr(str1, "test");
  console_log_cstr("LINE");
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
  array_t* arr = array_new(u64);
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
void date_test() {
  console_log_cstr(CONSOLE_FORE_LIGHTBLUE "DATE" CONSOLE_RESET);
  assert(date_now() > 0);
  console_log("%llu", date_now);
}
void snowflake_test() {
  console_log_cstr(CONSOLE_FORE_LIGHTBLUE "SNOWFLAKE" CONSOLE_RESET);
  assert(snowflake_uid() > 0);
}
void map_test() {
  console_log_cstr(CONSOLE_FORE_LIGHTBLUE "MAP" CONSOLE_RESET);
  map_t* map_u64 = map_new(u64);
  map_set_cstr(map_u64, "key1", (u64)10);
  assert(map_get_cstr(map_u64, u64, "key1") == 10);
  map_delete_cstr(map_u64, "key1");
  assert(map_get_cstr_p(map_u64, u64, "key1") == 0);
  map_free(map_u64);
}


void on_tcp_on_read(net_tcp_t* this, const byte* data, u64 length, const void* context) {
  console_log("on_tcp_on_read=%llu", length);
  console_log_str(data, length);
  console_log_cstr();
}
void on_tcp_on_write(net_tcp_t* this, const void* context) {
  console_log_cstr("on_tcp_on_write");
  net_tcp_read(this, 0, on_tcp_on_read);
}

void on_tcp_on_connect(net_tcp_t* this) {
  console_log_cstr("on_tcp_on_connect");
  const char request [] = "GET / HTTP/1.1\r\nConnection: close\r\n\r\n";
  net_tcp_write(this, request, sizeof(request) - 1, on_tcp_on_write);
}
void net_tcp_test() {
  console_log_cstr(CONSOLE_FORE_LIGHTBLUE "NET_TCP_CLIENT" CONSOLE_RESET);
  net_tcp_t* socket = net_tcp_new();
  net_tcp_ip4_connect(socket, "google.com.br", 80, on_tcp_on_connect);
}

void tcp_server_on_connection(net_tcp_client_t* client) {
  console_log_cstr("on_tcp_server_connection");
}
void net_tcp_server_test() {
  console_log_cstr(CONSOLE_FORE_LIGHTBLUE "NET_TCP_SERVER" CONSOLE_RESET);
  net_tcp_server_t* server = net_tcp_server_new();
  net_tcp_server_ip4_listen(server, 8080, tcp_server_on_connection);
  net_tcp_server_t* server = net_tcp_server_new();
  net_tcp_server_ip4_listen(server, 8080, tcp_server_on_connection);
}

i32 main() {
  console_inicialize();
  net_inicialize();

  // string_test();
  // array_test();
  // date_test();
  // snowflake_test();
  // map_test();
  net_tcp_test();
  // net_tcp_server_test();

  console_log_cstr(
    CONSOLE_FORE_GREEN
    "SUCCESS"
    CONSOLE_FORE_GREEN
    CONSOLE_RESET
  );
  return app_run();
}