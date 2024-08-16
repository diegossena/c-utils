#include <sdk/udp.h>
#ifdef PLATFORM_WINDOWS
#include <winsock2.h>

SDK_EXPORT void udp_bind(udp_t* this, u16 port) {
  struct sockaddr_in address = {
    .sin_family = AF_INET,
    .sin_port = htons(port),
    .sin_addr.S_un.S_addr = INADDR_ANY
  };
  if (bind(this->socket, (struct sockaddr*)&address, sizeof(struct sockaddr)) == SOCKET_ERROR) {
    error("bind", ERR_UNKNOWN);
  }
}
SDK_EXPORT void udp_service(udp_t* this) {
  buffer_default_t buffer;
  udp_message_t message;
  message.udp = this;
  message.data = buffer;
  message.length = recvfrom(
    this->socket, buffer, BUFFER_SIZE, 0,
    (struct sockaddr*)&message.address, 0
  );
  if (message.length > 0) {
    this->onmessage(&message);
  }
}
SDK_EXPORT void udp_writer_task(udp_send_t* this) {
  i32 sent = sendto(
    this->_udp->socket, this->__ptr, this->__remaining, 0,
    (struct sockaddr*)&this->address, sizeof(net_address_t)
  );
  console_log("udp_writer_task %d", sent);
  if (sent > 0) {
    this->updated_at = date_now();
    this->__ptr += sent;
    this->__remaining -= sent;
    if (!this->__remaining) {
      udp_writer_shutdown(this);
    }
  } else {
    udp_writer_shutdown(this);
  }
}

#endif