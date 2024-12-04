#include <sdk/udp.h>
#ifdef PLATFORM_WINDOWS
#include <winsock2.h>

export void udp_bind(udp_t* this, u16 net_port) {
  struct sockaddr_in address = {
    .sin_family = AF_INET,
    .sin_port = net_port,
    .sin_addr.S_un.S_addr = INADDR_ANY
  };
  if (bind(this->__socket, (struct sockaddr*)&address, sizeof(struct sockaddr)) == SOCKET_ERROR) {
    console_error("bind", ERR_UNKNOWN);
  }
}
export void __udp_read(udp_t* this) {
  DWORD bytes, flags = 0;
  WSABUF buffer = { 0 };
  OVERLAPPED overlapped = { 0 };
  err_t result = WSARecvFrom(
    this->__socket, &buffer, 1, &bytes,
    &flags, 0, 0, &overlapped, 0
  );
  if (result == SOCKET_ERROR) {
    result = WSAGetLastError();
    if (result != ERR_IO_PENDING) {
      console_error("WSARecvFrom", result);
      return _task_call_destroy(&this->_promise);
    }
  }
  this->_promise.callback = (task_callback_t)__udp_onread;
}
export void _udp_send_task(udp_send_t* this) {
  err_t sent = sendto(
    this->udp->__socket, this->data, this->length, 0,
    (struct sockaddr*)&this->address, sizeof(net_address_t)
  );
  if (sent > 0) {
    this->length -= sent;
    if (this->length) {
      this->data += sent;
      this->__updated_at = date_now();
      return;
    }
  } else {
    this->error_code = WSAGetLastError();
    console_error("_udp_send_task", this->error_code);
    _task_call_destroy(&this->udp->_promise);
  }
onend:
  _task_call_destroy(&this->_task);
}

export void __udp_onread(udp_t* this) {
  udp_message_t message;
  WSABUF buffer = { BUFFER_DEFAULT_SIZE, message.data };
  DWORD bytes, flags;
  err_t address_size = sizeof(struct sockaddr);
  err_t result = WSARecvFrom(this->__socket, &buffer, 1, &bytes, &flags, (struct sockaddr*)&message.address, &address_size, 0, 0);
  if (result == SOCKET_ERROR) {
    result = WSAGetLastError();
    if (result != WSAEWOULDBLOCK) {
      console_error("WSARecvFrom", result);
      goto exit;
    }
  }
  message.length = bytes;
  this->onmessage(&message);
exit:
  __udp_read(this);
}

#endif