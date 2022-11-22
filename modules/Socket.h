#pragma once
#include <stdio.h>
#include <stdint.h>
#include <winsock2.h>
#include <windows.h>

#include "smart_ptr.h"
#include "String_t.h"
#include "Error.h"

#pragma comment(lib, "ws2_32.lib")

#define MAX_HTTP_BYTES (uint64_t)1001654

u_int __socket_startup = 0;
void socket_init() {
  if (!__socket_startup++) {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult)
      Error("WSAStartup", iResult);
  }
}

typedef struct Socket {
  SOCKET __socket;
} _Socket;

void _socket_free(const _Socket* sock) {
  if (sock->__socket) {
    closesocket(sock->__socket);
    if (--__socket_startup)
      WSACleanup();
  }
}
#define Socket  _Socket __attribute__((cleanup(_socket_free)))

_String _dns_resolve4(_String* host) {
  struct hostent* remoteHost = gethostbyname(host->cptr);
  if (remoteHost == NULL)
    Error("gethostbyname", -1);
  struct in_addr addr;
  addr.s_addr = *(u_long*)remoteHost->h_addr_list[0];
  return string_cstr_new(inet_ntoa(addr));
}

_Socket socket_tcp_connect_timeout(_String address, uint16_t port, uint32_t miliseconds) {
  socket_init();
  String ipv4 = _dns_resolve4(&address);
  SOCKET __socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (__socket == INVALID_SOCKET)
    Error("gethostbyname", WSAGetLastError());

  struct sockaddr_in connInfo;
  connInfo.sin_family = AF_INET;
  connInfo.sin_addr.s_addr = inet_addr(ipv4.cptr);
  connInfo.sin_port = htons(port);

  int iResult;
    /**
     * If iMode = 0, blocking is enabled;
     * If iMode != 0, non-blocking mode is enabled.
     */
  u_long iMode = 1;
  if (miliseconds)
    if (ioctlsocket(__socket, FIONBIO, &iMode) != NO_ERROR)
      Error("ioctlsocket", WSAGetLastError());

  iResult = connect(__socket, (SOCKADDR*)&connInfo, sizeof(connInfo));

  if (miliseconds) {
    iResult = WSAGetLastError();
    if (iResult != WSAEWOULDBLOCK)
      Error("connect", iResult);

    TIMEVAL timeout = { 0, miliseconds * 1000 };
    fd_set readable, writable;
    FD_ZERO(&readable);
    FD_SET(__socket, &readable);
    FD_ZERO(&writable);
    FD_SET(__socket, &writable);
    iResult = select(0, &readable, &writable, NULL, &timeout);
    if (iResult <= 0)
      Error("ETIMEDOUT", ETIMEDOUT);
  } else if (iResult == SOCKET_ERROR)
    Error("connect", WSAGetLastError());
  return (_Socket) { .__socket = __socket };
}
_Socket _socket_tcp_connect_cstr(const char* address, uint16_t port) {
  String _address = string_cstr_new(address);
  return socket_tcp_connect_timeout(_address, port, 0);
}
_Socket _socket_tcp_connect(_String address, uint16_t port) {
  return socket_tcp_connect_timeout(address, port, 0);
}
#define socket_tcp_connect(address, port)	_Generic((address),	\
	char*:				_socket_tcp_connect_cstr,				\
	const char*:	_socket_tcp_connect_cstr,				\
	_String:			_socket_tcp_connect							\
)(address, port)

_String socket_read_buffer(_Socket sock, uint64_t length) {
  _String data;
  int bytesReceived = 0;
  uint64_t buffer_length = length + 1;
  while (length > 0) {
    char buffer[buffer_length];
    if ((bytesReceived = recv(sock.__socket, buffer, length, 0)) <= 0)
      break;
    string_append(data, string_ref(buffer));
    length -= bytesReceived;
  }
  if (bytesReceived < 0)
    Error("socket_read_buffer", WSAGetLastError());
  return data;
}
_String socket_read(_Socket sock) {
  _String data;
  char buffer[MAX_HTTP_BYTES + 1];
  uint64_t bytesReceived;
  while ((bytesReceived = recv(sock.__socket, buffer, MAX_HTTP_BYTES, 0)) > 0) {
    string_append(data, string_ref(buffer));
    memset(buffer, '\0', bytesReceived);
  }
  if (bytesReceived < 0)
    Error("socket_read", WSAGetLastError());
  return data;
}
void _socket_write(_Socket* sock, const _String* src_strings, uint64_t count) {
  while (count--) {
    const _String* src_string = src_strings++;
    int iResult = send(sock->__socket, src_string->cptr, src_string->length, 0);
    if (iResult == SOCKET_ERROR)
      Error("send", WSAGetLastError());
  }
}
#define socket_write(sock, ...) ({	\
	const _String args[] = { __VA_ARGS__ };	\
	_socket_write(&sock, args, sizeof(args)/sizeof(args[0]));	\
})

char socket_get_byte(_Socket sock) {
  char byte;
  int iResult = recv(sock.__socket, &byte, 1, 0);
  if (iResult == SOCKET_ERROR)
    Error("socket_get_byte", WSAGetLastError());
  return byte;
}
_String socket_get_line(_Socket sock) {
  char
    buffer[BUFSIZ] = "",
    caracter,
    * ptr = buffer;
  while ((caracter = socket_get_byte(sock)) != '\n')
    *ptr++ = caracter;
  if (*--ptr == '\r')
    *ptr = '\0';
  return string_cstr_new(buffer);
}