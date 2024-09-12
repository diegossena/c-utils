
#include <sdk/tcp.h>
#include <sdk/unity.h>

void http_ondata(tcp_t* this, byte_t* chunk, u32 length) {
  // console_log("ondata.chunk[%d]", length);
  // console_write_cstr("`");
  // console_write_str(chunk, length);
  // console_write_cstr("`\n");
}
u64 response_count = 0;
void http_onresponse(tcp_t* tcp) {
  console_log("%llu %d http_onresponse %s", date_now(), tcp->__socket, error_cstr(tcp->error_code));
  // if (tcp->error_code) {
  //   error("http_onresponse", tcp->error_code);
  // } else {
  // }
}
void http_onrequest(tcp_t* tcp) {
  if (tcp->error_code) {
    error("http_onrequest", tcp->error_code);
  } else {
    tcp->ondata = http_ondata;
    tcp->onend = http_onresponse;
    tcp_read(tcp, 0);
  }
}
void http_onconnect(tcp_t* tcp) {
  console_log("%llu %d http_onresponse %s", date_now(), tcp->__socket, error_cstr(tcp->error_code));
  if (tcp->error_code) {
    // error("http_onconnect", tcp->error_code);
  } else {
    const char http_request [] =
      "GET / HTTP/1.1\r\n"
      "Host: www.google.com.br\r\n"
      "Connection: close\r\n"
      "\r\n";
    tcp->onend = http_onrequest;
    tcp_write(tcp, http_request, sizeof(http_request) - 1);
  }
}

i32 main(i32 argc, char** argv) {
  taskmanager_t taskmanager;
  taskmanager_constructor(&taskmanager);
  for (i32 i = 0; i < 1000; i++) {
    tcp_t* tcp = tcp_new(&taskmanager);
    tcp->address.ip4 = ip4_from_bytes(18, 188, 168, 78);
    tcp->address.net_port = net_port_from_short(80);
    tcp->onend = http_onconnect;
  }
  taskmanager_run(&taskmanager);
  console_color(ANSI_FORE_LIGHTGREEN);
  console_log("SUCCESS");
  console_color(ANSI_RESET);
  return 0;
}