#include <sdk/tcp.h>

export tcp_t tcp_new() {
  return _socket_new(SOCKET_TYPE_STREAM);
}
export void tcp_free(tcp_t this) {
  _socket_free(this);
}
export i32 tcp_read(tcp_t this, char* target, u64 size) {
  i32 result;
  char* stream = target;
  do {
    result = tcp_read_stream(this, stream, size);
    if (result > 0) {
      stream += result;
      size -= result;
    } else if (result == 0) {
      break;
    } else {
      return -1;
    }
  } while (size > 0);
  *stream = '\0';
  return stream - target;
}
export i32 tcp_send(tcp_t this, const char* buffer, u64 size) {
  i32 result;
  const char* stream = buffer;
  do {
    result = tcp_send_stream(this, stream, size);
    if (result > 0) {
      stream += result;
      size -= result;
    } else if (result < 0) {
      return -1;
    }
  } while (size > 0);
  return stream - buffer;
}