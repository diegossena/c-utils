#include <sdk/net.h>
#ifdef PLATFORM_WINDOWS
#include <winsock2.h>

// inline socket_t _socket_async_new(socket_type_t type) {
//   SOCKET this = socket(AF_INET, type, 0);
//   CreateIoCompletionPort((HANDLE)this, _iocp, (u_long)0, 0);
//   return this;
// }

#endif