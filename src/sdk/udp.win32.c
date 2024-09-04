#include <sdk/udp.h>
#ifdef PLATFORM_WINDOWS
#include <winsock2.h>

SDK_EXPORT void udp_bind(udp_t* this, u16 net_port) {
  struct sockaddr_in address = {
    .sin_family = AF_INET,
    .sin_port = net_port,
    .sin_addr.S_un.S_addr = INADDR_ANY
  };
  if (bind(this->__socket, (struct sockaddr*)&address, sizeof(struct sockaddr)) == SOCKET_ERROR) {
    error("bind", ERR_UNKNOWN);
  }
}
SDK_EXPORT void _udp_service(udp_t* this) {
  buffer_default_t buffer;
  udp_message_t message;
  message.udp = this;
  message.data = buffer;
  i32 address_size = sizeof(struct sockaddr);
  i32 error_code = recvfrom(
    this->__socket, buffer, BUFFER_DEFAULT_SIZE, 0,
    (struct sockaddr*)&message.address, &address_size
  );
  if (error_code > 0) {
    message.length = error_code;
    this->onmessage(&message);
  } else if (error_code < 0) {
    error_code = WSAGetLastError();
    if (error_code != ERR_EWOULDBLOCK && error_code != ERR_ECONNRESET) {
      error("_udp_service", error_code);
      _task_call_destroy(&this->_service);
    }
  }
}
SDK_EXPORT void _udp_send_task(udp_send_t* this) {
  i32 sent = sendto(
    this->udp->__socket, this->data, this->length, 0,
    (struct sockaddr*)&this->address, sizeof(net_address_t)
  );
  if (sent > 0) {
    this->__updated_at = date_now();
    this->data += sent;
    this->length -= sent;
    if (this->length)
      return;
  } else {
    this->error_code = WSAGetLastError();
    error("_udp_send_task", this->error_code);
    return _task_call_destroy(&this->udp->_service);
  }
onend:
  if (this->callback) {
    this->callback(this);
  }
  _task_call_destroy(&this->_task);
}

#endif