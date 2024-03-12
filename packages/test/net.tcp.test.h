#include <sdk/sdk.h>

void on_tcp_on_read(net_tcp_t* this, const byte* data, u64 length, const void* context) {
  console_log("on_tcp_on_read=%llu", length);
  console_write_cstr(LOG_LEVEL_INFO, "'", 1);
  console_log_str(data, 12);
  console_write_cstr(LOG_LEVEL_INFO, "'\n", 2);
}
void on_tcp_on_write(net_tcp_t* this, const void* context) {
  // console_log_cstr("on_tcp_on_write");
  net_tcp_read(this, 0, on_tcp_on_read);
}
void on_tcp_on_connect(net_tcp_t* this) {
  // console_log_cstr("on_tcp_on_connect");
  const char request [] = "GET / HTTP/1.1\r\nConnection: close\r\n\r\n";
  net_tcp_write(this, &request[0], sizeof(request) - 1, on_tcp_on_write);
}
void net_tcp_test(application_t* app) {
  console_log_cstr(CONSOLE_FORE_LIGHTBLUE "NET_TCP_CLIENT" CONSOLE_RESET);
  for (u64 i = 0; i < 100; i++) {
    net_tcp_t* socket = net_tcp_new(app);
    net_tcp_ip4_connect(socket, "google.com.br", 80, on_tcp_on_connect);
  }
}