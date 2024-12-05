#include <sdk/udp.h>
#ifdef PLATFORM_WINDOWS
#include <winsock2.h>

export i32 udp_read(udp_t this, char* target, u64 length, net_address_t* address) {
  return recvfrom(this, target, length, 0, (struct sockaddr*)address, 0);
}
export i32 udp_send(udp_t this, const char* data, u64 length, net_address_t* address) {
  return sendto(
    this, data, length, 0,
    (struct sockaddr*)&address, sizeof(net_address_t)
  );
}

#endif