#include <sdk/net/tcp/server.h>
#include "net.tcp.test.h"

#include <sdk/assert.h>
#include <sdk/array.h>
#include <sdk/snowflake.h>
#include <sdk/map.h>
#include "path.test.h"
#include "process.test.h"

void string_test() {
  console_log_cstr(CONSOLE_FORE_LIGHTBLUE "STRING" CONSOLE_RESET);
  string_t str1;
  string_t str2;
  string_new(&str1);
  string_new(&str2);
  string_append_cstr(&str1, "test");
  assert(string_equal_cstr(&str1, "test") == true);
  string_append_char(&str1, '1');
  assert(string_equal_cstr(&str1, "test1") == true);
  string_append_cstr(&str2, "test2");
  assert(string_equal_cstr(&str2, "test2") == true);
  assert(string_equal_string(&str1, &str2) == false);
  assert(string_compare(&str1, &str2) != 0);
  string_append_string(&str1, &str2);
  assert(string_equal_cstr(&str1, "test1test2") == true);
}
void array_test() {
  console_log_cstr(CONSOLE_FORE_LIGHTBLUE "ARRAY" CONSOLE_RESET);
  array_t arr;
  array_constructor(&arr, u64);
  array_push(&arr, (u64)10);
  array_push(&arr, (u64)11);
  array_push(&arr, (u64)12);
  assert(array_at(&arr, u64, 0) == 10);
  assert(array_at(&arr, u64, 1) == 11);
  assert(array_at(&arr, u64, 2) == 12);
  assert(array_length(&arr) == 3);
  assert(array_capacity(&arr) == 4);
  array_deconstructor(&arr);
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
  tilemap_t map_u64;
  map_constructor(&map_u64, u64);
  map_set_cstr(&map_u64, "key1", (u64)10);
  assert(map_get_cstr(&map_u64, u64, "key1") == 10);
  map_delete_cstr(&map_u64, "key1");
  assert(map_get_cstr_p(&map_u64, u64, "key1") == 0);
  map_deconstructor(&map_u64);
}

void tcp_server_on_write(net_tcp_client_t* this, const void* context) {}
void tcp_server_on_read(net_tcp_client_t* this, const byte* data, u64 length,
  const void* context) {
  const char http_response [] = "HTTP/1.1 200 OK\r\nConnection: Close\r\n\r\n";
  net_tcp_client_write(this, http_response, sizeof(http_response) - 1, tcp_server_on_write);
}
void tcp_server_on_connection(net_tcp_client_t* this) {
  net_tcp_client_read(this, 0, tcp_server_on_read);
}
void net_tcp_server_test(application_t* app) {
  console_log_cstr(CONSOLE_FORE_LIGHTBLUE "NET_TCP_SERVER" CONSOLE_RESET);
  net_tcp_server_t* server = net_tcp_server_new(app);
  net_tcp_server_ip4_listen(server, 8080, tcp_server_on_connection);
}

i32 main() {
  console_startup();
  net_startup();
  application_t app;
  app_new(&app);

  string_test();
  array_test();
  date_test();
  snowflake_test();
  map_test();
  net_tcp_test(&app);
  // net_tcp_server_test(&app);
  path_test();
  process_test();

  app_run(&app);
  console_log_cstr(CONSOLE_FORE_GREEN "SUCCESS" CONSOLE_FORE_GREEN CONSOLE_RESET);
}
