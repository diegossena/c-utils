#include <sdk/udp.h>
#ifdef PLATFORM_WINDOWS
#include <winsock2.h>

SDK_EXPORT void udp_deconstructor(udp_t* this) {
  closesocket((SOCKET)this->__socket);
  callback_emit(&this->_tasks);
  queue_remove(&this->_service.queue);
}
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
  i32 error_code = recvfrom(
    this->__socket, buffer, BUFFER_DEFAULT_SIZE, 0,
    (struct sockaddr*)&message.address, 0
  );
  if (error_code > 0) {
    message.length = error_code;
    this->onmessage(&message);
  } else if (error_code < 0) {
    error_code = WSAGetLastError();
    error("udp_service", error_code);
    task_call_destroy(&this->_service);
  }
}
SDK_EXPORT void _udp_send_task(udp_send_t* this) {
  console_log("_udp_send_task %d", this->udp->__socket);
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
    error("sendto", this->error_code);
    task_call_destroy(&this->udp->_service);
  }
onend:
  if (this->callback) {
    this->callback(this);
  }
  task_call_destroy(&this->_task);
}

#endif