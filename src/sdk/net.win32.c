#include <sdk/net.h>
#ifdef PLATFORM_WINDOWS
#include <winsock2.h>

error_t net_startup() {
  WSADATA wsa_data;
  i32 code = WSAStartup(MAKEWORD(2, 2), &wsa_data);
  if (code) {
    WSACleanup();
    return code;
  }
  return ERR_SUCCESS;
}
inline void net_shutdown() { WSACleanup(); }

inline socket_t _socket_new(socket_type_t type) { return socket(AF_INET, type, 0); }
inline void _socket_free(socket_t fd) { closesocket((SOCKET)fd); }

#endif