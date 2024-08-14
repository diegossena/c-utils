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
  static udp_message_t message = { .data = buffer_shared };
  message.udp = this;
  message.length = recvfrom(
    this->socket, buffer_shared, BUFFER_SIZE, 0,
    (struct sockaddr*)&message.address, 0
  );
  if (message.length > 0) {
    this->onmessage(&message);
  }
}
SDK_EXPORT void udp_writer_task(udp_writer_t* this) {
  i32 sent = sendto(
    this->udp->socket, this->ptr, this->remaining, 0,
    (struct sockaddr*)&this->address, sizeof(net_address_t)
  );
  if (sent > 0) {
    // console_log("%x udp_write_handle %d", this, sent);
    this->updated_at = date_now();
    this->ptr += sent;
    this->remaining -= sent;
    if (!this->remaining) {
      if (this->callback) {
        this->callback(this);
      }
      udp_writer_shutdown(this);
    }
  } else {
    udp_writer_shutdown(this);
  }
}

#endif