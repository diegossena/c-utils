#include "internal/platform.h"

#if PLATFORM_WINDOWS

#include "sdk/net.server.h"
#include "sdk/error.h"
#include <winsock2.h>

error_code net_server_listen(net_server* this, u64 port) {
  SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (listenSocket == INVALID_SOCKET) {
    error("socket", WSAGetLastError());
    closesocket(listenSocket);
    return error_last;
  }
  // Definir o endereço e a porta do servidor
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);
  // Vincular o socket ao endereço e porta
  if (bind(listenSocket, &address, sizeof(address)) == SOCKET_ERROR) {
    error("bind", WSAGetLastError());
    closesocket(listenSocket);
    return error_last;
  }
  // Escutar por conexões
  if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
    error("listen", WSAGetLastError());
    closesocket(listenSocket);
  }
  // Configurar o socket de escuta como não bloqueante
  u_long mode = 1;
  if (ioctlsocket(listenSocket, FIONBIO, &mode) == SOCKET_ERROR) {
    error("ioctlsocket", WSAGetLastError());
    closesocket(listenSocket);
  }
  // Aceitar conexões e lidar com elas
  printf("Servidor HTTP iniciado. Aguardando conexões...\n");
  while (true) {
    SOCKET client_socket = accept(listenSocket, 0, 0);
    u64 start = date_now();
    if (client_socket == INVALID_SOCKET) {
      error_last = WSAGetLastError();
      if (error_last != WSAEWOULDBLOCK) {
        error("accept", error_last);
        return error_last;
      }
    } else {

    }
  }
  // Fechar o socket de escuta
  closesocket(listenSocket);
}

#endif