
#include <sdk/tcp.h>
#include <sdk/unity.h>

void http_ondata(tcp_t* this, char* chunk, u32 length) {
  // console_log("ondata.chunk[%d]", length);
  // console_write_cstr("`");
  // console_write_str(chunk, length);
  // console_write_cstr("`\n");
}
void http_onresponse(tcp_t* tcp, error_code_t error_code) {
  console_log("%x http_onresponse %s", tcp, error_cstr(error_code));
}
void http_onrequest(tcp_t* tcp, error_code_t error_code) {
  console_log("%x http_onrequest %s", tcp, error_cstr(error_code));
  if (!error_code) {
    tcp->ondata = http_ondata;
    tcp->onend = http_onresponse;
    tcp_read(tcp, 0);
  }
}
void http_onconnect(tcp_t* tcp, error_code_t error_code) {
  console_log("%x http_onconnect %s", tcp, error_cstr(error_code));
  if (!error_code) {
    const char http_request [] =
      "GET / HTTP/1.1\r\n"
      "Host: www.google.com.br\r\n"
      "Connection: close\r\n"
      "\r\n";
    tcp->onend = http_onrequest;
    tcp_write(tcp, http_request, sizeof(http_request) - 1);
  }
}
// MAIN 263586 bytes
i32 main(i32 argc, char** argv) {
  task_manager_startup();
  for (i32 i = 0; i < 1; i++) {
    tcp_t* tcp = tcp_new();
    tcp->address.ip4 = ip4_from_bytes(142, 250, 79, 35);
    tcp->address.net_port = net_port_from_short(80);
    tcp->onend = http_onconnect;
  }
  task_manager_run();
  console_color(ANSI_FORE_LIGHTGREEN);
  console_log("SUCCESS");
  console_color(ANSI_RESET);
  return 0;
}