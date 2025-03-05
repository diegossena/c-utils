#include <sdk/udp.h>
#ifdef PLATFORM_WINDOWS
#include <winsock2.h>

export error_t udp_bind(udp_t this, net_port_t port) {
  struct sockaddr_in address = {
    .sin_family = AF_INET,
    .sin_port = port,
    .sin_addr.S_un.S_addr = INADDR_ANY
  };
  if (bind(this, (struct sockaddr*)&address, sizeof(struct sockaddr)) == SOCKET_ERROR) {
    return WSAGetLastError();
  }
  return ERR_SUCCESS;
}
export error_t udp_read(udp_t this, u8* target, u64 length, net_address_t* address) {
  i32 size = sizeof(net_address_t);
  i32 result = recvfrom(this, (char*)target, length, 0, (struct sockaddr*)address, &size);
  if (result < 0) {
    return WSAGetLastError();
  }
  return ERR_SUCCESS;
}
export error_t udp_send(udp_t this, const u8* data, u64 length, net_address_t* address) {
  i32 result = sendto(
    this, (char*)data, length, 0,
    (struct sockaddr*)address, sizeof(net_address_t)
  );
  if (result < 0) {
    return WSAGetLastError();
  }
  return ERR_SUCCESS;
}

#endif