#include "internal/stream.h"

// i32 socket_write(net_socket* this, const byte* chunk, u32 length) {
//   i32 code = send(this->id, chunk, length, 0);
//   if (code == SOCKET_ERROR) {
//     error("send", WSAGetLastError());
//   }
//   return code;
// }
// i32 socket_read(net_socket* this, byte* buffer, u32 length) {
//   i32 received = recv(this->id, buffer, length, 0);
//   if (received < 0) {
//     error("recv", WSAGetLastError());
//   }
//   return received;
// }


void stream_init(stream* this, handle_type type) {
  handle_init(&this->stream_handle, type);
}